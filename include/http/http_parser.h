#ifndef _LAFFTALE_HTTP_PARSER_H_
#define _LAFFTALE_HTTP_PARSER_H_

#include "http_request.h"
#include "http_response.h"

int http_parser_data_to_request(http_request_t *out, const char *data, size_t data_len);
int http_parser_response_to_data(http_response_t *in, const char *data, size_t *data_len);

#endif