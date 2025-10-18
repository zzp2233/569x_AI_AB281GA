#ifndef __BSP_VDDGR_TRIM_H
#define __BSP_VDDGR_TRIM_H

enum
{
    VDDHR_TRIM_IDLE,            //空闲
    VDDHR_TRIM_START,           //启动
    VDDHR_TRIM_VERIFY,          //校验
    VDDHR_TRIM_VERIFY_END,      //校验结束
    VDDHR_TRIM_SAVE,            //保存
    VDDHR_TRIM_OFF,             //结束
    VDDHR_TRIM_TEST,            //测试
};

void bsp_vddhr_trim_init(void);
void bsp_vddhr_trim_save(void);
void bsp_vddhr_trim_proc(void);
void bsp_vddhr_test(void);

#endif