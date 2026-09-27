#ifndef LAFFTALE_NETWORK_ERROR_H_
#define LAFFTALE_NETWORK_ERROR_H_

#define LNET_SUCCESS             0  /** Network action succeeded */
#define LNET_WOULD_BLOCK        -1  /** Network action would block */
#define LNET_CON_CLOSED         -2  /** Connection closed */
#define LNET_BUFFER_FULL        -3  /** Connection buffer is full */
#define LNET_MEM_ERR            -4  /** Memory error */
#define LNET_STATIC_ERR         -5  /** Static error */
#define LNET_UNKNOWN_ERR        -6  /** Unkown state reached */
#define LNET_SLICE_LIM_HIT      -7  /** Slice byte limit hit */
#define LNET_MAX_CON_REACHED    -8  /** Max connections reached */
#define LNET_NOT_FOUND          -9  /** Unable to locate target */
#define LNET_FATAL              -10 /** Fatal error, unable to recover */

#endif