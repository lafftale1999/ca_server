#ifndef _LAFFTALE_HTTP_RESPONSE_CODE_H_
#define _LAFFTALE_HTTP_RESPONSE_CODE_H_

#include "http_common.h"

typedef struct _http_response_code_ {
    const char *str;
    int         num;
} http_response_code_t;

/**
 * \brief Initialize a http_response_code with a string
 * 
 * \param *out response code to initialize
 * \param *in response code in string
 * \param in_len response code string length
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR | LHTTP_NOT_FOUND
 */
int http_response_code_parse_str(http_response_code_t *out, const char *in, size_t in_len);

/**
 * \brief Initialize a http_response_code with a number
 * 
 * \param *out response code to initialize
 * \param num response code number
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR | LHTTP_NOT_FOUND
 */
int http_response_code_parse_num(http_response_code_t *out, int num);

#endif
