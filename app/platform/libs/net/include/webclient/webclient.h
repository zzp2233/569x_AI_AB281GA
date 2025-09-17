/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-05-05     Bernard      the first version
 * 2013-06-10     Bernard      fix the slow speed issue when download file.
 * 2015-11-14     aozima       add content_length_remainder.
 * 2017-12-23     aozima       update gethostbyname to getaddrinfo.
 * 2018-01-04     aozima       add ipv6 address support.
 * 2018-07-26     chenyong     modify log information
 * 2018-08-07     chenyong     modify header processing
 */

#ifndef __WEBCLIENT_H__
#define __WEBCLIENT_H__

#include <stddef.h>

//#define WEBCLIENT_USING_MBED_TLS
#if defined(WEBCLIENT_USING_MBED_TLS) || defined(WEBCLIENT_USING_SAL_TLS)
#include <tls_client.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */

#define RT_TRUE true
#define RT_FALSE false

#define LOG_D(...) //my_printf(__VA_ARGS__);my_printf("\n")
#define LOG_I(...) //my_printf(__VA_ARGS__);my_printf("\n")
#define LOG_E(...) //my_printf(__VA_ARGS__);my_printf("\n")

#define rt_kprintf(...) //my_printf(__VA_ARGS__)

#define RT_ASSERT(n)                                                         \
    do {                                                                       \
        if (!(n)) {                                                            \
            my_printf("Assert at File(%s), Line(%d)!", __FILE__, __LINE__);       \
            while (1)                                                          \
                ;                                                              \
        }                                                                      \
    } while (0)


void *webclient_malloc(size_t size);
void *webclient_calloc(size_t nitems, size_t size);
void *webclient_realloc(void *p, size_t new_size);
void webclient_free(void *p);

/**
 * The gcc libc api is not threadsafe,
 * especially the float type operation.
 * So, use rt_xxx whose RT-Thread threadsafe api to instead of strandard libc api.
 */
#ifndef web_memset
#define web_memset                      memset
#endif

#ifndef web_memcpy
#define web_memcpy                      memcpy
#endif

#ifndef web_memcmp
#define web_memcmp                      memcmp
#endif

#ifndef web_snprintf
#define web_snprintf                    snprintf
#endif

#ifndef web_vsnprintf
#define web_vsnprintf                   vsnprintf
#endif

#ifndef web_strdup
#define web_strdup                      strdup
#endif

#define WEBCLIENT_SW_VERSION           "2.3.0"
#define WEBCLIENT_SW_VERSION_NUM       0x20300

#define WEBCLIENT_HEADER_BUFSZ         4096
#define WEBCLIENT_RESPONSE_BUFSZ       4096

enum WEBCLIENT_STATUS
{
    WEBCLIENT_OK,
    WEBCLIENT_ERROR,
    WEBCLIENT_TIMEOUT,
    WEBCLIENT_NOMEM,
    WEBCLIENT_NOSOCKET,
    WEBCLIENT_NOBUFFER,
    WEBCLIENT_CONNECT_FAILED,
    WEBCLIENT_DISCONNECT,
    WEBCLIENT_FILE_ERROR,
};

enum WEBCLIENT_METHOD
{
    WEBCLIENT_USER_METHOD,
    WEBCLIENT_GET,
    WEBCLIENT_POST,
    WEBCLIENT_HEAD
};

struct  webclient_header
{
    char *buffer;
    size_t length;                      /* content header buffer size */

    size_t size;                        /* maximum support header size */
};

struct webclient_session
{
    struct webclient_header *header;    /* webclient response header information */
    int socket;
    int resp_status;

    char *host;                         /* server host */
    char *req_url;                      /* HTTP request address*/

    int chunk_sz;
    int chunk_offset;

    int content_length;
    size_t content_remainder;           /* remainder of content length */
    int (*handle_function)(char *buffer, int size); /* handle function */

#ifdef WEBCLIENT_USING_MBED_TLS
    bool is_tls;                   /* HTTPS connect */
    MbedTLSSession *tls_session;        /* mbedtls connect session */
#endif
};

/* create webclient session and set header response size */
struct webclient_session *webclient_session_create(size_t header_sz);

/* send HTTP GET request */
int webclient_get(struct webclient_session *session, const char *URI);

/* send HTTP HEAD request */
int webclient_shard_head_function(struct webclient_session *session, const char *URI, int *length);

/* send HTTP Range parameter, shard download */
int webclient_shard_position_function(struct webclient_session *session, const char *URI, int start, int length, int mem_size);
int *webclient_register_shard_position_function(struct webclient_session *session, int (*handle_function)(char *buffer, int size));

/* send HTTP POST request */
int webclient_post(struct webclient_session *session, const char *URI, const void *post_data, size_t data_len);

/* close and release wenclient session */
int webclient_close(struct webclient_session *session);

int webclient_set_timeout(struct webclient_session *session, int millisecond);

/* send or receive data from server */
int webclient_read(struct webclient_session *session, void *buffer, size_t size);
int webclient_write(struct webclient_session *session, const void *buffer, size_t size);

/* webclient GET/POST header buffer operate by the header fields */
int webclient_header_fields_add_valist(struct webclient_session *session, const char *fmt, va_list args);
int webclient_header_fields_add(struct webclient_session *session, const char *fmt, ...);
const char *webclient_header_fields_get(struct webclient_session *session, const char *fields);

/* send HTTP POST/GET request, and get response data */
int webclient_response(struct webclient_session *session, void **response, size_t *resp_len);
int webclient_request(const char *URI, const char *header, const void *post_data, size_t data_len, void **response, size_t *resp_len);
int webclient_request_header_add(char **request_header, const char *fmt, ...);
int webclient_resp_status_get(struct webclient_session *session);
int webclient_content_length_get(struct webclient_session *session);

#ifdef RT_USING_DFS
/* file related operations */
int webclient_get_file(const char *URI, const char *filename);
int webclient_post_file(const char *URI, const char *filename, const char *form_data);
#endif

#ifdef  __cplusplus
}
#endif

#endif
