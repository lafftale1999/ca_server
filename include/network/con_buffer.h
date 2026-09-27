#ifndef LAFFTALE_CON_BUFFER_H_
#define LAFFTALE_CON_BUFFER_H_

#include "net_common.h"

/**
 * \brief Connection buffer containing the data buffer and
 * metadata regarding the structure
 * 
 * \note read and write position is updated with read / write operations.
 */
typedef struct _con_buffer {
    char   *buf;        /** Buffer containing data */
    char   *read_pos;   /** Read position in buffer */
    char   *write_pos;  /** Write position in buffer */
    size_t  size;       /** Allocated memory for buffer */
    size_t  len;        /** Initialized length of buffer */
} con_buffer_t;

/**
 * \brief Append data to buffer
 * 
 * \param *buf connection buffer to append data to
 * \param *data data to append
 * \param data_len length of data
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_UNKNOWN_ERR | LNET_MEM_ERR | LNET_BUFFER_FULL
 */
int con_buffer_append(con_buffer_t *buf, const char *data, size_t data_len);

/**
 * \brief Gracefully clear connection buffer
 * 
 * \param *buf connection buffer to clear
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR
 */
int con_buffer_clear(con_buffer_t *buf);

#endif