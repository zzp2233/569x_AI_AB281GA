#ifndef _UTE_DRV_AMOLED_S368X448_GC3A71_HK0J1931972B03_QSPI_H_
#define _UTE_DRV_AMOLED_S368X448_GC3A71_HK0J1931972B03_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_AMOLED_S368X448_GC3A71_HK0J1931972B03_QSPI_SUPPORT

__STATIC_INLINE void drvScreenAmoledS368X448Gc3A71Hk0J1931972B03PowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29,NULL,0);
    uteModulePlatformDelayMs(10);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenAmoledS368X448Gc3A71Hk0J1931972B03PowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenAmoledS368X448Gc3A71Hk0J1931972B03SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}
__STATIC_INLINE void drvScreenAmoledS368X448Gc3A71Hk0J1931972B03Init(void)
{
    uint8_t tmp[34];

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(1000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(5000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(120000);

    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0xFF, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x3A, tmp, 1); //RGB565
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x35, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x51,&light[0],1);
}

__STATIC_INLINE void drvScreenAmoledS368X448Gc3A71Hk0J1931972B03SetBackLight(uint8_t backLightPercent)
{
    uint8_t light[2];
    light[0] = backLightPercent*255/100;
    light[1] = 0;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x51,&light[0],2);//light
}

const ute_drv_screen_common_config_t uteDrvScreenAmoledS368X448Gc3A71Hk0J1931972B03Config =
{
    .name = "AmoledS368X448Gc3A71Hk0J1931972B03",
    .init = drvScreenAmoledS368X448Gc3A71Hk0J1931972B03Init,
    .powerOn = drvScreenAmoledS368X448Gc3A71Hk0J1931972B03PowerOn,
    .powerOff = drvScreenAmoledS368X448Gc3A71Hk0J1931972B03PowerOff,
    .setWindow = drvScreenAmoledS368X448Gc3A71Hk0J1931972B03SetWindow,
    .setBackLight = drvScreenAmoledS368X448Gc3A71Hk0J1931972B03SetBackLight,
};
#endif
#endif /* _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_ */


