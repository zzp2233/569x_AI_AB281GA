#ifndef __SPT5113C_H__
#define __SPT5113C_H__

#include "include.h"

/*
 * 函数名：spt5113c_init
 * 描述  ：SPT5113C触摸初始化
 * 输入  ：void
 * 输出  ：bool    初始化成功返回true，失败返回false
 */
bool spt5113c_init(void);

/*
 * 函数名：spt5113c_readkick
 * 描述  ：SPT5113C进入休眠
 * 输入  ：void
 * 输出  ：void
 */
void spt5113c_enter_sleep(void);

/*
 * 函数名：spt5113c_readkick
 * 描述  ：SPT5113C 中断内读触摸数据
 * 输入  ：void
 * 输出  ：void
 */
void spt5113c_readkick(void);

/*
 * 函数名：spt5113c_get_point
 * 描述  ：SPT5113C触摸坐标获取
 * 输入  ：int32_t *x      X坐标
 *         int32_t *y      Y坐标
 * 输出  ：bool    有触摸返回true，无触摸返回false
 */
bool spt5113c_get_point(int32_t *x, int32_t *y);

/*
 * 函数名：spt5113c_wake_up
 * 描述  ：SPT5113C喚醒
 * 输入  ：void
 * 输出  ：void
 */
void spt5113c_wake_up(void);

#endif
