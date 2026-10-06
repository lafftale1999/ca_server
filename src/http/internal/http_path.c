#include "http_path.h"

static int path_str_check_terminator(const char *p, size_t size) {
    if (p == NULL) {
        fprintf(stderr, "path_str_check_terminator failed, p is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (p[size - 1] == '\0') return 0;
    return LHTTP_NOT_FOUND;
}

int http_path_str_to_p(http_path_t *out, const char *in, size_t in_size) {
    int res;

    if (in == NULL) {
        fprintf(stderr, "http_path_str_to_p failed, in is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    res = path_str_check_terminator(in, in_size);
    if (res == LHTTP_NOT_FOUND) {
        in_size++;
    }

    char *tmp = calloc(in_size, sizeof(char));
    if (tmp == NULL) {
        fprintf(stderr, "http_path_str_to_p failed, unable to allocate memory\n");
        return LHTTP_MEMORY_ERR;
    }

    memcpy(tmp, in, in_size);
    tmp[in_size - 1] = '\0';
    
    out->path = tmp;
    out->size = in_size;

    return LHTTP_SUCCESS;
}

int http_path_free(http_path_t *path) {
    if (path == NULL) {
        fprintf(stderr, "http_path_free failed, path is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (path->path != NULL) {
        free(path->path);
    }

    path->path = NULL;
    path->size = 0;

    return LHTTP_SUCCESS;
}