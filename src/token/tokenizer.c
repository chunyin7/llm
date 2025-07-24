#include <token/tokenizer.h>
#include <token/vocabulary.h>
#include <util/array.h>

#include <ctype.h>
#include <stdio.h>

Array *tokenize(uint8_t *in, size_t len, Vocabulary *voc) {
  Array *tokens = arr_init(sizeof(Token));

  // segment into 2d byte array
  Array *words = arr_init(sizeof(Array *));
  Array *cur = NULL;

  uint16_t eow_id = 257;

  for (size_t i = 0; i < len; i++) {
    uint16_t id = (uint8_t)in[i];
    if (isalnum(in[i])) {
      if (cur == NULL) {
        cur = arr_init(sizeof(Token));
      }

      Token b = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(b.ids, &id);
      arr_append(cur, &b);
    } else {
      // punctuation or whitespace
      // first add the entire word preceding the punctuation
      if (cur != NULL) {
        Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
        arr_append(eow.ids, &eow_id);
        arr_append(cur, &eow);
        arr_append(words, &cur);
        cur = NULL;
      }

      // then add the punctuation
      Array *punct = arr_init(sizeof(Token));
      Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(eow.ids, &eow_id);
      Token tmp = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(tmp.ids, &id);
      arr_append(punct, &tmp);
      arr_append(punct, &eow);
      arr_append(words, &punct);
    }
  }

  if (cur != NULL) {
    Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
    arr_append(eow.ids, &eow_id);
    arr_append(cur, &eow);
    arr_append(words, &cur);
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
          arr_free(a.ids);
          arr_free(b.ids);
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

Array *encode(uint8_t *in, size_t len, Vocabulary *voc) {
  Array *tl = tokenize((uint8_t *)in, len, voc);

  // convert token list to ids
  Array *ids = arr_init(sizeof(long));
  for (size_t i = 0; i < tl->len; i++) {
    long id = map_get(voc->t2i, ((Token *) tl->data)[i]);
    arr_append(ids, &id);
  }

  for (size_t i = 0; i < tl->len; i++) {
    Token *tok = &(((Token *)tl->data)[i]);
    if (tok->ids) {
      arr_free(tok->ids);
    }
  }

  arr_free(tl);
  return ids;
}

Array *decode(Array *ids, Vocabulary *voc) {
  Array *bytes = arr_init(sizeof(uint8_t));

  for (size_t i = 0; i < ids->len; i++) {
    Token tok = ((Token *)voc->i2t->data)[((long *)ids->data)[i]];
    if (tok.ids != NULL) {
      for (size_t j = 0; j < tok.ids->len; j++) {
        arr_append(bytes, &((uint8_t *)tok.ids->data)[j]);
      }
    }
    
  }

  return bytes;
}

void save_token_sequence(Array *ids) {
  FILE *out = fopen("bin/token_sequence", "wb+");
  uint32_t len = ids->len;
  fwrite(&len, sizeof(size_t), 1, out);
  fwrite(ids->data, sizeof(long), ids->len, out);

  fclose(out);
}

void load_token_sequence(const char *path) {
  FILE *in = fopen(path, "rb");
  if (!in) {
    return;
  }

  uint32_t len;
  Array *ids = arr_init(sizeof(long));

  fread(&len, sizeof(size_t), 1, in);
  for (size_t i = 0; i < len; i++) {
    long id;
    fread(&id, sizeof(long), 1, in);
    arr_append(ids, &id);
  }

  fclose(in);
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
  Array *cur = NULL; // avoid unnecessary mallocs

  for (size_t i = 0; i < len; i++) {
    uint16_t id = (uint8_t)in[i];
    if (isalnum(in[i])) {
      if (cur == NULL) {
        cur = arr_init(sizeof(Token));
      }
      Token b = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(b.ids, &id);
      arr_append(cur, &b);
    } else {
      if (cur != NULL) {
        Token eow_local = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
        arr_append(eow_local.ids, &eow_id);
        arr_append(cur, &eow_local);
        arr_append(words, &cur);
        cur = NULL;
      }

      // then add the punctuation
      Array *punct = arr_init(sizeof(Token));
      Token eow = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(eow.ids, &eow_id);
      Token tmp = { .type = RAW, .ids = arr_init(sizeof(uint16_t)) };
      arr_append(tmp.ids, &id);
      arr_append(punct, &tmp);
      arr_append(punct, &eow);
      arr_append(words, &punct);
    }
  }

  if (cur != NULL) {
    Token eow_local = { .type = EOW, .ids = arr_init(sizeof(uint16_t)) };
    arr_append(eow_local.ids, &eow_id);
    arr_append(cur, &eow_local);
    arr_append(words, &cur);
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
        long new_count = (count == -1 ? 1 : count + 1);
        map_update(pair_counts, merged, new_count);

        if (count != -1) {
          arr_free(merged.ids);
        }
      }
    }

    // get most frequest pair
    Entry max;
    max.val = 0;
    for (size_t j = 0; j < pair_counts->cap; j++) {
      if (pair_counts->entries[j].occupied) {
        if (pair_counts->entries[j].val > max.val) {
          max = pair_counts->entries[j];
        }
      }
    }

    // if no pairs found, break early
    if (max.val == 0) {
      map_free(pair_counts);
      break;
    }

    // add the token to vocab
    Token new_token = { .type = max.key.type, .ids = arr_dup(max.key.ids, max.key.ids->len, max.key.ids->size) };
    voc_add(voc, new_token);

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

        if (arr_cmp(merged.ids, new_token.ids) && new_token.type == merged.type) {
          arr_free(a.ids);
          arr_free(b.ids);
          arr_append(new, &merged);
          k++; // skip the next iteration
        } else {
          arr_free(merged.ids);
          arr_append(new, &a);
        }
      }

      // replace with rebuilt word
      ((Array **)words->data)[j] = new;
      arr_free(word);
    }

    map_free(pair_counts);
  }

  for (size_t i = 0; i < words->len; i++) {
    Array *word = ((Array **)words->data)[i];
    for (size_t j = 0; j < word->len; j++) {
      arr_free(((Token *)word->data)[j].ids);
    }
    arr_free(word);
  }
  arr_free(words);

  return voc;
}
