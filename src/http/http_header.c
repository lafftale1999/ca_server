#include "http_header.h"

/**
 * \brief Length restricted string tokenization
 * 
 * \param **pos pointer to position in string
 * \param *end pointer to end
 * \param *delims array of delimiter characters
 * \param **start_out pointer to actual start of token
 * \param *len_out pointer to where to store the start_out len
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR
 */
static int restricted_strtok(const char **pos,
                             const char *end,
                             const char *delims,
                             const char **start_out,
                             size_t     *len_out) {
    const char *p;

    if (pos == NULL || *pos == NULL) {
        fprintf(stderr, "restricted_strtok failed, pos is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (end == NULL) {
        fprintf(stderr, "restricted_strtok failed, end is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (delims == NULL) {
        fprintf(stderr, "restricted_strtok failed, delims is NULL\n");
        return LHTTP_STATIC_ERR;
    }
    
    if (len_out == NULL) {
        fprintf(stderr, "restricted_strtok failed, len_out is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    p = *pos;
    *len_out = 0;
    *start_out = NULL;

    while (p < end && strchr(delims, *p))
        p++;

    if (p == end) {
        return LHTTP_SUCCESS;
    }

    *start_out = p;

    while (p < end && !strchr(delims, *p))
        p++;
    
    *len_out = (size_t) (p - *start_out);
    *pos = p;
    
    return LHTTP_SUCCESS;
}

/**
 * \brief Allocates memory for the key and value
 * 
 * \param *out pointer to a header structure to populate
 * \param *key key
 * \param key_len length restriction for key
 * \param *val value
 * \param val_len length restriction for value
 * 
 * \return LHTTP_SUCCESS | LHTTP_STATIC_ERR | LHTTP_MEMORY_ERR
 */
static int http_header_allocate_key_val(http_header_t *out,
                                        const char *key, size_t key_len,
                                        const char *val, size_t val_len) {
    char *tmp_key;
    char *tmp_val;
    int res;

    if (out == NULL) {
        fprintf(stderr, "http_header_allocate_key_val failed, out is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (key == NULL) {
        fprintf(stderr, "http_header_allocate_key_val failed, key is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    if (val == NULL) {
        fprintf(stderr, "http_header_allocate_key_val failed, val is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    tmp_key = calloc(key_len, sizeof(char));
    tmp_val = calloc(val_len, sizeof(char));

    if (tmp_key == NULL || tmp_val == NULL) {
        fprintf(stderr, "http_header_allocate_key_val failed, unable to allocate memory\n");
        res = LHTTP_MEMORY_ERR;
        goto cleanup;
    }

    memcpy(tmp_key, key, key_len);
    memcpy(tmp_val, val, val_len);

    out->key = tmp_key;
    out->val = tmp_val;
    out->key_len = key_len;
    out->val_len = val_len;

    return LHTTP_SUCCESS;

cleanup:
    if (tmp_key != NULL) free(tmp_key);
    if (tmp_val != NULL) free(tmp_val);
    return res;
}

int http_header_parse_line_to_header(http_header_t *out, const char *line, size_t line_size) {
    int res;

    const char *tmp_key;
    size_t tmp_key_len;
    const char *tmp_val;
    size_t tmp_val_len;

    const char *pos;
    const char *end;

    if (line == NULL) {
        fprintf(stderr, "parse_line_to_header failed, in is NULL\n");
        return LHTTP_STATIC_ERR;
    }

    pos = line;
    end = line + line_size;

    res = restricted_strtok(&pos, end, ":", &tmp_key, &tmp_key_len);
    if (res != LHTTP_SUCCESS) {
        fprintf(stderr, "http_header_parse_line_to_header failed, unable to parse key\n");
        return res;
    }

    res = restricted_strtok(&pos, end, " :", &tmp_val, &tmp_val_len);
    if (res != LHTTP_SUCCESS) {
        fprintf(stderr, "http_header_parse_line_to_header failed, unable to parse value\n");
        return res;
    }
    
    res = http_header_allocate_key_val(out, tmp_key, tmp_key_len, tmp_val, tmp_val_len);
    if (res != LHTTP_SUCCESS) {
        fprintf(stderr, "http_header_parse_line_to_header failed, unable to allocate memory\n");
        return res;
    }

    return LHTTP_SUCCESS;
}
