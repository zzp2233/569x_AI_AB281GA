#ifndef _UTE_DRV_TFT_S240X284_C_JD9853_WV018LZQ_QSPI_H_
#define _UTE_DRV_TFT_S240X284_C_JD9853_WV018LZQ_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X284_C_JD9853_WV018LZQ_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284CJd9853Wv018LzqPowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29,NULL,0);
    uteModulePlatformDelayMs(10);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284CJd9853Wv018LzqPowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284CJd9853Wv018LzqSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    uint8_t data[4];

    data[0] = xStart >> 8;
    data[1] = xStart & 0xff;
    data[2] = xEnd >> 8;
    data[3] = xEnd & 0xff;
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2A, data, 4);

    data[0] = yStart >> 8;
    data[1] = yStart & 0xff;
    data[2] = yEnd >> 8;
    data[3] = yEnd & 0xff;
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x2B, data, 4);

    uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}
__STATIC_INLINE void drvScreenTft240X284CJd9853Wv018LzqInit(void)
{
    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(50);

    uint8_t tmp[34];

    memcpy(&tmp[0], "\x98\x53", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, &tmp[0], 2);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x22", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, &tmp[0], 1);

    memcpy(&tmp[0], "\x00\x2D\x00\x55", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, &tmp[0], 4);

    memcpy(&tmp[0], "\x1F\x9A\x55\x73\x63\xF0", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, &tmp[0], 6);

    memcpy(&tmp[0], "\x24\xA4", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, &tmp[0], 2);

    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, &tmp[0], 1);

    memcpy(&tmp[0], "\x7D\x07\x14\x06\xC8\x71\x6C\x77", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, &tmp[0], 8);

    memcpy(&tmp[0], "\x00\x00\x8E\x79\x1E\x1A\x16\x79\x1E\x1A\x16\x82", 12);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 12);

    memcpy(&tmp[0],
           "\x3F\x2C\x21\x1A\x1F\x20\x1B\x1B\x19\x19\x16\x0A\x08\x03\x05\x00"
           "\x3F\x2C\x21\x1A\x1F\x20\x1B\x1B\x19\x19\x16\x0A\x08\x03\x05\x00",
           32);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, &tmp[0], 32);

    memcpy(&tmp[0], "\x04\x06\x6B\x0F\x00", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, &tmp[0], 5);

    memcpy(&tmp[0], "\x00\x30", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, &tmp[0], 2);

    memcpy(&tmp[0], "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, &tmp[0], 1);

    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x03\x13\xEF\x35\x35", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, &tmp[0], 5);

    memcpy(&tmp[0], "\x14\x15\xC0", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, &tmp[0], 3);

    memcpy(&tmp[0], "\x06\x3A", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, &tmp[0], 2);

    memcpy(&tmp[0], "\x72\x12", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 2);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 1);

    memcpy(&tmp[0], "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, &tmp[0], 1);

    memcpy(&tmp[0], "\x00\x00\x00\xEF", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2A, &tmp[0], 4);

    memcpy(&tmp[0], "\x00\x00\x01\x1B", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2B, &tmp[0], 4);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284CJd9853Wv018LzqConfig =
{
    .name = "Tft240X284CJd9853Wv018Lzq",
    .init = drvScreenTft240X284CJd9853Wv018LzqInit,
    .powerOn = drvScreenTft240X284CJd9853Wv018LzqPowerOn,
    .powerOff = drvScreenTft240X284CJd9853Wv018LzqPowerOff,
    .setWindow = drvScreenTft240X284CJd9853Wv018LzqSetWindow,
    .setBackLight = NULL,
    //.refreshRate = 60,
    //.startLine = 0,
    //.gramMaxLine = 284,
};
#endif
#endif /* _UTE_DRV_TFT_S240X284_C_JD9853_WV018LZQ_QSPI_H_ */


