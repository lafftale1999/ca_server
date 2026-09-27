#include "connection.h"
#include "net_error.h"

#define CON_POOL_INIT_SIZE 10
#define CON_POOL_MODIFIER  2

/**
 * \brief Allocates memory for connections in connection pool
 * 
 * \param *pool connection pool
 * \param new_size new size of array
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_MEM_ERR
 */
static int _realloc_connections(connection_pool_handle_t *pool, size_t new_size) {
    struct pollfd *tmp_arr;

    if (pool == NULL) {
        fprintf(stderr, "_realloc_connections failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    tmp_arr = realloc(pool->connections, (sizeof(struct pollfd) * new_size));
    if (tmp_arr == NULL) {
        fprintf(stderr, "_realloc_connections failed to reallocate memory: %s\n", strerror(errno));
        return LNET_MEM_ERR;
    }

    pool->connections = tmp_arr;

    return LNET_SUCCESS;
}

/**
 * \brief Allocates memory for connection buffers in connection pool
 * 
 * \param *pool connection pool
 * \param new_size new size of array
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_MEM_ERR
 */
static int _realloc_buf_struct(connection_pool_handle_t *pool, size_t new_size) {
    con_buffer_t *tmp_arr;

    if (pool == NULL) {
        fprintf(stderr, "_realloc_buf_struct failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    tmp_arr = realloc((pool->buffers), (sizeof(con_buffer_t) * new_size));
    if (tmp_arr == NULL) {
        fprintf(stderr, "_realloc_buf_struct failed, unable to reallocate memory: %s\n", strerror(errno));
        return LNET_MEM_ERR;
    }

    pool->buffers = tmp_arr;

    return LNET_SUCCESS;
}

int pool_open(connection_pool_handle_t *pool) {
    if (pool == NULL) {
        fprintf(stderr, "pool_open failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    pool->len = 0;
    pool->size = 0;
    pool->buffers = NULL;
    pool->connections = NULL;

    return LNET_SUCCESS;
}

int pool_close(connection_pool_handle_t *pool) {
    if (pool == NULL) {
        fprintf(stderr, "pool_close failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (pool->connections != NULL && pool->buffers != NULL) {
        for (size_t idx = 0; idx < pool->len; idx++) {
            int fd = pool->connections[idx].fd;
            con_buffer_t *con_buf = &pool->buffers[idx];

            if (close(fd) < 0) {
                fprintf(stderr, "pool_close failed to close socket %d: %s", fd, strerror(errno));
            }
            
            if (con_buffer_clear(con_buf) < 0) {
                fprintf(stderr, "pool_close failed to clear buffer for socket %d", fd);
            }
        }
    }

    free(pool->connections);
    free(pool->buffers);

    return LNET_SUCCESS;
}

int pool_find_connection(connection_pool_handle_t *pool, int target) {
    if (pool == NULL) {
        fprintf(stderr, "pool_find_connection failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    for (size_t idx = 0; idx < pool->len; idx++) {
        if (pool->connections[idx].fd == target) {
            return idx;
        }
    }

    return LNET_NOT_FOUND;
}

int pool_add_connection(connection_pool_handle_t *connections, int connection) {
    if (connections == NULL) {
        fprintf(stderr, "pool_add_connection failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    if ((connections->len + 1) >= POOL_MAX_CONNECTIONS) {
        fprintf(stderr, "pool_add_connection failed, max connections reached\n");
        return LNET_MAX_CON_REACHED;
    }

    if (connections->len >= connections->size || connections->len >= connections->size) {
        size_t new_size = connections->size > 0 ? connections->size * CON_POOL_MODIFIER : CON_POOL_INIT_SIZE;

        if (new_size >= POOL_MAX_CONNECTIONS) {
            new_size = POOL_MAX_CONNECTIONS;
        }

        if (_realloc_connections(connections, new_size) < 0) {
            fprintf(stderr, "pool_add_connection failed because of _realloc_connections\n");
            return LNET_MEM_ERR;
        }

        if (_realloc_buf_struct(connections, new_size) < 0) {
            fprintf(stderr, "pool_add_connection failed because of _realloc_buf_struct\n");
            return LNET_MEM_ERR;
        }

        connections->size = new_size;
    }

    connections->buffers[connections->len] = (con_buffer_t){0};
    connections->connections[connections->len] = (struct pollfd) {
        .fd = connection,
        .events = POLLIN,
        .revents = 0
    };

    connections->len++;

    return LNET_SUCCESS;
}

int pool_del_connection(connection_pool_handle_t *pool, int fd) {
    int res = LNET_SUCCESS;

    if (pool == NULL) {
        fprintf(stderr, "pool_del_connection failed, pool is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (pool->len == 0) {
        fprintf(stderr, "pool_del_connection failed, tried to remove elements from an empty array\n");
        return LNET_STATIC_ERR;
    }

    int idx = pool_find_connection(pool, fd);
    if (idx < 0) {
        fprintf(stderr, "pool_del_connection failed, unable to locate connection\n");
        return idx;
    }

    if (close(pool->connections[idx].fd) < 0) {
        fprintf(stderr, "pool_del_connection: error while closing socket: %s\n", strerror(errno));
    }

    if ((res = con_buffer_clear(&pool->buffers[idx])) < 0) {
        fprintf(stderr, "pool_del_connection: error while clearing buffer\n");
        return res;
    }

    for (size_t i = idx; i + 1 < pool->len; i++) {
        pool->connections[i] = pool->connections[i + 1];
        pool->buffers[i] = pool->buffers[i + 1];
    }

    pool->len--;
    pool->buffers[pool->len] = (con_buffer_t){0};

    return LNET_CON_CLOSED;
}

int con_read(con_buffer_t *con_buf, int fd, size_t slice_limit_bytes) {
    size_t        tot_bytes_read = 0;
    char          tmp_buf[POOL_TMP_BUFFER_SIZE] = {0};
    int           res = 0;

    if (con_buf == NULL) {
        fprintf(stderr, "con_read failed, con_buf is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (con_buf->size >= CON_BYTE_BUF_MAX_SIZE || slice_limit_bytes == 0) {
        fprintf(stderr, "con_read failed, buffer is full\n");
        return LNET_BUFFER_FULL;
    }

    while(tot_bytes_read < slice_limit_bytes) {
        memset(tmp_buf, 0, sizeof(tmp_buf));

        ssize_t bytes_read = recv(fd, tmp_buf, sizeof(tmp_buf), 0);

        if (bytes_read > 0) {
            res = con_buffer_append(con_buf, tmp_buf, (size_t) bytes_read);
            if (res < 0) {
                fprintf(stderr, "con_read failed, unable to append to buffer\n");
                return res;
            }
            tot_bytes_read += bytes_read;
            
            continue;
        }
        else if (bytes_read == 0) {
            return LNET_CON_CLOSED;
        }
        else if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return LNET_WOULD_BLOCK;
        }
        else if (errno == EINTR) {
            continue;
        }
        else {
            fprintf(stderr, "con_read failed: %s", strerror(errno));
            return LNET_UNKNOWN_ERR;
        }
    }

    return LNET_SLICE_LIM_HIT;
}

int con_send(con_buffer_t *con_buf, int fd, size_t slice_limit_bytes) {
    size_t        bytes_already_sent = 0;
    size_t        tot_bytes_sent = 0;
    size_t        bytes_to_send = 0;

    if (con_buf == NULL) {
        fprintf(stderr, "con_send failed, con_buf is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (con_buf->buf == NULL) {
        fprintf(stderr, "con_send failed, buffer is NULL");
        return LNET_STATIC_ERR;
    }

    bytes_already_sent = con_buf->read_pos - con_buf->buf;
    bytes_to_send = con_buf->len - bytes_already_sent;

    while (tot_bytes_sent < slice_limit_bytes) {
        if (bytes_to_send == 0) return LNET_SUCCESS; 
        ssize_t bytes_sent = send(fd, con_buf->read_pos, bytes_to_send, 0);

        if (bytes_sent > 0) {
            tot_bytes_sent += bytes_sent;
            con_buf->read_pos += bytes_sent;
            bytes_to_send -= bytes_sent;
            continue;
        }
        else if (bytes_sent == 0) {
            return LNET_CON_CLOSED;
        }
        else if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return LNET_WOULD_BLOCK;
        }
        else if (errno == EINTR) {
            continue;
        }
        else {
            fprintf(stderr, "con_send failed unexpectedly: %s\n", strerror(errno));
            return LNET_UNKNOWN_ERR;
        }
    }

    return LNET_SLICE_LIM_HIT;
}
