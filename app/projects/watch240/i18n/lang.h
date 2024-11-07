#ifndef _LANG_H
#define _LANG_H

//enum {
//    LANG_EN,
//    LANG_ZH,
//};

enum
{
    STR_CLOCK,
    STR_CLOCK_PREVIEW,
    STR_MESSAGE,
    STR_PHONE,
    STR_SPORTS,
    STR_SPORTS_CONFIG,

    STR_FOOTBALL,
//    STR_INDOOR_CYCLING,
//    STR_INDOOR_RUN,
//    STR_OUTDOOR_CYCLING,
//    STR_RUN,
//    STR_SWIM,
//    STR_WALK,

    STR_SPORT_RUN,          //�ܲ�
    STR_SPORT_RIDE_BIKE,    // ����
    STR_SPORT_JUMP_ROPE,    //����
    STR_SPORT_SWIMMING,     // ��Ӿ
    STR_SPORT_BADMINTON,    // ��ë��
    STR_SPORT_TABLE_TENNIS, //ƹ����
    STR_SPORT_TENNIS,       //����
    STR_SPORT_CLIMBING,     //��ɽ
    STR_SPORT_WALKING,      //ͽ��
    STR_SPORT_BASKETBALL,   //����
    STR_SPORT_FOOTBALL,     //����
    STR_SPORT_BASEBALL,     //����
    STR_SPORT_VOLLEYBALL,   //����
    STR_SPORT_CRICKET,      //����
    STR_SPORT_RUGBY,        //�����
    STR_SPORT_HOCKEY,       // ������
    STR_SPORT_DANCE,        //����
    STR_SPORT_SPINNING,     //���е���
    STR_SPORT_YOGA,         //�٤
    STR_SPORT_SIT_UP,       //��������
    STR_SPORT_TREADMILL,    // �ܲ���
    STR_SPORT_GYMNASTICS,   //���
    STR_SPORT_BOATING,      // ����
    STR_SPORT_JUMPING_JACK, //������
    STR_SPORT_FREE_TRAINING,//����ѵ��20191231

    STR_SPORT_INDOOR_WALK,  //������·
    STR_SPORT_INDOOR_RUN,   //�����ܲ�
    STR_SPORT_STRENGTH_TRAINING,    //����ѵ��
    STR_SPORT_STEP_TRAINING,    //̤��
    STR_SPORT_HORSE_RIDING,     //����
    STR_SPORT_ELLIPTICAL_TRAINER,       //��Բ��
    STR_SPORT_TAI_CHI,      //̫��
    STR_SPORT_SHUTTLECOCK,  //���
    STR_SPORT_BOXING,       //ȭ��
    STR_SPORT_OUTDOOR_WALK, //���⽡��
    STR_SPORT_TRAIL_RUNNING,    //ԽҰ��
    STR_SPORT_SKIING,       //��ѩ
    STR_SPORT_ICE_HOCKEY,   //����
    STR_SPORT_TAEKWONDO,    //��ȭ��
    STR_SPORT_VO2MAX_TEST,  //�������������
    STR_SPORT_ROWING_MACHING,   //������
    STR_SPORT_AIR_WALKER,       //������
    STR_SPORT_HIKING,       //ͽ��
    STR_SPORT_ATHLETICS,    //�ﾶ
    STR_SPORT_WAIST_TRAINING,   //����ѵ��
    STR_SPORT_KARATE,       //���ֵ�
    STR_SPORT_COOLDOWN,     //�������
    STR_SPORT_CROSS_TRAINING,   //����ѵ��
    STR_SPORT_PILATES,      //������
    STR_SPORT_CROSS_FIT,    //�������
    STR_SPORT_FUNCTIONAL_TRAINING,      //������ѵ��
    STR_SPORT_PHYSICAL_TRAINING,        //����ѵ��
    STR_SPORT_ARCHERY,          //���
    STR_SPORT_FLEXIBILITY,      //���Ͷ�
    STR_SPORT_MIXED_CARDIO,     //�������
    STR_SPORT_LATIN_DANCE,      //������
    STR_SPORT_STREET_DANCE,     //����
    STR_SPORT_KICKBOXING,       //���ɲ���
    STR_SPORT_BARRE,            //����
    STR_SPORT_AUSTRALIAN_FOOTBALL,  //��ʽ����
    STR_SPORT_MARTIAL_ARTS,     //����
    STR_SPORT_STAIRS,           //��¥
    STR_SPORT_HANDBALL,         //����
    STR_SPORT_BOWLING,          //������
    STR_SPORT_RACQUETBALL,      //����
    STR_SPORT_CURLING,          //����
    STR_SPORT_HUNTING,          //����
    STR_SPORT_SNOWBOARDING,     //���廬ѩ
    STR_SPORT_PLAY,             //�����˶�
    STR_SPORT_AMERICAN_FOOTBALL,    //��ʽ�����
    STR_SPORT_HAND_CYCLING,     //��ҡ��
    STR_SPORT_FISHING,          //����
    STR_SPORT_DISC_SPORTS,      //����
    STR_SPORT_RUGGER,           //�����
    STR_SPORT_GOLF,             //�߶���
    STR_SPORT_FOLK_DANCE,       //������
    STR_SPORT_DOWNHILL_SKIING,      //��ɽ��ѩ
    STR_SPORT_SNOW_SPORTS,      //ѩ���˶�
    STR_SPORT_MIND_BODY,        //�滺ڤ�����˶�
    STR_SPORT_CORE_TRAINING,    //����ѵ��
    STR_SPORT_SKATING,          //����
    STR_SPORT_FITNESS_GAMING,   //������Ϸ
    STR_SPORT_AEROBICS,         //�����
    STR_SPORT_GROUP_TRAINING,   //�����
    STR_SPORT_KENDO,            //������
    STR_SPORT_LACROSSE,         //��������
    STR_SPORT_ROLLING,          //��ĭ���Ĥ����
    STR_SPORT_WRESTLING,        //ˤ��
    STR_SPORT_FENCING,          //����
    STR_SPORT_SOFTBALL,         //����
    STR_SPORT_SINGLE_BAR,       //����
    STR_SPORT_PARALLEL_BARS,    //˫��
    STR_SPORT_ROLLER_SKATING,   //�ֻ�
    STR_SPORT_HULA_HOOP,        //����Ȧ
    STR_SPORT_DARTS,            //����
    STR_SPORT_PICKLEBALL,       //ƥ����
    STR_SPORT_HIIT,             //HIIT
    STR_SPORT_SHOOTING,         //���
    STR_SPORT_JUDO,             //���
    STR_SPORT_TRAMPOLINE,       //�Ĵ�
    STR_SPORT_SKATEBOARDING,    //����
    STR_SPORT_HOVERBOARD,       //ƽ�⳵
    STR_SPORT_BLADING,          //�ﺵ��
    STR_SPORT_PARKOUR,          //�ܿ�
    STR_SPORT_DIVING,           //��ˮ
    STR_SPORT_SURFING,          //����
    STR_SPORT_SNORKELING,       //��Ǳ
    STR_SPORT_PULL_UP,          //��������
    STR_SPORT_PUSH_UP,          //���Գ�
    STR_SPORT_PLANKING,         //ƽ��֧��
    STR_SPORT_ROCK_CLIMBING,    //����
    STR_SPORT_HIGHTJUMP,        //����
    STR_SPORT_BUNGEE_JUMPING,   //�ļ�
    STR_SPORT_LONGJUMP,         //��Զ
    STR_SPORT_MARATHON,         //������
    STR_SPORT_OTHER,            //�������жϴ����ͣ���ת����������
    STR_SPORT_CYWEE_SWIM_GNSS,  //��������Ӿ�㷨����gps���� 20201009
    STR_SPORT_CYWEE_SWIM,       //��������Ӿ�㷨 20201009
    STR_SPORT_EXIT_MSG1,
    STR_SPORT_EXIT_MSG2,
    STR_SPORT_EXIT_MSG3,

    STR_STEP,
    STR_SLEEP,
    STR_MUSIC,
    STR_HEART_RATE,
    STR_BLOOD_PRESSURE,
    STR_BLOOD_OXYGEN,
    STR_PRESSURE,
    STR_BLOOD_SUGAR,
    STR_WEATHER,
    STR_BREATHE,
    STR_BREATHE_INHALE,
    STR_BREATHE_EXHALE,
    STR_ACTIVITY_RECORD,
    STR_CONNECT_PHONE,
    STR_CALCULATOR,
    STR_ALARM_CLOCK,
    STR_ALARM_CLOCK_EDIT,
    STR_ALARM_CLOCK_REPEAT,
    STR_ALARM_CLOCK_SET,
    STR_ALARM_CLOCK_MON,
    STR_ALARM_CLOCK_TUE,
    STR_ALARM_CLOCK_WED,
    STR_ALARM_CLOCK_THU,
    STR_ALARM_CLOCK_FRI,
    STR_ALARM_CLOCK_SAT,
    STR_ALARM_CLOCK_SUN,
    STR_ALARM_CLOCK1,
    STR_ALARM_CLOCK2,
    STR_ALARM_CLOCK3,
    STR_TIMER,
    STR_STOP_WATCH,
    STR_STOP_WATCH_RECORD,
    STR_CAMERA,
    STR_VOICE_ASSISTANT,
    STR_FIND_PHONE,
    STR_FIND_PHONEING,
    STR_GAME,
    STR_SETTING,
    STR_SETTING_LIGHT,
    STR_SETTING_DOUSING,
    STR_FIVE_SEC,
    STR_TEN_SEC,
    STR_TWENTY_SEC,
    STR_THIRTY_SEC,
    STR_ONE_MIN,
    STR_FIVE_MIN,
    STR_NEVER,
    STR_SETTING_UP,
    STR_SETTING_DISTURD,
    STR_DISTURD_ALL,
    STR_DISTURD_TIM,
    STR_DISTURD_TIM_CTT,
    STR_DISTURD_TIM_START,
    STR_DISTURD_TIM_END,
    STR_SETTING_SAV,
    STR_VOL,
    STR_SHK,
    STR_MEDIA_VOL,
    STR_CALL_VOL,
    STR_SETTING_LANGUAGE,
    STR_LANGUAGE_CN,
    STR_LANGUAGE_ENG,
    STR_LANGUAGE_FN,
    STR_LANGUAGE_RU,
    STR_LANGUAGE_AT,
    STR_LANGUAGE_JP,
    STR_SETTING_TIME,
    STR_CUSTOM_CALENDAR,
    STR_CUSTOM_TIME,
    STR_CUSTOM_DATA,
    STR_SETTING_CALENDAR,
    STR_SETTING_PASSWORD,
    STR_SETTING_ABOUT,
    STR_SETTING_RESTART,
    STR_SETTING_RSTFY,
    STR_SETTING_OFF,
    STR_SETTING_4G,
    STR_CALL_RECENT,
    STR_CALL_RECENT_ONE,
    STR_CALL_RECENT_TWS,
    STR_CALL_RECENT_THR,
    STR_CALL_RECENT_FOUR,
    STR_CALL_RECENT_FIVE,
    STR_CALL_RECENT_SIX,
    STR_CALL_RECENT_SEV,
    STR_CALL_LINK,
    STR_CALL_LINK_ONE,
    STR_CALL_LINK_TWS,
    STR_CALL_LINK_THR,
    STR_CALL_LINK_FOUR,
    STR_CALL_LINK_FIVE,
    STR_CALL_LINK_SIX,
    STR_CALL_LINK_SEV,
    STR_CALL_DIAL,
    STR_ALIPAY,
    STR_ALIPAY_BIND_TIP,
    STR_ALIPAY_UNBIND_TIP,
    STR_ALIPAY_UNBIND_TIP2,
    STR_CONFIMR,
    STR_CANCEL,
    STR_BINDING,
    STR_BIND_FAILED,
    STR_BIND_SUCCESS,
    STR_PAYCODE,
    STR_HELP,
    STR_HELP_TILE,
    STR_SWITCH_QRCODE,
    STR_SWITCH_BARCODE,
    STR_UNBIND,
    STR_ALTITUDE,
    STR_COMPASS,
    STR_FLASHLIGHT,
    STR_MAP,
    STR_SCAN,
    STR_STYLE,
    STR_VOICE,
    STR_VOLUME,
    STR_CALENDAR,
    STR_LATEST_APP,
    STR_CALORIE,
    STR_STEPS,
    STR_DISTANCE,
    STR_RECORDER,
    STR_BAROMETER,
    STR_TEMPERATURE,
    STR_CELSIUS,
    STR_FAHRENHEIT,
    STR_YEAR,
    STR_MON,
    STR_MDAY,

    STR_SUNDAY,
    STR_MONDAY,
    STR_TUESDAY,
    STR_WEDNESDAY,
    STR_THURSDAY,
    STR_FRIDAY,
    STR_SATURDAY,
    STR_CLOUDY,
    STR_SUNNY,
    STR_SNOWY,
    STR_RAINY,
    STR_OVERCAST,
    STR_SAND_AND_DUST,
    STR_WINDY,
    STR_HAZE,
    STR_KCAL,
    STR_KM,
    STR_HOUR,
    STR_MIN,
    STR_SEC,
    STR_MILE,
    STR_START,
    STR_MEASURING,
    STR_WEAR_CHECK,
    STR_UNKNOWN,
    STR_COMPO_SELECT,

    STR_STYLE_SUDOKU_1,
    STR_STYLE_SUDOKU_2,
    STR_STYLE_SUDOKU_3,
    STR_STYLE_DISK,
    STR_STYLE_GRID_1,
    STR_STYLE_GRID_2,
    STR_STYLE_HEXAGON,
    STR_STYLE_HONEYCOMB,
    STR_STYLE_KALEIDOSCOPE,
    STR_STYLE_LIST_1,
    STR_STYLE_LIST_2,
    STR_STYLE_RING,
    STR_STYLE_SKYRER,
    STR_STYLE_WATERFALL,
    STR_STYLE_FOOTBALL,
    STR_STYLE_GRID_HONEYCOMB,

    STR_DEV_NEME,
    STR_BLE_MAC,
    STR_SYS_VERSION,
    STR_NULL,

    STR_SOOTHING,
    STR_SLOW,
    STR_FASTER,

    STR_OK,
    STR_MINUTE,
    STR_SETTING_MODE,
    STR_DRIZZLE,
    STR_SLEET,
    STR_TOMORROW_NIGHT,
    STR_CLOUDY_NIGHT,
    STR_RAINY_NIGHT,
    STR_RAINY_SHOWERS,
    STR_MODERATE_RAIN,
    STR_HEAVY_RAIN,

    STR_NO_MSG,
    STR_FIND_WATCH,
    STR_SET_DATA_READ,
    STR_SET_STEP_COUNT,
    STR_SET_DISTANCE,
    STR_SET_CALORIE,
    STR_GOAL_ACHIEVE,
    STR_SEDENTARY_REMIND,
    STR_DEEP_SLEEP,
    STR_LIGHT_SLEEP,
    STR_ALWAKE_SLEEP,
    STR_EYE_MOVE_SLEEP,
};

extern const char * const *i18n;

void lang_select(int lang_id);

#endif
