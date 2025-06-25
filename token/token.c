// word tokenizer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

TokenList *tl_init() {
    TokenList *tl = malloc(sizeof(TokenList));
    tl->len = 0;
    tl->cap = 16;
    tl->data = malloc(tl->cap * sizeof(char *));

    return tl;
}

void tl_add(TokenList *tl, char *tok, size_t tok_len) {
    if (tl->len >= tl->cap - 1) {
        tl->cap *= 2;
        tl->data = realloc(tl->data, tl->cap * sizeof(char *));
    }

    tl->data[tl->len] = malloc(sizeof(char) * (tok_len + 1));
    memcpy(tl->data[tl->len], tok, tok_len);
    tl->data[tl->len][tok_len] = '\0';
    tl->len++;

    return;
}

void tl_free(TokenList *tl) {
    for (size_t i = 0; i < tl->len; i++) {
        free(tl->data[i]);
    }

    free(tl->data);
    free(tl);

    return;
}

void tokenize(TokenList *tl, char *str, size_t len) {
    size_t start = 0;

    for (size_t end = 0; end < len; end++) {
        if (isalnum(str[end])) {
            continue;
        } else if (isspace(str[end])) {
            if (start != end) {
                char *tmp = malloc(sizeof(char) * (end - start + 1));
                memcpy(tmp, str + start, end - start);
                tmp[end - start] = '\0';
                tl_add(tl, tmp, end - start);
                free(tmp);
            }

            start = end + 1;
        } else {
            // punctuation
            // first add the entire word preceding the punctuation
            if (end - start > 1) {
                char *tmp = malloc(sizeof(char) * (end - start + 1));
                memcpy(tmp, str + start, end - start);
                tmp[end - start] = '\0';
                tl_add(tl, tmp, end - start);
                free(tmp);
            }

            // then add the punctuation
            char tmp[2];
            tmp[0] = str[end];
            tmp[1] = '\0';
            tl_add(tl, tmp, 1);

            start = end + 1;
        }
    }

    return;
}

// util / debug
void tl_print(TokenList *tl) {
    printf("TOKEN LIST, length: %zu, capacity: %zu:\n", tl->len, tl->cap);
    printf("[ ");
    for (size_t i = 0; i < tl->len; i++) {
        printf("'%s' ", tl->data[i]);
    }
    printf(" ]\n");

    return;
}

/* Load entire file into a NUL‐terminated buffer. */
char *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END)) { fclose(f); return NULL; }
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, len, f) != (size_t)len) {
        free(buf);
        fclose(f);
        return NULL;
    }
    buf[len] = '\0';
    fclose(f);
    return buf;
}

int main(void) {
    TokenList *tl = tl_init();
    char *str = read_file("theverdict.txt");
    tokenize(tl, str, strlen(str));
    tl_print(tl);

    tl_free(tl);

    return 0;
}
