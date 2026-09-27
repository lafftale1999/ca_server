#include "con_buffer.h"
#include "net_error.h"

/**
 * PRIVATE FUNCTIONS
 */

/**
 * \brief Allocates memory for data buffer
 * 
 * \param *buf connection buffer to allocate memory for
 * 
 * \return LNET_SUCCESS | LNET_STATIC_ERR | LNET_BUFFER_FULL | LNET_MEM_ERR
 */
static int _realloc_str_buf(con_buffer_t *buf) {
    char  *tmp_arr;
    size_t tmp_size;
    size_t read_off = buf->read_pos ? (size_t) (buf->read_pos - buf->buf) : 0;

    if (buf == NULL) {
        fprintf(stderr, "_realloc_str_buf failed, buf is NULL\n");
        return LNET_STATIC_ERR;
    }

    tmp_arr = NULL;
    tmp_size = buf->size > 0 ? buf->size * CON_BYTE_BUF_MODIFIER : CON_BYTE_BUF_INIT_SIZE;

    if (tmp_size > CON_BYTE_BUF_MAX_SIZE) {
        fprintf(stderr, "_realloc_str_buf failed, buffer reached max capacity\n");
        return LNET_BUFFER_FULL;
    }

    tmp_arr = realloc(buf->buf, (sizeof(*buf->buf) * tmp_size));
    if (tmp_arr == NULL) {
        fprintf(stderr, "_realloc_str_buf failed, unable to reallocate memory: %s\n", strerror(errno));
        return LNET_MEM_ERR;
    }

    buf->buf = tmp_arr;
    buf->read_pos = tmp_arr + read_off;
    buf->write_pos = tmp_arr + buf->len;
    buf->size = tmp_size;

    return LNET_SUCCESS;
}

/**
 * PUBLIC FUNCTIONS
 */

int con_buffer_append(con_buffer_t *buf, const char *data, size_t data_len) {
    size_t new_len = 0;
    int res = 0;

    if (buf == NULL) {
        fprintf(stderr, "con_buffer_append failed: buf is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (data == NULL) {
        fprintf(stderr, "con_buffer_append failed: data is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (data_len == 0) {
        fprintf(stderr, "con_buffer_append failed: data_len is 0\n");
        return LNET_UNKNOWN_ERR;
    }

    new_len = buf->len + data_len;

    if (new_len > CON_BYTE_BUF_MAX_SIZE) {
        fprintf(stderr, "con_buffer_append failed: buffer reached max capacity\n");
        return LNET_BUFFER_FULL;
    }

    while(new_len > buf->size) {
        if ((res = _realloc_str_buf(buf)) < 0) {
            fprintf(stderr, "con_buffer_append failed: unable to reallocate string buffer\n");
            return res;
        }
    }

    memcpy(buf->write_pos, data, data_len);
    buf->len = new_len;
    buf->write_pos = buf->buf + buf->len;

    return LNET_SUCCESS;
}

int con_buffer_clear(con_buffer_t *buf) {
    if (buf == NULL) {
        fprintf(stderr, "con_buffer_clear failed: buf is NULL\n");
        return LNET_STATIC_ERR;
    }

    if (buf->buf != NULL) {
        free(buf->buf);
    }

    *buf = (con_buffer_t){0};

    return LNET_SUCCESS;
}