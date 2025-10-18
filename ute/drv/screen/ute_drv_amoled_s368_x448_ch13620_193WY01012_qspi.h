#ifndef _UTE_DRV_AMOLED_S368X448_CH13620_193WY01012_QSPI_H_
#define _UTE_DRV_AMOLED_S368X448_CH13620_193WY01012_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_AMOLED_S368X448_CH13620_193WY01012_QSPI_SUPPORT

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012IdleMode(bool enable)
{
    if(enable)
    {
        uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x39,NULL,0);
    }
    else
    {
        uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x38,NULL,0);
    }
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s()", __func__);
}
__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012SetBackLightInIdleMode(uint8_t backLightPercent)
{
    uint16_t light = UTE_MODULE_SCREEN_SAVE_DEFAULT_LIGHT_PERCENT*0x8f/100; // 查IC datasheet获取寄存器亮度范围 0~0x3ff
    //light = (((light>>8)&0xff)|(light<<8)); // 根据IC寄存器赋值要求，高低八位交换
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x4A, (uint8_t *)&light,2);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s()", __func__);
}
#endif

__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012PowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11, NULL, 0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29, NULL, 0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012PowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28, NULL, 0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10, NULL, 0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2A, data, 4);

    // yStart += 20;
    // yEnd   += 20;

    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2B, data, 4);

    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN, false);
}
__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012Init(void)
{
    uint8_t tmp[34];

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(30);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(50);

    memcpy(tmp, "\x50", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xF0, tmp, 1);
    memcpy(tmp, "\x58\x72", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xB1, tmp, 2);
    memcpy(tmp, "\x00\x00\x01\x6f", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2A, tmp, 4);
    memcpy(tmp, "\x00\x00\x01\xbf", 4);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2B, tmp, 4);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x35, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x3A, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x64, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x67, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x68, tmp, 1);
    memcpy(tmp, "\x5f", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xF0, tmp, 1);
    memcpy(tmp, "\x93", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xE0, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x51, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x60, tmp, 1);
}

__STATIC_INLINE void drvScreenAmoledS368X448Ch13620193WY01012SetBackLight(uint8_t backLightPercent)
{
    uint8_t light[2];
    light[0] = backLightPercent * 255 / 100;
    light[1] = 0;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x51, &light[0], 2); // light
}

const ute_drv_screen_common_config_t uteDrvScreenAmoledS368X448Ch13620193WY01012Config =
{
    .name = "AmoledS368X448Ch13620193WY01012",
    .init = drvScreenAmoledS368X448Ch13620193WY01012Init,
    .powerOn = drvScreenAmoledS368X448Ch13620193WY01012PowerOn,
    .powerOff = drvScreenAmoledS368X448Ch13620193WY01012PowerOff,
    .setWindow = drvScreenAmoledS368X448Ch13620193WY01012SetWindow,
    .setBackLight = drvScreenAmoledS368X448Ch13620193WY01012SetBackLight,
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    .idleMode = drvScreenAmoledS368X448Ch13620193WY01012IdleMode,
    .setBackLightIdleMode = drvScreenAmoledS368X448Ch13620193WY01012SetBackLightInIdleMode,
#endif
};
#endif
#endif
