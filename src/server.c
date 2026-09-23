#include <server.h>

#include <fcntl.h>
#include <arpa/inet.h>

static int _pool_calloc(connection_pool_handle_t *pool) {
    if (pool == NULL) {
        printf("_pool_calloc failed, pool is NULL\n");
        return -1;
    }

    int *tmp_arr = NULL;
    size_t tmp_size = pool->size > 0 ? pool->size * 2 : 10;

    tmp_arr = realloc(pool->connections, (sizeof(struct pollfd) * tmp_size));
    if (tmp_arr == NULL) {
        printf("_pool_calloc failed to reallocate memory: %s\n", strerror(errno));
        return -1;
    }

    pool->connections = tmp_arr;
    pool->size = tmp_size;

    return 0;
}

static int _add_connection(server_ctx_t *ctx, int connection) {
    if (ctx == NULL) {
        printf("_add_connection failed, ctx is NULL\n");
        return -1;
    }

    if (ctx->connections.len >= ctx->max_connections) {
        printf("_add_connection failed, max connections reached\n");
        return -1;
    }
    
    if (ctx->connections.len >= ctx->connections.size) {
        if (_pool_calloc(&ctx->connections) < 0) {
            printf("_add_connection failed because of _pool_calloc\n");
            return -1;
        }
    }

    ctx->connections.connections[ctx->connections.len].fd = connection;
    ctx->connections.connections[ctx->connections.len].events = POLLIN;
    ctx->connections.connections[ctx->connections.len].revents = 0;
    
    ctx->connections.len++;

    return 0;
}

static int _find_connection(server_ctx_t *ctx, int connection) {
    if (ctx == NULL) {
        printf("_find_connection failed, ctx is NULL\n");
        return -1;
    }

    for (size_t idx = 0; idx < ctx->connections.len; idx++) {
        if (ctx->connections.connections[idx].fd == connection) {
            return idx;
        }
    }

    return -1;
}

static int _remove_connection(server_ctx_t *ctx, int connection) {
    if (ctx == NULL) {
        printf("_remove_connection failed, ctx is NULL\n");
        return -1;
    }

    if (ctx->connections.len == 0) {
        printf("_remove_connection failed, tried to remove elements from an empty array\n");
        return -1;
    }

    int idx = _find_connection(ctx, connection);
    if (idx < 0) {
        printf("_remove_connection failed, unable to locate connection\n");
        return -1;
    }

    if (close(ctx->connections.connections[idx].fd) < 0) {
        printf("_remove_connection failed, unable to close socket: %s\n", strerror(errno));
        return -1;
    }

    for (size_t i = ctx->connections.len - 1; i > (size_t) idx; i--) {
        ctx->connections.connections[i - 1] = ctx->connections.connections[i];
    }

    ctx->connections.len--;

    return 0;
}

static int _accept_connection(server_ctx_t *ctx) {
    if (ctx == NULL) {
        printf("_accept_connection failed, ctx is NULL\n");
        return -1;
    }

    struct sockaddr_in client_address;
    socklen_t client_size = sizeof(client_address);

    int client_socket = accept(ctx->server_fd, (struct sockaddr *) &client_address, &client_size);
    if (client_socket < 0) {
        printf("_accept_connection failed to accept client socket: %s\n", strerror(errno));
        return -1;
    }

    if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0) {
        printf("_accept_connection failed to set socket to nonblocking: %s\n", strerror(errno));
        (void)close(client_socket);
        return -1;
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_address.sin_addr, ip_str, sizeof(ip_str));

    printf("Incoming connection from: %s\n", ip_str);

    if (_add_connection(ctx, client_socket) < 0) {
        printf("_accept_connection failed, unable to add connection\n");
        (void)close(client_socket);
        return -1;
    }

    return 0;
}

int server_init(server_ctx_t *ctx, int port, size_t max_connections) {
    if (ctx == NULL) {
        printf("No ctx to initializen\n");
        return -1;
    }
    
    ctx->server_addr.sin_family = AF_INET;
    ctx->server_addr.sin_port = htons(port);
    ctx->server_addr.sin_addr.s_addr = INADDR_ANY;
    
    ctx->max_connections = max_connections;

    ctx->server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ctx->server_fd < 0) {
        printf("Failed to open the socket: %s\n", strerror(errno));
        goto cleanup;
    }
    
    int opt = 1;
    if (setsockopt(ctx->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        printf("Failed to configure the socket: %s\n", strerror(errno));
        goto cleanup;
    }

    if (bind(ctx->server_fd, (const struct sockaddr *) &(ctx->server_addr), sizeof(ctx->server_addr)) != 0) {
        printf("Failed to bind socket: %s\n", strerror(errno));
        goto cleanup;
    }

    if (listen(ctx->server_fd, ctx->max_connections) < 0) {
        printf("Failed to activate socket: %s\n", strerror(errno));
        goto cleanup;
    }

    ctx->connections.connections = NULL;
    ctx->connections.len = 0;
    ctx->connections.size = 0;

    return 0;

cleanup:
    if (ctx->server_fd >= 0) {
        close(ctx->server_fd);
    }

    return -1;
}

void server_run(server_ctx_t *ctx) {
    if (ctx == NULL) {
        printf("Failed to run server, ctx is NULL\n");
        return;
    }

    if (_add_connection(ctx, ctx->server_fd) < 0) {
        printf("Failed to run server, unable to add server socket\n");
        return;
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

        for (size_t i = 0; i < ctx->connections.len; i++) {
            if (!(ctx->connections.connections[i].revents & POLLIN)) continue;

            if (ctx->connections.connections[i].fd == ctx->server_fd) {
                _accept_connection(ctx);
            } else {
                /**
                 * Incoming data
                 */
            }
        }
    }

cleanup:
    /** Free memory and close all file descriptors */
}
