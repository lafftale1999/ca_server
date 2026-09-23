#ifndef LTALE_SERVER_H_
#define LTALE_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <stdint.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#include <signal.h>
#include <time.h>


typedef struct _connection_pool_handle_ {
    struct pollfd *connections;
    size_t         size;
    size_t         len;
} connection_pool_handle_t;

typedef struct server_ctx {
    int                         server_fd;
    int                         port;
    size_t                      max_connections;
    connection_pool_handle_t    connections;
    struct sockaddr_in          server_addr;
} server_ctx_t;

int server_init(server_ctx_t *ctx, int port, size_t max_connections);
void server_run(server_ctx_t *ctx);

#endif
