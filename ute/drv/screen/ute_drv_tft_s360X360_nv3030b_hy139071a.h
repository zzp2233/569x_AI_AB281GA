#ifndef _UTE_DRV_TFT_S360X360_NV3030B_HY139071A_H_
#define _UTE_DRV_TFT_S360X360_NV3030B_HY139071A_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

__STATIC_INLINE void drvScreenTft360X360Nv3030BHy139071aInit(void)
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

    // WriteComm(0x80)
    memcpy(&tmp[0], "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x80, &tmp[0], 1);

    // WriteComm(0x81)
    memcpy(&tmp[0], "\x30", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x81, &tmp[0], 1);

    // WriteComm(0x82)
    memcpy(&tmp[0], "\x09", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x82, &tmp[0], 1);

    // WriteComm(0x83)
    memcpy(&tmp[0], "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, &tmp[0], 1);

    // WriteComm(0x84)
    memcpy(&tmp[0], "\x72", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, &tmp[0], 1);

    // WriteComm(0x89)
    memcpy(&tmp[0], "\x18", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x89, &tmp[0], 1);

    // WriteComm(0x8A)
    memcpy(&tmp[0], "\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8A, &tmp[0], 1);

    // WriteComm(0x8B)
    memcpy(&tmp[0], "\x0A", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8B, &tmp[0], 1);

    // WriteComm(0x3a)
    memcpy(&tmp[0], "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3a, &tmp[0], 1);

    // WriteComm(0x36)
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, &tmp[0], 1);

    // WriteComm(0xEC)
    memcpy(&tmp[0], "\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEC, &tmp[0], 1);

    // WriteComm(0x74)
    memcpy(&tmp[0], "\x01\x80\x00\x00\x00\x00", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x74, &tmp[0], 6);

    // WriteComm(0x98)
    memcpy(&tmp[0], "\x3E", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x98, &tmp[0], 1);

    // WriteComm(0x99)
    memcpy(&tmp[0], "\x3E", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x99, &tmp[0], 1);

    // WriteComm(0xA1)
    memcpy(&tmp[0], "\x01\x04", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA1, &tmp[0], 2);

    // WriteComm(0xA2)
    memcpy(&tmp[0], "\x01\x04", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA2, &tmp[0], 2);

    // WriteComm(0xCB)
    memcpy(&tmp[0], "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCB, &tmp[0], 1);

    // WriteComm(0x7C)
    memcpy(&tmp[0], "\xB6\x24", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7C, &tmp[0], 2);

    // WriteComm(0xAC)
    memcpy(&tmp[0], "\x34", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAC, &tmp[0], 1);

    // WriteComm(0xF6)
    memcpy(&tmp[0], "\x80", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF6, &tmp[0], 1);

    // WriteComm(0xB5)
    memcpy(&tmp[0], "\x0D\x0D", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, &tmp[0], 2);

    // WriteComm(0xEB)
    memcpy(&tmp[0], "\x01\x67", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEB, &tmp[0], 2);

    // WriteComm(0x60)
    memcpy(&tmp[0], "\x38\x0E\x1F\x56", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x60, &tmp[0], 4);

    // WriteComm(0x63)
    memcpy(&tmp[0], "\x38\x10\x1F\x56", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, &tmp[0], 4);

    // WriteComm(0x64)
    memcpy(&tmp[0], "\x38\x12\x72\xEA\x1F\x56", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, &tmp[0], 6);

    // WriteComm(0x66)
    memcpy(&tmp[0], "\x38\x16\x72\xEE\x1F\x56", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, &tmp[0], 6);

    // WriteComm(0x68)
    memcpy(&tmp[0], "\x00\x0A\xEE\x0A\xEE\x1C\x1C", 7);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, &tmp[0], 7);

    // WriteComm(0x69)
    memcpy(&tmp[0], "\x00\x0A\xF2\x0A\xF2\x1C\x1C", 7);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x69, &tmp[0], 7);

    // WriteComm(0x6E)
    memcpy(&tmp[0], "\x07\x01\x1A\x00\x0B\x0B\x09\x09\x13\x13\x11\x11\x19\x1E\x1D\x00\x00\x1D\x1E\x19\x12\x12\x14\x14\x0A\x0A\x0C\x0C\x00\x1A\x02\x08", 32);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6E, &tmp[0], 32);

    // WriteComm(0x6C)
    memcpy(&tmp[0], "\xCC\x0C\xCC\x84\xCC\x04\x5F", 7);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6C, &tmp[0], 7);

    // WriteComm(0x7D)
    memcpy(&tmp[0], "\x72", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7D, &tmp[0], 1);

    // WriteComm(0x70)
    memcpy(&tmp[0], "\x02\x03\x09\x07\x09\x03\x09\x07\x09\x03", 10);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x70, &tmp[0], 10);

    // WriteComm(0x90)
    memcpy(&tmp[0], "\x06\x06\x05\x06", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x90, &tmp[0], 4);

    // WriteComm(0x93)
    memcpy(&tmp[0], "\x45\xFF\x00", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x93, &tmp[0], 3);

    // WriteComm(0xC3)
    memcpy(&tmp[0], "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, &tmp[0], 1);

    // WriteComm(0xC4)
    memcpy(&tmp[0], "\x36", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 1);

    // WriteComm(0xC9)
    memcpy(&tmp[0], "\x3f", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC9, &tmp[0], 1);

    // WriteComm(0xF0)
    memcpy(&tmp[0], "\x4a\x09\x0a\x06\x00\x2e", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, &tmp[0], 6);

    // WriteComm(0xF2)
    memcpy(&tmp[0], "\x4a\x0a\x0a\x05\x00\x2e", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF2, &tmp[0], 6);

    // WriteComm(0xF1)
    memcpy(&tmp[0], "\x47\xD3\x9C\x2D\x2F\xCF", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, &tmp[0], 6);

    // WriteComm(0xF3)
    memcpy(&tmp[0], "\x46\xD2\x9C\x2D\x2F\xCF", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF3, &tmp[0], 6);

    // WriteComm(0xBE)
    memcpy(&tmp[0], "\x11", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, &tmp[0], 1);

    // WriteComm(0xFB)
    memcpy(&tmp[0], "\x00\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFB, &tmp[0], 2);

    // WriteComm(0xB4)
    memcpy(&tmp[0], "\x0A", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB4, &tmp[0], 1);

    // WriteComm(0x35)
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 1);

    // WriteComm(0x44)
    memcpy(&tmp[0], "\x00\x90", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x44, &tmp[0], 2);

    // WriteComm(0xfe)
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfe, &tmp[0], 1);

    // WriteComm(0xee)
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xee, &tmp[0], 1);

    // WriteComm(0x11)
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11, &tmp[0], 1);
    uteModulePlatformDelayUs(120000);

    // WriteComm(0x29)
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29, &tmp[0], 1);
    uteModulePlatformDelayUs(120000);

    // WriteComm(0x2C)
    memcpy(&tmp[0], "\x00\x00\x00\x00", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2C, &tmp[0], 4);

    // 再次 WriteComm(0x2C)
    memcpy(&tmp[0], "\x00\x00\x00\x00", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2C, &tmp[0], 4);
    uteModulePlatformDelayUs(120000);
}

const ute_drv_screen_common_config_t uteDrvScreenTft360X360Nv3030BHy139071aConfig =
{
    .name = "Tft360X360Nv3030BHy139071aQspi",
    .init = drvScreenTft360X360Nv3030BHy139071aInit,
    .powerOn = drvScreenTft360X360Nv3030BHy139071aPowerOn,
    .powerOff = drvScreenTft360X360Nv3030BHy139071aPowerOff,
    .setWindow = drvScreenTft360X360Nv3030BHy139071aSetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


