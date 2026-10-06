#include "http_method.h"

static const char GET_STR[]     = "GET";
static const char PUT_STR[]     = "PUT";
static const char POST_STR[]    = "POST";
static const char DELETE_STR[]  = "DELETE";

static int http_method_check_boundaries(const char *str) {
    size_t size = 0;

    while (size++ < HTTP_METHOD_MAX_STR_SIZE) {
        if (str[size - 1] == '\0') {
            return 0;
        }
    }

    return -1;
}

int http_method_str_to_m(const char *str) {
    int res = LHTTP_UNKOWN_METHOD;

    if (str == NULL) {
        fprintf(stderr, "http_method_str_to_m failed, str is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (http_method_check_boundaries(str) != 0) {
        fprintf(stderr, "http_method_str_to_m failed, boundary check failed\n");
        return LHTTP_STR_MALFORMED;
    }

    if (!strncmp(str, GET_STR, sizeof(GET_STR))) {
        res = HTTP_METHOD_GET;
    }
    else if (!strncmp(str, PUT_STR, sizeof(PUT_STR))) {
        res = HTTP_METHOD_PUT;
    }
    else if (!strncmp(str, POST_STR, sizeof(POST_STR))) {
        res = HTTP_METHOD_POST;
    }
    else if (!strncmp(str, DELETE_STR, sizeof(DELETE_STR))) {
        res = HTTP_METHOD_DELETE;
    }

    return res;
}

const char* http_method_m_to_str(http_method_t m) {
    switch (m) {
        case HTTP_METHOD_GET:
            return GET_STR;

        case HTTP_METHOD_PUT:
            return PUT_STR;

        case HTTP_METHOD_POST:
            return POST_STR;

        case HTTP_METHOD_DELETE:
            return DELETE_STR;

        default:
            fprintf(stderr, "http_method_m_to_str failed, method not recognized\n");
            return NULL;
    }
}