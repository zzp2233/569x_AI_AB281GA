#include "include.h"

int extern_vsnprintf_builtin(char * buffer, size_t count, const char * format, va_list va);

extern u32 __net_data_start, __net_data_size;
void net_bss_init(void)
{
    memset(&__net_data_start, 0, (u32)&__net_data_size);
}

void *netlib_malloc(size_t size, const char *tag)
{
    return ab_malloc(size);
}

void *netlib_calloc(size_t nitems, size_t size, const char *tag)
{
    return ab_calloc(nitems, size);
}

void *netlib_realloc(void *p, size_t new_size, const char *tag)
{
    return ab_realloc(p, new_size);
}

void netlib_free(void *p)
{
    return ab_free(p);
}

struct timeval
{
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* and microseconds */
};
int gettimeofday(struct timeval *__tp, void *__tzp)
{
    if (__tp)
    {
        __tp->tv_sec = RTCCNT;
        __tp->tv_usec = RTCCNT * 1000 * 1000;
    }

    return 0;
}

// 需要完整的vsnprintf实现
int net_vsnprintf(char * buffer, size_t count, const char * format, va_list va)
{
    return extern_vsnprintf_builtin(buffer, count, format, va);
}
