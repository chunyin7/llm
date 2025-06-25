#ifndef TOKEN
#define TOKEN

typedef struct {
    char **data;
    size_t len, cap;
} TokenList;

TokenList *tl_init();

void tl_add(TokenList *tl, char *tok, size_t tok_len);

void tl_free(TokenList *tl);

TokenList *tokenize(char *str, size_t len);

void tl_print(TokenList *tl);

#endif
