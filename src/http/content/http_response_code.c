#include "http_response_code.h"

static const http_response_code_t response_code_map[] = {
    {.num = 200, .str = "OK"},
    {.num = 404, .str = "Not Found"},
    {.num = 413, .str = "Content Too Large"},
    {.num = 500, .str = "Internal Server Error"}
};

static const size_t response_code_map_len = sizeof(response_code_map) / sizeof(response_code_map[0]);

int http_response_code_parse_str(http_response_code_t *out, const char *in, size_t in_len) {
    if (out == NULL) {
        fprintf(stderr, "http_response_code_parse_str failed, out is NULL\n");
        return LHTTP_STATIC_ERR;
    }
    
    if (in == NULL) {
        fprintf(stderr, "http_response_code_parse_str failed, in is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    long num = strtol(in, in + in_len, 10);

    for (size_t idx = 0; idx < response_code_map_len; idx++) {
        http_response_code_t *tmp = &response_code_map[idx];
        if (num == tmp->num) {
            out->num = tmp->num;
            out->str = tmp->str;

            return LHTTP_SUCCESS;
        }
    }

    return LHTTP_NOT_FOUND;
}

int http_response_code_parse_num(http_response_code_t *out, int num) {
    if (out == NULL) {
        fprintf(stderr, "http_response_code_parse_str failed, out is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    for (size_t idx = 0; idx < response_code_map_len; idx++) {
        http_response_code_t *tmp = &response_code_map[idx];
        if (num == tmp->num) {
            out->num = tmp->num;
            out->str = tmp->str;

            return LHTTP_SUCCESS;
        }
    }

    return LHTTP_NOT_FOUND;
}