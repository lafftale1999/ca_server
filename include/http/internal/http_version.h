#ifndef _LAFFTALE_HTTP_VERSION_H_
#define _LAFFTALE_HTTP_VERSION_H_

#include "http_common.h"

typedef enum _http_version_ {
    HTTP_VERSION_1_1
} http_version_t;

int http_version_str_to_v(const char *str);
const char* http_version_v_to_str(http_version_t v);

#endif
