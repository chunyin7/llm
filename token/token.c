// word tokenizer
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "tokenmap/tokenmap.h"
#include "../arr/array.h"

Vocabulary *voc_init() {
  Vocabulary *voc = malloc(sizeof(Vocabulary));
  voc->t2i = map_init();
  voc->i2t = arr_init(sizeof(Token));

  return voc;
}

Token *tok_dup(Token *tok) {
  Token *cpy = malloc(sizeof(Token));
  cpy->type = tok->type;
  
  if (tok->ids == NULL) {
    cpy->ids = NULL;
  } else {
    cpy->ids = arr_init(sizeof(uint16_t));
    for (size_t i = 0; i < tok->ids->len; i++) {
      arr_append(cpy->ids, &((uint16_t *)tok->ids->data)[i]);
    }
  }

  return cpy;
}

void voc_add(Vocabulary *voc, Token tok) {
  int r = map_add(voc->t2i, tok, voc->t2i->len);
  if (r == voc->i2t->len) {
    arr_append(voc->i2t, &tok);
  }
}

Array *tokenize(uint8_t *in, size_t len, Vocabulary *voc) {
  Array *tokens = arr_init(sizeof(Token));

  // segment into 2d byte array
  Array *words = arr_init(sizeof(Array *));
  Array *cur = arr_init(sizeof(Token));

  size_t start = 0;
  uint16_t eow_id = 257;

  for (size_t end = 0; end < len; end++) {
    uint16_t id = (uint8_t)in[end];
    if (isalnum(in[end])) {
      Token b = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(b.ids, &id);
      arr_append(cur, &b);
    } else {
      // punctuation or whitespace
      // first add the entire word preceding the punctuation
      if (end - start > 0) {
        Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
        arr_append(eow.ids, &eow_id);
        arr_append(cur, &eow);
        arr_append(words, &cur);
      }

      // then add the punctuation
      cur = arr_init(sizeof(Token));
      Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(eow.ids, &eow_id);
      Token tmp = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(tmp.ids, &id);
      arr_append(cur, &tmp);
      arr_append(cur, &eow);
      arr_append(words, &cur);

      start = end + 1;
      cur = arr_init(sizeof(Token));
    }
  }

  if (start != len) {
    arr_append(words, &cur);
  } else {
    arr_free(cur);
  }

  // recursive merge within words
  for (size_t i = 0; i < words->len; i++) {
    Array *word = ((Array **)words->data)[i];
    Array *new = arr_init(sizeof(Token));
    int pair_found = 1;
    while (pair_found) {
      pair_found = 0;
      for (size_t k = 0; k < word->len; k++) {
        Token a = ((Token *)word->data)[k];

        if (k == word->len - 1) {
          arr_append(new, &a);
          break;
        }

        Token b = ((Token *)word->data)[k + 1];
        Token merged = { .type = RAW, .ids = arr_join(a.ids, b.ids) };

        if (map_get(voc->t2i, merged) != -1) {
          // pair exists
          pair_found = 1;
          arr_append(new, &merged);
          // add the rest of the word to new
          for (size_t j = k + 1; j < word->len; j++) {
            arr_append(new, &((Token *)word->data)[j]);
          }

          break; // we only want the first pair
        } else {
          arr_append(new, &a);
        }
      }

      if (pair_found) {
        arr_free(word);
        word = new;
        new = arr_init(sizeof(Token));
      }
    }

    ((Array **)words->data)[i] = new;
    arr_free(word);
  }

  // flatten the 2d array into a token list
  for (size_t i = 0; i < words->len; i++) {
    Array *word = ((Array **)words->data)[i];
    for (size_t j = 0; j < word->len; j++) {
      arr_append(tokens, &((Token *)word->data)[j]);
    }
    arr_free(word);
  }
  arr_free(words);

  return tokens;
}

Array *encode(char *str, Vocabulary *voc) {
  Array *tl = tokenize((uint8_t *)str, strlen(str), voc);

  // convert token list to ids
  Array *ids = arr_init(sizeof(long));
  for (size_t i = 0; i < tl->len; i++) {
    long id = map_get(voc->t2i, ((Token *) tl->data)[i]);
    arr_append(ids, &id);
  }

  for (size_t i = 0; i < tl->len; i++) {
    Token *tok = &(((Token *)tl->data)[i]);
    if (tok->ids) {
      free(tok->ids);
    }
  }

  arr_free(tl);
  return ids;
}

Array *decode(Array *ids, Vocabulary *voc) {
  Array *s = arr_init(sizeof(uint8_t));

  for (size_t i = 0; i < ids->len; i++) {
    Token tok = ((Token *)voc->i2t->data)[((long *)ids->data)[i]];
    if (tok.ids != NULL) {
      for (size_t j = 0; j < tok.ids->len; j++) {
        arr_append(s, &((uint8_t *)tok.ids->data)[j]);
      }
    } else {
      char *tmp;
      switch (tok.type) {
        case UNK:
          tmp = "<|unk|>";
          break;
        case EOS:
          tmp = "<|eos|>";
          break;
        default:
          tmp = "";
          break;
      };

      for (size_t i = 0; i < strlen(tmp); i++) {
        arr_append(s, tmp + i);
      }
    }
  }

  char null = '\0';
  arr_append(s, &null);
  return s;
}

void voc_free(Vocabulary *voc) {
  // The hashmap owns the tokens and will free them
  map_free(voc->t2i);
  // i2t array just has references, so only free the array itself
  arr_free(voc->i2t);
  free(voc);
}

Vocabulary *bpe(size_t voc_size, uint8_t *in, size_t len) {
  if (voc_size < 256) {
    printf("Use larger vocabulary size\n");
    return NULL;
  }

  Vocabulary *voc = voc_init();

  // STEP 1: pre-load with all individual ids
  for (uint16_t u = 0; u <= 255; u++) {
    Token tok = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
    arr_append(tok.ids, &u);
    voc_add(voc, tok);
  }

  // add special tokens
  Token eos = { .type = EOS, .ids = arr_init(sizeof(uint16_t)) };
  uint16_t eos_id = 256;
  arr_append(eos.ids, &eos_id);
  voc_add(voc, eos);

  Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
  uint16_t eow_id = 257;
  arr_append(eow.ids, &eow_id);
  voc_add(voc, eow);

  // STEP 2: divide the string into an 2d token arrays
  Array *words = arr_init(sizeof(Array *));
  Array *cur = arr_init(sizeof(Token));

  size_t start = 0;

  for (size_t end = 0; end < len; end++) {
    uint16_t id = (uint8_t)in[end];
    if (isalnum(in[end])) {
      Token b = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(b.ids, &id);
      arr_append(cur, &b);
    } else {
      // punctuation or whitespace
      // first add the entire word preceding the punctuation
      if (end - start > 0) {
        Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
        arr_append(eow.ids, &eow_id);
        arr_append(cur, &eow);
        arr_append(words, &cur);
      }

      // then add the punctuation
      cur = arr_init(sizeof(Token));
      Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(eow.ids, &eow_id);
      Token tmp = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(tmp.ids, &id);
      arr_append(cur, &tmp);
      arr_append(cur, &eow);
      arr_append(words, &cur);

      start = end + 1;
      cur = arr_init(sizeof(Token));
    }
  }

  if (start != len) {
    arr_append(words, &cur);
  } else {
    arr_free(cur);
  }

  // STEP 3: recursive merging

  for (size_t i = 256; i < voc_size; i++) {
    Map *pair_counts = map_init();

    // calculate pair counts
    for (size_t j = 0; j < words->len; j++) {
      Array *word = ((Array **)words->data)[j];
      for (size_t k = 0; k < word->len - 1; k++) {
        // merge tokens k and k + 1
        Token a = ((Token *)word->data)[k];
        Token b = ((Token *)word->data)[k + 1];
        Token merged = { .type = RAW, .ids = arr_join(a.ids, b.ids) };

        long count = map_get(pair_counts, merged);
        map_update(pair_counts, merged, count + 1);

        if (count != -1) {
          arr_free(merged.ids);
        }
      }
    }

    // get most frequest pair
    Entry max;
    for (size_t j = 0; j < pair_counts->cap; j++) {
      if (pair_counts->entries[j].occupied) {
        if (pair_counts->entries[j].val > max.val) {
          max = pair_counts->entries[j];
        }
      }
    }

    // add the token to vocab
    voc_add(voc, max.key);

    // perform the merge in place on the words
    for (size_t j = 0; j < words->len; j++) {
      Array *word = ((Array **)words->data)[j];
      Array *new = arr_init(sizeof(Token));
      for (size_t k = 0; k < word->len; k++) {
        Token a = ((Token *)word->data)[k];

        if (k == word->len - 1) {
          arr_append(new, &a);
          break;
        }

        Token b = ((Token *)word->data)[k + 1];
        Token merged = { .type = RAW, .ids = arr_join(a.ids, b.ids) };

        if (arr_cmp(merged.ids, max.key.ids) && max.key.type == merged.type) {
          arr_append(new, &merged);
          k++; // skip the next iteration
        } else {
          arr_append(new, &a);
        }
      }

      // replace with rebuilt word
      ((Array **)words->data)[j] = new;
      arr_free(word);
    }
  }

  return voc;
}
