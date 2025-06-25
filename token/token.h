#ifndef TOKEN
#define TOKEN

typedef struct {
    char **data;
    size_t len, cap;
} TokenList;

typedef struct  {
    char **data;
    size_t len, cap;
} Vocabulary;

Vocabulary *voc_init();

TokenList *tl_init();

void voc_add(Vocabulary *voc, char *tok, size_t tok_len);

void build_voc(TokenList *tl, Vocabulary *voc);

void tl_add(TokenList *tl, char *tok, size_t tok_len);

void tl_free(TokenList *tl);

void tokenize(TokenList *tl, char *str, size_t len);

void tl_print(TokenList *tl);

#endif
