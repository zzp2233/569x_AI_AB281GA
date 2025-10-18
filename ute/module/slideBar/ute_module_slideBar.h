/**
*@file
*@brief     ������ģ��,������������ϱ���
*@details
*@author        ellison
*@date        ellison add ,2022-Jun-24 9:34
*@version       v1.0
*/
#ifndef _UTE_MODULE_SLIDE_BAR_H_
#define _UTE_MODULE_SLIDE_BAR_H_

#include "ute_application_common.h"
#include "ute_project_config.h"
#include "ute_module_platform.h"


#if UTE_MODULE_SLIDE_BAR_SUPPORT

#ifndef UTE_MODULE_SLIDEBAR_MAX_APP
#define UTE_MODULE_SLIDEBAR_MAX_APP 6 //��󱣴�6�����������APP
#endif
/*!д����� ellison  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SLIDEBAR_SET_INFO "systemparam/slideBarInfo"
/* ellison add ,2022-Jul-13
*ע��:����� SLIDE_BAR_xxx_ID�������ǰ���@slide_bar_id_type_t�����������µ�id�������
*/
typedef enum
{
    SLIDE_BAR_ABOUT_WATCH_ID = 0x00,    //�����ֻ�����
    SLIDE_BAR_LOCAL_ALARM_LIST_ID,      // �������ý��� AlarmSettings
    SLIDE_BAR_QR_CODE_ID,               //��ά�����
    SLIDE_BAR_VOICE_ASSISTANT_ID,       //��������
    SLIDE_BAR_BLOOD_OXYGEN_ID,          // Ѫ������ BloodOxygen
    SLIDE_BAR_BLOOD_PRESSURE_ID = 5,    // Ѫѹ���� BloodPressure
    SLIDE_BAR_BREATH_RATE_ID,           //������
    SLIDE_BAR_BREATH_TRAINING_ID,       // ����ѵ�� BreathTraining
    SLIDE_BAR_BACK_LIGHT_ID,            //���ȵ���
    SLIDE_BAR_CALCULATOR_ID,            //������
    SLIDE_BAR_WATCH_PREVIEW0_ID = 10,   //Ԥ����������
    SLIDE_BAR_FATIGUE_ID,               //ƣ�Ͷ�
    SLIDE_BAR_FIND_PHONE_ID,            // �����ֻ� FindPhone
    SLIDE_BAR_FLASHLIGHT_ID,            // �ֵ�Ͳ FlashLight
    SLIDE_BAR_GAME_LIST_ID,             //��Ϸ�б�
    SLIDE_BAR_HEART_RATE_ID = 15,       // ���ʽ��� heartrateScreen
    SLIDE_BAR_MESSAGE_LIST_ID,          // ��Ϣ�б� NotifycationList
    SLIDE_BAR_EMOTION_ID,               //����
    SLIDE_BAR_MORE_LIST_ID,             //���๦���б�
    SLIDE_BAR_MUSIC_CONTROL_ID,         // ���ֿ��� MusicControl
    SLIDE_BAR_WHETHER_POWEROFF_ID = 20, //�Ƿ�ػ�
    SLIDE_BAR_LOCK_SCREEN_ID,           //����
    SLIDE_BAR_TELEPHONE_CALL_LIST_ID,   //����
    SLIDE_BAR_PRESSURE_ID,              // ѹ��
    SLIDE_BAR_SPORTS_HISTORY_LIST_ID,   //�˶���¼ SportHistoryList
    SLIDE_BAR_WHETHER_RESET_ID = 25,    // �ظ���������
    SLIDE_BAR_DISPLAY_OFF_TIME_ID,      //����ʱ��
    SLIDE_BAR_FUNCTION_LIST_ID,         // �����б� FunctionList
    SLIDE_BAR_SLEEP_TIME_ID,            // ˯�� SleepTimeDisplay
    SLIDE_BAR_HOME_STATUS_ID,           // �˶�״̬���� homestatue
    SLIDE_BAR_STOP_WATCH_ID = 30,       // ���Stopwatch
    SLIDE_BAR_TEMPERATURE_ID,           //���½��� Temperature
    SLIDE_BAR_THEME_CHANGE_ID,          //����
    SLIDE_BAR_COUNT_DOWN_ID,            // ����ʱ
    SLIDE_BAR_TRAINING_LIST_ID,         // �˶��б���� trainingList
    SLIDE_BAR_SOUND_SETTINGS_ID = 35,   //��������
    SLIDE_BAR_WEATHER_TODAY_ID,         // �������� WeatherToday
    SLIDE_BAR_MENSTRUAL_CYCLE_ID,       //Ů�Խ���
    SLIDE_BAR_SOS_ID,                   // SOS
    SLIDE_BAR_ALIPAY_ID,                // ֧����
    SLIDE_BAR_TAKE_PICTURE_ID = 40,     // ���
    SLIDE_BAR_ECG_ID,                   //�ĵ�
    SLIDE_BAR_BLOOD_SUGAR_ID,           //Ѫ��
    //

    // ...
    SLIDE_BAR_TYPE_MAX,
    // ...
} slide_bar_id_type_t;

typedef struct
{
    int screenIds[UTE_MODULE_SLIDEBAR_MAX_APP]; // �����id�����ڴ洢֮ǰ���ĸ�APP��ת���˶�Ӧ���棬��󱣴�6��id,����������0��ʼ��ʾ,��ids[0]�����һ��
} ute_module_SlideBar_saveApp_t;

/**
*@brief        ��ʼ������
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarInit(void);

/**
*@brief        ���õ�ǰ����Ľ���ID�����������ʾ��AppId��
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarSetCurrentScreenIdToAppIds(int screenId);

/**
*@brief        ��ȡ��ǰҪ��ʾ�����еĽ���id
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarGetScreenIds(int *screenIds);

/**
*@brief        ��ȡ��ǰҪ��ʾ��APPid
*@details
*@input  int screenId ,�����ID
*@return int appId
*@author       ellison
*@date       2022-06-24
*/
int uteModuleSlideBarGetAppIdFormScreenId(int screenId);

#endif

#endif
