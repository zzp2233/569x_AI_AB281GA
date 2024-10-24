/**
*@file
*@brief     ab281项目配置文件
*@details
*@author        zn.zeng
*@date        2024-03-22
*@version       v1.0
*/
#ifndef _UTE_PROJECT_CONFIG_AB281_H_
#define _UTE_PROJECT_CONFIG_AB281_H_

#define DEFAULT_BLE_DEV_NEME "AB281"
#define UTE_SW_VERSION "AB281V000001"

#define UTE_DRV_DSPI_FOR_SCREEN_SUPPORT 0
#define UTE_DRV_QSPI_FOR_SCREEN_SUPPORT 1

#define UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT 1

/*! 屏最大亮度百分比 dengli.lu, 2021-10-29  */
#define DEFAULT_BACK_LIGHT_PERCENT_MAX 100

/*! 屏最小亮度百分比 dengli.lu, 2021-10-29  */
#define DEFAULT_BACK_LIGHT_PERCENT_MIN 20

/*! 调节屏亮度时增加或者减少的百分比 dengli.lu, 2021-10-29  */
#define BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE 20

/*! 默认背光百分比zn.zeng, 2021-09-24  */
#define DEFAULT_SCREEN_BACKLIGHT_PERCENT    (BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE*3)

#endif//_UTE_PROJECT_CONFIG_AB281_H_