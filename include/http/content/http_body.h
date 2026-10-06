#ifndef _LAFFTALE_HTTP_BODY_H_
#define _LAFFTALE_HTTP_BODY_H_

#include "http_common.h"

typedef struct _http_body_ {
    char    *data;
    size_t   size;
} http_body_t;

int http_body_initialize(http_body_t *out, const char *data_in, size_t limit);

int http_body_free(http_body_t *body);

#endif
