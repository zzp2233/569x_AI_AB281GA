/**
 *@file
 *@brief        情绪压力处理模块
 *@details
 *@author       xjc
 *@date       2022-02-15
 *@version      v1.0
 */
#ifndef _UTE_MODULE_EMOTION_PRESSURE_H_
#define _UTE_MODULE_EMOTION_PRESSURE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
#include "StressEst.h"
#endif
#include "ute_module_systemtime.h"

#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
/*!写入情绪压力测试参数 xjc, 2022-02-15  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_SET_INFO "systemparam/EPInfo"
/*!写入情绪压力请求码 xjc, 2022-02-15  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_REQ_CODE "systemparam/EPReqCode"
/*!写入情绪压力激活码 xjc, 2022-02-15  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_KEY_CODE "systemparam/EPKeyCode"
/*!写入情绪压力算法是否激活 xjc, 2022-02-15  */
// #define UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_ALGO_ACTIVE "systemparam/EPAlgoActive"
/*! 情绪压力24小时自动测试数据保存目录 xjc, 2022-02-15  */
#define UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR "EPAutoData"
/*! 最后一次血氧测量信息,wang.luo 2025-09-27 */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_LAST_INFO "systemparam/EPLastInfo"

#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
#define EMOTION_PRESSURE_DEFAULT_KEY_CODE "ElMSEkzC9IkWUmFYiBELzBygiF+rJ5CYr9NBiuXXsOYa5kDqsRxCrEIc3kLIymAZgL4SMwq/3Lif88er7PcHfw=="
#define EMOTION_PRESSURE_KEY_CODE_LEN 88 // 定长88
#define EMOTION_PRESSURE_DEFAULT_REQ_CODE "6+m78fFJkFKu1mzhhL4jKRJTEhJMwvSJFlJhWIgRC8wfyB0HFw8JMURNQ6c3MP/U"
#define EMOTION_PRESSURE_REQ_CODE_LEN 64 // 定长64
#define EMOTION_PRESSURE_DEFAULT_FACT "ute"
#define EMOTION_PRESSURE_FACT_LEN 3
#if UTE_DRV_BLOODPRESSURE_DN02_SUPPORT
#define EMOTION_PRESSURE_ALGORITHM_BUFF_MAX_LEN 8000 // 情绪压力算法buff大小，大于等于 3 * 100 * SAMPLETIME * sizeof(int) + 30 * 10 * sizeof(int) + 25 * 8 * sizeof(float)
#define VCXX_SAMPLERATE 100                          // 采样频率
#else
#define VCXX_SAMPLERATE SAMPLERATE
/**
现在是1s调用一次算法
**/
#define APP_TEST_BLE_SEND_APP_DEFAULT_MAX_EPMIDVALUE_COUNT 140 // APP 发起测试，当计算的中间值数为70时，BLE向APP发送结束帧
// #define APP_TEST_SAMPLETIME 1 //APP 发起的测试。不要更改
#define APP_TEST_SAMPLETIME_BUFFER 4    // 缓存3秒的PPG数据，最好4秒
#define APP_START_TEST_OVER_TIME 1      // APP测试超时时间
#define LOCAL_START_TEST_OVER_TIME 2    // 本地测试超时时间
#define LOCAL_TEST_OUTPUT_VALUE_TIME 45 // 单位秒 设置本地测试出值时间,设置的值需超过30秒
#define UTE_DRV_EMOTION_PRESSURE_TEST_TIMEOUT_SECOND (60 * LOCAL_START_TEST_OVER_TIME) // 2分钟超时
#endif
#endif

#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
typedef struct
{
    uint32_t algoCaculatePerSampleTimeMidValueCnt;                // lib_emotion.h --> #define SAMPLETIME 中定义
    uint32_t appTestStartToEndEPMidValueTotalCnt;                 // APP发起一次测试后，计数从测试到结束所产生的中间值总的个数
    uint32_t algoExeCnt;                                          // 算法执行次数
    uint32_t buffer[APP_TEST_SAMPLETIME_BUFFER][VCXX_SAMPLERATE]; // 测试PPG缓存
    uint16_t index;
    bool bufferIsFull;
    uint32_t ppgMinValue;
    uint8_t ppgDataBufferIndex[APP_TEST_SAMPLETIME_BUFFER];
    int32_t oneSecondData[VCXX_SAMPLERATE + 20];
    int32_t emotionData[APP_TEST_SAMPLETIME_BUFFER * VCXX_SAMPLERATE + 20];
    uint16_t oneSencondDataCnt;
    float emotionOutPutFMvaule[25][8];
#if UTE_DRV_BLOODPRESSURE_DN02_SUPPORT
    uint8_t algorithmBuff[EMOTION_PRESSURE_ALGORITHM_BUFF_MAX_LEN];
#endif
} emotion_pressure_ppg_data_t;
#endif

typedef struct
{
    uint8_t lastEmotionValue;
    uint8_t lastPressureValue;
    uint8_t lastFatigueValue;
    ute_module_systemtime_measure_time_t lastTestTime;
} ute_module_emotion_pressure_test_last_info_t;

typedef struct
{
    uint32_t HighCnt;
    uint16_t HighPercent;
    uint32_t MediumCnt;
    uint16_t MediumPercent;
    uint32_t NormalCnt;
    uint16_t NormalPercent;
    uint32_t RelaxCnt;
    uint16_t RelaxPercent;
    uint32_t TotalValidCnt;
} emotion_pressure_result_data_t;

/*! 情绪压力数据结构,xjc, 2022-02-15*/
typedef struct
{
    bool isSingleTesting;
    bool isAppStartTesting;
    int16_t appStartScreen;
    uint8_t emotionValue; // 0:消极情绪，1：中性情绪，2：积极情绪
    uint8_t pressureValue;
    uint8_t fatigueValue;
    uint8_t testingSecond;
    bool isAutoTesting;
    bool isAutoTimeBucketTesting;
    uint16_t intervalMin;
    uint16_t startTimeHourMin;
    uint16_t endTimeHourMin;
    bool isEmotionPressureAutoTestFlag;
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
    bool isAlgoActive;                                  // 算法是否激活
    uint8_t algoKeyCode[EMOTION_PRESSURE_KEY_CODE_LEN]; // 接收暂存送服务器段发过来的激活码
    uint8_t algoReqCode[EMOTION_PRESSURE_REQ_CODE_LEN]; // 请求码
    emotion_pressure_ppg_data_t ppgData;
    bool stopSendMidValue; // 是否停止发送中间值
#if DUG_EMOTION_PRESSURE_SUPPORT
    uint8_t vcxxSendPPGBuff[44];
#endif
#else
    bool isVkPressureTesting;
    uint8_t vkPressureValue;
#endif
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
    uint8_t curDayPressureMinValue;
    uint8_t curDayPressureMaxValue;
    uint8_t curDayPressureAvgValue;
    uint8_t lastPressureValue;
    uint8_t curDayFatigueMinValue;
    uint8_t curDayFatigueMaxValue;
    uint8_t curDayFatigueAvgValue;
    uint8_t lastFatigueValue;
#endif
    emotion_pressure_result_data_t result;
    ute_module_emotion_pressure_test_last_info_t lastTestInfo;
    uint8_t stopReasion;
} ute_module_emotion_pressure_data_t;

typedef enum
{
    EP_STOP_REASION_SUCCESS = 0x00,         // 正常出值
    EP_STOP_REASION_NETWORK = 0xF0,         // 服务器网络异常
    EP_STOP_REASION_PARSE = 0xF1,           // 服务器解析异常
    EP_STOP_REASION_TIMEOUT = 0xF2,         // 测试超时
    EP_STOP_REASION_UNWEAR = 0xF3,          // 脱手
    EP_STOP_REASION_TRAINING = 0xF4,        // 运动中
    EP_STOP_REASION_INITIATIVE_QUIT = 0xF5, // 手环端主动退出
    EP_STOP_REASION_TESTING = 0xF6,         // 测试中
} EMOTION_PRESSURE_STOP_REASION_T;

void uteModuleEmotionPressureInit(void);
void uteModuleEmotionPressureReadConfig(void);
void uteModuleEmotionPressureEverySecond(void);
void uteModuleEmotionPressureStartSingleTesting(bool isAppStart);
void uteModuleEmotionPressureStopSingleTesting(uint8_t stopReasion);
void uteModuleEmotionPressureStopSingleTestingMsgHandler(uint32_t param);
void uteModuleEmotionPressureStartSingleTestingMsgHandler(uint32_t param);
bool uteModuleEmotionPressureIsWear(void);
EMOTION_PRESSURE_STOP_REASION_T uteModuleEmotionPressureGetStopReasion(void);
void uteModuleEmotionPressureSetKeyCode(uint8_t *keyCode);
void uteModuleEmotionPressureGetKeyCode(uint8_t *keyCode);
void uteModuleEmotionPressureGetReqCode(uint8_t *reqCode);
void uteModuleEmotionPressureSetEmotionPressureValue(uint8_t emotionValue, uint8_t pressureValue, uint8_t fatigueValue);
uint8_t uteModuleEmotionPressureGetEmotionValue(void);
uint8_t uteModuleEmotionPressureGetPressureValue(void);
uint8_t uteModuleEmotionPressureGetFatigueValue(void);
bool uteModuleEmotionPressureIsTesting(void);
void uteModuleEmotionPressureSaveAutoIntervalParam(bool isAutoTesting, uint16_t intervalMin);
void uteModuleEmotionPressureSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting, uint16_t startTimeHourMin, uint16_t endTimeHourMin);
void uteModuleEmotionPressureSaveKeyCode(void);
void uteModuleEmotionPressureStartSendAutoTestHistoryData(ute_module_systemtime_time_t time);
void uteModuleEmotionPressureAutoSaveData(void);
void uteModuleEmotionPressureDelHistoryData(void);
bool uteModuleEmotionPressureIsEmotionPressureAutoTesting(void);
void uteModuleEmotionPressureActiveAlgo(bool isAppActiveAlgo);
void uteModuleEmotionPressureSaveKeyqCode(void);
void uteModuleEmotionPressureSaveReqCode(void);
void uteModuleEmotionPressureSaveAlgoActiveStatus(void);
void uteModuleEmotionPressureSetAlgoActiveStatus(bool isAlgoActive);
bool uteModuleEmotionPressureGetAlgoActiveStatus(void);
void uteModuleEmotionPressureSendDeviceInfomationToService(void);
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
void uteModuleEmotionPressureAlgoInputData(vcxxCommonSampleData_t *sampleData, uint8_t size);
#if !UTE_DRV_BLOODPRESSURE_DN02_SUPPORT
void uteModuleEmotionPressureAlgoGetSystemTimeHandler(lib_time_t *crt_time);
#endif
#endif
bool uteModuleEmotionPressureGetTodayPressureHistoryData(uint8_t *pressureHistoryGraph, uint8_t pressureHistoryGraphCount);
bool uteModuleEmotionPressureGetTodayFatigueHistoryData(uint8_t *fatigueHistoryGraph, uint8_t fatigueHistoryGraphCount);
bool uteModuleEmotionPressureGetTodayEmotionHistoryData(uint8_t *emotionHistoryGraph, uint8_t emotionHistoryGraphCount);
void uteModuleEmotionPressureArrayEnemLeftMove(uint8_t *arr, uint8_t len, bool isInit);
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
void uteModuleCalculateMaxMinAvgPressValue(void);
#if !UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
void uteModuleEmotionPressureSetEmotionPressureMaxAndMinValue(uint8_t lastPressureValue);
#endif
uint8_t uteModuleEmotionPressureGetEmotionPressureMaxValue(void);
uint8_t uteModuleEmotionPressureGetEmotionPressureMinValue(void);
uint8_t uteModuleEmotionPressureGetEmotionPressureAvgValue(void);
#endif

bool uteModuleEmotionPressureCalculatePressurePercent(void);
void uteModuleEmotionPressureCalculateGetPressurePercent(emotion_pressure_result_data_t *param);

void uteModuleEmotionPressureSetLastTestInfo(uint8_t lastEmotionValue, uint8_t lastPressureValue, uint8_t lastFatigueValue, ute_module_systemtime_time_t time);
void uteModuleEmotionPressureReadLastTestInfo(void);
void uteModuleEmotionPressureGetLastTestInfo(ute_module_emotion_pressure_test_last_info_t *info);

#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
void uteDrvHeartVcxxStartPressureSample(void);
void uteDrvHeartVcxxStopPressureSample(uint8_t stopReasion);
bool uteDrvHeartVcxxIsPressureTesting(void);
uint8_t uteDrvHeartVcxxGetVkPressureValue(void);
#endif

#endif
#endif //_UTE_MODULE_EMOTION_PRESSURE_H_
