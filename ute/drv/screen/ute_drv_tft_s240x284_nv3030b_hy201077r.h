#ifndef _UTE_DRV_TFT_S240X284_NV3030B_HY201077R_H_
#define _UTE_DRV_TFT_S240X284_NV3030B_HY201077R_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X284_NV3030B_HY201077R_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201077rPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201077rPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201077rSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    yStart += 12;
    yEnd += 12;

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
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}

__STATIC_INLINE void drvScreenTft240X284Nv3030BHy201077rInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(1000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(5000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(120000);

    uint8_t tmp[16];  // 假设最大参数长度为16字节

    memcpy(&tmp[0], "\x06\x08", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFD, &tmp[0], 2);

    memcpy(&tmp[0], "\x07\x07", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x61, &tmp[0], 2);

    memcpy(&tmp[0], "\x70", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, &tmp[0], 1);

    memcpy(&tmp[0], "\x00\x44\x40", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x62, &tmp[0], 3);

    memcpy(&tmp[0], "\x41\x07\x12\x12", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, &tmp[0], 4);

    memcpy(&tmp[0], "\x37", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, &tmp[0], 1);

    memcpy(&tmp[0], "\x09\x10\x21", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x65, &tmp[0], 3);

    memcpy(&tmp[0], "\x09\x10\x21", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, &tmp[0], 3);

    memcpy(&tmp[0], "\x21\x40", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, &tmp[0], 2);

    memcpy(&tmp[0], "\x9c\x30\x21\x27", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, &tmp[0], 4);

    memcpy(&tmp[0], "\x0F\x02\x02", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb1, &tmp[0], 3);

    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB4, &tmp[0], 1);

    memcpy(&tmp[0], "\x02\x02\x0A\x14", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, &tmp[0], 4);

    memcpy(&tmp[0], "\x04\x01\x9F\x00\x02", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB6, &tmp[0], 5);

    memcpy(&tmp[0], "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xdf, &tmp[0], 1);

    memcpy(&tmp[0], "\x23\x1D\x1E\x2F\x30\x3F", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE2, &tmp[0], 6);

    memcpy(&tmp[0], "\x3F\x30\x2F\x1E\x23\x22", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE5, &tmp[0], 6);

    memcpy(&tmp[0], "\x25\x6B", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE1, &tmp[0], 2);

    memcpy(&tmp[0], "\x6C\x25", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE4, &tmp[0], 2);

    memcpy(&tmp[0], "\x05\x05\x10\x0E\x10\x0F\x13\x18", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE0, &tmp[0], 8);

    memcpy(&tmp[0], "\x18\x13\x0F\x0F\x10\x09\x07\x05", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE3, &tmp[0], 8);

    memcpy(&tmp[0], "\x00\xFF", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, &tmp[0], 2);

    memcpy(&tmp[0], "\x01\x04\x03\x03\x00\x12", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE7, &tmp[0], 6);

    memcpy(&tmp[0], "\x00\x70\x00", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE8, &tmp[0], 3);

    memcpy(&tmp[0], "\x52", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEc, &tmp[0], 1);

    memcpy(&tmp[0], "\x01\x01\x02", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, &tmp[0], 3);

    memcpy(&tmp[0], "\x01\x30\x00\x00", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF6, &tmp[0], 4);

    memcpy(&tmp[0], "\xFA\xFC", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfd, &tmp[0], 2);

    memcpy(&tmp[0], "\x55", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3a, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 1);

    memcpy(&tmp[0], "\xc0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, &tmp[0], 1);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x21, &tmp[0], 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11, &tmp[0], 0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29, &tmp[0], 0);
    uteModulePlatformDelayMs(50);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284Nv3030BHy201077rConfig =
{
    .name = "Tft240X284Nv3030BHy201077rQspi",
    .init = drvScreenTft240X284Nv3030BHy201077rInit,
    .powerOn = drvScreenTft240X284Nv3030BHy201077rPowerOn,
    .powerOff = drvScreenTft240X284Nv3030BHy201077rPowerOff,
    .setWindow = drvScreenTft240X284Nv3030BHy201077rSetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


