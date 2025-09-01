#include "include.h"

typedef struct
{
    u32 free_size;
    u32 used_size;
    uint32_t free_biggest_size;
} mem_monitor_t;

static void mem_walker(void * ptr, size_t size, int used, void * user)
{
    mem_monitor_t * mon_p = user;
    if(used)
    {
        mon_p->used_size += size;
    }
    else
    {
//        mon_p->free_cnt++;
        mon_p->free_size += size;
        if(size > mon_p->free_biggest_size)
            mon_p->free_biggest_size = size;
    }
    printf("ptr:0x%x, size:0x%x, used:%d\n", (u32)ptr, size, used);
}

void mem_monitor_run(void)
{
    if (sys_tlsf == NULL)
    {
        printf("%s sys_tlsf=NULL\n", __func__);
        return;
    }

    mem_monitor_t mon = {0};
    tlsf_walk_pool(tlsf_get_pool(sys_tlsf), mem_walker, &mon);
    printf("### ram inof: use:%d%%, frag:%d%%, free size: 0x%x, used size: 0x%x\n", mon.used_size * 100U / CUSTOMER_HEAP_SIZE, 100 - (mon.free_biggest_size * 100U / mon.free_size), mon.free_size, mon.used_size);
}
