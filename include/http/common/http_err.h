#ifndef LAFFTALE_HTTP_ERROR_H_
#define LAFFTALE_HTTP_ERROR_H_

#define LHTTP_SUCCESS                   0   /** HTTP succesfully handled */
#define LHTTP_STATIC_ERR                -1  /** Static error */
#define LHTTP_HEADER_TOO_LARGE          -2  /** Header size is bigger */
#define LHTTP_UNKOWN_METHOD             -3  /** Unkown method passed */
#define LHTTP_STR_MALFORMED             -4  /** String is malformed */
#define LHTTP_MEMORY_ERR                -5  /** Memory error */
#define LHTTP_UNKOWN_VERSION            -6  /** Unkown version */
#define LHTTP_NOT_FOUND                 -7  /** Not found */
#define LHTTP_REQUEST_INCOMPLETE        -8  /** Not received the request in full */

#endif