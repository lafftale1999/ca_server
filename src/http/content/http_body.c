#include "http_body.h"

int http_body_initialize(http_body_t *out, const char *data_in, size_t limit) {
    if (out == NULL) {
        fprintf(stderr, "http_body_initialize failed, out is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (data_in == NULL) {
        fprintf(stderr, "http_body_initialize failed, data_in is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    char *tmp = calloc(limit, sizeof(char));
    if (tmp == NULL) {
        fprintf(stderr, "http_body_initialize failed, unable to allocate memory\n");
        return LHTTP_MEMORY_ERR;
    }

    out->data = tmp;
    out->size = limit;

    memcpy(out->data, data_in, limit);

    return LHTTP_SUCCESS;
}

int http_body_free(http_body_t *body) {
    if (body == NULL) {
        fprintf(stderr, "http_body_free failed, body is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (body->data != NULL) {
        free(body->data);
    }

    body->data = NULL;
    body->size = 0;

    return LHTTP_SUCCESS;
}