#ifndef TOKEN
#define TOKEN

#include <stdlib.h>
#include "../hashmap/hashmap.h"

typedef struct {
    char **data;
    size_t len, cap;
} TokenList;

Map *voc_init();

TokenList *tl_init();

void voc_add(Map *voc, char *tok, size_t tok_len);

void build_voc(TokenList *tl, Map *voc);

void tl_add(TokenList *tl, char *tok, size_t tok_len);

void tl_free(TokenList *tl);

void tokenize(TokenList *tl, char *str, size_t len);

void tl_print(TokenList *tl);

void voc_print(Map *voc);

#endif
