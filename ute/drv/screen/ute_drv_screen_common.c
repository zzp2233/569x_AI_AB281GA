/**
*@file
*@brief     屏驱动文件
*@details    屏驱动公共层文件
*@author        zn.zeng
*@date        2021-09-06
*@version       v1.0
*/
#include "ute_drv_screen_common.h"
#include "ute_application_common.h"
#include "ute_module_message.h"
#include "ute_module_log.h"
#include "gui.h"


#if DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_SUPPORT
#include "ute_drv_tft_s240x284_i18_st7789p3_xhl183ba2401.h"
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_nv3030b_hy201068avc.h"
#elif DRV_BOE_S240X284_I183_JD9853_WV018LZQ_QSPI_SUPPORT
#include "ute_drv_boe_s240x284_i183_jd9853_wv018lzq_qspi.h"
#elif UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_nv3030b_zd183g1196.h"
#elif UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_nv3030b_hy139071a.h"
#elif UTE_DRV_TFT_S360X360_GC9B71_ZD138G1616_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_gc9b71_zd138g1616.h"
#elif UTE_DRV_TFT_S240X284_JD9853_HY018214OV_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_jd9853_hy018214ov.h"
#elif UTE_DRV_TFT_S240X284_I183_JD9853_0185A035_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_i183_jd9853_0185A035_qspi.h"
#elif UTE_DRV_TFT_S240X296_GC9309_JS202018A_QSPI_SUPPORT
#include "ute_drv_tft_s240x296_gc9309_js202018a_qspi.h"
#elif UTE_DRV_TFT_S360_X360_ST77916_HY138026A_OV_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_st77916_hy138026a_ov_qspi.h"
#elif UTE_DRV_TFT_S360X360_I172_ST77916_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_i172_st77916_qspi.h"
#elif UTE_DRV_TFT_S240X240_NV3002C_HY138026A_QSPI_SUPPORT
#include "ute_drv_tft_s240X240_nv3002c_hy138026a.h"
#elif UTE_DRV_AMOLED_S368X448_GC3A71_HK0J1931972B03_QSPI_SUPPORT
#include "ute_drv_amoled_s368_x448_gc3a71_hk0j1931972b03_qspi.h"
#elif UTE_DRV_TFT_S360X360_GC9B71_HY139074A_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_gc9b71_hy139074a.h"
#elif UTE_DRV_TFT_S360X360_GC9B71_JS139005A_QSPI_SUPPORT
#include "ute_drv_tft_s360X360_gc9b71_js139005a.h"
#elif UTE_DRV_TFT_S240X284_I183_JD9853_SXT180G1911_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_i183_jd9853_sxt180g1911_qspi.h"
#elif UTE_DRV_TFT_S320_X380_ST77916_TRULY_191_QSPI_SUPPORT
#include "ute_drv_tft_s320X380_st77916_truly_191_qspi.h"
#elif UTE_DRV_AMOLED_S368X448_CH13620_193WY01012_QSPI_SUPPORT
#include "ute_drv_amoled_s368_x448_ch13620_193WY01012_qspi.h"
#elif UTE_DRV_TFT_S240X296_JD9853_HKOE19124B01_QSPI_SUPPORT
#include "ute_drv_tft_s240x296_jd9853_hkoe19124b01_qspi.h"
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201077R_QSPI_SUPPORT
#include "ute_drv_tft_s240x284_nv3030b_hy201077r.h"
#endif

/*! 配置屏的接口zn.zeng, 2021-09-06  */
const ute_drv_screen_common_config_t *uteDrvScreenCommonFunction=NULL;
ute_drv_screen_common_data_t uteDrvScreenCommonData;

/**
*@brief   屏gpio初始化函数
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
void uteDrvScreenCommonGpioInit(void)
{

}
/**
*@brief   屏接口初始化函数
*@details
*@author        zn.zeng
*@date        2022-05-05
*/
void uteDrvScreenCommonInterfaceInit(void)
{
#if DRV_TFT_S280X284_I18_ST7789P3_XHL183BA2401_DSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft280X284St7789p3Xhl183ba2401Config;
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Nv3030BHy201068AvcConfig;
#elif DRV_BOE_S240X284_I183_JD9853_WV018LZQ_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenBoe240X284Wv018lzqConfig;
#elif UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Nv3030BZd183g1196Config;
#elif UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360Nv3030BHy139071aConfig;
#elif UTE_DRV_TFT_S240X284_JD9853_HY018214OV_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Jd9853Hy018214OvConfig;
#elif UTE_DRV_TFT_S360X360_GC9B71_ZD138G1616_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360Gc9b71Zd138g1616Config;
#elif UTE_DRV_TFT_S240X284_I183_JD9853_0185A035_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Jd98530185A035Config;
#elif UTE_DRV_TFT_S240X296_GC9309_JS202018A_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X296Gc9309Js202018aConfig;
#elif UTE_DRV_TFT_S360_X360_ST77916_HY138026A_OV_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360St77916Hy138026AConfig;
#elif UTE_DRV_TFT_S360X360_I172_ST77916_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360I172St77916QspiConfig;
#elif UTE_DRV_TFT_S240X240_NV3002C_HY138026A_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X240nv3002chy138026aConfig;
#elif UTE_DRV_AMOLED_S368X448_GC3A71_HK0J1931972B03_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenAmoledS368X448Gc3A71Hk0J1931972B03Config;
#elif UTE_DRV_TFT_S360X360_GC9B71_HY139074A_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360Gc9b71Hy139074aConfig;
#elif UTE_DRV_TFT_S360X360_GC9B71_JS139005A_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft360X360Gc9b71Js139005aConfig;
#elif UTE_DRV_TFT_S240X284_I183_JD9853_SXT180G1911_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Jd9853Sxt180g1911Config;
#elif UTE_DRV_TFT_S320_X380_ST77916_TRULY_191_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft320X380St77916Truly191Config;
#elif UTE_DRV_AMOLED_S368X448_CH13620_193WY01012_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenAmoledS368X448Ch13620193WY01012Config;
#elif UTE_DRV_TFT_S240X296_JD9853_HKOE19124B01_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Jd9853Hkoe19124B01Config;
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201077R_QSPI_SUPPORT
    uteDrvScreenCommonFunction = &uteDrvScreenTft240X284Nv3030BHy201077rConfig;
#else
#error "Please select a screen driver"
#endif
#if UTE_DRV_8080_FOR_SCREEN_SUPPORT
    uteModulePlatform8080Init();
#elif UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformScreenQspiInit();
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformScreenDspiInit();
#endif
    uteDrvScreenCommonGpioInit();
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,%s", __func__,uteDrvScreenCommonFunction->name);
}

/**
*@brief   屏初始化函数
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
void uteDrvScreenCommonInit(void)
{
    uteDrvScreenCommonInterfaceInit();
#if UTE_LOG_DRV_SCREEN_LVL
    uteDrvScreenCommonReadId();
#else
    DESPIBAUD = tft_cb.despi_baud;
#endif
    if(uteDrvScreenCommonFunction->init)
    {
        uteDrvScreenCommonFunction->init();
    }
#if UTE_DRV_CLEAR_SCREEN_BEFORE_BACKLIGHT_ON_SUPPORT
    uteDrvScreenCommonClearWindow(0, 0, UTE_DRV_SCREEN_WIDTH, UTE_DRV_SCREEN_HEIGHT, 0);
#endif
    if(uteDrvScreenCommonFunction->powerOn)
    {
        uteDrvScreenCommonFunction->powerOn();
    }
    uteDrvScreenCommonData.isInit = true;
    uteDrvScreenCommonData.currBackLightPercent = 0;
}

/**
 * @brief        屏幕初始化标志
 * @details
 * @return       true 已经初始化 false 未初始化
 * @author       Wang.Luo
 * @date         2025-06-11
 */
__SCREEN_COMMON bool uteDrvScreenCommonIsInit(void)
{
    return uteDrvScreenCommonData.isInit;
}

/**
*@brief   设置屏reset 脚电平
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
__SCREEN_COMMON void uteDrvScreenCommonSetResetPin(bool isHeight)
{
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_RST_GPIO_PIN,isHeight);
}
/**
*@brief   设置屏电源电平
*@details
*@author        zn.zeng
*@date        2021-09-06
*/
void uteDrvScreenCommonSetPowerEnable(bool isEnable)
{
#if UTE_DRV_SCREEN_POWER_CTRL_SUPPORT
    if (UTE_DRV_SCREEN_ENABLE_ACTIVE_LEVEL)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_POWER_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_POWER_GPIO_PIN, !isEnable);
    }
#endif
#ifdef UTE_DRV_SCREEN_VCI_EN_GPIO_PIN
    if (UTE_DRV_SCREEN_VCI_EN_ACTIVE_LEVEL)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_VCI_EN_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_VCI_EN_GPIO_PIN, !isEnable);
    }
#endif
#ifdef UTE_DRV_SCREEN_IM1_GPIO_PIN
    if (UTE_DRV_SCREEN_IM1_SELECT)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM1_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM1_GPIO_PIN, !isEnable);
    }
#endif
#ifdef UTE_DRV_SCREEN_IM0_GPIO_PIN
    if (UTE_DRV_SCREEN_IM0_SELECT)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM0_GPIO_PIN, isEnable);
    }
    else
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_IM0_GPIO_PIN, !isEnable);
    }
#endif
    if (!isEnable)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_RST_GPIO_PIN, false);
    }
}
/**
*@brief   设置显示区域
*@details
*@author        zn.zeng
*@date        2021-09-07
*/
__SCREEN_COMMON void uteDrvScreenCommonSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uteDrvScreenCommonFunction->setWindow(xStart,yStart,xEnd-1,yEnd-1);
}
/**
*@brief   清屏函数
*@details
*@author        zn.zeng
*@date        2021-09-07
*/
void uteDrvScreenCommonClearWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,uint16_t color)
{
    if(uteDrvScreenCommonFunction->setWindow)
    {
        uteDrvScreenCommonFunction->setWindow(xStart,yStart,xEnd,yEnd);
    }
    gui_clear_screen(color,(yEnd+1-yStart)*(xEnd+1-xStart));
}
#if UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT
/**
*@brief   Sgm3132yde8g背光
*@details   16级，0~15
*@param[in] uint8_t percent   0~100% ，为0时关闭背光
*@author        zn.zeng
*@date        2021-09-08
*/
void uteDrvScreenCommonSgm3132yde8g(uint8_t percent)
{
    // There are totally 16 steps of current that could be set by users.
    uint8_t brightLevel = 0;
    percent = (percent > 100) ? 100 : percent;
    brightLevel = 16 - (uint8_t)((percent/100.0f) * 16)+(16-UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_MAX_LVL);
    if(brightLevel>=(UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_MAX_LVL-1))
    {
        brightLevel = (UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_MAX_LVL-1);
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
    uteModulePlatformDelayUs(5000);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,true);
    uteModulePlatformDelayUs(50);
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
    uteModulePlatformDelayUs(10);
    while(brightLevel)
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,true);
        uteModulePlatformDelayUs(10);
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
        uteModulePlatformDelayUs(5);
        brightLevel--;
    }
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,true);
}
#endif
/**
*@brief   打开关闭背光
*@details
*@param[in] uint8_t percent   0~100% ，为0时关闭背光
*@author        zn.zeng
*@date        2021-09-08
*/
void uteDrvScreenCommonOpenBacklight(uint8_t percent)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,percent=%d,currBackLightPercent=%d", __func__,percent,uteDrvScreenCommonData.currBackLightPercent);
    if(percent!=0)
    {
        if(percent!=uteDrvScreenCommonData.currBackLightPercent)
        {
#if (UTE_DRV_SCREEN_BACKLIGHT_PWM_SUPPORT||UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT)
            uteModulePlatformDelayMs(30);
#endif
            if(uteDrvScreenCommonFunction->setBackLight!=NULL)
            {
                uteDrvScreenCommonFunction->setBackLight(percent);
            }
            else
            {
#if UTE_DRV_SCREEN_BACKLIGHT_PWM_SUPPORT
                uint8_t pwm_percent = percent;
#ifdef SCREEN_BACK_LIGHT_INCREASE_PERCENT
                pwm_percent = (uint8_t)(pwm_percent*SCREEN_BACK_LIGHT_INCREASE_PERCENT);
#endif
                uteModulePlatformPwmInit(UTE_DRV_SCREEN_BACKLIGHT_PWM_ID,UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,pwm_percent,UTE_DRV_SCREEN_BACKLIGHT_PWM_HZ);
                uteModulePlatformPwmEnable(UTE_DRV_SCREEN_BACKLIGHT_PWM_ID);
#elif UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT
                uteDrvScreenCommonSgm3132yde8g(percent);
#endif
            }
        }
    }
    else
    {
#if UTE_DRV_SCREEN_BACKLIGHT_PWM_SUPPORT
        uteModulePlatformPwmDisable(UTE_DRV_SCREEN_BACKLIGHT_PWM_ID,UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN);
#elif UTE_DRV_SCREEN_BACKLIGHT_ONE_LINE_PULSE_SGM3132YDE8G_SUPPORT
        uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_BACKLIGHT_GPIO_PIN,false);
#endif
    }
    uteDrvScreenCommonData.currBackLightPercent = percent;
}
/**
*@brief   关闭显示
*@details
*@author        zn.zeng
*@date        2021-09-24
*/

void uteDrvScreenCommonDisplayOff(void)
{
    uteDrvScreenCommonOpenBacklight(0);
    uteDrvScreenCommonFunction->powerOff();
    uteDrvScreenCommonData.isInit = false;
}

__SCREEN_COMMON void uteModulePlatformScreenWriteDataStart(void)
{
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformScreenQspiWriteGram(0x2c);
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
    uteModulePlatformDspiWriteGram(0x2c);
#endif
}

__SCREEN_COMMON uint32_t uteDrvScreenCommonReadId(void)
{
    uint32_t id = 0;
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
    id = uteDrvScreenCommonGc9c01QspiReadId();
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT

#endif
    id = ((id & 0xFF000000) >> 24) |
         ((id & 0x00FF0000) >> 8) |
         ((id & 0x0000FF00) << 8) |
         ((id & 0x000000FF) << 24);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,id=0x%x", __func__,id);
    return id;
}

#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT

/**
*@brief   gc9c01 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
__SCREEN_COMMON void uteDrvScreenCommonGc9c01QspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    static uint8_t sdat[36];// = {0x02, 0x00, cmd, 0x00,};
    sdat[0] = 0x02;
    sdat[1] = 0x00;
    sdat[2] = cmd;
    sdat[3] = 0x00;
    if(size>32)
    {
        return;
    }
    if(size>0)
    {
        memcpy(&sdat[4],data,size);
    }
    uteModulePlatformScreenQspiWriteCmd(sdat, size+4);
}

/**
*@brief   gc9c01 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
__SCREEN_COMMON void uteDrvScreenCommonGc9c01QspiWriteCmdParam(uint8_t cmd,uint8_t data)
{
    uint8_t sdata[2] = {0,0};
    sdata[0] = data;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(cmd,&sdata[0],1);
}
/**
*@brief   ft2308 qspi write
*@details  16bit cmd
*@author        zn.zeng
*@date        2022-04-19
*/
__SCREEN_COMMON void uteDrvScreenCommonFt2308QspiWrite16bitCmdParams(uint16_t cmd,uint8_t *data,uint8_t size)
{
    static uint8_t sdat[36];// = {0x02, 0x00, cmd, 0x00,};
    sdat[0] = 0x02;
    sdat[1] = 0x00;
    sdat[2] = cmd>>8&0xff;
    sdat[3] = cmd&0xff;
    if(size>32)
    {
        return;
    }
    if(size>0)
    {
        memcpy(&sdat[4],data,size);
    }
    uteModulePlatformScreenQspiWriteCmd(sdat, size+4);
}
/**
*@brief   ft2308 qspi write
*@details  8bit cmd
*@author        zn.zeng
*@date        2022-04-19
*/
__SCREEN_COMMON void uteDrvScreenCommonFt2308QspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(cmd,data,size);
}
/**
*@brief   NV3030B qspi write
*@details  8bit cmd
*@author        zn.zeng
*@date        2022-04-19
*/
__SCREEN_COMMON void uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(cmd,data,size);
}

__SCREEN_COMMON uint32_t uteDrvScreenCommonGc9c01QspiReadId(void)
{
    uint32_t id = 0;
    uteModulePlatformScreenQspiReadCmd(0x04,(uint8_t *)&id,4);
    return id;
}

#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   st7789p3 dspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size)
{
    uint8_t sdat[36];
    sdat[0] = cmd;
    if(size>32)
    {
        return;
    }
    if(size>0)
    {
        memcpy(&sdat[1],data,size);
    }
    uteModulePlatformScreenDspiWriteCmd(sdat, size+1);
}
/**
*@brief   st7789p3 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParam(uint8_t cmd,uint8_t data)
{
    uint8_t sdata[2] = {0,0};
    sdata[0] = data;
    uteDrvScreenCommonSt7789p3DspiWriteCmdParams(cmd,&sdata[0],1);
}
#endif
/*! ESD测试屏幕TE中断异常复位处理,wang.luo 2023-03-21 */
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
void *screenEsdTeIntErrorCheckTimerPointer = NULL;
uint8_t screenEsdTeIntErrorCheckCnt = 0;
/**
*@brief        延时初始化处理函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckHandlerMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,", __func__);
    if (uteApplicationCommonIsPowerOn())
    {
        if (!sys_cb.gui_sleep_sta)
        {
            tft_exit();
            ctp_exit();
            power_gate_3v3_off();
            sys_cb.gui_sleep_sta = 1;
            tft_cb.tft_bglight_first_set = false;
            delay_5ms(10);
            gui_wakeup();
            uteDrvScreenEsdTeIntErrorCheckTimerStart(1000);
        }
        else
        {
            uteDrvScreenEsdTeIntErrorCheckTimerStop();
        }
    }
    else
    {
        uteDrvScreenEsdTeIntErrorCheckTimerStop();
    }
    screenEsdTeIntErrorCheckCnt = 0;
}
/**
*@brief        延时初始化回调函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerCallback(void *pxTimer)
{
    if(screenEsdTeIntErrorCheckCnt >= 3)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SCREEN_ESD_TE_INT_ERROR_RESET,0);
    }
    else
    {
        uteDrvScreenEsdTeIntErrorCheckTimerStart(1000);
    }
    screenEsdTeIntErrorCheckCnt ++;
}

AT(.com_text.tft_spi)
void uteDrvScreenEsdTeIntErrorCheckCntReset(void)
{
    screenEsdTeIntErrorCheckCnt = 0;
}

/**
*@brief        延时初始化定时器创建函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerCreate(void)
{
    if(screenEsdTeIntErrorCheckTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&screenEsdTeIntErrorCheckTimerPointer, "spd2010ErrorCheckTimer",1, 1000, false, uteDrvScreenEsdTeIntErrorCheckTimerCallback);
    }
}
/**
*@brief   启动初始化定时器创建函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerStart(uint16_t ms)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s,%d ms", __func__,ms);
    uteDrvScreenEsdTeIntErrorCheckTimerCreate();
    uteModulePlatformRestartTimer(&screenEsdTeIntErrorCheckTimerPointer, ms);
}
/**
*@brief   停止初始化定时器创建函数
*@details
*@author       xjc
*@date       2022-05-17
*/
void uteDrvScreenEsdTeIntErrorCheckTimerStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uteModulePlatformStopTimer(&screenEsdTeIntErrorCheckTimerPointer);
    uteModulePlatformDeleteTimer(&screenEsdTeIntErrorCheckTimerPointer);
    screenEsdTeIntErrorCheckTimerPointer = NULL;
}
#endif

