#ifndef _UTE_DRV_AMOLED_S368X448_GC3A71_193WG2014V1_QSPI_H_
#define _UTE_DRV_AMOLED_S368X448_GC3A71_193WG2014V1_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_AMOLED_S368X448_GC3A71_193WG2014V1_QSPI_SUPPORT

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1IdleMode(bool enable)
{
    uint8_t tmp[5];
    memcpy(&tmp[0], "\x55\xAA\x66", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);

    if(enable)
    {
        uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x39,NULL,0);
    }
    else
    {
        uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x38,NULL,0);
    }

    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFA, tmp, 1);
    memcpy(&tmp[0], "\x66\x99\x55", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);

    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s()", __func__);
}
__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1SetBackLightInIdleMode(uint8_t backLightPercent)
{
    uint16_t light = UTE_MODULE_SCREEN_SAVE_DEFAULT_LIGHT_PERCENT*0x8f/100; // 查IC datasheet获取寄存器亮度范围 0~0x3ff
    //light = (((light>>8)&0xff)|(light<<8)); // 根据IC寄存器赋值要求，高低八位交换
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x4A, (uint8_t *)&light,2);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s()", __func__);
}
#endif

__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1PowerOn(void)
{
    uint8_t tmp[5];
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29,NULL,0);
    uteModulePlatformDelayMs(50);

    memcpy(tmp, "\x24", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x8C, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x8D, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFA, tmp, 1);
    memcpy(&tmp[0], "\x66\x99\x55", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);
    uteModulePlatformDelayMs(20);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1PowerOff(void)
{
    uint8_t tmp[5];
    memcpy(&tmp[0], "\x55\xAA\x66", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28,NULL,0);
    uteModulePlatformDelayMs(20);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(20);
    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x4F, &tmp[0], 1);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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
__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1Init(void)
{
    uint8_t tmp[34];

    uteDrvScreenCommonSetPowerEnable(true);
    uteModulePlatformDelayMs(10);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(15);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayMs(15);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(30);

    memcpy(&tmp[0], "\x55\xAA\x66", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x3A, tmp, 1); //RGB565
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x35, tmp, 1);
    memcpy(&tmp[0], "\x00\x0A", 2);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x44, &tmp[0], 2);

    memcpy(tmp, "\xA3", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);
    memcpy(tmp, "\xAA", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xF8, tmp, 1);
    memcpy(tmp, "\xEF", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x68, tmp, 1);
    memcpy(tmp, "\x24", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);
    memcpy(tmp, "\x18", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x8C, tmp, 1);
    memcpy(tmp, "\x18", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x8D, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);

    memcpy(&tmp[0], "\x00\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x51, &tmp[0], 2); // light
}

__STATIC_INLINE void drvScreenAmoledS368X448GC3A71193WG2014V1SetBackLight(uint8_t backLightPercent)
{
    uint8_t tmp[5];
    memcpy(&tmp[0], "\x55\xAA\x66", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);

    uint8_t light[2];
    light[0] = backLightPercent * 255 / 100;
    light[1] = 0;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x51, &light[0], 2); // light

    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFA, tmp, 1);
    memcpy(&tmp[0], "\x66\x99\x55", 3);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, &tmp[0], 3);
}

const ute_drv_screen_common_config_t uteDrvScreenAmoledS368X448GC3A71193WG2014V1Config =
{
    .name = "AmoledS368X448GC3A71193WG2014V1",
    .init = drvScreenAmoledS368X448GC3A71193WG2014V1Init,
    .powerOn = drvScreenAmoledS368X448GC3A71193WG2014V1PowerOn,
    .powerOff = drvScreenAmoledS368X448GC3A71193WG2014V1PowerOff,
    .setWindow = drvScreenAmoledS368X448GC3A71193WG2014V1SetWindow,
    .setBackLight = drvScreenAmoledS368X448GC3A71193WG2014V1SetBackLight,
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    .idleMode = drvScreenAmoledS368X448GC3A71193WG2014V1IdleMode,
    .setBackLightIdleMode = drvScreenAmoledS368X448GC3A71193WG2014V1SetBackLightInIdleMode,
#endif
};
#endif
#endif
