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
  
  if (tok->bytes == NULL) {
    cpy->bytes = NULL;
  } else {
    cpy->bytes = arr_init(sizeof(uint8_t));
    for (size_t i = 0; i < tok->bytes->len; i++) {
      arr_append(cpy->bytes, &((uint8_t *)tok->bytes->data)[i]);
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

void build_voc(Array *tl, Vocabulary *voc) {
  for (size_t i = 0; i < tl->len; i++) {
    voc_add(voc, ((Token *)tl->data)[i]);
  }

  Token unk = { .bytes = NULL, .type = UNK };
  voc_add(voc, unk);

  Token eos = { .bytes = NULL, .type = EOS };
  voc_add(voc, eos);

  return;
}

void tokenize(Array *tokens, char *str, size_t len) {
  size_t start = 0;

  for (size_t end = 0; end < len; end++) {
    if (isalnum(str[end])) {
      continue;
    } else {
      // punctuation or whitespace
      // first add the entire word preceding the punctuation
      if (end - start > 0) {
        Token tmp = { .type = RAW, .bytes = arr_dup(str + start, end - start, sizeof(uint8_t)) };
        arr_append(tokens, &tmp);
      }

      // then add the punctuation
      Token tmp = { .type = RAW, .bytes = arr_init(sizeof(uint8_t)) };
      arr_append(tmp.bytes, str + end);
      arr_append(tokens, &tmp);

      start = end + 1;
    }
  }

  if (start != len) {
    Token tmp = { .type = RAW, .bytes = arr_dup(str + start, len - start, sizeof(uint8_t)) };
    arr_append(tokens, &tmp);
  }

  return;
}

Array *encode(char *str, Vocabulary *voc) {
  Array *tl = arr_init(sizeof(Token));
  tokenize(tl, str, strlen(str));

  // convert token list to ids
  Array *ids = arr_init(sizeof(long));
  for (size_t i = 0; i < tl->len; i++) {
    long id = map_get(voc->t2i, ((Token *) tl->data)[i]);
    if (id == -1) {
      Token unk = { .type = UNK, .bytes = NULL };
      id = map_get(voc->t2i, unk);
    }

    arr_append(ids, &id);
  }

  for (size_t i = 0; i < tl->len; i++) {
    Token *tok = &(((Token *)tl->data)[i]);
    if (tok->bytes) {
      free(tok->bytes);
    }
  }

  arr_free(tl);
  return ids;
}

Array *decode(Array *ids, Vocabulary *voc) {
  Array *s = arr_init(sizeof(uint8_t));

  for (size_t i = 0; i < ids->len; i++) {
    Token tok = ((Token *)voc->i2t->data)[((long *)ids->data)[i]];
    if (tok.bytes != NULL) {
      for (size_t j = 0; j < tok.bytes->len; j++) {
        arr_append(s, &((uint8_t *)tok.bytes->data)[j]);
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

Vocabulary *bpe(uint8_t *in, size_t len) {
  Vocabulary *voc = voc_init();

  // STEP 1: pre-load with all individual bytes
  for (unsigned char u = 0; u <= 255; u++) {
    Token tok = { .type = RAW, .bytes = arr_init(sizeof(uint8_t)) };
    arr_append(tok.bytes, &u);
    voc_add(voc, tok);
  }

  // add special tokens
  Token eos = { .type = EOS, .bytes = NULL };
  voc_add(voc, eos);

  Token eow = { .type = EOW, .bytes = NULL };
  voc_add(voc, eow);

  // STEP 2: divide the string into an 2d token arrays
  Array *words = arr_init(sizeof(Array *));
  Array *cur = arr_init(sizeof(Token));

  size_t start = 0;

  for (size_t end = 0; end < len; end++) {
    if (isalnum(in[end])) {
      Token b = { .type = RAW, .bytes = arr_init(sizeof(uint8_t)) };
      arr_append(b.bytes, in + end);
      arr_append(cur, &b);
    } else {
      // punctuation or whitespace
      // first add the entire word preceding the punctuation
      if (end - start > 0) {
        arr_append(words, &cur);
      }

      // then add the punctuation
      cur = arr_init(sizeof(Token));
      Token tmp = { .type = RAW, .bytes = arr_init(sizeof(uint8_t)) };
      arr_append(tmp.bytes, in + end);
      arr_append(cur, &tmp);
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

  return voc;
}
