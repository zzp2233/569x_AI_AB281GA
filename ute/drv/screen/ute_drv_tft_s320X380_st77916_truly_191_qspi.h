#ifndef _UTE_DRV_TFT_S320_X380_ST77916_TRULY_191_QSPI_H_
#define _UTE_DRV_TFT_S320_X380_ST77916_TRULY_191_QSPI_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S320_X380_ST77916_TRULY_191_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft320X380St77916Truly191PowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft320X380St77916Truly191PowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft320X380St77916Truly191SetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

__STATIC_INLINE void drvScreenTft320X380St77916Truly191Init(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(50000);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayUs(50000);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayUs(120000);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x28", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF2, (uint8_t *)"\x28", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, (uint8_t *)"\xF0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7C, (uint8_t *)"\xD1", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, (uint8_t *)"\xE0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, (uint8_t *)"\x61", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF2, (uint8_t *)"\x82", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB0, (uint8_t *)"\x5B", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB1, (uint8_t *)"\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, (uint8_t *)"\x2D", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB3, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB4, (uint8_t *)"\x69", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, (uint8_t *)"\x45", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB6, (uint8_t *)"\xAB", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, (uint8_t *)"\x41", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB8, (uint8_t *)"\x86", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB9, (uint8_t *)"\x15", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBA, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, (uint8_t *)"\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBC, (uint8_t *)"\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBD, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBF, (uint8_t *)"\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, (uint8_t *)"\x80", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, (uint8_t *)"\x09", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, (uint8_t *)"\x36", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, (uint8_t *)"\x80", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, (uint8_t *)"\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC5, (uint8_t *)"\x35", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC6, (uint8_t *)"\xA9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC7, (uint8_t *)"\x41", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC9, (uint8_t *)"\xA9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCA, (uint8_t *)"\x41", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCB, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCC, (uint8_t *)"\x7F", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCD, (uint8_t *)"\x7F", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xCE, (uint8_t *)"\xFF", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, (uint8_t *)"\xD1", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD1, (uint8_t *)"\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD2, (uint8_t *)"\x7C", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF5, (uint8_t *)"\x00\xA5", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF1, (uint8_t *)"\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x02", 1);

    // 连续 set 合并（E0 命令）
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE0, (uint8_t *)"\xF0\x0A\x10\x0A\x09\x05\x37\x33\x4F\x19\x18\x17\x2E\x30", 14);
    // E1 命令
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE1, (uint8_t *)"\xF0\x09\x0F\x09\x09\x15\x37\x32\x4F\x39\x17\x17\x2D\x2F", 14);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF3, (uint8_t *)"\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE0, (uint8_t *)"\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE1, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE2, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE3, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE4, (uint8_t *)"\xC0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE5, (uint8_t *)"\x06", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE6, (uint8_t *)"\x21", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE7, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE8, (uint8_t *)"\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xE9, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEA, (uint8_t *)"\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEB, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEC, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xED, (uint8_t *)"\x14", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEE, (uint8_t *)"\xFF", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xEF, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF8, (uint8_t *)"\xFF", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF9, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFA, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFB, (uint8_t *)"\x30", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFC, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFD, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFE, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xFF, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x60, (uint8_t *)"\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x61, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x62, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, (uint8_t *)"\x43", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x65, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x69, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6A, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6B, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x70, (uint8_t *)"\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x71, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x72, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, (uint8_t *)"\x43", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x74, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x75, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x76, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x77, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x78, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x79, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7A, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7B, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x80, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x81, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x82, (uint8_t *)"\x06", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, (uint8_t *)"\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, (uint8_t *)"\xFE", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x85, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x86, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x87, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x88, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x89, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8A, (uint8_t *)"\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8B, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8C, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8D, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8E, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8F, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x90, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x91, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x92, (uint8_t *)"\x0A", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x93, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x94, (uint8_t *)"\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x95, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x96, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x97, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x98, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x99, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9A, (uint8_t *)"\x0C", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9B, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9C, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9D, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9E, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9F, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA0, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA1, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA2, (uint8_t *)"\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA3, (uint8_t *)"\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA4, (uint8_t *)"\xFD", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA5, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA6, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA7, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA8, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xA9, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAA, (uint8_t *)"\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAB, (uint8_t *)"\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAC, (uint8_t *)"\xFF", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAD, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAE, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xAF, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB0, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB1, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB2, (uint8_t *)"\x09", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB3, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB4, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB5, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB6, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB7, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB8, (uint8_t *)"\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xB9, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBA, (uint8_t *)"\x0B", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBB, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBC, (uint8_t *)"\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBD, (uint8_t *)"\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBE, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xBF, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC0, (uint8_t *)"\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC1, (uint8_t *)"\x47", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC2, (uint8_t *)"\x56", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC3, (uint8_t *)"\x65", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC4, (uint8_t *)"\x74", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC5, (uint8_t *)"\x88", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC6, (uint8_t *)"\x99", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC7, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC8, (uint8_t *)"\xBB", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xC9, (uint8_t *)"\xAA", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD0, (uint8_t *)"\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD1, (uint8_t *)"\x47", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD2, (uint8_t *)"\x56", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD3, (uint8_t *)"\x65", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD4, (uint8_t *)"\x74", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD5, (uint8_t *)"\x88", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD6, (uint8_t *)"\x99", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD7, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD8, (uint8_t *)"\xBB", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xD9, (uint8_t *)"\xAA", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF3, (uint8_t *)"\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xF0, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, (uint8_t *)"\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, (uint8_t *)"\x55", 1);

    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x21, NULL, 0);  // 无参数命令
}
const ute_drv_screen_common_config_t uteDrvScreenTft320X380St77916Truly191Config =
{
    .name = "Tft320X380St77916Truly191Qspi",
    .init = drvScreenTft320X380St77916Truly191Init,
    .powerOn = drvScreenTft320X380St77916Truly191PowerOn,
    .powerOff = drvScreenTft320X380St77916Truly191PowerOff,
    .setWindow = drvScreenTft320X380St77916Truly191SetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S360_X360_ST77916_TRULY_191_QSPI_H_ */


