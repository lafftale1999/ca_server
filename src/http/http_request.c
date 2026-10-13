#include "http_request.h"

int http_request_free(http_request_t *req) {
    int res = LHTTP_SUCCESS;

    if (req == NULL) {
        fprintf(stderr, "http_request_free failed, req is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if ((res = http_body_free(&req->body)) < 0) {
        fprintf(stderr, "http_request_free unable to free body\n");
    }

    if ((res = http_headers_arr_free(&req->headers)) < 0) {
        fprintf(stderr, "http_request_free unable to free headers\n");
    }

    if ((res = http_path_free(&req->path)) < 0) {
        fprintf(stderr, "http_request_free unable to free path\n");
    }

    if (res < 0) {
        fprintf(stderr, "http_request_free failed\n");
    }

    return res;
}