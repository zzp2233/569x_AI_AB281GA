#ifndef _UTE_DRV_TFT_S240X284_JD9853_HKOE19124B01_H_
#define _UTE_DRV_TFT_S240X284_JD9853_HKOE19124B01_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X296_JD9853_HKOE19124B01_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284Jd9853Hkoe19124B01PowerOn(void)
{
    uint8_t tmp[4];
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11, NULL, 0);
    uteModulePlatformDelayMs(50);
    memcpy(tmp, "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x23", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29, NULL, 0);
    uteModulePlatformDelayMs(10);
    memcpy(tmp, "\x00\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, tmp, 2);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Hkoe19124B01PowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Hkoe19124B01SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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
    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Hkoe19124B01Init(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(30000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(30000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(50000);

    uint8_t tmp[34];
    memcpy(tmp, "\x98\x53", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, tmp, 2);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x08\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD8, tmp, 2);
    memcpy(tmp, "\x15", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, tmp, 1);
    memcpy(tmp, "\x00\x35\x00\x5D", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, tmp, 4);
    memcpy(tmp, "\x44\x12\x17\x71\x73\xF0", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, tmp, 6);
    memcpy(tmp, "\x26\xA6", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, tmp, 2);
    memcpy(tmp, "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, tmp, 1);
    memcpy(tmp, "\x7D\x08\x0A\x0C\xC4\x73\x22\x77", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, tmp, 8);
    memcpy(tmp, "\x00\x00\x94\xA8\x25\x0B\x16\x79\x0B\x0B\x16\x82", 12);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, tmp, 12);
    memcpy(tmp, "\x77", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBC, tmp, 1);
    memcpy(tmp, "\x3F\x24\x1B\x15\x1C\x1F\x1B\x1C\x1C\x1B\x19\x0E\x0B\x05\x02\x00\x3F\x24\x1B\x15\x1C\x1F\x1B\x1C\x1C\x1B\x19\x0E\x0B\x05\x02\x00", 32);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, tmp, 32);
    memcpy(tmp, "\x04\x04\x6C\x1C\x03", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, tmp, 5);
    memcpy(tmp, "\x00\x30", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, tmp, 2);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, tmp, 1);
    memcpy(tmp, "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, tmp, 1);
    memcpy(tmp, "\x03\x13\xE5\x38\x38", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, tmp, 5);
    memcpy(tmp, "\x14\x15\xC0", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, tmp, 3);
    memcpy(tmp, "\x06\x3A", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, tmp, 2);
    memcpy(tmp, "\x72\x12", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, tmp, 2);
    memcpy(tmp, "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC5, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x20, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, NULL, 0);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, tmp, 1);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284Jd9853Hkoe19124B01Config =
{
    .name = "Tft240X284Jd9853Hkoe19124B01Qspi",
    .init = drvScreenTft240X284Jd9853Hkoe19124B01Init,
    .powerOn = drvScreenTft240X284Jd9853Hkoe19124B01PowerOn,
    .powerOff = drvScreenTft240X284Jd9853Hkoe19124B01PowerOff,
    .setWindow = drvScreenTft240X284Jd9853Hkoe19124B01SetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


