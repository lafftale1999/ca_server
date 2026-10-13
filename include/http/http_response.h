#ifndef LAFFTALE_HTTP_RESPONSE_H_
#define LAFFTALE_HTTP_RESPONSE_H_

#include "http_base_content.h"

typedef struct _http_response_ {
    http_version_t          version;
    http_response_code_t    response_code;
    http_headers_arr_t      headers;
    http_body_t             body;
} http_response_t;

#endif