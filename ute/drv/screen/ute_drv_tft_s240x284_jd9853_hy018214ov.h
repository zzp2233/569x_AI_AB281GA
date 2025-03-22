#ifndef _UTE_DRV_TFT_S240X284_JD9853_HY018214OV_H_
#define _UTE_DRV_TFT_S240X284_JD9853_HY018214OV_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X284_JD9853_HY018214OV_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284Jd9853Hy018214OvPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Hy018214OvPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Hy018214OvSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

__STATIC_INLINE void drvScreenTft240X284Jd9853Hy018214OvInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uint8_t tmp[34];

    memcpy(&tmp[0], "\x98\x53", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, &tmp[0], 2);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x22", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, &tmp[0], 1);

    memcpy(&tmp[0], "\x00\x21\x00\x49", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, &tmp[0], 4);

    memcpy(&tmp[0], "\x24\x2B\x27\x61\x63\xF0", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, &tmp[0], 6);

    memcpy(&tmp[0], "\x26\xA6", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, &tmp[0], 2);

    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, &tmp[0], 1);

    memcpy(&tmp[0], "\x7D\x07\x14\x06\xC8\x71\x6C\x77", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, &tmp[0], 8);

    memcpy(&tmp[0], "\x00\x00\x8E\xB6\x25\x0A\x16\xB6\x25\x0A\x16\x82", 12);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 12);

    memcpy(&tmp[0], "\x3F\x32\x29\x23\x28\x29\x24\x24\x22\x21\x1F\x13\x0F\x09\x03\x00\x3F\x32\x29\x23\x28\x29\x24\x24\x22\x21\x1F\x13\x0F\x09\x03\x00", 32);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, &tmp[0], 32);

    memcpy(&tmp[0], "\x77", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBC, &tmp[0], 1);

    memcpy(&tmp[0], "\x04\x06\x6B\x0F\x00", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, &tmp[0], 5);

    memcpy(&tmp[0], "\x00\x20", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, &tmp[0], 2);

    memcpy(&tmp[0], "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, &tmp[0], 1);

    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, &tmp[0], 1);

    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, &tmp[0], 1);

    memcpy(&tmp[0], "\x03\x13\xEF\x35\x35", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, &tmp[0], 5);

    memcpy(&tmp[0], "\x14\x15\xC0", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, &tmp[0], 3);

    memcpy(&tmp[0], "\x06\x3A\xC7", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, &tmp[0], 3);

    memcpy(&tmp[0], "\x72\x12", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 2);

    memcpy(&tmp[0], "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC5, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, &tmp[0], 1);

    memcpy(&tmp[0], "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x03\x5A", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBD, &tmp[0], 2);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, &tmp[0], 1);

    memcpy(&tmp[0], "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, &tmp[0], 1);

    memcpy(&tmp[0], "\x00\x00\x00\xEF", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2A, &tmp[0], 4);

    memcpy(&tmp[0], "\x00\x00\x01\x1B", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2B, &tmp[0], 4);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284Jd9853Hy018214OvConfig =
{
    .name = "Tft240X284Jd9853Hy018214OvQspi",
    .init = drvScreenTft240X284Jd9853Hy018214OvInit,
    .powerOn = drvScreenTft240X284Jd9853Hy018214OvPowerOn,
    .powerOff = drvScreenTft240X284Jd9853Hy018214OvPowerOff,
    .setWindow = drvScreenTft240X284Jd9853Hy018214OvSetWindow,
    .setBackLight = NULL,
};
