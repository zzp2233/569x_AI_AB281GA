/**
*@file
*@brief        ���˶�ģ��ͷ�ļ�
*@details     ����������·���ܲ�����ɽ�������˶�ģʽ
*@author       zn.zeng
*@date       2021-07-27
*@version      v1.0
*/

#ifndef _UTE_MODULE_SPORT_H_
#define _UTE_MODULE_SPORT_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_systemtime.h"
#include "include.h"


/*! sport �Ʋ����ݱ���Ŀ¼ zn.zeng, 2021-08-23  */
#define UTE_MODULE_FILESYSTEM_SPORT_STEP_DIR "sportStep"
/*!д��������ѣ�̧��������Ŀ�경����Ϣ���� zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SPORTINFO  "systemparam/sportInfo"
/*! sport ���˶����ݱ���Ŀ¼,ֻ�����˶����ݣ��������ݲ�������ڸ��ļ����� zn.zeng, 2021-11-12  */
#define UTE_MODULE_FILESYSTEM_SPORT_MORE_SPORTS_DIR "moreSports"
/*! ������˶��б���ʾ���˶����������� xjc, 2022-03-29  */
#define UTE_MODULE_FILESYSTEM_SPORT_HUNDRED_SPORT_SORT_DATA "systemparam/sportSort"

/*! վ��ʱ�����˶�ʱ������·�����������Ŀ���������� dengli.lu, 2022-01-17  */
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_TARGET_NOTIFY_DATA "systemparam/targetNotifyData"
#endif
/*! վ��ʱ������ dengli.lu, 2022-01-17  */
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_STAND_TIME_DATA "satndTime"
#endif

/*! ÿСʱ�����˶��Ĳ������� casen, 2022-07-14  */
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_STEP_DATA "systemparam/everyHourAllSportStep"
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_ALLSPORT_KCAL_DATA "systemparam/everyHourAllSportKcal"
#endif

#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_STEP_DATA "systemparam/everyHourSportStep"
#define UTE_MODULE_FILESYSTEM_SPORT_TODAY_EVERYHOUR_SPORT_KCAL_DATA "systemparam/everyHourSportKcal"
#endif

/*���˶�Ŀ�����ñ���*/
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
#define UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA "systemparam/moreSportsTarget"
#endif

#define SPORTS_HEART_DATAS_CACHE_LENGTH 90

/*! �Ʋ����ͣ��Ʋ���˯�ߡ����Ʋ���˯�� zn.zeng, 2021-08-05  */
typedef enum
{
    STEP_TYPE_NONE,/*! ���Ʋ���˯��zn.zeng, 2021-08-05  */
    STEP_TYPE_STEP,/*! �Ʋ�zn.zeng, 2021-08-05  */
    STEP_TYPE_SLEEP,/*! ˯��zn.zeng, 2021-08-05  */
} ute_module_sport_step_type;
/*! �����������ݽṹzn.zeng, 2021-08-06  */
typedef struct
{
    bool isOpen;
    uint8_t setMin;
    uint8_t startPeriodHour;
    uint8_t startPeriodMin;
    uint8_t endPeriodHour;
    uint8_t endPeriodMin;
    uint8_t isOpenNoonSleep;
    uint16_t noonSleepStartTime;//���ݿ�ʼʱ�䣬��λ�����ӣ�ʱ*60+��
    uint16_t noonSleepEndTime;//���ݽ���ʱ�䣬��λ�����ӣ�ʱ*60+��
    uint16_t sedentaryRemindSecCnt;
} ute_module_sport_sedentary_param_t;
/*! ���˶����� zn.zeng, 2021-11-11  */
typedef enum
{
    ALL_SPORT_STATUS_CLOSE      = 0X00,
    ALL_SPORT_STATUS_OPEN       = 0x11,
    ALL_SPORT_STATUS_PAUSE      = 0x22,
    ALL_SPORT_STATUS_CONTINUE   = 0x33,
    ALL_SPORT_STATUS_UPDATE_DATA =0x44,
} all_sport_status_type;
typedef struct
{
    uint32_t step;
    uint16_t distance;
    uint16_t caloire;
} ute_sports_today_with_step_data_t;
typedef struct
{
    uint8_t  startInterval;// �洢���ʱ��,10s һ����λ
    uint8_t  sportsType; //�˶���־
    struct prf_date_time startSportTime; //��ʼʱ��
    struct prf_date_time stopSportTime;  //����ʱ��
    uint32_t sportStep; //�˶���,����
    uint16_t sportTimes;    //�˶���,����
    uint16_t sportCaloire;  //�˶���,��·��
    uint8_t  sportDistanceInteger;  //�˶���,���루������ //���루2byteǰһ���ֽ�������λ����һ���ֽ���С��λ��
    uint8_t  sportDistanceDecimals; //�˶���,���루С����
    uint8_t  avgHeartRate;  //ƽ������
    uint8_t  maxHeartRate;  //�������
    uint8_t  minHeartRate;  //��С����
    uint8_t  avgTimeMinute; //ƽ�����٣�2bytes ��ʽΪ����+�롱��0x08����8�֣�0x32����50�룬ƽ�����ٵ���˼�ǣ����ƶ�һ������Ҫ����ʱ�䡱��
    uint8_t  avgTimeSecond;
} ute_sports_save_data_t;
typedef struct
{
    uint8_t status;/*! �˶�״̬����ʼ/ֹͣ����ͣ������ zn.zeng, 2021-11-11  */
    bool isAppStart;/*! �Ƿ�Ϊapp������ zn.zeng, 2021-11-11  */
    uint8_t heartCache[SPORTS_HEART_DATAS_CACHE_LENGTH];//15min
    uint16_t heartTotalCnt;
    uint32_t heartTotalSum;
    uint32_t totalSportTime; /*! ��ʱ������λΪ��zn.zeng, 2021-11-11  */
    int8_t countZeroIndex;/*! ����ʱ��ʼ zn.zeng, 2021-11-11  */
    ute_sports_save_data_t saveData;/*! ��Ҫ��������� zn.zeng, 2021-11-11  */
    bool isAppStartSportReConnection;
    uint16_t appStartSportDisconnectTimeOutCnt;
} ute_module_more_sports_data_t;

#if 1//APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
typedef struct
{
    bool isTodayKcalTargetNotify;
    bool isTodayStepTargetNotify;
    bool isTodayDistanceTargetNotify;
    bool isTodayStandTimeTargetNotify;
    bool isTodaySportTimeTargetNotify;
    bool isTodayAllTargetNotify;
} ute_module_today_target_notify_type_t;

typedef struct
{
    uint16_t todayKcalTarget;
    bool isTodayKcalTargetNotifyOpen;
    bool isTodayKcalTargetHasNotify;
    bool isTodayStepTargetNotifyOpen;
    uint32_t todayDistanceTarget;//��λ��
    bool isTodayDistanceTargetNotifyOpen;
    bool isTodayDistanceTargetHasNotify;

    bool isStandTimeTargetNotifyOpen;
    bool isStandTimeTargetHasNotify;
    uint16_t standTimeTargetSetMin;//����
    uint16_t currStandMin;
    uint32_t allMinStepCnt;
    uint32_t lastMinStepCnt;
    uint8_t standStartHour;//Сʱ
    uint8_t standStartMin;//����

    bool isSportTimeTargetNotifyOpen;
    bool isSportTimeTargetHasNotify;
    uint16_t sportTimeTargetSetMin;//����
    uint32_t currentDayAllSportTimeSec;//�����˶�ʱ�����룬����Ҫ����

    bool isTodayAllTargetHasNotify;
    ute_module_today_target_notify_type_t todayTargetNotifyType;
} ute_module_target_notify_data_t;
#endif
#if (UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT)||(GUI_SCREEN_SIZE_368X448RGB_I260001_SUPPORT)
typedef struct
{
    uint32_t everyHourAllSportStep[24];
    uint16_t everyHourAllSportKcal[24];
    uint32_t everyHourRunStep[24];
    uint32_t everyHourWalkStep[24];
    uint16_t everyHourKcal[24];
    uint16_t everyHourSportTime[24];
    uint16_t everyHourStandTime[24];
    uint32_t weekDayTotalStep[7];
    bool sedentarinessFlag;
#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
    uint32_t currentDayEveryHourSportStep[24];
    uint16_t currentDayEveryHourSportKcal[24];
    uint32_t lastHourSportStep;
    uint16_t lastHourSportKcal;
    bool needSaveCurrentDayEveryHourSportData;
#endif
} ute_module_daily_activity_data_t;
#endif
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
typedef struct
{
    sport_mode_type sportMode; //�˶�����
    uint8_t sportOnOff; //�˶�����
    uint8_t sportOrder; //�˶����
} ute_sports_list_param_t;
typedef struct
{
    ute_sports_list_param_t sportListData[UTE_MODULE_SPORT_MAX_SPORT_NUM];
    uint8_t receiveCrcCheck;
    uint16_t receiveSportBytesLen;
    uint8_t receiveSportBytes[UTE_MODULE_SPORT_MAX_SPORT_NUM*3];
    uint16_t minSportNum; //������ʾ�˶�����
    uint16_t maxSportNum; //�����ʾ�˶�����
    bool appUpdateTrainingListFlag; //APP�޸��˶��б�
} ute_sports_hundred_sort_data_t;
#endif

#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
typedef struct
{
    ute_sports_save_data_t oneHistoryRecordData[UTE_MODULE_SPORT_MORE_SPORTS_DATA_MAX_CNT];
    uint8_t sportsHistoryRecordTotalCnt;
    uint8_t displayIndex;
    bool IsHistoryRecordData;
} ute_module_sports_record_data_t;
#endif

#if UTE_MODULE_SCREENS_WORKOUTS_LIST_SUPPORT
typedef enum
{
    WORKOUTS_LIST_MODE_RUNNING_AND_WALKING =0,
    WORKOUTS_LIST_MODE_CYCLING =1,
    WORKOUTS_LIST_MODE_OUTDOOR,
    WORKOUTS_LIST_MODE_INDOOR,
    WORKOUTS_LIST_MODE_DANCE,
    WORKOUTS_LIST_MODE_COMBAT,
    WORKOUTS_LIST_MODE_BALL,
    WORKOUTS_LIST_MODE_WATER,
    WORKOUTS_LIST_MODE_WINTER,
    WORKOUTS_LIST_MODE_LEISURE,
    WORKOUTS_LIST_MODE_OTHER,
} ute_module_workout_list_mode_t;
#endif
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
typedef struct
{
    uint32_t sportDistanceTargeCnt;//��
    bool isSportDistanceTargetNotify;//Ŀ���������
    bool isSportDistanceTargetHasNotify;//Ŀ��ʱ�����Ѿ����ѹ�
    uint32_t sportTimeTargetSec;
    bool isSportTimeTargetNotify;//Ŀ��ʱ������
    bool isSportTimeTargetHasNotify;//Ŀ��ʱ���Ѿ����ѹ�
    uint16_t sportKcalTarget;
    bool isSportKcalTargetNotify;
    bool isSportKcalTargetHasNotify;
    bool isSportIntegerHourNotify;//�˶���Сʱ����
    bool isSportIntegerHourHasNotify;
    bool isSportIntegerKmNotify;//�˶���ǧ������
    bool isSportIntegerKmHasNotify;
    bool isNoTarget;//��Ŀ��
} ute_module_sports_target_data_t;
#endif

#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
typedef enum
{
    TRAIN_LIST_PAGE_MAIN_SPORT = 0, //Ĭ���˶�|�˶�����
    TRAIN_LIST_PAGE_MORE_SPORT,     //�����˶�
    TRAIN_LIST_PAGE_NORMAL,         //����ģʽ
    TRAIN_LIST_PAGE_OUTDOOR,        //����ģʽ
    TRAIN_LIST_PAGE_BALL,           //����
    TRAIN_LIST_PAGE_DANCE,          //�赸
    TRAIN_LIST_PAGE_MARTIAL_ART,    //����
    TRAIN_LIST_PAGE_GYMNASTICS,     //���
    TRAIN_LIST_PAGE_OTHER,          //�����˶�
    TRAIN_LIST_PAGE_MAX,
} TRAIN_LIST_PAGE_T;
#endif


/*! sport���ݽṹzn.zeng, 2021-07-27  */
typedef struct
{
    gsensor_mode_type accRange;/*! �����㷨acc ��Χ��ǣ�����������zn.zeng, 2021-07-27  */
    gsensor_id_type gsensorId;/*! �����㷨gsensor�ͺű�� zn.zeng, 2021-07-27  */
    ute_module_sport_step_type stepType;
    ute_module_sport_step_type lastStepType;/* ��¼��һ�ε����� zn.zeng 2021-12-27*/
    ute_step_sleep_param_t stepSleepData;
    uint16_t lastHourStepCnt;
    ute_module_sport_sedentary_param_t sedentary;
    bool isOpenHandScreenOn;
    uint32_t stepsTargetCnt;
    bool isTakingPictures;
    uint32_t currDayTotalStep;
    uint32_t currDayWalkStep;
    uint32_t currDayRunStep;
    uint32_t currDayReadTotalStep;//��������ǰСʱ
    uint32_t currDayReadWalkStep;//��������ǰСʱ
    uint32_t currDayReadRunStep;//��������ǰСʱ
    int lastDayReadHour;//��һ�ζ�ȡ���ݵ�Сʱ�����ٹ���
    bool isTargetCompleted;
    uint32_t currDayTotaladdSportStep;
    ute_module_more_sports_data_t moreSportData;
    ute_sports_today_with_step_data_t todaySportWithStepsData;
#if 1//APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
    ute_module_target_notify_data_t todayTargetNotify;
#endif
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    ute_sports_hundred_sort_data_t sportSort;
#endif
#if (UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT)||(GUI_SCREEN_SIZE_368X448RGB_I260001_SUPPORT)
    ute_module_daily_activity_data_t dailyActivity;
#endif
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
    ute_module_sports_record_data_t sportsHistoryRecord;
#endif
#if UTE_MODULE_SCREENS_WORKOUTS_LIST_SUPPORT
    ute_module_workout_list_mode_t modeWorkOutList;
#endif
#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
    ute_module_sports_target_data_t moreSportsTarget;
#endif
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
    bool updateTodayAllSportData;
#endif
#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
    TRAIN_LIST_PAGE_T trainListPage;
#endif
#if APP_DBG_GSENSOR_DATA
    bool appDebugGsensorDataSwitch;
#endif
#if UTE_MODULE_WATCH_ROTATE_SUPPORT
    bool isGsensorRotateTesting;//�ֱ���ת��
#endif
} ute_module_sport_data_t;

#if UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT
typedef struct
{
    uint8_t reducePer;
    uint8_t burnFatPer;
    uint8_t heartLungPer;
    uint8_t noOxygenPer;
    uint8_t limitPer;
} ute_module_sport_heart_zone_data_t;
#endif

void uteModuleSportInit(void);
void uteModuleSportEverySecond(void);
void uteModuleSportSetGsensorIdAccRange(void);
void uteModuleSportUpdateGsensorIdAccRange(void);
void uteModuleSportStepTypeSetStep(void);
void uteModuleSportStepTypeSetSleep(void);
void uteModuleSportStepTypeSetNone(void);
void uteModuleSprotResetRovllverScreenMode(void);
void uteModuleSportInputDataBeforeAlgo(void);
void uteModuleSportSendRealTimeStepInfo(void);
void uteModuleSportSaveStepData(void);
void uteModuleSportStartSendStepHistoryData(ute_module_systemtime_time_t time);
void uteModuleSportSetTakePictureEnable(bool isEnable);
void uteModuleSportTakePictureHandlerMsg(void);
void uteModuleSportOpenAppTakePictureScreenHandler(void);
void uteModuleSportExitTakePicture(void);
bool uteModuleSportIsTakePicture(void);
void uteModuleSportSaveSedentaryParam(ute_module_sport_sedentary_param_t *param);
void uteModuleSportReadSedentaryParam(ute_module_sport_sedentary_param_t *param);
void uteModuleSportSaveHandScreenOnStepsTargetCnt(bool isHandOpen,uint32_t targetCnt);
void uteModuleSportReadConfig(void);
void uteModuleSportGetCurrDayStepCnt(uint32_t *total,uint32_t *walk,uint32_t *run);
uint32_t uteModuleSportGetStepsTargetCnt(void);
void uteModuleSportSetIsTargetCompleted(bool isTargetCompleted);
bool uteModuleSportGetIsTargetCompleted(void);
bool uteModuleSportGetIsOpenHandScreenOn(void);
uint16_t uteModuleSportGetCurrDayDistanceData(void);
uint16_t uteModuleSportGetCurrDayKcalData(void);
void uteModuleSportReadCurrDayStepCnt(ute_module_systemtime_time_t time);
void uteModuleSportStartMoreSports(uint8_t sportsType,uint8_t startInterval,bool isAppStart);
void uteModuleSportStartMoreSportsMsgHandler(uint32_t param);
void uteModuleSportStopMoreSports(void);
bool uteModuleSportMoreSportsIsLessData(void);
void uteModuleSportStopMoreSportsMsgHandler(void);
void uteModuleSportMoreSportsEverySecond(ute_module_systemtime_time_t *time);

#if UTE_GUI_SCREEN_GET_SPORT_DATA_NEW_FUNC_SUPPORT
void uteModuleSportCalculatorEveryHourSportDataHandler(bool isFinish);
void uteModuleSportSaveEveryHourData(void);
void uteModuleSportReadEveryHourData(void);
#endif

void uteModuleSportMoreSportsSaveData(bool isStopSport);
void uteModuleSportMoreSportsDeleteLessData(void);
void uteModuleSportSyncAppMoreSportData(const uint8_t *receive);
void uteModuleSportStartSendMoreSportHistoryData(ute_module_systemtime_time_t time);
void uteModuleSportSetCountZeroIndex(uint8_t countZeroIndex);
void uteModuleSportDisconnectHandler(void);
void uteModuleSportSedentaryMsg(void);
void uteModuleSportUnWearToWearSwitch(void);
void uteModuleSportSyncAppSportStatus(all_sport_status_type sportStatus);
void uteModuleSportDelaySetTakePictureEnableHandlerMsg(void);
void uteModuleSportDelaySetTakePictureEnableTimerStart(uint16_t ms);
bool uteModuleSportMoreSportIsRuning(void);
uint8_t uteModuleSportMoreSportGetType(void);
uint8_t uteModuleSportMoreSportGetStatus(void);
void uteModuleSportMoreSportSetStatus(all_sport_status_type sportStatus);
bool uteModuleSportMoreSportIsAppStart(void);
void uteModuleSportMoreSportSetIsAppStartReconnection(bool isAppStartSportReConnection);
bool uteModuleSportMoreSportIsAppStartReconnection(void);
void uteModuleSportGetMoreSportsDatas(ute_module_more_sports_data_t *data);
int8_t uteModuleSportMoreSportGetCountZeroIndex(void);
void uteModuleSprotCountdownTimerHandlerMsg(void);
void uteModuleSprotCountdownTimerStart(uint16_t ms);
void uteModuleSprotCountdownTimerStop(void);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
void uteModuleSportHundredSportCmd(uint8_t*receive,uint8_t length);
int uteModuleSportFindHundredSportIndex(uint8_t sportOrder);
uint8_t uteModuleSportGetRealIndex(uint8_t sportMode, const uint8_t *sportList);
int uteModuleSportGetHundredSportValidNumber(void);
bool uteModuleSportGetAppUpdateTrainingListFlag(void);
void uteModuleSportSetAppUpdateTrainingListFlag(bool isUpdate);
#endif
void uteModuleSportSystemtimeChange(ute_module_systemtime_time_t curr,ute_module_systemtime_time_t newSet);
#if UTE_MODULE_SPORT_TODAY_ADD_ALL_SPORT_DATA_SUPPORT
void uteModuleSportClearTodayAllSportData(void);
void uteModuleSportReadTodayAllSportDataConfig(ute_module_systemtime_time_t time,ute_sports_today_with_step_data_t *stepData,ute_module_target_notify_data_t *targetNotifyData);
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
void uteModuleSportReadTodayTargetNotifyData(void);
void uteModuleSportSaveTodayTargetNotifyData(void);
void uteModuleSportTodayTargetHandler(void);
void uteModuleSportSetTodayTargetNotifyData(ute_module_target_notify_data_t setTodayTarget);
void uteModuleSportGetTodayTargetNotifyData(ute_module_target_notify_data_t *todayTarget);
void uteModuleSportProtocolGetTodayTargetNotifyData(ute_module_target_notify_data_t *todayTarget);
void uteModuleSportSaveStandTimeData(uint8_t standStartHour,uint8_t standStartMin,uint8_t currStandMin);
void uteModuleSportStartSendStandTimeHistoryData(void);
#endif
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
void uteModuleSportReadHistoryRecordData(void);
void uteModuleSportsHistoryRecordGetData(ute_module_sports_record_data_t *data);
void uteModuleSportsHistoryRecordSetIsHistoryRecordData(bool IsHistoryRecordData);
bool uteModuleSportsHistoryRecordGetIsHistoryRecordData(void);
uint8_t uteModuleSportsHistoryGetRecordTotalCnt(void);
void uteModuleSportsSetHistoryRecordDisplayIndex(uint8_t displayIndex);
#endif
#if UTE_GUI_SCREEN_ACTIVITY_LIST_SUPPORT
void uteModuleSportReadTodayEveryHourAllSportStepData(void);
void uteModuleSportSaveTodayEveryHourAllSportStepData(void);
void uteModuleSportReadTodayEveryHourAllSportKcalData(void);
void uteModuleSportSaveTodayEveryHourAllSportKcalData(void);
uint32_t uteModuleSportGetCurrDayEveryHourSportTime(void);
uint16_t uteModuleSportGetCurrDayEveryHourKcal(void);
uint16_t uteModuleSportGetCurrDayEveryHourStandTime(void);
uint32_t uteModuleSportLoadTodayEveryHourStepHistoryData(UT_GraphsParam *everyHourStepHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
bool uteModuleSportLoadTodayEveryHourKcalHistoryData(UT_GraphsParam *everyHourKcalHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
bool uteModuleSportLoadTodayEveryHourSportTimeHistoryData(UT_GraphsParam *everyHourSportTimeHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
uint16_t uteModuleSportLoadTodayEveryHourStandTimeHistoryData(UT_GraphsParam *everyHourStandTimeHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
void uteModuleSportReadWeekDayStep(void);
uint32_t uteModuleSportLoadWeekDayStepHistoryData(UT_GraphsParam *weekDayStepHistorygramGraph,uint32_t color, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange,uint32_t *totalStep);
#endif

#if UTE_MODULE_SCREENS_SPORT_TARGET_NOTIFY_SUPPORT
void uteModuleSportMoreSportsTargetInit(void);
void uteModuleSportMoreSportsTargetReadConfig(void);
void uteModuleSportMoreSportsTargetSaveData(void);
void uteModuleSportSetMoreSportsTargetData(ute_module_sports_target_data_t setMoreSportsTarget);
void uteModuleSportGetMoreSportsTargetData(ute_module_sports_target_data_t *sportsTarget);
void uteModuleSportMoreSportsTargetsProcess(void);
#endif
#if UTE_SPORTS_HEART_MAX_MIN_WARNING_NOTIFY_SUPPORT
void uteModuleSportMoreSportsHeartWarningProcess(void);
#endif
uint16_t uteModuleSportMoreSportGetDisconnectTimeoutCnt(void);

#if UTE_MODULE_SPORT_CLASSIFICATION_SUPPORT
TRAIN_LIST_PAGE_T uteModuleSportMoreSportGetTrainListPage(void);
void uteModuleSportMoreSportSetTrainListPage(TRAIN_LIST_PAGE_T trainListPage);
#endif

#if UTE_MODULE_SPORTS_HISTORY_HEART_ZONE_SUPPORT
void uteModuleSportGetHeartZoneArray(ute_module_sport_heart_zone_data_t *zoneData);
void uteModuleSportReadHistoryHeartZoneData(ute_module_systemtime_time_t *queryTime);
void uteModuleSportLoadHeartZoneGrapData(UT_GraphsParam (*zoneGramGraph)[5],int16_t x, int16_t y, uint16_t drawWidth, uint16_t drawHight);
#endif

#if APP_DBG_GSENSOR_DATA
void uteModuleSportSendGsensorDataToPhone(int16_t tempx, int16_t tempy,int16_t tempz);
void uteModuleSportSetAppDebugGsensorDataSwitch(bool appDebugGsensorDataSwitch);
bool uteModuleSportGetAppDebugGsensorDataSwitch(void);
#endif

#if UTE_MODULE_WATCH_ROTATE_SUPPORT
bool uteModuleSportGetGsensorRotateTesting(void);
void uteModuleSportSetGsensorRotateTesting(bool isGsensorRotateTesting);
uint16_t uteModuleSportGetGsensorRotateCnt(void);
void uteModuleSportClearGsensorRotateData(void);
#endif

void uteModuleSportAlgoTimerStart(uint16_t ms);
void uteModuleSprotAlgoTimerStop(void);
void uteModuleSprotInputDataBeforeAlgoTimerHandler(void);

#endif //_UTE_MODULE_SPORT_H_

