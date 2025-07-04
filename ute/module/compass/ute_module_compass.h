/**
*@file
*@brief        指南针校验模块头文件
*@details
*@author        pcm
*@date        2024-11-25
*@version      v1.0
*/
#ifndef _UTE_MODULE_COMPASS_H_
#define _UTE_MODULE_COMPASS_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_drv_gsensor_common.h"
#if UTE_MODULE_MAGNETIC_SUPPORT
/*! 指南针模块保存目录 pcm, 2024-11-25  */
#define UTE_MODULE_COMPASS_DATA_DIR "compass"
/*! 校准数据保存文件 pcm, 2024-11-25  */
#define UTE_MODULE_COMPASS_CALIBRATION_FILE "compass/calibration"

typedef struct
{
    /* These variables are used to save the calibrated orientation output. */
    float fAzimuth;
    float fPitch;
    float fRoll;
    /* This variable is used to save the calibration accuracy. */
    int iAccuracy;

    bool calibration;//是否校准  0：未校准 1：已校准
    bool compassOnoff;//开关状态
} ute_module_compass_data_t;
/**
*@brief  初始化
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassInit(void);
/**
*@brief  算法初始化
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassAlgoInit(void);
/**
*@brief  mag acc算法处理函数
*@details  计算出方向角度，地磁质量
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassAlgohandler(float *rawMag,float *rawAcc);
/**
*@brief  开始测量
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassStart(void);
/**
*@brief  结束测量
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassStop(void);
/**
*@brief  获取角度值，正北方向为0度
*@details  0~359度
*@author        pcm
*@date        2024-11-25
*/
int uteModuleCompassGetAzimuth(void);
/**
*@brief  获取水平角度值，水平面为0度
*@details  绕着X轴旋转
*@author        pcm
*@date        2024-11-25
*/
float uteModuleCompassGetPitch(void);
/**
*@brief
*@details  绕着Z轴旋转
*@author        pcm
*@date        2024-11-25
*/
float uteModuleCompassGetRoll(void);
/**
*@brief  获取地磁质量值
*@details
*@author        pcm
*@date        2024-11-25
*/
int uteModuleCompassGetAccuracy(void);
/**
*@brief  是否校准完成
*@details
*@author        pcm
*@date        2024-11-25
*/
bool uteModuleCompassIsCalibrationFinish(void);
/**
*@brief  获取原始三轴数据
*@details
*@author        pcm
*@date        2024-11-25
*/
float* uteModuleCompassAccrawData(void);
/**
*@brief  获取厂测三轴数据
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassAccrawgetData(int32_t* gsensor_data);
/**
*@brief  开始定时器中断处理，处理数据发送给算法
*@details
*@author        pcm
*@date        2024-11-27
*/
void uteModuleCompassDataputHandler(void);
/**
 * @brief 获取罗盘开关状态
 * @details 该函数用于获取罗盘的开关状态。
 * @author pcm
 * @date 2024-11-25
 * @return bool 返回罗盘的开关状态，true表示开启，false表示关闭。
 */
bool uteModulecompassOnoff(void);
#endif
#endif //_UTE_MODULE_COMPASS_H_

