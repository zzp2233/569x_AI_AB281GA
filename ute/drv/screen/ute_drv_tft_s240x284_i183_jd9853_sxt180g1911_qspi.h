#ifndef _UTE_DRV_TFT_S240X284_I183_JD9853_SXT180G1911_QSPI_H_
#define _UTE_DRV_TFT_S240X284_I183_JD9853_SXT180G1911_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X284_I183_JD9853_SXT180G1911_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft240X284Jd9853Sxt180g1911PowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29,NULL,0);
    uteModulePlatformDelayMs(10);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Sxt180g1911PowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft240X284Jd9853Sxt180g1911SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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
__STATIC_INLINE void drvScreenTft240X284Jd9853Sxt180g1911Init(void)
{
    uint8_t tmp[34];

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(50);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x01, NULL, 0);
    uteModulePlatformDelayMs(10);
    memcpy(tmp, "\x98\x53", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, tmp, 2);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x23", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, tmp, 1);
    memcpy(tmp, "\x00\x21\x00\x49", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, tmp, 4);
    memcpy(tmp, "\x1D\x9A\x55\x73\x63\xF0", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, tmp, 6);
    memcpy(tmp, "\x22\x22", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, tmp, 2);
    memcpy(tmp, "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, tmp, 1);
    memcpy(tmp, "\x7D\x07\x14\x06\xC8\x71\x6C\x77", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, tmp, 8);
    memcpy(tmp, "\x00\x00\x8E\x79\x1E\x1A\x16\x79\x1E\x1A\x16\x82", 12);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, tmp, 12);
    memcpy(tmp, "\x3F\x3B\x36\x3C\x38\x3D\x38\x38\x36\x33\x2E\x1D\x17\x10\x0A\x00\x3F\x3B\x36\x3C\x38\x3D\x38\x38\x36\x33\x2E\x1D\x17\x10\x0A\x00", 32);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, tmp, 32);
    memcpy(tmp, "\x04\x06\x6B\x0F\x00", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, tmp, 5);
    memcpy(tmp, "\x00\x30", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, tmp, 2);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x03\x13\xEF\x35\x35", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, tmp, 5);
    memcpy(tmp, "\x14\x15\xC0", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, tmp, 3);
    memcpy(tmp, "\x06\x3A\xC7", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, tmp, 3);
    memcpy(tmp, "\x72\x12", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, tmp, 2);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, tmp, 1);
}

const ute_drv_screen_common_config_t uteDrvScreenTft240X284Jd9853Sxt180g1911Config =
{
    .name = "Tft240X284Jd9853Sxt180g1911",
    .init = drvScreenTft240X284Jd9853Sxt180g1911Init,
    .powerOn = drvScreenTft240X284Jd9853Sxt180g1911PowerOn,
    .powerOff = drvScreenTft240X284Jd9853Sxt180g1911PowerOff,
    .setWindow = drvScreenTft240X284Jd9853Sxt180g1911SetWindow,
    .setBackLight = NULL,
    .refreshRate = 60,
    .startLine = 0,
    .gramMaxLine = 284,
};
#endif
#endif /* _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_ */


