#ifndef _LAFFTALE_HTTP_HEADER_H_
#define _LAFFTALE_HTTP_HEADER_H_


#include "http_common.h"

/**
 * --------------------------------------------------------------------------------------------------
 * SINGLE HEADER
 * --------------------------------------------------------------------------------------------------
 */

/**
 * http_header structure
 */
typedef struct _http_header_ {
    char    *key;
    size_t   key_len;
    char    *val;
    size_t   val_len;
} http_header_t;

/**
 * \brief Parse "\r\n" terminated line into a http_header_t
 * 
 * \param *out header to initialize
 * \param *line data to fill header
 * 
 * \return 
 */
int http_header_parse_line_to_header(http_header_t *out, const char *line, size_t line_size);

/**
 * \brief Destroy initialized http_header_t
 * 
 * \param *header header to destroy
 * 
 * \return 
 */
int http_header_free(http_header_t *header);

/**
 * --------------------------------------------------------------------------------------------------
 * ARRAY OF HEADERS
 * --------------------------------------------------------------------------------------------------
 */

/**
 * \brief Array of http headers
 */
typedef struct _http_headers_ {
    http_header_t  *headers;    /** Array of headers */
    size_t          len;        /** Length of array */
    size_t          size;       /** Size of allocated memory */
} http_headers_arr_t;

/**
 * \brief Append headers to array
 * 
 * \param *headers array of headers
 * \param h header to append
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR
 */
int http_headers_arr_append(http_headers_arr_t *headers, http_header_t h);

/**
 * \brief Find related HTTP header
 * 
 * \param *headers array of headers
 * \param *target string target of header (case sensitive)
 * 
 * \return idx for header in array. Returns < 0 if unsuccesful
 */
int http_headers_arr_find(http_headers_arr_t *headers, const char *target);

/**
 * \brief Free array of http headers
 * 
 * \param *headers array of headers
 */
int http_headers_arr_free(http_headers_arr_t *headers);

#endif
