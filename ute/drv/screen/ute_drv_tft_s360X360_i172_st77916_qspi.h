#ifndef _UTE_DRV_TFT_S360X360_GC9B71_ZD138G1616_H_
#define _UTE_DRV_TFT_S360X360_GC9B71_ZD138G1616_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S360X360_I172_ST77916_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft360X360I172St77916QspiPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2c, (uint8_t *)"\x00\x00\x00\x00", 4);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2c, (uint8_t *)"\x00\x00\x00\x00", 4);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X360I172St77916QspiPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X360I172St77916QspiSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

__STATIC_INLINE void drvScreenTft360X360I172St77916QspiInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
    uint8_t tmp[34];

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayMs(50);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(120);

    memcpy(tmp, "\x28", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    memcpy(tmp, "\x28", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf2, tmp, 1);
    memcpy(tmp, "\xf0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, tmp, 1);
    memcpy(tmp, "\xd1", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7c, tmp, 1);
    memcpy(tmp, "\xe0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, tmp, 1);
    memcpy(tmp, "\x61", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, tmp, 1);
    memcpy(tmp, "\x82", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf2, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    //uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf2, "\x08", 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf1, tmp, 1);// \x4b 调新屏，如不用就使用\x4b
    memcpy(tmp, "\x5f", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb0, tmp, 1);
    memcpy(tmp, "\x14", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb1, tmp, 1);
    memcpy(tmp, "\x45", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb2, tmp, 1);
    memcpy(tmp, "\x49", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb4, tmp, 1);
    memcpy(tmp, "\x35", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb5, tmp, 1);
    memcpy(tmp, "\x8b", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb6, tmp, 1);
    memcpy(tmp, "\x31", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb7, tmp, 1);
    memcpy(tmp, "\x86", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb8, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xba, tmp, 1);
    memcpy(tmp, "\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbb, tmp, 1);
    memcpy(tmp, "\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbc, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbd, tmp, 1);//60hz
    memcpy(tmp, "\x80", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc0, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc1, tmp, 1);
    memcpy(tmp, "\x37", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc2, tmp, 1);//60hz
    memcpy(tmp, "\x80", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc3, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc4, tmp, 1);
    memcpy(tmp, "\x37", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc5, tmp, 1);
    memcpy(tmp, "\xa9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc6, tmp, 1);
    memcpy(tmp, "\x41", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc7, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc8, tmp, 1);
    memcpy(tmp, "\xa9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc9, tmp, 1);
    memcpy(tmp, "\x41", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xca, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xcb, tmp, 1);
    memcpy(tmp, "\x91", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd0, tmp, 1);
    memcpy(tmp, "\x68", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd1, tmp, 1);
    memcpy(tmp, "\x68", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd2, tmp, 1);
    memcpy(tmp, "\x00\xa5", 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf5, tmp, 2);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf1, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);

    //gama
    //    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe0, "\xf0\x07\x0c\x0a\x0a\x06\x30\x44\x47\x18\x13\x14\x2d\x32", 14);
    //    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe1, "\xf0\x07\x0c\x0a\x09\x26\x30\x43\x47\x37\x13\x13\x2d\x31", 14);
    memcpy(tmp, "\xf0\x0b\x12\x0b\x0a\x06\x39\x43\x4f\x07\x14\x14\x2f\x34", 14);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe0, tmp, 14);
    memcpy(tmp, "\xf0\x0b\x11\x0a\x09\x05\x32\x33\x48\x07\x13\x13\x2c\x33", 14);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe1, tmp, 14);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf3, tmp, 1);
    memcpy(tmp, "\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe0, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe1, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe2, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe3, tmp, 1);
    memcpy(tmp, "\xe0", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe4, tmp, 1);
    memcpy(tmp, "\x06", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe5, tmp, 1);
    memcpy(tmp, "\x21", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe6, tmp, 1);
    memcpy(tmp, "\x03", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe7, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe8, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe9, tmp, 1);
    memcpy(tmp, "\xe9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xea, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xeb, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xec, tmp, 1);
    memcpy(tmp, "\x14", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xed, tmp, 1);
    memcpy(tmp, "\xff", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xee, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xef, tmp, 1);
    memcpy(tmp, "\xff", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf8, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf9, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfa, tmp, 1);
    memcpy(tmp, "\x30", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfb, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfc, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfd, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xfe, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xff, tmp, 1);
    memcpy(tmp, "\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x60, tmp, 1);
    memcpy(tmp, "\x05", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x61, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x62, tmp, 1);
    memcpy(tmp, "\x42", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, tmp, 1);
    memcpy(tmp, "\xda", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x65, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x69, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6a, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6b, tmp, 1);
    memcpy(tmp, "\x40", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x70, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x71, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x72, tmp, 1);
    memcpy(tmp, "\x42", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, tmp, 1);
    memcpy(tmp, "\xd9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x74, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x75, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x76, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x77, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x78, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x79, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7a, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7b, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x80, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x81, tmp, 1);
    memcpy(tmp, "\x07", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x82, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, tmp, 1);
    memcpy(tmp, "\xd7", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x85, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x86, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x87, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x88, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x89, tmp, 1);
    memcpy(tmp, "\x09", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8a, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8b, tmp, 1);
    memcpy(tmp, "\xd9", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8c, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8d, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8e, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8f, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x90, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x91, tmp, 1);
    memcpy(tmp, "\x0b", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x92, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x93, tmp, 1);
    memcpy(tmp, "\xdb", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x94, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x95, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x96, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x97, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x98, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x99, tmp, 1);
    memcpy(tmp, "\x0d", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9a, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9b, tmp, 1);
    memcpy(tmp, "\xdd", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9c, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9d, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9e, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9f, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa0, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa1, tmp, 1);
    memcpy(tmp, "\x06", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa2, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa3, tmp, 1);
    memcpy(tmp, "\xd6", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa4, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa5, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa6, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa7, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa8, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa9, tmp, 1);
    memcpy(tmp, "\x08", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xaa, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xab, tmp, 1);
    memcpy(tmp, "\xd8", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xac, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xad, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xae, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xaf, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb0, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb1, tmp, 1);
    memcpy(tmp, "\x0a", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb2, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb3, tmp, 1);
    memcpy(tmp, "\xda", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb4, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb5, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb6, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb7, tmp, 1);
    memcpy(tmp, "\x48", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb8, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb9, tmp, 1);
    memcpy(tmp, "\x0c", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xba, tmp, 1);
    memcpy(tmp, "\x02", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbb, tmp, 1);
    memcpy(tmp, "\xdc", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbc, tmp, 1);
    memcpy(tmp, "\x04", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbd, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbe, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbf, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc0, tmp, 1);
    memcpy(tmp, "\x47", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc1, tmp, 1);
    memcpy(tmp, "\x56", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc2, tmp, 1);
    memcpy(tmp, "\x65", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc3, tmp, 1);
    memcpy(tmp, "\x74", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc4, tmp, 1);
    memcpy(tmp, "\x88", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc5, tmp, 1);
    memcpy(tmp, "\x99", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc6, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc7, tmp, 1);
    memcpy(tmp, "\xbb", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc8, tmp, 1);
    memcpy(tmp, "\xaa", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc9, tmp, 1);
    memcpy(tmp, "\x10", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd0, tmp, 1);
    memcpy(tmp, "\x47", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd1, tmp, 1);
    memcpy(tmp, "\x56", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd2, tmp, 1);
    memcpy(tmp, "\x65", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd3, tmp, 1);
    memcpy(tmp, "\x74", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd4, tmp, 1);
    memcpy(tmp, "\x88", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd5, tmp, 1);
    memcpy(tmp, "\x99", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd6, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd7, tmp, 1);
    memcpy(tmp, "\xbb", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd8, tmp, 1);
    memcpy(tmp, "\xaa", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd9, tmp, 1);
    memcpy(tmp, "\x01", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf3, tmp, 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    //      uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, "\x01", 1);
    memcpy(tmp, "\x00", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, tmp, 1);
    memcpy(tmp, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x21, tmp, 0);
    memcpy(tmp, NULL, 0);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11, tmp, 0);
    memcpy(tmp, "\x55", 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3a, tmp, 1);

}
const ute_drv_screen_common_config_t uteDrvScreenTft360X360I172St77916QspiConfig =
{
    .name = "Tft360X360I172St77916Qspi",
    .init = drvScreenTft360X360I172St77916QspiInit,
    .powerOn = drvScreenTft360X360I172St77916QspiPowerOn,
    .powerOff = drvScreenTft360X360I172St77916QspiPowerOff,
    .setWindow = drvScreenTft360X360I172St77916QspiSetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X284_I18_ST7789P3_XHL183BA2401_QSPI_H_ */


