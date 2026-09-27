#include <stdio.h>

#include "server.h"

int main(void) {
    server_ctx_t ctx;

    if (server_open(&ctx, 8080) != LNET_SUCCESS) {
        exit(1);
    }

    server_run(&ctx);

    return 0;
}