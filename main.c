#include <stdio.h>

#include "server.h"
#include "lhttp_parser.h"

int main(void) {
    /* server_ctx_t ctx;

    if (server_open(&ctx, 8080) != LNET_SUCCESS) {
        exit(1);
    }

    server_run(&ctx); */
    int res = 0;
    http_request_t req = {0};

    const char data[] = \
        "POST /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Content-Length: 26\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\"message\": \"Hello World\"}"
        "\r\n";

    if ((res = http_request_from_data(&req, data, sizeof(data) - 1)) != LHTTP_SUCCESS) {
        printf("Unable to parse request: %d\n", res);
        return 1;
    }
    
    printf("Body:\n");
    printf("%s\n", req.body.data);

    return 0;
}