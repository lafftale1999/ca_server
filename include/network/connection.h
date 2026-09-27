#ifndef LAFFTALE_CONNECTION_H_
#define LAFFTALE_CONNECTION_H_

#include "net_common.h"
#include "con_buffer.h"

/**
 * \brief Structure to handle connection pool.
 * 
 * \note Arrays are synchronized to be related by index number.
 */
typedef struct _connection_pool_handle_ {
    struct pollfd *connections;     /** Array of pollfd */
    con_buffer_t  *buffers;         /** Array of connection buffers */
    size_t         size;            /** Allocated size for arrays */
    size_t         len;             /** Amount of elements in arrays */
} connection_pool_handle_t;

/**
 * \brief Gracefully open connection pool
 * 
 * \param *pool pool context
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR
 */
int pool_open(connection_pool_handle_t *pool);

/**
 * \brief Gracefully close connection pool
 * 
 * \param *pool pool context
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR
 */
int pool_close(connection_pool_handle_t *pool);

/**
 * \brief Add connection to pool
 * 
 * \param *pool pool context
 * \param fd file descriptor to add
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_MAX_CON_REACHED | LNET_MEM_ERR
 */
int pool_add_connection(connection_pool_handle_t *pool, int fd);

/**
 * \brief Delete connection from pool
 * 
 * \param *pool pool context
 * \param fd file descriptor to delete
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_NOT_FOUND
 */
int pool_del_connection(connection_pool_handle_t *pool, int fd);

/**
 * \brief Find connection in pool based on file descriptor number
 * 
 * \param *pool pool context
 * \param fd file descriptor to find
 * 
 * \return idx if succesful | LNET_STATIC_ERR | LNET_NOT_FOUND
 */
int pool_find_connection(connection_pool_handle_t *pool, int fd);

/**
 * \brief Read data related to the connection buffer to the file descriptor
 * 
 * \param *con_buf connection buffer to store data in
 * \param fd file descriptor to read data from
 * \param slice_limit_bytes max bytes to read before switching context
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_BUFFER_FULL | LNET_MEM_ERR | LNET_UNKNOWN_ERR | LNET_CON_CLOSED | LNET_WOULD_BLOCK | LNET_SLICE_LIM_HIT
 */
int con_read(con_buffer_t *con_buf, int fd, size_t slice_limit_bytes);

/**
 * \brief Send data from connection buffer to file descriptor
 * 
 * \param *con_buf connection buffer containg data to send
 * \param fd file descriptor to send data to
 * \param slice_limit_bytes max bytes to send before switching context
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_CON_CLOSED | LNET_WOULD_BLOCK | LNET_UNKNOWN_ERR | LNET_SLICE_LIM_HIT
 */
int con_send(con_buffer_t *con_buf, int fd, size_t slice_limit_bytes);

#endif
