#ifndef _UTE_DRV_TFT_S240X296_GC9309_JS202018A_QSPI_H_
#define _UTE_DRV_TFT_S240X296_GC9309_JS202018A_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X296_GC9309_JS202018A_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X296Gc9309Js202018aPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11, NULL, 0);
    uteModulePlatformDelayMs(80);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe8, (uint8_t *)"\xa0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe8, (uint8_t *)"\xf0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2c, NULL, 0);
    uteModulePlatformDelayMs(10);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfe, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xee, NULL, 0);

    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X296Gc9309Js202018aPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X296Gc9309Js202018aSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    yStart += 0;
    yEnd += 0;

    uint8_t data[4];
    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2A, data, 4);
    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2B, data, 4);
    // uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}

__STATIC_INLINE void drvScreenTft240X296Gc9309Js202018aInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uint8_t tmp[34];

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(1000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(5000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(120000);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFE, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEF, NULL, 0);
    memcpy(tmp, "\xC0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x80, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x81, tmp, 1);
    memcpy(tmp, "\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x82, tmp, 1);
    memcpy(tmp, "\x38", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, tmp, 1);
    memcpy(tmp, "\x64", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x88, tmp, 1);
    memcpy(tmp, "\x86", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x89, tmp, 1);
    memcpy(tmp, "\x3C", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8B, tmp, 1);
    memcpy(tmp, "\x51", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8D, tmp, 1);
    memcpy(tmp, "\x50", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8E, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, tmp, 1);
    memcpy(tmp, "\x1C", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBF, tmp, 1);
    memcpy(tmp, "\x45\x06", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7D, tmp, 2);
    memcpy(tmp, "\x00\x06", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEE, tmp, 2);
    memcpy(tmp, "\x53", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF4, tmp, 1);
    memcpy(tmp, "\x17\x08", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF6, tmp, 2);
    memcpy(tmp, "\x4F\x4F", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x70, tmp, 2);
    memcpy(tmp, "\x12\x20", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x71, tmp, 2);
    memcpy(tmp, "\x12\x20", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x72, tmp, 2);
    memcpy(tmp, "\x25", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA9, tmp, 1);
    memcpy(tmp, "\x50", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBA, tmp, 1);
    memcpy(tmp, "\x71\x55", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEC, tmp, 2);
    memcpy(tmp, "\x00\x0D", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7B, tmp, 2);
    memcpy(tmp, "\x0D\x03", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7C, tmp, 2);
    memcpy(tmp, "\x02\x10\x12", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF5, tmp, 3);
    memcpy(tmp, "\x00\x00\x00\xEF", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2A, tmp, 4);
    memcpy(tmp, "\x00\x00\x01\x17", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2B, tmp, 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2C, NULL, 0);
    memcpy(tmp, "\x09\x0C\x0B\x0A\x05\x2D\x43\xAE\x9C\x2E\x30\x7F", 12);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, tmp, 12);
    memcpy(tmp, "\x08\x0C\x0B\x07\x07\x2D\x44\x5D\x8F\x26\x2C\xCF", 12);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, tmp, 12);
    memcpy(tmp, "\x30", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, tmp, 1);
    memcpy(tmp, "\x19", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, tmp, 1);
    memcpy(tmp, "\x29", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC7, tmp, 1);
    memcpy(tmp, "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCA, tmp, 1);
    memcpy(tmp, "\xF0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE8, tmp, 1);
    memcpy(tmp, "\x06", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCB, tmp, 1);
    memcpy(tmp, "\x5C\x40\x40", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB6, tmp, 3);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCC, tmp, 1);
    memcpy(tmp, "\x33", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCD, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, tmp, 1);

}

const ute_drv_screen_common_config_t uteDrvScreenTft240X296Gc9309Js202018aConfig =
{
    .name = "Tft240X296Gc9309Js202018aQspi",
    .init = drvScreenTft240X296Gc9309Js202018aInit,
    .powerOn = drvScreenTft240X296Gc9309Js202018aPowerOn,
    .powerOff = drvScreenTft240X296Gc9309Js202018aPowerOff,
    .setWindow = drvScreenTft240X296Gc9309Js202018aSetWindow,
    .setBackLight = NULL,
};

#endif
#endif


