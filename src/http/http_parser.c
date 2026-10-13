#include "http_parser.h"

/**
 * \brief Find the end of headers, and beginning of body.
 */
static int http_parser_find_body(const char *pos, const char *end, const char **out) {
    while (pos < end) {
        if (*pos == '\r') {
            if ((pos + 4) > end) {
                fprintf(stderr, "http_parser_find_body failed, malformed data\n");
                return LHTTP_STR_MALFORMED;
            }

            if (*(pos + 1) == '\n' &&
                *(pos + 2) == '\r' &&
                *(pos + 3) == '\n') {
                    *out = pos + 4;
                    return LHTTP_SUCCESS;
                }
        }

        pos++;
    }

    return LHTTP_REQUEST_INCOMPLETE;
}

static int http_parser_next_line(const char *pos, const char *end, const char **out, const char **next_start) {
    /**
     * While position isn't end or after
     */
    while (pos < end) {
        /**
         * Check if the current char is \r
         */
        if (*pos == '\r') {
            /**
             * Check that the next char is \n
             */
            if (*(pos + 1) != '\n') {
                fprintf(stderr, "http_parser_next_line failed, data is malformed\n");
                return LHTTP_STR_MALFORMED;
            }

            /**
             * Set the out to position
             */
            *out = pos - 1;

            /** Check if there are more headers */
            if ((pos + 2) < end)
                *next_start = (pos + 2);
            else
                *next_start = NULL;

            return LHTTP_SUCCESS;
        }

        pos++;
    }

    return LHTTP_STR_MALFORMED;
}

static int http_parser_request_first_line(http_request_t *out, const char *line, size_t line_size) {
    const char delim = ' ';
    const char *pos = line;
    const char *end = pos + line_size;

    int res;
    
    char buf[512];
    size_t tmp_size;
    
    const char *pos_arr[3];
    size_t idx = 0;

    pos_arr[idx++] = pos;

    while (pos < end && idx < 3) {
        if (*pos == delim) {
            pos_arr[idx++] = (pos + 1);
        }
        pos++;
    }

    if (idx < 3) {
        fprintf(stderr, "http_parser_request_firs_line failed, line is malformed\n");
        return LHTTP_STR_MALFORMED;
    }

    /**
     * Add method
     */
    tmp_size = pos_arr[1] - pos_arr[0];
    memcpy(buf, pos_arr[0], tmp_size);
    buf[tmp_size - 1] = '\0';
    res = http_method_str_to_m(buf);
    if (res < 0) {
        fprintf(stderr, "http_parser_request_first_line failed, method is malformed\n");
        return res;
    }

    out->method = res;

    /**
     * Add path
     */
    tmp_size = pos_arr[2] - pos_arr[1];
    memcpy(buf, pos_arr[1], tmp_size);
    buf[tmp_size - 1] = '\0';
    res = http_path_str_to_p(&out->path, buf, tmp_size);
    if (res < 0) {
        fprintf(stderr, "http_parser_request_first_line failed, path is malformed\n");
        return res;
    }

    /**
     * Add version
     */
    tmp_size = end - pos_arr[2] + 1;
    memcpy(buf, pos_arr[2], tmp_size);
    buf[tmp_size] = '\0';
    printf("Version: %s\n", buf);
    res = http_version_str_to_v(buf);
    if (res < 0) {
        fprintf(stderr, "http_parser_request_first_line failed, version is malformed\n");
        (void)http_path_free(&out->path);
        return res;
    }

    out->version = res;

    return LHTTP_SUCCESS;
}

int http_parser_data_to_request(http_request_t *out, const char *data, size_t data_len) {
    const char *data_start, *data_end;
    const char *line_start, *new_line_start, *line_end;
    const char *body_start, *body_end;

    int res;
    
    if (out == NULL) {
        fprintf(stderr, "http_parser_data_to_request failed, out is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (data == NULL) {
        fprintf(stderr, "http_parser_data_to_request failed, data is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    data_start = data;
    data_end = data + data_len;

    /**
     * Find BODY
     */
    res = http_parser_find_body(data_start, data_end, &body_start);
    if (res < 0) {
        fprintf(stderr, "http_parser_data_to_request failed, request is malformed\n");
        return res;
    }

    body_end = data_end;

    /**
     * Parse First Line
     */
    line_start = data_start;
    res = http_parser_next_line(line_start, body_start, &line_end, &new_line_start);
    if (res < 0) {
        fprintf(stderr, "http_parser_data_to_request failed, unable to parse first line\n");
        goto cleanup;
    }

    res = http_parser_request_first_line(out, line_start, (size_t)(line_end - line_start));
    if (res < 0) {
        fprintf(stderr, "http_parser_data_to_request failed, unable to initialize first line\n");
        goto cleanup;
    }
    
    printf("First line variables:\n");
    printf("Method: %s\n", http_method_m_to_str(out->method));
    printf("Path: %s\n", out->path.path);
    printf("Version: %s\n", http_version_v_to_str(out->version));

    int count = 0;

    while (new_line_start != NULL && new_line_start < body_start) {
        printf("Count=%d\n", count++);
        http_header_t tmp = {0};
        line_start = new_line_start;
        res = http_parser_next_line(line_start, body_start, &line_end, &new_line_start);

        const char *tmp_pointer = line_start;

        printf("----------\n");
        while (tmp_pointer <= line_end) {
            printf(".");
            printf("%c", *tmp_pointer);
            tmp_pointer++;
        }

        printf("\n");
        res = http_header_parse_line_to_header(&tmp, line_start, (size_t)(line_end - line_start + 1));
        if (res < 0) {
            fprintf(stderr, "http_parser_data_to_request failed, unable to parse line to header\n");
            goto cleanup;
        }

        res = http_headers_arr_append(&out->headers, &tmp);
        if (res < 0) {
            fprintf(stderr, "http_parser_data_to_request failed, unable to add header\n");
            goto cleanup;
        }

        
    }

    for (size_t i = 0; i < out->headers.len; i++) {
        printf("%s: %s\n", out->headers.headers[i].key, out->headers.headers[i].val);
    }

    return LHTTP_SUCCESS;

cleanup:
    if (out != NULL) {
        http_request_free(out);
    }

    return res;
}
