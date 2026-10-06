#ifndef _LAFFTALE_HTTP_METHOD_H_
#define _LAFFTALE_HTTP_METHOD_H_

#include "http_common.h"

#define HTTP_METHOD_MAX_STR_SIZE    sizeof("DELETE")

typedef enum _http_method_ {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
} http_method_t;

int http_method_str_to_m(const char *str);

const char* http_method_m_to_str(http_method_t m);

#endif