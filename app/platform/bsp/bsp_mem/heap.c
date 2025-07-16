#include "include.h"
#include "tlsf.h"
#include "rt_thread.h"

// clang-format off
tlsf_t sys_tlsf;
static struct os_mutex sys_tlsf_mutex;
// clang-format on

#if CUSTOMER_HEAP_SIZE
AT(.heap.custom)
static u8 customer_heap[CUSTOMER_HEAP_SIZE];
void get_customer_heap_info(void **buf, u32 *size)
{
    *buf = customer_heap;
    *size = CUSTOMER_HEAP_SIZE;
}



#else
void get_customer_heap_info(void **buf, u32 *size)
{
    *buf  = NULL;
    *size = 0;
}
#endif

void customer_heap_init(void *buf, u32 size)
{
    sys_tlsf = NULL;
    if (size > 0)
    {
        printf("malloc heap size:%d\n", size);
        os_mutex_init(&sys_tlsf_mutex, "mm", OS_IPC_FLAG_FIFO);
        sys_tlsf = tlsf_create_with_pool(buf, size);
    }
}

void *ab_malloc(size_t size)
{
    os_mutex_take(&sys_tlsf_mutex, OS_WAITING_FOREVER);
    void *ptr = tlsf_malloc(sys_tlsf, size);
    os_mutex_release(&sys_tlsf_mutex);
    return ptr;
}

void *ab_zalloc(size_t size)
{
    os_mutex_take(&sys_tlsf_mutex, OS_WAITING_FOREVER);
    void *ptr = tlsf_malloc(sys_tlsf, size);

    if (ptr)
    {
        memset(ptr, 0, size);
    }
    else
    {
        printf("ab warning zalloc ptr NULL:%x\n", size);
    }
    os_mutex_release(&sys_tlsf_mutex);
    return ptr;
}



void ab_free(void *ptr)
{
    os_mutex_take(&sys_tlsf_mutex, OS_WAITING_FOREVER);
    tlsf_free(sys_tlsf, ptr);
    os_mutex_release(&sys_tlsf_mutex);
}

void *ab_calloc(size_t nitems, size_t size)
{
    os_mutex_take(&sys_tlsf_mutex, OS_WAITING_FOREVER);
    size_t total = nitems * size;
    void  *ptr   = tlsf_malloc(sys_tlsf, total);
    if (ptr)
    {
        memset(ptr, 0, total);
    }
    else
    {
        printf("ab warning calloc ptr NULL:%x\n", total);
    }
    os_mutex_release(&sys_tlsf_mutex);
    return ptr;
}

void *ab_realloc(void *p, size_t new_size)
{
    os_mutex_take(&sys_tlsf_mutex, OS_WAITING_FOREVER);
    void *ptr = tlsf_realloc(sys_tlsf, p, new_size);
    if (!ptr)
    {
        printf("ab warning realloc ptr NULL:%x\n", new_size);
    }
    os_mutex_release(&sys_tlsf_mutex);
    return ptr;
}

void *_malloc_r(struct _reent * r, size_t size)
{
    return ab_malloc(size);
}

void *_calloc_r(struct _reent * r, size_t a, size_t b)
{
    return ab_calloc(a, b);
}

void _free_r(struct _reent * r, void *ptr)
{
    ab_free(ptr);
}

void *_realloc_r(struct _reent * r, void *ptr, size_t size)
{
    return ab_realloc(ptr, size);
}
