#include <stdio.h>

#include "server.h"

int main(void) {
    server_ctx_t ctx;

    server_init(&ctx, 8080, 100);
    return 0;
}