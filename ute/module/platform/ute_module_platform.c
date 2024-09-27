/**
*@file
*@brief        BLE平台api文件
*@details  和BLE平台相关的io操作等
*@author       zn.zeng
*@date       Jun 29, 2021
*@version      v1.0
*/
#include "ute_module_platform.h"
#include "ute_module_log.h"
#include "ute_project_config.h"
#include "ute_module_message.h"
#include "ute_task_application.h"
#include "ute_module_systemtime.h"
/**
*@brief   us延时函数
*@details   小于10ms使用此函数
*@param[in] uint32_t us  延时us数
*@return  void
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformDelayUs(uint32_t us)
{
    delay_us(us);
}
/**
*@brief   ms延时函数
*@details   大于于10ms使用此函数
*@param[in] uint32_t ms  延时ms数
*@return  void
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformDelayMs(uint32_t ms)
{
    delay_ms(ms);
}
/**
*@brief   系统复位重启
*@details
*@author        zn.zeng
*@date        2023-03-22
*/
void uteModulePlatformSystemReboot(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s",__func__);
    WDT_RST();
}
static ute_module_platform_timer_t uteModulePlatformTimerData[UTE_MODULE_PLATFORM_TIMER_MAX] AT(.disp.ute);
uint8_t uteModulePlatformTimerCounter = 0;
static void uteModulePlatformTimerCallback(co_timer_t *timer, void *param)
{
    uteModulePlatformTimerCallback_t func = (uteModulePlatformTimerCallback_t)param;
    func(NULL);
}
/**
*@brief   创建定时器
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
bool uteModulePlatformCreateTimer(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                                  uint32_t interval_ms, bool reload, void (*p_timer_callback)(void *))
{
    bool result = false;
    /*在创建的定时器指针要为NULL*/
    if(*pp_handle != NULL)
    {
        return false;
    }
    /*在timer 数为0时清空数据*/
    if(uteModulePlatformTimerCounter == 0)
    {
        for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
        {
            uteModulePlatformTimerData[i].isCreate = false;
            uteModulePlatformTimerData[i].reLoad = false;
        }
    }
    /*通过声明的全局数组返回对应指针*/
    if(uteModulePlatformTimerCounter<UTE_MODULE_PLATFORM_TIMER_MAX)
    {
        for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
        {
            if(uteModulePlatformTimerData[i].isCreate == false)
            {
                /*返回一个指针*/
                (*pp_handle) = &uteModulePlatformTimerData[i];
                uteModulePlatformTimerData[i].duration = interval_ms;
                uteModulePlatformTimerData[i].reLoad = reload;
                uteModulePlatformTimerData[i].isCreate = true;
                uteModulePlatformTimerData[i].currTimerCallback = (uint32_t)p_timer_callback;
                uteModulePlatformTimerCounter++;
                if(reload)
                {
                    result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_REPEAT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                }
                else
                {
                    result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_ONE_SHOT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                }
                co_timer_set_sleep(&uteModulePlatformTimerData[i].timer,true);
                co_timer_enable(&uteModulePlatformTimerData[i].timer,false);
                break;
            }
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,result=%d,pp_handle=%p,uteModulePlatformHrTimerCounter = %d",__func__,result,*pp_handle,uteModulePlatformTimerCounter);
    return result;
}
/**
*@brief   重新启动定时器
*@details
*@author  casen
*@date    2023-03-09
*/
bool uteModulePlatformRestartTimer(void **pp_handle, uint32_t interval_ms)
{
    bool result = false;
    if(interval_ms==0 || (pp_handle == NULL || (*pp_handle) == NULL))
    {
        return false;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
    {
        if(&uteModulePlatformTimerData[i] == (*pp_handle))
        {
            if(uteModulePlatformTimerData[i].isCreate)
            {
                if(uteModulePlatformTimerData[i].duration==interval_ms)
                {
                    result = co_timer_reset(&uteModulePlatformTimerData[i].timer);
                }
                else
                {
                    uteModulePlatformTimerData[i].duration = interval_ms;
                    co_timer_del(&uteModulePlatformTimerData[i].timer);
                    if(uteModulePlatformTimerData[i].reLoad)
                    {
                        result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_REPEAT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                    }
                    else
                    {
                        result = co_timer_set(&uteModulePlatformTimerData[i].timer, interval_ms, TIMER_ONE_SHOT, LEVEL_LOW_PRI, uteModulePlatformTimerCallback,(void*)uteModulePlatformTimerData[i].currTimerCallback);
                    }
                    co_timer_set_sleep(&uteModulePlatformTimerData[i].timer,true);
                }
                result = co_timer_enable(&uteModulePlatformTimerData[i].timer,true);
            }
            break;
        }
    }
    return result;
}
/**
*@brief   停止定时器
*@details
*@author        zn.zeng
*@date        2021-08-31
*/
bool uteModulePlatformStopTimer(void **pp_handle)
{
    bool result = false;
    if(pp_handle == NULL || (*pp_handle) == NULL)
    {
        return false;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
    {
        if((&uteModulePlatformTimerData[i]) == (*pp_handle))
        {
            if(uteModulePlatformTimerData[i].isCreate)
            {
                co_timer_enable(&uteModulePlatformTimerData[i].timer,false);
                result = true;
                break;
            }
        }
    }
    return result;
}
/**
*@brief   删除定时器
*@details
*@author        zn.zeng
*@date        2021-08-31
*/
bool uteModulePlatformDeleteTimer(void **pp_handle)
{
    bool result = false;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,pp_handle = %p,uteModulePlatformTimerCounter=%d",__func__,*pp_handle,uteModulePlatformTimerCounter);
    if(pp_handle == NULL || (*pp_handle) == NULL)
    {
        return result;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_TIMER_MAX; i++)
    {
        if((&uteModulePlatformTimerData[i]) == (*pp_handle))
        {
            /*清除对应数据*/
            (*pp_handle) = NULL;
            uteModulePlatformTimerData[i].isCreate = false;
            uteModulePlatformTimerData[i].reLoad = false;
            co_timer_del(&uteModulePlatformTimerData[i].timer);
            result = true;
            uteModulePlatformTimerCounter--;
            break;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,result=%d",__func__,result);
    return result;
}
/**
*@brief   关闭系统看门狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgDisable(void)
{
    WDT_DIS();
}
/**
*@brief   打开系统看门狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgStart(void)
{
    WDT_EN();
}
/**
*@brief   系统看门狗喂狗
*@details
*@author        zn.zeng
*@date        2021-11-26
*/
void uteModulePlatformWdgFeed(void)
{
    WDT_CLR();
}
// static ute_module_platform_mutex_t uteModulePlatformMutexBuff[UTE_MODULE_PLATFORM_MUTEX_MAX] AT(.disp.ute);
// uint8_t uteModulePlatformMutexCnt = 0;
/**
*@brief   创建互斥量
*@details
*@return 成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformCreateMutex(void **pp_handle)
{
    bool result = false;
#if 0
    if(uteModulePlatformMutexCnt==0)
    {
        memset((void*)&uteModulePlatformMutexBuff[0],0,sizeof(ute_module_platform_mutex_t)*UTE_MODULE_PLATFORM_MUTEX_MAX);
    }
    if(uteModulePlatformMutexCnt<UTE_MODULE_PLATFORM_MUTEX_MAX)
    {
        for(uint8_t i=0; i<UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
        {
            if(!uteModulePlatformMutexBuff[i].isCreate)
            {
                os_err_t ret = os_mutex_init(&uteModulePlatformMutexBuff[i].mutex,"ute",OS_IPC_FLAG_FIFO);
                if(ret==OS_EOK)
                {
                    *pp_handle = &uteModulePlatformMutexBuff[i];
                    uteModulePlatformMutexBuff[i].isCreate = true;
                    uteModulePlatformMutexCnt++;
                    result = true;
                }
                else
                {
                    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,init error,ret=%d",__func__,ret);
                }
                break;
            }
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,uteModulePlatformMutexCnt=%d,is max",__func__,uteModulePlatformMutexCnt);
    }
#endif
    return result;
}
/**
*@brief   删除互斥量
*@details
*@return 成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformDeleteMutex(void **pp_handle)
{
    bool result = false;
#if 0
    if(pp_handle == NULL || (*pp_handle) == NULL)
    {
        return result;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
    {
        if((&uteModulePlatformMutexBuff[i]) == (*pp_handle))
        {
            os_err_t ret = os_mutex_delete(&uteModulePlatformMutexBuff[i].mutex);
            if(ret==OS_EOK)
            {
                (*pp_handle) = NULL;
                uteModulePlatformMutexBuff[i].isCreate = false;
                uteModulePlatformMutexCnt--;
                result = true;
            }
            else
            {
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,del error,ret=%d",__func__,ret);
            }
            break;
        }
    }
#endif
    return result;
}
/**
*@brief   使用互斥量
*@details  超时500ms
*@return  成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformTakeMutex(void *pp_handle)
{
    bool result = false;
#if 0
    if(pp_handle == NULL)
    {
        return result;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
    {
        if((&uteModulePlatformMutexBuff[i]) == pp_handle)
        {
            os_err_t ret = os_mutex_take(&uteModulePlatformMutexBuff[i].mutex,500);
            if(ret==OS_EOK)
            {
                result = true;
            }
            else
            {
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,take error,ret=%d",__func__,ret);
            }
            break;
        }
    }
#endif
    return result;
}
/**
*@brief   释放互斥量
*@details
*@return  成功返回true
*@author        zn.zeng
*@date        2021-09-23
*/
bool uteModulePlatformGiveMutex(void *pp_handle)
{
    bool result = false;
#if 0
    if(pp_handle == NULL)
    {
        return result;
    }
    for(uint8_t i=0; i<UTE_MODULE_PLATFORM_MUTEX_MAX; i++)
    {
        if((&uteModulePlatformMutexBuff[i]) == pp_handle)
        {
            os_err_t ret = os_mutex_release(&uteModulePlatformMutexBuff[i].mutex);
            if(ret==OS_EOK)
            {
                result = true;
            }
            else
            {
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,take error,ret=%d",__func__,ret);
            }
            break;
        }
    }
#endif
    return result;
}
/**
* @brief   nor flash 读函数
*@param[in] uint8_t   *data 数据指针
*@param[in] uint32_t addr  读取地址
*@param[in] uint32_t len 读取长度
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorRead(uint8_t   *data, uint32_t addr, uint32_t len)
{
    os_spiflash_read( (void *)data,addr, len);
}
/**
* @brief   nor flash 写函数
*@param[in] uint8_t   *data 数据指针
*@param[in] uint32_t addr  写地址
*@param[in] uint32_t len 写长度
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorWrite(uint8_t *data, uint32_t addr, uint32_t len)
{
    if(addr<UTE_SYSTEM_USER_DATA_START_ADDRESS)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,err addr =0x%x", __func__,addr);
        return;
    }
    os_spiflash_program((void *)data,addr,len);
}
/**
*@brief   nor flash 擦除
*@details 擦除一个SECTOR，4K大小
*@param[in] uint32_t addr  擦除地址
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformFlashNorErase(uint32_t addr)
{
    if(addr<UTE_SYSTEM_USER_DATA_START_ADDRESS)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,err addr =0x%x", __func__,addr);
        return;
    }
    os_spiflash_erase(addr);
}
/**
* @brief   发送消息到app task
*@param[in] uint16_t type ，消息类型
*@param[in] uint32_t param ，消息参数，可以传递一个4byte的参数
* @return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformSendMsgToUteApplicationTask(uint16_t type, uint32_t param)
{
    ute_task_application_message_t p_msg;
    p_msg.type= type;
    p_msg.param = param;
    bool isSend = uteTaskApplicationSendMsg(&p_msg);
    if (!isSend)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,send msg fail,type=%d", __func__, type);
    }
}
/**
*@brief   发送消息到app task
*@param[in] uint16_t type ，消息类型
*@param[in] uint32_t param ，消息参数，可以传递一个4byte的参数
*@return  void
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformSendMsgToAppTask(uint16_t type, uint32_t param)
{
    ute_task_application_message_t p_msg;
    p_msg.type= type;
    p_msg.param = param;
    bool isSend = uteTaskApplicationSendMsg(&p_msg);
    if (!isSend)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,send msg fail,type=%d", __func__, type);
    }
}
uint64_t uteModulePlatformSystemTickCnt = 0;
void *uteModulePlatformRtcTimer=NULL;
void uteModulePlatformRtcTimerHandler(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SYSTEM_TIME_SEC_BASE, 0);
}
/**
*@brief RTC初始化函数
*@details RTC初始化函数，使用RTC，每秒产生一个消息，作为系统每秒的时间
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcInit(void)
{
    uteModulePlatformCreateTimer(&uteModulePlatformRtcTimer,"rtc",0,1000,true,uteModulePlatformRtcTimerHandler);
}
/**
*@brief RTC start函数
*@details RTC开始运行
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcStart(void)
{
    uteModulePlatformRestartTimer(&uteModulePlatformRtcTimer,1000);
}
/**
*@brief RTC 设置时间函数
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformRtcSetTime(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec)
{
    tm_t set;
    set.year = year;
    set.mon = month;
    set.day = day;
    set.hour = hour;
    set.min = min;
    set.sec = sec;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,%04d-%02d-%02d,%02d:%02d:%02d",__func__,set.year,set.mon,set.day,set.hour,set.min,set.sec);
    rtc_clock_set(set);
}
/**
*@brief RTC 校准时间函数
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModulePlatformCalibrationSystemTimer(void)
{
    tm_t getRtcTime = rtc_clock_get();
    ute_module_systemtime_time_t getSystemTime;
    uteModuleSystemtimeGetTime(&getSystemTime);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL,"%s,rtc    %04d-%02d-%02d,%02d:%02d:%02d",__func__,getRtcTime.year,getRtcTime.mon,getRtcTime.day,getRtcTime.hour,getRtcTime.min,getRtcTime.sec);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL,"%s,system %04d-%02d-%02d,%02d:%02d:%02d",__func__,getSystemTime.year,getSystemTime.month,getSystemTime.day,getSystemTime.hour,getSystemTime.min,getSystemTime.sec);
}
/**
*@brief 系统滴答计数
*@details 系统滴答计数，1/32000，基于系统RTC
*@return 返回计数值，最大为 0xFFFFFF，24位
*@author        zn.zeng
*@date        Jul 05, 2021
*/
uint32_t uteModulePlatformGetSystemTick(void)
{
    return uteModulePlatformSystemTickCnt;
}
/**
*@brief   动态申请内存
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void *uteModulePlatformMemoryAlloc(size_t size)
{
    return func_zalloc(size);
}
/**
*@brief   释放动态申请的内存
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
void uteModulePlatformMemoryFree(void * p)
{
    if (p != NULL)
    {
        func_free(p);
    }
}
/**
*@brief   设置gpio输出
*@details
*@param[in] uint8_t pinNum
*@param[in] bool isHeight   输出高低电平
*@author        zn.zeng
*@date        2021-09-07
*/
void uteModulePlatformOutputGpioSet(uint8_t pinNum, bool isHeight)
{
    if(pinNum==0)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,error pinNum is 0", __func__);
        return;
    }
    uint8_t gprop = (pinNum-1)/8;
    uint8_t pin = (pinNum-1)%8;
    if(isHeight)
    {
        if(gprop==0)
        {
            GPIOAFEN &= ~BIT(pin);
            GPIOADE  |= BIT(pin);
            GPIOADIR &= ~BIT(pin);
            GPIOASET = BIT(pin);
        }
        else if(gprop==1)
        {
            GPIOBFEN &= ~BIT(pin);
            GPIOBDE  |= BIT(pin);
            GPIOBDIR &= ~BIT(pin);
            GPIOBSET = BIT(pin);
        }
        else if(gprop==2)
        {
            GPIOEFEN &= ~BIT(pin);
            GPIOEDE  |= BIT(pin);
            GPIOEDIR &= ~BIT(pin);
            GPIOESET = BIT(pin);
        }
        else if(gprop==3)
        {
            GPIOFFEN &= ~BIT(pin);
            GPIOFDE  |= BIT(pin);
            GPIOFDIR &= ~BIT(pin);
            GPIOFSET = BIT(pin);
        }

    }
    else
    {
        if(gprop==0)
        {
            GPIOAFEN &= ~BIT(pin);
            GPIOADE  |= BIT(pin);
            GPIOADIR &= ~BIT(pin);
            GPIOACLR = BIT(pin);
        }
        else if(gprop==1)
        {
            GPIOBFEN &= ~BIT(pin);
            GPIOBDE  |= BIT(pin);
            GPIOBDIR &= ~BIT(pin);
            GPIOBCLR = BIT(pin);
        }
        else if(gprop==2)
        {
            GPIOEFEN &= ~BIT(pin);
            GPIOEDE  |= BIT(pin);
            GPIOEDIR &= ~BIT(pin);
            GPIOECLR = BIT(pin);
        }
        else if(gprop==3)
        {
            GPIOFFEN &= ~BIT(pin);
            GPIOFDE  |= BIT(pin);
            GPIOFDIR &= ~BIT(pin);
            GPIOFCLR = BIT(pin);
        }
    }
}
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
/**
*@brief   qspi 初始化
*@details  四通道
*@author         zn.zeng
*@date     2024-04-01
*/
void uteModulePlatformScreenQspiInit(void)
{
    //clk init
    CLKDIVCON2 = (CLKDIVCON2 & ~(BIT(4) * 0xF)) | BIT(4) * 2;   // TFTDE div
    CLKGAT2 |= BIT(4);
    RSTCON0 |= BIT(8);

    GPIOAFEN &= ~BIT(5);                        // CS
    GPIOADE  |=  BIT(5);
    GPIOASET  =  BIT(5);
    GPIOADIR &= ~BIT(5);

    GPIOAFEN |=  BIT(4);                        // CLK
    GPIOADE  |=  BIT(4);
    GPIOACLR  =  BIT(4);
    GPIOADIR &= ~BIT(4);

    GPIOAFEN |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));  // D0/D1/D2/D3
    GPIOADE  |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    GPIOADIR |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));

#if MODE_4WIRE_8BIT
    DC_ENABLE();
#endif

    FUNCMCON2 = BIT(28);

    //te
    //TE port interrupt
    port_irq_register(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_VECTORn, tft_te_isr);
    port_wakeup_init(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PIN,UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_TRIGGER, UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PULL);               //开内部上拉, 下降沿唤醒

    //TICK Timer init
    CLKGAT0 |= BIT(27);
    TICK0CON = BIT(6) | BIT(5) | BIT(2);                //div64[6:4], xosc26m[3:1]
    TICK0PR = 0xFFFF;
    TICK0CNT = 0;
    TICK0CON |= BIT(0);                                 //TICK EN

    TICK1CON = BIT(7) | BIT(6) | BIT(5) | BIT(2);       //TIE, div64[6:4], xosc26m[3:1]
    TICK1PR = (int)((XOSC_CLK_HZ / 1000) * TFT_TE_CYCLE_DELAY) / 64;
    TICK1CNT = 0;
    sys_irq_init(IRQ_TE_TICK_VECTOR, 0, tick_te_isr);

    sys_cb.te_mode = false;                             //初始化
    sys_cb.te_mode_next = false;

    tft_set_temode(DEFAULT_TE_MODE);

    sys_irq_init(IRQ_DESPI_VECTOR, 0, tft_spi_isr);

    DESPICON = BIT(27) | BIT(9) | BIT(7) | BIT(3) | BIT(2) | BIT(0);    //[28:27]IN RGB565, [9]MultiBit, [7]IE, [3:2]4BIT, [0]EN
}
/**
*@brief   qspi 写命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenQspiWriteCmd(uint8_t *buf, uint32_t len)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    delay_us(1);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~BIT(3);                        //1BIT
    for(uint32_t i=0; i<len; i++)
    {
        tft_spi_sendbyte(buf[i]);
    }
}
/**
*@brief   Qspi 写gram数据
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenDspiWriteCmd(void)
{
    DESPICON |= BIT(3)|BIT(2);
}
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   dspi 初始化
*@details  双通道写
*@author         zn.zeng
*@date     2024-04-01
*/
void uteModulePlatformScreenDspiInit(void)
{
    //clk init
    CLKDIVCON2 = (CLKDIVCON2 & ~(BIT(4) * 0xF)) | BIT(4) * 2;   // TFTDE div
    CLKGAT2 |= BIT(4);
    RSTCON0 |= BIT(8);

    GPIOAFEN &= ~BIT(5);                        // CS
    GPIOADE  |=  BIT(5);
    GPIOASET  =  BIT(5);
    GPIOADIR &= ~BIT(5);

    GPIOAFEN |=  BIT(4);                        // CLK
    GPIOADE  |=  BIT(4);
    GPIOACLR  =  BIT(4);
    GPIOADIR &= ~BIT(4);

    // GPIOAFEN |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));  // D0/D1/D2/D3
    // GPIOADE  |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    // GPIOADIR |= (BIT(2)|BIT(3)|BIT(1)|BIT(0));
    GPIOAFEN |= (BIT(2)|BIT(3));  // D0/D1
    GPIOADE  |= (BIT(2)|BIT(3));
    GPIOADIR |= (BIT(2)|BIT(3));

    FUNCMCON2 = BIT(28);

    //te
    //TE port interrupt
    port_irq_register(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_VECTORn, tft_te_isr);
    port_wakeup_init(UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PIN,UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_TRIGGER, UTE_DRV_SCREEN_TE_SIGNAL_INT_GPIO_PULL);               //开内部上拉, 下降沿唤醒

    //TICK Timer init
    CLKGAT0 |= BIT(27);
    TICK0CON = BIT(6) | BIT(5) | BIT(2);                //div64[6:4], xosc26m[3:1]
    TICK0PR = 0xFFFF;
    TICK0CNT = 0;
    TICK0CON |= BIT(0);                                 //TICK EN

    TICK1CON = BIT(7) | BIT(6) | BIT(5) | BIT(2);       //TIE, div64[6:4], xosc26m[3:1]
    TICK1PR = (int)((XOSC_CLK_HZ / 1000) * TFT_TE_CYCLE_DELAY) / 64;
    TICK1CNT = 0;
    sys_irq_init(IRQ_TE_TICK_VECTOR, 0, tick_te_isr);

    sys_cb.te_mode = false;                             //初始化
    sys_cb.te_mode_next = false;

    tft_set_temode(DEFAULT_TE_MODE);

    sys_irq_init(IRQ_DESPI_VECTOR, 0, tft_spi_isr);

    DESPICON = BIT(27) | BIT(9) | BIT(7)| BIT(0);    //[28:27]IN RGB565, [9]MultiBit, [7]IE, [3:2]4BIT, [0]EN

    DESPIBAUD = sys_cb.despi_baud;

    sys_cb.tft_bglight_kick = true;

}
/**
*@brief   dspi 写命令
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformScreenDspiWriteCmd(uint8_t *buf, uint32_t len)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    uteModulePlatformDelayUs(2);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~(3<<2);
    DESPICON |= BIT(18);                        //1BIT 3wire control singnal
    DESPICON &= ~BIT(26);
    DESPICON &= ~BIT(19);                       // DC为 0
    DESPICON &= ~BIT(9);
    tft_spi_sendbyte(buf[0]);
    DESPICON |= BIT(19);
    for(uint32_t i=1; i<len; i++)
    {
        tft_spi_sendbyte(buf[i]);
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
}
/**
*@brief   dspi 写gram数据
*@details
*@author         zn.zeng
*@date     2021-10-12
*/
void uteModulePlatformDspiWriteGram(uint8_t cmd)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~(3<<2);
    DESPICON |= BIT(18);                        //9bit 1BIT 3wire control singnal
    DESPICON &= ~BIT(26);                       // 双通道关闭
    DESPICON &= ~BIT(19);                       // DC为 0
    tft_spi_sendbyte(cmd);

    DESPICON |= (2<<2);
    DESPICON |= BIT(18);
    DESPICON |= BIT(26);
    DESPICON |= BIT(19);                        // DC为 1
    DESPICON |= BIT(9);

}
/**
*@brief   dspi 读寄存器命令
*@details
*@author         zn.zeng
*@date     2024-04-09
*/
void uteModulePlatformScreenDspiReadCmd(uint8_t cmd,uint8_t *buf, uint32_t len,uint8_t dummyClockByte)
{
    DESPIBAUD = 32;
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
    DESPICON &= ~(3<<2);
    DESPICON |= BIT(18);                        //1BIT 3wire control singnal
    DESPICON &= ~BIT(26);
    DESPICON &= ~BIT(19);                       // DC为 0
    DESPICON &= ~BIT(9);
    tft_spi_sendbyte(cmd);

    DESPICON &= ~BIT(18);
    DESPICON |= BIT(2);
    for(uint32_t i=0; i<dummyClockByte; i++)
    {
        tft_spi_getbyte();
    }
    for(uint32_t i=0; i<len; i++)
    {
        buf[i] = tft_spi_getbyte();
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,true);
    DESPIBAUD = sys_cb.despi_baud;
}
#endif
/**
*@brief   PWM 初始化
*@details
*@param[in] pwm_gpio id ,
*@param[in] uint8_t pinNum ,输出pin
*@param[in] uint8_t duty, 占空比
*@param[in] uint32_t rateHz, 频率
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmInit(pwm_gpio id,uint8_t pinNum,uint8_t duty,uint32_t rateHz)
{
    bsp_pwm_freq_set(rateHz);
    bsp_pwm_duty_set(id,duty,false);
}
/**
*@brief   PWM 打开输出
*@details  不实现，init 就开始了
*@param[in] pwm_gpio id
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmEnable(pwm_gpio id)
{

}
/**
*@brief   PWM 关闭输出
*@details
*@param[in] uint8_t id ,timer id ,0~7
*@author        zn.zeng
*@date        2021-10-12
*/
void uteModulePlatformPwmDisable(pwm_gpio id,uint8_t pinNum)
{
    bsp_pwm_disable(id);
}

