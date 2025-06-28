#include "token/token.h"
#include "hashmap/hashmap.h"
#include <string.h>

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
    char *str = read_file("./token/theverdict.txt");
    IDList *ids = encode(str);
    id_print(ids);
    id_free(ids);

    free(str);
    return 0;
}
