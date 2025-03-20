/**
 * @file       ute_module_simple_fifo.h
 * @brief
 * @author     Wang.Luo
 * @date       2023-02-21
 * @version    v1.0
 */

#ifndef __UTE_MODULE_SIMPLE_FIFO_H__
#define __UTE_MODULE_SIMPLE_FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief      fifo数据结构体
 * @details    使用fifo前需要定义好结构体变量
 */
typedef struct
{
    uint32_t            max_len;    /* buffer max len */
    uint32_t            head;
    uint32_t            tail;
    uint8_t *           data;       /* buffer */
} ute_module_simple_fifo_param_t;

void uteModuleSimpleFifoInit(ute_module_simple_fifo_param_t * pfifo, uint8_t * data, uint32_t data_size);
void uteModuleSimpleFifoClear(ute_module_simple_fifo_param_t * pfifo);
uint32_t uteModuleSimpleFifoGetFreeLen(ute_module_simple_fifo_param_t * pfifo);
uint32_t uteModuleSimpleFifoGetLen(ute_module_simple_fifo_param_t * pfifo);
bool uteModuleSimpleFifoPush(ute_module_simple_fifo_param_t * pfifo, uint8_t * data, uint32_t len);
bool uteModuleSimpleFifoPop(ute_module_simple_fifo_param_t * pfifo, uint8_t * data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __UTE_MODULE_SIMPLE_FIFO_H__ */

