#ifndef _UTE_DRV_TFT_S240X240_NV3002C_HY138026A_H_
#define _UTE_DRV_TFT_S240X240_NV3002C_HY138026A_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_screen_common.h"

#if UTE_DRV_TFT_S240X240_NV3002C_HY138026A_QSPI_SUPPORT

__STATIC_INLINE void drvScreenTft360X3603002chy138026aPowerOn(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x11,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x29,NULL,0);
    //uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2c, (uint8_t *)"\x00\x00\x00\x00", 4);
    //uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x2c, (uint8_t *)"\x00\x00\x00\x00", 4);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X3603002chy138026aPowerOff(void)
{
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x28,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x10,NULL,0);
    uteModulePlatformDelayMs(120);
    uteDrvScreenCommonSetPowerEnable(false);
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);
}

__STATIC_INLINE void drvScreenTft360X3603002chy138026aSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
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

__STATIC_INLINE void drvScreenTft360X3603002chy138026aInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_DRV_SCREEN_LVL, "%s", __func__);

    uteDrvScreenCommonSetPowerEnable(true);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(10);
    uteDrvScreenCommonSetResetPin(false);
    uteModulePlatformDelayMs(10);
    uteDrvScreenCommonSetResetPin(true);
    uteModulePlatformDelayMs(220);

    uint8_t tmp[2]; // 用于存储命令参数（最多2个字节）

    // 初始化序列
    tmp[0] = 0xa5;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xff, tmp, 1);

    tmp[0] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x41, tmp, 1); //00:3 SPI 01:2dataLine SPI

    // porch设置
    tmp[0] = 0x08;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x53, tmp, 1);  //vbp
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x54, tmp, 1);  //vfp

    tmp[0] = 0x60;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x55, tmp, 1);  //hbp
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x56, tmp, 1);  //hfp

    // 显示接口配置
    tmp[0] = 0x05;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x3A, tmp, 1); //05: 565 06:666

    // 时钟选择
    tmp[0] = 0x24;
    tmp[1] = 0x03;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x8f, tmp, 2); //PUMP CLK SEL

    // 电源电压设置
    tmp[0] = 0x48;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9a, tmp, 1); // vsp 6.0v 3*VCI
    tmp[0] = 0x68;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9b, tmp, 1); //vsn -4.6
    tmp[0] = 0xA0;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9c, tmp, 1); //VGH VCI*7
    tmp[0] = 0x13;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9d, tmp, 1); //vgh=12V
    tmp[0] = 0xC6;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x9E, tmp, 1); //vgl= -11.5V

    // gamma LDO设置
    tmp[0] = 0x76;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x84, tmp, 1); //GVDD 5V
    tmp[0] = 0x4b;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x83, tmp, 1); //GVCL ADJ -4V
    tmp[0] = 0x1c;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x85, tmp, 1); //GVSP 0.575

    // 源驱动器设置
    tmp[0] = 0x11;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe6, tmp, 1);
    tmp[0] = 0x22;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xe9, tmp, 1);
    tmp[0] = 0x2e;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xeb, tmp, 1); //12e
    tmp[0] = 0x01;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xec, tmp, 1);
    tmp[0] = 0x2e;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf0, tmp, 1);
    tmp[0] = 0x46;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xf2, tmp, 1);

    // 完整的gamma 2.2设置
    // V63
    tmp[0] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x60, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x70, tmp, 1);
    // V0
    tmp[0] = 0x0f;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6e, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7e, tmp, 1);

    // V62
    tmp[0] = 0x0b;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x61, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x71, tmp, 1);
    // V1
    tmp[0] = 0x35;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6d, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7d, tmp, 1);

    // V61
    tmp[0] = 0x11;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x62, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x72, tmp, 1);
    tmp[0] = 0x34;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6c, tmp, 1);
    tmp[0] = 0x2a;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7c, tmp, 1);

    // V43
    tmp[0] = 0x38;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x66, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x76, tmp, 1);
    tmp[0] = 0x50;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x68, tmp, 1);
    tmp[0] = 0x4a;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x78, tmp, 1);

    // V59
    tmp[0] = 0x0a;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x63, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x73, tmp, 1);
    tmp[0] = 0x14;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6b, tmp, 1);
    tmp[0] = 0x10;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7b, tmp, 1);

    // V57
    tmp[0] = 0x09;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x64, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x74, tmp, 1);
    tmp[0] = 0x14;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x6a, tmp, 1);
    tmp[0] = 0x10;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x7a, tmp, 1);

    // V50
    tmp[0] = 0x06;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x65, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x75, tmp, 1);
    tmp[0] = 0x09;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x69, tmp, 1);
    tmp[0] = 0x07;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x79, tmp, 1);

    // V36 v27
    tmp[0] = 0x33;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x67, tmp, 1);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x77, tmp, 1);

    // 面板配置
    tmp[0] = 0x08;
    tmp[1] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa0, tmp, 2);
    tmp[0] = 0x89;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa1, tmp, 1);
    tmp[0] = 0x8A;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa2, tmp, 1);
    tmp[0] = 0x87;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa3, tmp, 1);
    tmp[0] = 0x88;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa4, tmp, 1);

    tmp[0] = 0x10;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xa9, tmp, 1);
    tmp[0] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xaa, tmp, 1);
    tmp[0] = 0x03;
    tmp[1] = 0xFF;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xab, tmp, 2);
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xac, tmp, 2);
    tmp[0] = 0x01;
    tmp[1] = 0xF6;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xad, tmp, 2);
    tmp[0] = 0x00;
    tmp[1] = 0x05;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xae, tmp, 2);

    // 更多面板配置
    tmp[0] = 0x86;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbd, tmp, 1);
    tmp[0] = 0x87;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbe, tmp, 1);
    tmp[0] = 0x84;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbf, tmp, 1);
    tmp[0] = 0x85;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc0, tmp, 1);
    tmp[0] = 0x82;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xb9, tmp, 1);
    tmp[0] = 0x83;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xba, tmp, 1);
    tmp[0] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbb, tmp, 1);
    tmp[0] = 0x81;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xbc, tmp, 1);

    // 电源配置
    tmp[0] = 0x03;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc4, tmp, 1);
    tmp[0] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc5, tmp, 1);
    tmp[0] = 0xf7;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc6, tmp, 1);
    tmp[0] = 0x01;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc7, tmp, 1);
    tmp[0] = 0xf6;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xc9, tmp, 1);
    tmp[0] = 0xf5;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xca, tmp, 1);
    tmp[0] = 0xf8;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xcb, tmp, 1);
    tmp[0] = 0xf7;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xcc, tmp, 1);
    tmp[0] = 0xf2;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xce, tmp, 1);
    tmp[0] = 0xf1;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xcf, tmp, 1);
    tmp[0] = 0xf4;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd0, tmp, 1);
    tmp[0] = 0xf3;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd1, tmp, 1);
    tmp[0] = 0x91;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd2, tmp, 1);
    tmp[0] = 0x01;
    tmp[1] = 0xf5;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd3, tmp, 2);
    tmp[0] = 0x4f;
    tmp[1] = 0x4c;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd7, tmp, 2);
    tmp[0] = 0xca;
    tmp[1] = 0xca;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd8, tmp, 2);
    tmp[0] = 0x30;
    tmp[1] = 0x30;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0xd9, tmp, 2); //goa_rst_tglue1[7:0], goa_rst_tglue2[7:0]

    // 显示控制
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x35, tmp, 0); // 无参数命令
    tmp[0] = 0x00;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x36, tmp, 1);
    tmp[0] = 0x11;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x46, tmp, 1);
    tmp[0] = 0x05;
    tmp[1] = 0x08;
    uteDrvScreenCommonGc9c01QspiWriteCmdParams(0x47, tmp, 2);

}
const ute_drv_screen_common_config_t uteDrvScreenTft360X3603002chy138026aConfig =
{
    .name = "Tft360X3603002chy138026aQspi",
    .init = drvScreenTft360X3603002chy138026aInit,
    .powerOn = drvScreenTft360X3603002chy138026aPowerOn,
    .powerOff = drvScreenTft360X3603002chy138026aPowerOff,
    .setWindow = drvScreenTft360X3603002chy138026aSetWindow,
    .setBackLight = NULL,
};

#endif
#endif /* _UTE_DRV_TFT_S240X240_NV3002C_HY138026A_H_ */


