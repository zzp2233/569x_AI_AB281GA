/**
 * @file       ute_module_simple_fifo.c
 * @brief
 * @author     Wang.Luo
 * @date       2023-02-21
 * @version    v1.0
 */

#include "ute_module_simple_fifo.h"
#include "ute_module_platform.h"
#include "ute_module_log.h"
#include <string.h>

#define FIFO_ASSERT( p )                 ( ( void ) 0U )
//#define FIFO_ASSERT( p )                 if( !(p) ) while(1)

#define FIFO_IS_VALID(b)                 ((b) != NULL && (b)->data != NULL && (b)->max_len > 0)
#define FIFO_MIN(x, y)                   ((x) < (y) ? (x) : (y))
#define FIFO_MAX(x, y)                   ((x) > (y) ? (x) : (y))

/*! fifo互斥量,wang.luo 2023-06-08 */
// void *uteModuleFifoMute;

/**
 * @brief        初始化fifo
 * @details
 * @author       Wang.Luo
 * @date         2023-02-21
 */
AT(.com_text.func.ute_fifo)
void uteModuleSimpleFifoInit(ute_module_simple_fifo_param_t * pfifo, uint8_t * data, uint32_t data_size)
{
    // if (!uteModuleFifoMute)
    // {
    //     bool status = false;
    //     status = uteModulePlatformCreateMutex(&uteModuleFifoMute);
    //     UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,status=%d",__func__,status);
    // }
    FIFO_ASSERT(pfifo!=NULL);
    FIFO_ASSERT(data!=NULL);
    FIFO_ASSERT(data_size);

    pfifo->data = data;
    pfifo->max_len = data_size;
    pfifo->head = 0;
    pfifo->tail = 0;
}

/**
 * @brief        清空fifo
 * @details
 * @author       Wang.Luo
 * @date         2023-02-21
 */
AT(.com_text.func.ute_fifo)
void uteModuleSimpleFifoClear(ute_module_simple_fifo_param_t * pfifo)
{
    if (FIFO_IS_VALID(pfifo))
    {
        pfifo->head = 0;
        pfifo->tail = 0;
    }
}

/**
 * @brief        获取fifo剩余可用大小
 * @details
 * @return       内存中的可用字节数
 * @author       Wang.Luo
 * @date         2023-02-21
 */
AT(.com_text.func.ute_fifo)
uint32_t uteModuleSimpleFifoGetFreeLen(ute_module_simple_fifo_param_t * pfifo)
{
    uint32_t size, tail, head;

    if (!FIFO_IS_VALID(pfifo))
    {
        return 0;
    }

    /* Use temporary values in case they are changed during operations */
    tail = pfifo->tail;
    head = pfifo->head;
    if (tail == head)
    {
        size = pfifo->max_len;
    }
    else if (head > tail)
    {
        size = head - tail;
    }
    else
    {
        size = pfifo->max_len - (tail - head);
    }

    /* Buffer free size is always 1 less than actual size */
    return size - 1;
}

/**
 * @brief        获取fifo中已用数据长度
 * @details
 * @return       内存中的已用字节数
 * @author       Wang.Luo
 * @date         2023-02-21
 */
AT(.com_text.func.ute_fifo)
uint32_t uteModuleSimpleFifoGetLen(ute_module_simple_fifo_param_t * pfifo)
{
    uint32_t tail, head, size;

    if (!FIFO_IS_VALID(pfifo))
    {
        return 0;
    }

    /* Use temporary values in case they are changed during operations */
    tail = pfifo->tail;
    head = pfifo->head;
    if (tail == head)
    {
        size = 0;
    }
    else if (tail > head)
    {
        size = tail - head;
    }
    else
    {
        size = pfifo->max_len - (head - tail);
    }
    return size;
}

/**
 * @brief        填入数据到fifo
 * @details
 * @param[in]    pfifo handle
 * @param[in]    data  Pointer to data to write into buffer
 * @param[in]    len   Number of bytes to write
 * @return
 * @author       Wang.Luo
 * @date         2023-02-21
 */
AT(.com_text.func.ute_fifo)
bool uteModuleSimpleFifoPush(ute_module_simple_fifo_param_t * pfifo, uint8_t * data, uint32_t len)
{
    uint32_t tocopy, free;
    const uint8_t* d = data;

    if (!FIFO_IS_VALID(pfifo) || data == NULL || len == 0)
    {
        return false;
    }

    /* Calculate maximum number of bytes available to write */
    free = uteModuleSimpleFifoGetFreeLen(pfifo);
    if (free < len)
    {
        return false;
    }

    // uteModulePlatformTakeMutex(uteModuleFifoMute);

    /* Step 1: Write data to linear part of buffer */
    tocopy = FIFO_MIN(pfifo->max_len - pfifo->tail, len);
    memcpy(&pfifo->data[pfifo->tail], d, tocopy);
    pfifo->tail += tocopy;
    len -= tocopy;

    /* Step 2: Write data to beginning of buffer (overflow part) */
    if (len > 0)
    {
        memcpy(pfifo->data, &d[tocopy], len);
        pfifo->tail = len;
    }

    /* Step 3: Check end of buffer */
    if (pfifo->tail >= pfifo->max_len)
    {
        pfifo->tail = 0;
    }

    // uteModulePlatformGiveMutex(uteModuleFifoMute);

    return true;
}

/**
 * @brief        从Fifo中弹出数据
 * @details
 * @param[in]    len 要从fifo中弹出的数据的长度
 * @param[out]   data 指向fifo中弹出的数据的保存地址
 * @return       void*
 * @author       Wang.Luo
 * @date         2023-02-21
 */
AT(.com_text.func.ute_fifo)
bool uteModuleSimpleFifoPop(ute_module_simple_fifo_param_t * pfifo, uint8_t * data, uint32_t len)
{
    uint32_t tocopy, full;
    uint8_t* d = data;

    if (!FIFO_IS_VALID(pfifo) || data == NULL || len == 0)
    {
        return false;
    }

    /* Calculate maximum number of bytes available to read */
    full = uteModuleSimpleFifoGetLen(pfifo);
    if (full < len)
    {
        return false;
    }

    // uteModulePlatformTakeMutex(uteModuleFifoMute);

    /* Step 1: Read data from linear part of buffer */
    tocopy = FIFO_MIN(pfifo->max_len - pfifo->head, len);
    memcpy(d, &pfifo->data[pfifo->head], tocopy);
    pfifo->head += tocopy;
    len -= tocopy;

    /* Step 2: Read data from beginning of buffer (overflow part) */
    if (len > 0)
    {
        memcpy(&d[tocopy], pfifo->data, len);
        pfifo->head = len;
    }

    /* Step 3: Check end of buffer */
    if (pfifo->head >= pfifo->max_len)
    {
        pfifo->head = 0;
    }

    // uteModulePlatformGiveMutex(uteModuleFifoMute);

    return true;
}


