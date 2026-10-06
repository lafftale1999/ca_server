#ifndef _LAFFTALE_HTTP_BODY_H_
#define _LAFFTALE_HTTP_BODY_H_

#include "http_common.h"

/**
 * \brief Structure for HTTP body
 */
typedef struct _http_body_ {
    char    *data;      /** Data */
    size_t   size;      /** Size of data */
} http_body_t;

/**
 * \brief Initialize a HTTP body
 *
 * \param *out pointer to structure to initialize
 * \param *data_in data to add to body
 * \param limit maximum size of data
 *
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR | LHTTP_MEMORY_ERR
 *
 * \note This will allocate memory on the heap, use `http_body_free`.
 */
int http_body_initialize(http_body_t *out, const char *data_in, size_t limit);

/**
 * \brief Free an initialized HTTP body
 *
 * \param *body body to free
 *
 * \return LHTTP_SUCCES | LHTTP_STATIC_ERR
 */
int http_body_free(http_body_t *body);

#endif
