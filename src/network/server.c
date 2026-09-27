#include <server.h>

#include <fcntl.h>
#include <arpa/inet.h>

#include "net_error.h"

/**
 * PRIVATE FUNCTIONS
 */

/**
 * \brief Attempts to accept incoming connection.
 * 
 * \param *ctx server context
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_UNKOWN_ERR | LNET_MAX_CON_REACHED | LNET_MEM_ERR
 */
static int _accept_connection(server_ctx_t *ctx) {
    int res = LNET_SUCCESS;

    if (ctx == NULL) {
        printf("_accept_connection failed, ctx is NULL\n");
        return LNET_STATIC_ERR;
    }

    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    int client_socket = accept(ctx->server_fd, (struct sockaddr *) &client_address, &client_size);
    if (client_socket < 0) {
        fprintf(stderr, "_accept_connection failed to accept client socket: %s\n", strerror(errno));
        return LNET_UNKNOWN_ERR;
    }

    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0) {
        fprintf(stderr, "_accept_connection failed to set socket to nonblocking: %s\n", strerror(errno));
        (void)close(client_socket);
        return LNET_UNKNOWN_ERR;
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, ip_str, sizeof(ip_str));

    printf("Incoming connection from: %s\n", ip_str);

    if ((res = pool_add_connection(&ctx->connections, client_socket)) < 0) {
        printf("_accept_connection failed, unable to add connection\n");
        (void)close(client_socket);
        return res;
    }

    printf("Currently connected: %ld\n", ctx->connections.len - 1);
    return res;
}

/**
 * \brief Handles incoming data on socket.
 * 
 * \param *ctx server context
 * \param idx index of connection in pool
 * 
 * \return \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_BUFFER_FULL | LNET_MEM_ERR | LNET_UNKNOWN_ERR | LNET_CON_CLOSED | LNET_WOULD_BLOCK | LNET_SLICE_LIM_HIT
 */
static int _incoming_data(server_ctx_t *ctx, size_t idx) {
    int fd = 0;
    int read_res = 0;
    int res = 0;
    size_t cur_len = 0;
    uint8_t data_received = 0;

    con_buffer_t *con_buf = NULL;

    if (ctx == NULL) {
        fprintf(stderr, "_incoming_data failed, ctx is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (idx == 0) {
        fprintf(stderr, "_incoming_data failed, idx is server socket\n");
        return LNET_UNKNOWN_ERR;
    }

    fd = ctx->connections.connections[idx].fd;
    con_buf = &ctx->connections.buffers[idx];
    cur_len = con_buf->data_in.len;

    read_res = con_read(&con_buf->data_in, fd, SERVER_READ_SLICE_LIM);
    data_received = cur_len != con_buf->data_in.len ? 1 : 0;

    char hello[] = "Hello World\n";
    res = con_buffer_append(&con_buf->data_out, hello, sizeof(hello));

    switch (read_res) {
        case LNET_SUCCESS:
        case LNET_SLICE_LIM_HIT:
        case LNET_WOULD_BLOCK:
        case LNET_CON_CLOSED:
            if (data_received == 1) {
                /**
                 * check if full message is received
                 */
                if (1) { // not full message received
                    printf("Sending response\n");
                    res = con_send(&con_buf->data_out, fd, SERVER_READ_SLICE_LIM);
                }
                else { // full message received

                }
            }

            if (read_res == LNET_CON_CLOSED) {
                printf("Connection closed\n");
                read_res = pool_del_connection(&ctx->connections, fd);
            }

            return read_res;

        case LNET_BUFFER_FULL:
            /**
             * TODO: send buffer is full
             */
            return pool_del_connection(&ctx->connections, fd);

        default:
            fprintf(stderr, "_incoming_data failed, con_read failed\n");
            return read_res;
    }
}

static int _handle_connections(server_ctx_t *ctx) {
    struct pollfd *tmp_connection;
    int res = LNET_SUCCESS;

    for (size_t i = 0; i < ctx->connections.len; i++) {
        tmp_connection = &ctx->connections.connections[i];

        if (!(tmp_connection->revents & POLLIN)) continue;

        /** Server */
        if (tmp_connection->fd == ctx->server_fd) {
            res = _accept_connection(ctx);

            if (res == LNET_SUCCESS || res == LNET_MAX_CON_REACHED) {
                continue;
            }
            else {
                fprintf(stderr, "server_run fatal error occured: %d\n", res);
                return LNET_FATAL;
            }
        } 
        /** Connections */
        else {
            res = _incoming_data(ctx, i);

            switch(res) {
                case LNET_SUCCESS:
                case LNET_WOULD_BLOCK:
                case LNET_SLICE_LIM_HIT:
                    continue;

                case LNET_CON_CLOSED:
                    i--;
                    continue;

                default:
                    return LNET_FATAL;
            }
        }
    }

    return res;
}

/**
 * PUBLIC FUNCTIONS
 */

int server_open(server_ctx_t *ctx, int port) {
    if (ctx == NULL) {
        fprintf(stderr, "server_open failed, ctx is NULL\n");
        return LNET_STATIC_ERR;
    }
    
    ctx->server_addr.sin_family = AF_INET;
    ctx->server_addr.sin_port = htons(port);
    ctx->server_addr.sin_addr.s_addr = INADDR_ANY;

    ctx->server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->server_fd < 0) {
        fprintf(stderr, "serv_open failed, unable to open the socket: %s\n", strerror(errno));
        goto cleanup;
    }
    
    int opt = 1;
    if (setsockopt(ctx->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "server_open failed, unable to configure the socket: %s\n", strerror(errno));
        goto cleanup;
    }

    if (bind(ctx->server_fd, (const struct sockaddr *) &(ctx->server_addr), sizeof(ctx->server_addr)) != 0) {
        fprintf(stderr, "server_open failed, unable to bind socket: %s\n", strerror(errno));
        goto cleanup;
    }

    if (listen(ctx->server_fd, POOL_MAX_CONNECTIONS) < 0) {
        fprintf(stderr, "server_open failed, unable to activate socket: %s\n", strerror(errno));
        goto cleanup;
    }

    if (pool_open(&ctx->connections) < 0) {
        fprintf(stderr, "server_open failed, unable to initialize the connection pool\n");
        goto cleanup;
    }

    return LNET_SUCCESS;

cleanup:
    if (ctx->server_fd >= 0) {
        close(ctx->server_fd);
    }

    return LNET_FATAL;
}

int server_close(server_ctx_t *ctx) {
    int res = LNET_SUCCESS;

    if (ctx == NULL) {
        fprintf(stderr, "server_close failed, ctx is NULL\n");
        return LNET_STATIC_ERR;
    }

    res = pool_close(&ctx->connections);
    if (res < 0) {
        fprintf(stderr, "server_close failed, unable to gracefully close connection pool: %d\n", res);
        return res;
    }

    return res;
}

int server_run(server_ctx_t *ctx) {
    int res;

    if (ctx == NULL) {
        printf("Failed to run server, ctx is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (pool_add_connection(&ctx->connections, ctx->server_fd) < 0) {
        printf("Failed to run server, unable to add server socket\n");
        return LNET_FATAL;
    }

    for (;;) {
        int ready = poll(ctx->connections.connections, ctx->connections.len, -1);

        if (ready < 0) {
            if (errno == EINTR) continue;
            else {
                printf("poll failed while running server: %s", strerror(errno));
                goto cleanup;
            }
        }

        if (ready == 0) continue;

        res = _handle_connections(ctx);

        if (res == LNET_FATAL) return res;
    }

cleanup:
    return server_close(ctx);
}
