// word tokenizer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "../hashmap/hashmap.h"

TokenList *tl_init() {
    TokenList *tl = malloc(sizeof(TokenList));
    tl->len = 0;
    tl->cap = 16;
    tl->data = malloc(tl->cap * sizeof(char *));

    return tl;
}

// util
int cmp(const void *a, const void *b) {
    char *str_a = (char *) a;
    char *str_b = (char *) b;

    return strcmp(str_a, str_b);
}

void build_voc(TokenList *tl, Map *voc) {
    // sort the tokens alphabetically
    qsort(tl->data, tl->len, sizeof(char *), cmp);
    printf("sorted\n");

    for (size_t i = 0; i < tl->len; i++) {
        map_get_or_add(voc, tl->data[i]);
    }

    return;
}

void voc_print(Map *voc) {
    for (size_t i = 0; i < voc->cap; i++) {
        if (voc->entries[i].key) { 
            printf("('%s', %d)\n", voc->entries[i].key, voc->entries[i].val);
        }
    }
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

