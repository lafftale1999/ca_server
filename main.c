#include <stdio.h>

#include "server.h"
#include "http_parser.h"

int main(void) {
    /* server_ctx_t ctx;

    if (server_open(&ctx, 8080) != LNET_SUCCESS) {
        exit(1);
    }

    server_run(&ctx); */
    http_request_t req = {0};

    const char data[] = \
        "GET / HTTP/1.1\r\n" \
        "Content-Type: application/json\r\n" \
        "Content-Length: 123\r\n" \
        "\r\n" \
        "{\"message\": \"Hello World\"}\r\n"; 

    http_parser_data_to_request(&req, data, sizeof(data));

    return 0;
}