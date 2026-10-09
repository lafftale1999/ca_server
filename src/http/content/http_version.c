#include "http_version.h"

static const char V_1_1_STR[] = "1.1";

static const char LEADING_V_STR[] = "HTTP/";

static int http_version_boundary_check(const char *str) {
    if (str == NULL) {
        fprintf(stderr, "http_version_boundary_check failed, str is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    size_t size = 0;

    while (size++ < sizeof(V_1_1_STR)) {
        if (str[size - 1] == '\0') return 0;
    }

    return -1;
}

int http_version_str_to_v(const char *str) {
    int res = LHTTP_UNKOWN_VERSION;
    const char *version;

    if (str == NULL) {
        fprintf(stderr, "http_version_str_to_v failed, str is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (strncmp(LEADING_V_STR, str, sizeof(LEADING_V_STR) - 1)) {
        fprintf(stderr, "http_version_str_to_v failed, string is missing \"HTTP/\"\n");
        return LHTTP_STR_MALFORMED;
    }

    version = str + (sizeof(LEADING_V_STR) - 1);

    if (http_version_boundary_check(version) != 0) {
        fprintf(stderr, "http_version_str_to_v failed, string is malformed\n");
        return LHTTP_STR_MALFORMED;
    }

    if (!strcmp(version, V_1_1_STR)) {
        res = HTTP_VERSION_1_1;
    }
    else {
        fprintf(stderr, "http_version_str_to_v failed, unkown version\n");
    }

    return res;
}

const char* http_version_v_to_str(http_version_t v) {
    switch (v) {
        case HTTP_VERSION_1_1:
            return V_1_1_STR;
        
        default:
            fprintf(stderr, "http_version_v_to_str failed, unkown version\n");
            return NULL;
    }
}