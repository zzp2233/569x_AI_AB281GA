#ifndef UTE_DRV_BOE_S240X284_I183_JD9853_HKOE1912534B01_H
#define UTE_DRV_BOE_S240X284_I183_JD9853_HKOE1912534B01_H

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if DRV_BOE_S240X296_I183_JD9853_HKOE1912534B01_QSPI_SUPPORT

__STATIC_INLINE void drvScreenBoeHkoe1912534b01PowerOn(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x11,NULL,0);
    uteModulePlatformDelayUs(120000);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x29,NULL,0);
    uteModulePlatformDelayUs(20000);
    // UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenBoeHkoe1912534b01PowerOff(void)
{
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x28,NULL,0);
    uteModulePlatformDelayUs(20000);
    uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(0x10,NULL,0);
    uteModulePlatformDelayUs(50000);
    // UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenBoeHkoe1912534b01SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

    // uteModulePlatformOutputGpioSet(UTE_DRV_SCREEN_CS_GPIO_PIN,false);
}
__STATIC_INLINE void drvScreenBoeHkoe1912534b01Init(void)
{
    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(20000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(20000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(20000);
    // 定义临时缓冲区
    uint8_t tmp[256];

    delay_ms(50);

    // SSD_CMD(0xDF) + 2个SSD_PAR
    memcpy(&tmp[0], "\x98\x53", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, &tmp[0], 2);

    // SSD_CMD(0xDE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    // SSD_CMD(0xD8) + 2个SSD_PAR
    memcpy(&tmp[0], "\x08\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD8, &tmp[0], 2);

    // SSD_CMD(0xB2) + 1个SSD_PAR
    memcpy(&tmp[0], "\x15", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, &tmp[0], 1);

    // SSD_CMD(0xB7) + 4个SSD_PAR
    memcpy(&tmp[0], "\x00\x35\x00\x5D", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, &tmp[0], 4);

    // SSD_CMD(0xBB) + 6个SSD_PAR
    memcpy(&tmp[0], "\x44\x12\x17\x71\x73\xF0", 6);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, &tmp[0], 6);

    // SSD_CMD(0xC0) + 2个SSD_PAR
    memcpy(&tmp[0], "\x26\xA6", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, &tmp[0], 2);

    // SSD_CMD(0xC1) + 1个SSD_PAR
    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, &tmp[0], 1);

    // SSD_CMD(0xC3) + 8个SSD_PAR
    memcpy(&tmp[0], "\x7D\x08\x0A\x0C\xC4\x73\x22\x77", 8);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, &tmp[0], 8);

    // SSD_CMD(0xC4) + 11个SSD_PAR
    memcpy(&tmp[0], "\x00\x00\x94\xA8\x25\x0B\x16\x79\x0B\x0B\x16\x82", 11);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 11);

    // SSD_CMD(0xBC) + 1个SSD_PAR
    memcpy(&tmp[0], "\x77", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBC, &tmp[0], 1);

    // SSD_CMD(0xC8) + 32个SSD_PAR（Gamma2.2参数）
    memcpy(&tmp[0],
           "\x3F\x24\x1B\x15\x1C\x1F\x1B\x1C\x1C\x1B\x19\x0E\x0B\x05\x02\x00"
           "\x3F\x24\x1B\x15\x1C\x1F\x1B\x1C\x1C\x1B\x19\x0E\x0B\x05\x02\x00",
           32);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, &tmp[0], 32);

    // SSD_CMD(0xD0) + 5个SSD_PAR
    memcpy(&tmp[0], "\x04\x04\x6C\x1C\x03", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, &tmp[0], 5);

    // SSD_CMD(0xD7) + 2个SSD_PAR
    memcpy(&tmp[0], "\x00\x30", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, &tmp[0], 2);

    // SSD_CMD(0xE6) + 1个SSD_PAR
    memcpy(&tmp[0], "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, &tmp[0], 1);

    // SSD_CMD(0xDE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    // SSD_CMD(0xBB) + 1个SSD_PAR
    memcpy(&tmp[0], "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, &tmp[0], 1);

    // SSD_CMD(0xD7) + 1个SSD_PAR
    memcpy(&tmp[0], "\x12", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, &tmp[0], 1);

    // SSD_CMD(0xB7) + 5个SSD_PAR
    memcpy(&tmp[0], "\x03\x13\xE5\x38\x38", 5);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, &tmp[0], 5);

    // SSD_CMD(0xC1) + 3个SSD_PAR
    memcpy(&tmp[0], "\x14\x15\xC0", 3);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, &tmp[0], 3);

    // SSD_CMD(0xC2) + 2个SSD_PAR
    memcpy(&tmp[0], "\x06\x3A", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, &tmp[0], 2);

    // SSD_CMD(0xC4) + 2个SSD_PAR
    memcpy(&tmp[0], "\x72\x12", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, &tmp[0], 2);

    // SSD_CMD(0xC5) + 1个SSD_PAR
    memcpy(&tmp[0], "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC5, &tmp[0], 1);

    // SSD_CMD(0xBE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, &tmp[0], 1);

    // SSD_CMD(0xDE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    // SSD_CMD(0x20) + 0个SSD_PAR
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x20, &tmp[0], 0);

    // SSD_CMD(0x35) + 0个SSD_PAR
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, &tmp[0], 0);

    // SSD_CMD(0x36) + 1个SSD_PAR
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, &tmp[0], 1);

    // SSD_CMD(0x3A) + 1个SSD_PAR
    memcpy(&tmp[0], "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, &tmp[0], 1);

    // SSD_CMD(0x2A) + 4个SSD_PAR
    memcpy(&tmp[0], "\x00\x00\x00\xEF", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2A, &tmp[0], 4);

    // SSD_CMD(0x2B) + 4个SSD_PAR
    memcpy(&tmp[0], "\x00\x00\x01\x27", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2B, &tmp[0], 4);

    // SSD_CMD(0x11) + 0个SSD_PAR
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11, &tmp[0], 0);
    uteModulePlatformDelayMs(120);

    // SSD_CMD(0xDE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    // SSD_CMD(0xB5) + 1个SSD_PAR
    memcpy(&tmp[0], "\x23", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, &tmp[0], 1);

    // SSD_CMD(0xDE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    // SSD_CMD(0xDE) + 1个SSD_PAR
    memcpy(&tmp[0], "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDE, &tmp[0], 1);

    // SSD_CMD(0x29) + 0个SSD_PAR
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29, &tmp[0], 0);
    uteModulePlatformDelayMs(10);

    // SSD_CMD(0xDF) + 2个SSD_PAR
    memcpy(&tmp[0], "\x00\x00", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xDF, &tmp[0], 2);
}
const ute_drv_screen_common_config_t uteDrvScreenBoe240X296Hkoe1912534b01Config =
{
    .name = "Boe240X284Hkoe1912534b01",
    .init = drvScreenBoeHkoe1912534b01Init,
    .powerOn = drvScreenBoeHkoe1912534b01PowerOn,
    .powerOff = drvScreenBoeHkoe1912534b01PowerOff,
    .setWindow = drvScreenBoeHkoe1912534b01SetWindow,
    .setBackLight = NULL,
    .refreshRate = 60,
    .startLine = 0,
    .gramMaxLine = 284,
};
#endif
#endif /* _UTE_DRV_BOE_S240X284_I183_WV8LZQ_QSPI_H_ */


