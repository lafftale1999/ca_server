#ifndef LAFFTALE_NETWORK_CONFIG_H_
#define LAFFTALE_NETWORK_CONFIG_H_

#define POOL_MAX_CONNECTIONS    100     /** Maximum of connections server can handle */
#define POOL_TMP_BUFFER_SIZE    512     /** Size of the temporary buffer when reading from socket */

#define SERVER_READ_SLICE_LIM   2048    /** Max bytes read / write before context switch */

#define CON_BYTE_BUF_INIT_SIZE 64       /** Connection buffers initial size */
#define CON_BYTE_BUF_MODIFIER  2        /** Modifier when increasing connection buffer size */
#define CON_BYTE_BUF_MAX_SIZE  2048     /** Max size of connection buffer data */

#endif