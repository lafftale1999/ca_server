#ifndef _LAFFTALE_HTTP_PATH_H_
#define _LAFFTALE_HTTP_PATH_H_

#include "http_common.h"

/**
 * \brief Small structure for handling path
 */
typedef struct _http_path_ {
    char *path;       /** Path string */
    size_t size;            /** Path string size */
} http_path_t;

/**
 * \brief Parse string into a path variable
 * 
 * \param *out path to initialize
 * \param *in path string
 * \param in_len length of string
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR | LHTTP_MEMORY_ERR
 */
int http_path_str_to_p(http_path_t *out, const char *in, size_t in_len);

/**
 * \brief Free HTTP path
 * 
 * \param *path to free
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR
 */
int http_path_free(http_path_t *path);

#endif
