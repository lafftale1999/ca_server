#ifndef LTALE_SERVER_H_
#define LTALE_SERVER_H_

#include "net_common.h"
#include "connection.h"

/**
 * \brief Server context with all necessary data.
 */
typedef struct server_ctx {
    int                         server_fd;          /** Server file description */
    int                         port;               /** Server port number */
    connection_pool_handle_t    connections;        /** Connection pool */
    struct sockaddr_in          server_addr;        /** Server address information */
} server_ctx_t;

/**
 * \brief Initialize server context
 * 
 * \param *ctx server context
 * \param port server port
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_FATAL
 */
int server_open(server_ctx_t *ctx, int port);

/**
 * \brief Graceful close of server
 * 
 * \param *ctx server context
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR
 */
int server_close(server_ctx_t *ctx);

/**
 * \brief Start server
 * 
 * \param *ctx server context
 * 
 * \return success == 0, failure < 0
 */
int server_run(server_ctx_t *ctx);

#endif
