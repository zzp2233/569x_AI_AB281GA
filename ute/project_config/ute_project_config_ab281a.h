/**
*@file
*@brief     ab281项目配置文件
*@details
*@author        zn.zeng
*@date        2024-03-22
*@version       v1.0
*/
#ifndef _UTE_PROJECT_CONFIG_AB281A_H_
#define _UTE_PROJECT_CONFIG_AB281A_H_

#define DEFAULT_BLE_DEV_NAME "T62"
#define UTE_SW_VERSION "AB281AV000027"

/*! 指定资源路径,如果不定义则使用对应项目号的路径,wang.luo 2025-01-07 */
#define UTE_UI_CONFIG_PATCH "AB281"

/* 睡眠数据采样支持 zn.zeng 2021-12-29*/
#define UTE_MODULE_SLEEP_SAMPLE_DATA_SUPPORT 1
/*gesnsor 采集原始数据*/
#define APP_DBG_GSENSOR_DATA 1

#define UTE_DRV_DSPI_FOR_SCREEN_SUPPORT 0
#define UTE_DRV_QSPI_FOR_SCREEN_SUPPORT 1

#define UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT 0
#define DRV_BOE_S240X284_I183_JD9853_WV018LZQ_QSPI_SUPPORT 0
#define UTE_DRV_TFT_S240X284_NV3030B_ZD183G1196_QSPI_SUPPORT 1
#define UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT 0
#define UTE_DRV_TFT_S360X360_NV3030B_ZD138G1616_QSPI_SUPPORT 0


/*! 屏最大亮度百分比 dengli.lu, 2021-10-29  */
#define DEFAULT_BACK_LIGHT_PERCENT_MAX 100

/*! 屏最小亮度百分比 dengli.lu, 2021-10-29  */
#define DEFAULT_BACK_LIGHT_PERCENT_MIN 20

/*! 调节屏亮度时增加或者减少的百分比 dengli.lu, 2021-10-29  */
#define BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE 20

/*! 默认背光百分比zn.zeng, 2021-09-24  */
#define DEFAULT_SCREEN_BACKLIGHT_PERCENT    (BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE*3)

//Gsensor
#define UTE_DRV_GSENSOR_SC7A20H_SUPPORT 1
#define UTE_DRV_STK8321_SUPPORT 0

/*! 抬手亮屏参数 zn.zeng, 2021-10-22  */
#define ROLLOVER_HAND_SCREEN_X_MIN  -40
#define ROLLOVER_HAND_SCREEN_X_MAX  140
#define ROLLOVER_HAND_SCREEN_Y_MIN  -46
#define ROLLOVER_HAND_SCREEN_Y_MAX  40
#define ROLLOVER_HAND_SCREEN_Z_MIN  -136
#define ROLLOVER_HAND_SCREEN_Z_MAX  128
#define ROLLOVER_HAND_SCREEN_ACC_DIFF_VALUE  20

#define QUICK_SWITCH_NOT_DISTURB_SUPPORT 1
#define QUICK_SWITCH_ANTILOST_SUPPORT   1
#define QUICK_SWITCH_DISPLAY_TIME_SUPPORT   1
#define QUICK_SWITCH_FINDBAND_SUPPORT 0
#define QUICK_SWITCH_TURNTHEWRIST_SUPPORT 1
#define QUICK_SWITCH_SHOCK_TIME_SUPPORT    1
#define QUICK_SWITCH_LOCAL_WURAO_SET_TIME_SUPPORT 1

/*! PWRKEY,wang.luo 2024-12-03 */
#define UTE_DRV_PWRKEY_SUPPORT 1
/*! PWRKEY按键最大数量,wang.luo 2024-12-03 */
#define UTE_DRV_PWRKEY_MAX_CNT 1

/*! 心率最大最小值数字显示跟随柱状图中的最大最小值 xjc, 2022-01-17  */
#define UTE_HEART_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT 1

/*! 心率警告功能 zn.zeng, 2021-10-22  */
#define UTE_MODULE_HEART_MIN_MAX_WARNING_VALUE_SUPPORT 1

/*! 新工厂测试,xjc 2022-02-09*/
#define UTE_MODULE_NEW_FACTORY_TEST_SUPPORT 1

#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
#define UTE_MODULE_SCREENS_NEW_FACTORY_MODE_SELECT_SUPPORT 1
#define UTE_MODULE_SCREENS_NEW_FACTORY_AGING_TEST_SUPPORT 1
#define UTE_MODULE_SCREENS_NEW_FACTORY_MODULE_TEST_SUPPORT 1

#define UTE_MODULE_NEW_FACTORY_TEST_JUST_HEART_CHECK_LIGHT_SUPPORT  0 //开机显示心率漏光界面
#define UTE_MODULE_NEW_FACTORY_MODULE_TEST_ALLOW_RETEST_SUPPORT 1 //fail项目允许在列表中点击重新测试
#define UTE_MODULE_NEW_FACTORY_AGING_REPORT_SUPPORT 0 //老化测试生成测试报告
#define UTE_MODULE_NEW_FACTORY_MODULE_4X5_TP_TEST_SUPPORT 1 //4*5 = 20宫格TP触摸测试
#define UTE_MODULE_NEW_FACTORY_MODULE_4X5_TP_TOTAL 20
// #define UTE_MODULE_NEW_FACTORY_MODULE_MAX 13
#define UTE_MODULE_NEW_FACTORY_MODULE_END_RESET_SUPPORT 1 //模块测试结束之后复位重启
#define UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT 0 //新工厂模块测试默认有心率漏光测试
#define UTE_DRV_NEW_FACTORY_TEST_BATTERY_CE_AUTH_ALLOW_TEMPERATURE 58.0f //工厂测试温度限制
#define UTE_DRV_NEW_FACTORY_TEST_BATTERY_CE_AUTH_NOT_ALLOW_TEMPERATURE 60.0f
#define UTE_MODULE_NEW_FACTORY_MODE_SELECT_SCREENS_STRING_ENLARGEMENT 2
#define UTE_MODULE_NEW_FACTORY_AGING_TEST_SCREENS_STRING_ENLARGEMENT 2
#define UTE_MODULE_NEW_FACTORY_MODULE_TEST_SCREENS_STRING_ENLARGEMENT 2
#define UTE_MODULE_NEW_FACTORY_TEST_CHAR_EN_SUPPORT  1
#endif

/* 电池容量 zn.zeng 2022-01-07*/
#define UTE_DRV_BATTERY_ELECTRICITY_POWER_MAH      280 //mAh

/* 电池曲线 zn.zeng 2022-01-03*/
#define UTE_DRV_BATTERY_000      3564
#define UTE_DRV_BATTERY_010      3698
#define UTE_DRV_BATTERY_020      3744
#define UTE_DRV_BATTERY_030      3765
#define UTE_DRV_BATTERY_040      3780
#define UTE_DRV_BATTERY_050      3807
#define UTE_DRV_BATTERY_060      3856
#define UTE_DRV_BATTERY_070      3908
#define UTE_DRV_BATTERY_080      3964
#define UTE_DRV_BATTERY_090      4045
#define UTE_DRV_BATTERY_100      4128

#define UTE_DRV_TP_X_AXIS_EXCHANGE 0 // 交换X轴左右坐标
#define UTE_DRV_TP_Y_AXIS_EXCHANGE 0 // 交换Y轴上下坐标

/*! 一级界面默认排序,wang.luo 2024-11-16 */
#define UTE_CUI_SCREEN_TBL_SORT_CNT_DEFAULT 8
#define UTE_CUI_SCREEN_TBL_SORT_ARRAY_DEFAULT {FUNC_CLOCK, FUNC_ACTIVITY, FUNC_BLOOD_OXYGEN, FUNC_HEARTRATE, FUNC_SLEEP, FUNC_WEATHER, FUNC_BT, FUNC_COMPO_SELECT}
/*! 默认表盘索引 zn.zeng, 2021-10-25  */
#define DEFAULT_WATCH_INDEX 0
/*! 最大表盘数量，不包括在线表盘 zn.zeng, 2021-10-23  */
#define UTE_MODULE_SCREENS_WATCH_CNT_MAX 6
/*! 表盘排序地址数组,wang.luo 2024-11-26 */
#define UTE_MODULE_WATCHS_SORT_ADDRESS_ARRAYS {UI_BUF_DIALPLATE_D17312001_BIN, \
                                                UI_BUF_DIALPLATE_D17196001_BIN, \
                                                UI_BUF_DIALPLATE_D17195001_BIN, \
                                                UI_BUF_DIALPLATE_D17194001_BIN, \
                                                UI_BUF_DIALPLATE_BTF_BIN, \
                                                UI_BUF_DIALPLATE_CUBE_BIN}

#define UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT 1 // 使用蝴蝶表盘
#define UTE_WATCHS_CUBE_DIAL_SUPPORT 1      // 使用立方体表盘
#define UTE_WATCHS_DIALPLATE_BTF_INDEX 4    // 蝴蝶表盘索引
#define UTE_WATCHS_DIALPLATE_CUBE_INDEX 5   // 立方体表盘索引

/*! SOS联系人拨号功能,xjc 2022-07-06  */
#define UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT 1

#define GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT      1
#define GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT      0

#define UTE_ALARM_NOTDISTURB_ALLOW_MOTOR_VIBRATION_SUPPORT 1 //勿扰模式闹钟开启马达震动

#if UTE_DRV_TFT_S360X360_NV3030B_HY139071A_QSPI_SUPPORT
#define UTE_DRV_SCREEN_WIDTH 360
#define UTE_DRV_SCREEN_HEIGHT 360
#define UTE_DRV_SCREEN_SHAPE 0
#elif UTE_DRV_TFT_S240X284_NV3030B_HY201068AVC_QSPI_SUPPORT
#define UTE_DRV_SCREEN_WIDTH 240
#define UTE_DRV_SCREEN_HEIGHT 296
#else
#define UTE_DRV_SCREEN_WIDTH 240
#define UTE_DRV_SCREEN_HEIGHT 284
#endif

#define UTE_MODULE_SCREENS_POWERON_SUPPORT            1 //开机logo界面
#define UTE_MODULE_SCREENS_UP_MENU_SUPPORT            1 //上拉菜单
#define UTE_MODULE_SCREENS_DWON_MENU_SUPPORT          1 //下拉菜单
#define UTE_MODULE_SCREENS_LINK_MAN_SUPPORT           1 //联系人
#define UTE_MODULE_SCREENS_TIMER_SUPPORT              1 //计时器
#define UTE_MODULE_SCREENS_CAMERA_SUPPORT             1 //遥控拍照
#define UTE_MODULE_SCREENS_STOPWATCH_SUPPORT          1 //秒表
#define UTE_MODULE_SCREENS_VOICE_SUPPORT              1 //语音
#define UTE_MODULE_SCREENS_ALARM_SUPPORT              1 //闹钟
#define UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT  1 //在线表盘同步界面
#define UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT 1 //低电提醒
#define UTE_MODULE_SCREENS_WEATHER_SUPPORT            1 //天气
#define UTE_MODULE_SCREENS_GAME_SUPPORT               1 //游戏
#define UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT         1 //找手机
#define UTE_MODULE_SCREENS_RESFY_SUPPORT              1 //恢复出厂
#define UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT       1 //血氧
#define UTE_MODULE_SCREENS_ACTIVITY_SUPPORT           1 //每日活动
#define UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT         1 //手电筒
#define UTE_MODULE_SCREENS_SPORT_SUPPORT              1 //运动
#define UTE_MODULE_SCREENS_LANGUAGE_SUPPORT           1 //语言
#define UTE_MODULE_SCREENS_BREATHE_SUPPORT            1 //呼吸
#define UTE_MODULE_SCREENS_HEARTRATE_SUPPORT          1 //心率
#define UTE_MODULE_SCREENS_STYLE_SUPPORT              1 //主题
#define UTE_MODULE_SCREENS_SLEEP_SUPPORT              1 //睡眠
#define UTE_MODULE_SCREENS_LIGHT_SUPPORT              1 //亮度调节
#define UTE_MODULE_SCREENS_MUSIC_SUPPORT              1 //音乐
#define UTE_MODULE_SCREENS_MESSAGE_SUPPORT            1 //信息
#define UTE_MODULE_SCREENS_CALL_SUPPORT               1 //电话
#define UTE_MODULE_SCREENS_SETTING_SUPPORT            1 //设置
#define UTE_MODULE_SCREENS_CALCULATOR_SUPPORT         1 //计算器
#define UTE_MODULE_SCREENS_RESTART_SUPPORT            1 //重启
#define UTE_MODULE_SCREENS_CALENDAER_SUPPORT          1 //日历
#define UTE_MODULE_SCREENS_SCAN_SUPPORT               1 //二维码
#define UTE_MODULE_SCREENS_HEART_WARNING_SUPPORT      1 //心率预警

/*! 默认语言 zn.zeng, 2021-08-23  */
#ifndef DEFAULT_LANGUAGE
#define DEFAULT_LANGUAGE 0x02//ENGLISH_LANGUAGE_ID
#define SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT              1 //中文 1
#define SCREEN_TITLE_MULTIPLE_ENGLISH_LANGUAGE_SUPPORT              1 //英文 2
#define SCREEN_TITLE_MULTIPLE_KOREAN_LANGUAGE_SUPPORT               0 //韩文 3
#define SCREEN_TITLE_MULTIPLE_JAPANESE_LANGUAGE_SUPPORT             1 //日文 4
#define SCREEN_TITLE_MULTIPLE_GERMAN_LANGUAGE_SUPPORT               1 //德文 5
#define SCREEN_TITLE_MULTIPLE_SPANISH_LANGUAGE_SUPPORT              1 //西班牙文 6
#define SCREEN_TITLE_MULTIPLE_FRENCH_LANGUAGE_SUPPORT               1 //法文 7
#define SCREEN_TITLE_MULTIPLE_ITALIAN_LANGUAGE_SUPPORT              1 //意大利文 8
#define SCREEN_TITLE_MULTIPLE_PORTUGUESE_LANGUAGE_SUPPORT           1 //葡萄牙文 9
#define SCREEN_TITLE_MULTIPLE_ARABIC_LANGUAGE_SUPPORT               0 //阿拉伯文 A
#define SCREEN_TITLE_MULTIPLE_INDIA_LANGUAGE_SUPPORT                0 //India     B
#define SCREEN_TITLE_MULTIPLE_HINDI_LANGUAGE_SUPPORT                0 //印地语  C
#define SCREEN_TITLE_MULTIPLE_POLISH_LANGUAGE_SUPPORT               0 //波兰语  D
#define SCREEN_TITLE_MULTIPLE_RUSSIAN_LANGUAGE_SUPPORT              1 //俄语  E
#define SCREEN_TITLE_MULTIPLE_DUTCH_LANGUAGE_SUPPORT                0 //荷兰语 F
#define SCREEN_TITLE_MULTIPLE_TURKISH_LANGUAGE_SUPPORT              1 //土耳其语  10
#define SCREEN_TITLE_MULTIPLE_BENGALI_LANGUAGE_SUPPORT              0 //孟加拉语  11  Bengali
#define SCREEN_TITLE_MULTIPLE_URDU_LANGUAGE_SUPPORT                 0 //乌尔都语   12  Urdu
#define SCREEN_TITLE_MULTIPLE_INDONESUAN_LANGUAGE_SUPPORT           0 //印度尼西亚语（爪哇语）  13  Indonesian
#define SCREEN_TITLE_MULTIPLE_PUNJABI_LANGUAGE_SUPPORT              0 //旁遮普语  14   Punjabi
#define SCREEN_TITLE_MULTIPLE_THAI_LANGUAGE_SUPPORT                 1 //泰文  15   Thai
#define SCREEN_TITLE_MULTIPLE_CZECH_LANGUAGE_SUPPORT                0 //捷克语  16  Czech
#define SCREEN_TITLE_MULTIPLE_TRADITIONAL_CHINESE_LANGUAGE_SUPPORT  1 //繁体中文
#define SCREEN_TITLE_MULTIPLE_ROMANIAN_LANGUAGE_SUPPORT             0 //罗马尼亚语 limba 0x1B
#define SCREEN_TITLE_MULTIPLE_VIETNAMESE_LANGUAGE_SUPPORT           0 //越南语 0x63
#endif // DEFAULT_LANGUAGE

/*! 多运动支持百种运动标志, xjc 2022-03-09  */
#define UTE_MODULE_SPORT_HUNDRED_SUPPORT 1
#define UTE_MODULE_SPORT_MAX_SPORT_NUM          24
#define UTE_MODULE_SPORT_DISPLAY_MIN_SPORT_NUM  4
#define UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM  (UTE_MODULE_SPORT_MAX_SPORT_NUM)
/*! 具体的运动类型,支持的置1,不支持置0, xjc 2222-03-29*/
#define UTE_MODULE_SPORT_RUNNING_SUPPORT                1 //跑步
#define UTE_MODULE_SPORT_BIKE_SUPPORT                   1 //骑行
#define UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT              1 //跳绳
#define UTE_MODULE_SPORT_SWIMMING_SUPPORT               0 //游泳
#define UTE_MODULE_SPORT_BADMINTION_SUPPORT             1 //羽毛球
#define UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT           1 //乒乓球
#define UTE_MODULE_SPORT_TENNIS_SUPPORT                 1 //网球
#define UTE_MODULE_SPORT_CLIMBING_SUPPORT               1 //登山
#define UTE_MODULE_SPORT_WALKING_SUPPORT                1 //徒步
#define UTE_MODULE_SPORT_BASKETBALL_SUPPORT             1 //篮球
#define UTE_MODULE_SPORT_FOOTBALL_SUPPORT               1 //足球
#define UTE_MODULE_SPORT_BASEBALL_SUPPORT               1 //棒球（或垒球）
#define UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT             1 //排球
#define UTE_MODULE_SPORT_CRICKET_SUPPORT                1 //板球
#define UTE_MODULE_SPORT_RUGBY_SUPPORT                  1 //橄榄球（或美式足球）
#define UTE_MODULE_SPORT_HOCKEY_SUPPORT                 1 //曲棍球
#define UTE_MODULE_SPORT_DANCE_SUPPORT                  1 //跳舞
#define UTE_MODULE_SPORT_SPINNING_SUPPORT               1 //动感单车
#define UTE_MODULE_SPORT_YOGA_SUPPORT                   1 //瑜伽
#define UTE_MODULE_SPORT_SIT_UP_SUPPORT                 1 //仰卧起坐
#define UTE_MODULE_SPORT_TREADMILL_SUPPORT              1 //跑步机
#define UTE_MODULE_SPORT_GYMNASTICS_SUPPORT             1 //体操
#define UTE_MODULE_SPORT_BOATING_SUPPORT                1 //划船
#define UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT           1 //开合跳
#define UTE_MODULE_SPORT_FREE_TRAINING_SUPPORT          1 //自由训练
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
#define UTE_MODULE_SPORT_INDOOR_WALK_SUPPORT            0 //0x1A,室内走路 
#define UTE_MODULE_SPORT_INDOOR_RUN_SUPPORT             0 //0x1B,室内跑步
#define UTE_MODULE_SPORT_STRENGTH_TRAINING_SUPPORT      0 //0x1C,力量训练
#define UTE_MODULE_SPORT_STEP_TRAINING_SUPPORT          0 //0x1D,踏步
#define UTE_MODULE_SPORT_HORSE_RIDING_SUPPORT           0 //0x1E,骑马
#define UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_SUPPORT     0 //0x1F,椭圆机
#define UTE_MODULE_SPORT_TAI_CHI_SUPPORT                0 //0x20,太极
#define UTE_MODULE_SPORT_SHUTTLECOCK_SUPPORT            0 //0x21,毽球
#define UTE_MODULE_SPORT_BOXING_SUPPORT                 0 //0x22,拳击
#define UTE_MODULE_SPORT_OUTDOOR_WALK_SUPPORT           0 //0x23,户外健走
#define UTE_MODULE_SPORT_TRAIL_RUNNING_SUPPORT          0 //0x24,越野跑
#define UTE_MODULE_SPORT_SKIING_SUPPORT                 0 //0x25,滑雪
#define UTE_MODULE_SPORT_ICE_HOCKEY_SUPPORT             0 //0x26,冰球
#define UTE_MODULE_SPORT_TAEKWONDO_SUPPORT              0 //0x27,跆拳道
#define UTE_MODULE_SPORT_VO2MAX_TEST_SUPPORT            0 //0x28,最大摄氧量测试
#define UTE_MODULE_SPORT_ROWING_MACHING_SUPPORT         0 //0x29,划船机
#define UTE_MODULE_SPORT_AIR_WALKER_SUPPORT             0 //0x2A,漫步机
#define UTE_MODULE_SPORT_HIKING_SUPPORT                 0 //0x2B,徒步
#define UTE_MODULE_SPORT_ATHLETICS_SUPPORT              0 //0x2C,田径
#define UTE_MODULE_SPORT_WAIST_TRAINING_SUPPORT         0 //0x2D,腰腹训练
#define UTE_MODULE_SPORT_KARATE_SUPPORT                 0 //0x2E,空手道
#define UTE_MODULE_SPORT_COOLDOWN_SUPPORT               0 //0x2F,整理放松
#define UTE_MODULE_SPORT_CROSS_TRAINING_SUPPORT         0 //0x30,交叉训练
#define UTE_MODULE_SPORT_PILATES_SUPPORT                0 //0x31,普拉提
#define UTE_MODULE_SPORT_CROSS_FIT_SUPPORT              0 //0x32,交叉配合
#define UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_SUPPORT    0 //0x33,功能性训练
#define UTE_MODULE_SPORT_PHYSICAL_TRAINING_SUPPORT      0 //0x34,体能训练
#define UTE_MODULE_SPORT_ARCHERY_SUPPORT                0 //0x35,射箭
#define UTE_MODULE_SPORT_FLEXIBILITY_SUPPORT            0 //0x36,柔韧度
#define UTE_MODULE_SPORT_MIXED_CARDIO_SUPPORT           0 //0x37,混合有氧
#define UTE_MODULE_SPORT_LATIN_DANCE_SUPPORT            0 //0x38,拉丁舞
#define UTE_MODULE_SPORT_STREET_DANCE_SUPPORT           0 //0x39,街舞
#define UTE_MODULE_SPORT_KICKBOXING_SUPPORT             0 //0x3A,自由搏击
#define UTE_MODULE_SPORT_BARRE_SUPPORT                  0 //0x3B,芭蕾
#define UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_SUPPORT    0 //0x3C,澳式足球
#define UTE_MODULE_SPORT_MARTIAL_ARTS_SUPPORT           0 //0x3D,武术
#define UTE_MODULE_SPORT_STAIRS_SUPPORT                 0 //0x3E,爬楼
#define UTE_MODULE_SPORT_HANDBALL_SUPPORT               0 //0x3F,手球
#define UTE_MODULE_SPORT_BOWLING_SUPPORT                0 //0x40,保龄球
#define UTE_MODULE_SPORT_RACQUETBALL_SUPPORT            0 //0x41,壁球
#define UTE_MODULE_SPORT_CURLING_SUPPORT                0 //0x42,冰壶
#define UTE_MODULE_SPORT_HUNTING_SUPPORT                0 //0x43,打猎
#define UTE_MODULE_SPORT_SNOWBOARDING_SUPPORT           0 //0x44,单板滑雪
#define UTE_MODULE_SPORT_PLAY_SUPPORT                   0 //0x45,休闲运动
#define UTE_MODULE_SPORT_AMERICAN_FOOTBALL_SUPPORT      0 //0x46,美式橄榄球
#define UTE_MODULE_SPORT_HAND_CYCLING_SUPPORT           0 //0x47,手摇车
#define UTE_MODULE_SPORT_FISHING_SUPPORT                0 //0x48,钓鱼
#define UTE_MODULE_SPORT_DISC_SPORTS_SUPPORT            0 //0x49,飞盘
#define UTE_MODULE_SPORT_RUGGER_SUPPORT                 0 //0x4A,橄榄球
#define UTE_MODULE_SPORT_GOLF_SUPPORT                   0 //0x4B,高尔夫
#define UTE_MODULE_SPORT_FOLK_DANCE_SUPPORT             0 //0x4C,民族舞
#define UTE_MODULE_SPORT_DOWNHILL_SKIING_SUPPORT        0 //0x4D,高山滑雪
#define UTE_MODULE_SPORT_SNOW_SPORTS_SUPPORT            0 //0x4E,雪上运动
#define UTE_MODULE_SPORT_MIND_BODY_SUPPORT              0 //0x4F,舒缓冥想类运动
#define UTE_MODULE_SPORT_CORE_TRAINING_SUPPORT          0 //0x50,核心训练
#define UTE_MODULE_SPORT_SKATING_SUPPORT                0 //0x51,滑冰
#define UTE_MODULE_SPORT_FITNESS_GAMING_SUPPORT         0 //0x52,健身游戏
#define UTE_MODULE_SPORT_AEROBICS_SUPPORT               0 //0x53,健身操
#define UTE_MODULE_SPORT_GROUP_TRAINING_SUPPORT         0 //0x54,团体操
#define UTE_MODULE_SPORT_KENDO_SUPPORT                  0 //0x55,搏击操
#define UTE_MODULE_SPORT_LACROSSE_SUPPORT               0 //0x56,长曲棍球
#define UTE_MODULE_SPORT_ROLLING_SUPPORT                0 //0x57,泡沫轴筋膜放松
#define UTE_MODULE_SPORT_WRESTLING_SUPPORT              0 //0x58,摔跤
#define UTE_MODULE_SPORT_FENCING_SUPPORT                0 //0x59,击剑
#define UTE_MODULE_SPORT_SOFTBALL_SUPPORT               0 //0x5A,垒球
#define UTE_MODULE_SPORT_SINGLE_BAR_SUPPORT             0 //0x5B,单杠
#define UTE_MODULE_SPORT_PARALLEL_BARS_SUPPORT          0 //0x5C,双杠
#define UTE_MODULE_SPORT_ROLLER_SKATING_SUPPORT         0 //0x5D,轮滑
#define UTE_MODULE_SPORT_HULA_HOOP_SUPPORT              0 //0x5E,呼啦圈
#define UTE_MODULE_SPORT_DARTS_SUPPORT                  0 //0x5F,飞镖
#define UTE_MODULE_SPORT_PICKLEBALL_SUPPORT             0 //0x60,匹克球
#define UTE_MODULE_SPORT_HIIT_SUPPORT                   0 //0x61,HIIT
#define UTE_MODULE_SPORT_SHOOTING_SUPPORT               0 //0x62,射击
#define UTE_MODULE_SPORT_JUDO_SUPPORT                   0 //0x63,柔道
#define UTE_MODULE_SPORT_TRAMPOLINE_SUPPORT             0 //0x64,蹦床
#define UTE_MODULE_SPORT_SKATEBOARDING_SUPPORT          0 //0x65,滑板
#define UTE_MODULE_SPORT_HOVERBOARD_SUPPORT             0 //0x66,平衡车
#define UTE_MODULE_SPORT_BLADING_SUPPORT                0 //0x67,溜旱冰
#define UTE_MODULE_SPORT_PARKOUR_SUPPORT                0 //0x68,跑酷
#define UTE_MODULE_SPORT_DIVING_SUPPORT                 0 //0x69,跳水
#define UTE_MODULE_SPORT_SURFING_SUPPORT                0 //0x6A,冲浪
#define UTE_MODULE_SPORT_SNORKELING_SUPPORT             0 //0x6B,浮潜
#define UTE_MODULE_SPORT_PULL_UP_SUPPORT                0 //0x6C,引体向上
#define UTE_MODULE_SPORT_PUSH_UP_SUPPORT                0 //0x6D,俯卧撑
#define UTE_MODULE_SPORT_PLANKING_SUPPORT               0 //0x6E,平板支撑
#define UTE_MODULE_SPORT_ROCK_CLIMBING_SUPPORT          0 //0x6F,攀岩
#define UTE_MODULE_SPORT_HIGHTJUMP_SUPPORT              0 //0x70,跳高
#define UTE_MODULE_SPORT_BUNGEE_JUMPING_SUPPORT         0 //0x71,蹦极
#define UTE_MODULE_SPORT_LONGJUMP_SUPPORT               0 //0x72,跳远
#define UTE_MODULE_SPORT_MARATHON_SUPPORT               0 //0x73,马拉松
/*! 运动显示开关,默认显示的置1,不显示置0, xjc 2222-03-29*/
#define UTE_MODULE_SPORT_RUNNING_ON_OFF                 1 //跑步
#define UTE_MODULE_SPORT_BIKE_ON_OFF                    1 //骑行
#define UTE_MODULE_SPORT_JUMP_ROPE_ON_OFF               1 //跳绳
#define UTE_MODULE_SPORT_SWIMMING_ON_OFF                0 //游泳
#define UTE_MODULE_SPORT_BADMINTION_ON_OFF              1 //羽毛球
#define UTE_MODULE_SPORT_TABLE_TENNIS_ON_OFF            1 //乒乓球
#define UTE_MODULE_SPORT_TENNIS_ON_OFF                  1 //网球
#define UTE_MODULE_SPORT_CLIMBING_ON_OFF                1 //登山
#define UTE_MODULE_SPORT_WALKING_ON_OFF                 1 //徒步
#define UTE_MODULE_SPORT_BASKETBALL_ON_OFF              1 //篮球
#define UTE_MODULE_SPORT_FOOTBALL_ON_OFF                1 //足球
#define UTE_MODULE_SPORT_BASEBALL_ON_OFF                1 //棒球（或垒球）
#define UTE_MODULE_SPORT_VOLLEYBALL_ON_OFF              1 //排球
#define UTE_MODULE_SPORT_CRICKET_ON_OFF                 1 //板球
#define UTE_MODULE_SPORT_RUGBY_ON_OFF                   1 //橄榄球（或美式足球）
#define UTE_MODULE_SPORT_HOCKEY_ON_OFF                  1 //曲棍球
#define UTE_MODULE_SPORT_DANCE_ON_OFF                   1 //跳舞
#define UTE_MODULE_SPORT_SPINNING_ON_OFF                1 //动感单车
#define UTE_MODULE_SPORT_YOGA_ON_OFF                    1 //瑜伽
#define UTE_MODULE_SPORT_SIT_UP_ON_OFF                  1 //仰卧起坐
#define UTE_MODULE_SPORT_TREADMILL_ON_OFF               1 //跑步机
#define UTE_MODULE_SPORT_GYMNASTICS_ON_OFF              1 //体操
#define UTE_MODULE_SPORT_BOATING_ON_OFF                 1 //划船
#define UTE_MODULE_SPORT_JUMPING_JACK_ON_OFF            1 //开合跳
#define UTE_MODULE_SPORT_FREE_TRAINING_ON_OFF           1 //自由训练
#define UTE_MODULE_SPORT_INDOOR_WALK_ON_OFF             0 //0x1A,室内走路 
#define UTE_MODULE_SPORT_INDOOR_RUN_ON_OFF              0 //0x1B,室内跑步
#define UTE_MODULE_SPORT_STRENGTH_TRAINING_ON_OFF       1 //0x1C,力量训练
#define UTE_MODULE_SPORT_STEP_TRAINING_ON_OFF           1 //0x1D,踏步
#define UTE_MODULE_SPORT_HORSE_RIDING_ON_OFF            1 //0x1E,骑马
#define UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_ON_OFF      1 //0x1F,椭圆机
#define UTE_MODULE_SPORT_TAI_CHI_ON_OFF                 1 //0x20,太极
#define UTE_MODULE_SPORT_SHUTTLECOCK_ON_OFF             1 //0x21,毽球
#define UTE_MODULE_SPORT_BOXING_ON_OFF                  0 //0x22,拳击
#define UTE_MODULE_SPORT_OUTDOOR_WALK_ON_OFF            1 //0x23,户外健走
#define UTE_MODULE_SPORT_TRAIL_RUNNING_ON_OFF           1 //0x24,越野跑
#define UTE_MODULE_SPORT_SKIING_ON_OFF                  1 //0x25,滑雪
#define UTE_MODULE_SPORT_ICE_HOCKEY_ON_OFF              1 //0x26,冰球
#define UTE_MODULE_SPORT_TAEKWONDO_ON_OFF               1 //0x27,跆拳道
#define UTE_MODULE_SPORT_VO2MAX_TEST_ON_OFF             0 //0x28,最大摄氧量测试
#define UTE_MODULE_SPORT_ROWING_MACHING_ON_OFF          1 //0x29,划船机
#define UTE_MODULE_SPORT_AIR_WALKER_ON_OFF              1 //0x2A,漫步机
#define UTE_MODULE_SPORT_HIKING_ON_OFF                  0 //0x2B,徒步
#define UTE_MODULE_SPORT_ATHLETICS_ON_OFF               1 //0x2C,田径
#define UTE_MODULE_SPORT_WAIST_TRAINING_ON_OFF          1 //0x2D,腰腹训练
#define UTE_MODULE_SPORT_KARATE_ON_OFF                  1 //0x2E,空手道
#define UTE_MODULE_SPORT_COOLDOWN_ON_OFF                0 //0x2F,整理放松
#define UTE_MODULE_SPORT_CROSS_TRAINING_ON_OFF          0 //0x30,交叉训练
#define UTE_MODULE_SPORT_PILATES_ON_OFF                 1 //0x31,普拉提
#define UTE_MODULE_SPORT_CROSS_FIT_ON_OFF               0 //0x32,交叉配合
#define UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_ON_OFF     0 //0x33,功能性训练
#define UTE_MODULE_SPORT_PHYSICAL_TRAINING_ON_OFF       0 //0x34,体能训练
#define UTE_MODULE_SPORT_ARCHERY_ON_OFF                 0 //0x35,射箭
#define UTE_MODULE_SPORT_FLEXIBILITY_ON_OFF             0 //0x36,柔韧度
#define UTE_MODULE_SPORT_MIXED_CARDIO_ON_OFF            0 //0x37,混合有氧
#define UTE_MODULE_SPORT_LATIN_DANCE_ON_OFF             1 //0x38,拉丁舞
#define UTE_MODULE_SPORT_STREET_DANCE_ON_OFF            1 //0x39,街舞
#define UTE_MODULE_SPORT_KICKBOXING_ON_OFF              0 //0x3A,自由搏击
#define UTE_MODULE_SPORT_BARRE_ON_OFF                   1 //0x3B,芭蕾
#define UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_ON_OFF     0 //0x3C,澳式足球
#define UTE_MODULE_SPORT_MARTIAL_ARTS_ON_OFF            1 //0x3D,武术
#define UTE_MODULE_SPORT_STAIRS_ON_OFF                  1 //0x3E,爬楼
#define UTE_MODULE_SPORT_HANDBALL_ON_OFF                1 //0x3F,手球
#define UTE_MODULE_SPORT_BOWLING_ON_OFF                 1 //0x40,保龄球
#define UTE_MODULE_SPORT_RACQUETBALL_ON_OFF             1 //0x41,壁球
#define UTE_MODULE_SPORT_CURLING_ON_OFF                 1 //0x42,冰壶
#define UTE_MODULE_SPORT_HUNTING_ON_OFF                 0 //0x43,打猎
#define UTE_MODULE_SPORT_SNOWBOARDING_ON_OFF            0 //0x44,单板滑雪
#define UTE_MODULE_SPORT_PLAY_ON_OFF                    0 //0x45,休闲运动
#define UTE_MODULE_SPORT_AMERICAN_FOOTBALL_ON_OFF       0 //0x46,美式橄榄球
#define UTE_MODULE_SPORT_HAND_CYCLING_ON_OFF            0 //0x47,手摇车
#define UTE_MODULE_SPORT_FISHING_ON_OFF                 1 //0x48,钓鱼
#define UTE_MODULE_SPORT_DISC_SPORTS_ON_OFF             1 //0x49,飞盘
#define UTE_MODULE_SPORT_RUGGER_ON_OFF                  0 //0x4A,橄榄球
#define UTE_MODULE_SPORT_GOLF_ON_OFF                    1 //0x4B,高尔夫
#define UTE_MODULE_SPORT_FOLK_DANCE_ON_OFF              1 //0x4C,民族舞
#define UTE_MODULE_SPORT_DOWNHILL_SKIING_ON_OFF         0 //0x4D,高山滑雪
#define UTE_MODULE_SPORT_SNOW_SPORTS_ON_OFF             0 //0x4E,雪上运动
#define UTE_MODULE_SPORT_MIND_BODY_ON_OFF               0 //0x4F,舒缓冥想类运动
#define UTE_MODULE_SPORT_CORE_TRAINING_ON_OFF           0 //0x50,核心训练
#define UTE_MODULE_SPORT_SKATING_ON_OFF                 1 //0x51,滑冰
#define UTE_MODULE_SPORT_FITNESS_GAMING_ON_OFF          0 //0x52,健身游戏
#define UTE_MODULE_SPORT_AEROBICS_ON_OFF                1 //0x53,健身操
#define UTE_MODULE_SPORT_GROUP_TRAINING_ON_OFF          0 //0x54,团体操
#define UTE_MODULE_SPORT_KENDO_ON_OFF                   0 //0x55,搏击操
#define UTE_MODULE_SPORT_LACROSSE_ON_OFF                1 //0x56,长曲棍球
#define UTE_MODULE_SPORT_ROLLING_ON_OFF                 0 //0x57,泡沫轴筋膜放松
#define UTE_MODULE_SPORT_WRESTLING_ON_OFF               0 //0x58,摔跤
#define UTE_MODULE_SPORT_FENCING_ON_OFF                 0 //0x59,击剑
#define UTE_MODULE_SPORT_SOFTBALL_ON_OFF                1 //0x5A,垒球
#define UTE_MODULE_SPORT_SINGLE_BAR_ON_OFF              0 //0x5B,单杠
#define UTE_MODULE_SPORT_PARALLEL_BARS_ON_OFF           0 //0x5C,双杠
#define UTE_MODULE_SPORT_ROLLER_SKATING_ON_OFF          0 //0x5D,轮滑
#define UTE_MODULE_SPORT_HULA_HOOP_ON_OFF               1 //0x5E,呼啦圈
#define UTE_MODULE_SPORT_DARTS_ON_OFF                   1 //0x5F,飞镖
#define UTE_MODULE_SPORT_PICKLEBALL_ON_OFF              1 //0x60,匹克球
#define UTE_MODULE_SPORT_HIIT_ON_OFF                    0 //0x61,HIIT
#define UTE_MODULE_SPORT_SHOOTING_ON_OFF                0 //0x62,射击
#define UTE_MODULE_SPORT_JUDO_ON_OFF                    0 //0x63,柔道
#define UTE_MODULE_SPORT_TRAMPOLINE_ON_OFF              1 //0x64,蹦床
#define UTE_MODULE_SPORT_SKATEBOARDING_ON_OFF           1 //0x65,滑板
#define UTE_MODULE_SPORT_HOVERBOARD_ON_OFF              0 //0x66,平衡车
#define UTE_MODULE_SPORT_BLADING_ON_OFF                 0 //0x67,溜旱冰
#define UTE_MODULE_SPORT_PARKOUR_ON_OFF                 1 //0x68,跑酷
#define UTE_MODULE_SPORT_DIVING_ON_OFF                  0 //0x69,跳水
#define UTE_MODULE_SPORT_SURFING_ON_OFF                 0 //0x6A,冲浪
#define UTE_MODULE_SPORT_SNORKELING_ON_OFF              0 //0x6B,浮潜
#define UTE_MODULE_SPORT_PULL_UP_ON_OFF                 1 //0x6C,引体向上
#define UTE_MODULE_SPORT_PUSH_UP_ON_OFF                 1 //0x6D,俯卧撑
#define UTE_MODULE_SPORT_PLANKING_ON_OFF                1 //0x6E,平板支撑
#define UTE_MODULE_SPORT_ROCK_CLIMBING_ON_OFF           1 //0x6F,攀岩
#define UTE_MODULE_SPORT_HIGHTJUMP_ON_OFF               1 //0x70,跳高
#define UTE_MODULE_SPORT_BUNGEE_JUMPING_ON_OFF          0 //0x71,蹦极
#define UTE_MODULE_SPORT_LONGJUMP_ON_OFF                1 //0x72,跳远
#define UTE_MODULE_SPORT_MARATHON_ON_OFF                0 //0x73,马拉松

/*! 默认显示的运动的顺序,不要重复,不显示的置0, xjc 2222-03-29*/
#define UTE_MODULE_SPORT_RUNNING_ORDER                  1 //跑步
#define UTE_MODULE_SPORT_BIKE_ORDER                     2 //骑行
#define UTE_MODULE_SPORT_JUMP_ROPE_ORDER                3 //跳绳
#define UTE_MODULE_SPORT_SWIMMING_ORDER                 0 //游泳
#define UTE_MODULE_SPORT_BADMINTION_ORDER               4 //羽毛球
#define UTE_MODULE_SPORT_TABLE_TENNIS_ORDER             5 //乒乓球
#define UTE_MODULE_SPORT_TENNIS_ORDER                   6 //网球
#define UTE_MODULE_SPORT_CLIMBING_ORDER                 7 //登山
#define UTE_MODULE_SPORT_WALKING_ORDER                  8 //徒步
#define UTE_MODULE_SPORT_BASKETBALL_ORDER               9 //篮球
#define UTE_MODULE_SPORT_FOOTBALL_ORDER                 10 //足球
#define UTE_MODULE_SPORT_BASEBALL_ORDER                 11 //棒球（或垒球）
#define UTE_MODULE_SPORT_VOLLEYBALL_ORDER               12 //排球
#define UTE_MODULE_SPORT_CRICKET_ORDER                  13 //板球
#define UTE_MODULE_SPORT_RUGBY_ORDER                    14 //橄榄球（或美式足球）
#define UTE_MODULE_SPORT_HOCKEY_ORDER                   15 //曲棍球
#define UTE_MODULE_SPORT_DANCE_ORDER                    16 //跳舞
#define UTE_MODULE_SPORT_SPINNING_ORDER                 18 //动感单车
#define UTE_MODULE_SPORT_YOGA_ORDER                     17 //瑜伽
#define UTE_MODULE_SPORT_SIT_UP_ORDER                   19 //仰卧起坐
#define UTE_MODULE_SPORT_TREADMILL_ORDER                20 //跑步机
#define UTE_MODULE_SPORT_GYMNASTICS_ORDER               21 //体操
#define UTE_MODULE_SPORT_BOATING_ORDER                  22 //划船
#define UTE_MODULE_SPORT_JUMPING_JACK_ORDER             23 //开合跳
#define UTE_MODULE_SPORT_FREE_TRAINING_ORDER            24 //自由训练
#define UTE_MODULE_SPORT_INDOOR_WALK_ORDER              0  //0x1A,室内走路 
#define UTE_MODULE_SPORT_INDOOR_RUN_ORDER               0  //0x1B,室内跑步
#define UTE_MODULE_SPORT_STRENGTH_TRAINING_ORDER        0 //0x1C,力量训练
#define UTE_MODULE_SPORT_STEP_TRAINING_ORDER            0 //0x1D,踏步
#define UTE_MODULE_SPORT_HORSE_RIDING_ORDER             0 //0x1E,骑马
#define UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_ORDER       0 //0x1F,椭圆机
#define UTE_MODULE_SPORT_TAI_CHI_ORDER                  0 //0x20,太极
#define UTE_MODULE_SPORT_SHUTTLECOCK_ORDER              0 //0x21,毽球
#define UTE_MODULE_SPORT_BOXING_ORDER                   0 //0x22,拳击
#define UTE_MODULE_SPORT_OUTDOOR_WALK_ORDER             0 //0x23,户外健走
#define UTE_MODULE_SPORT_TRAIL_RUNNING_ORDER            0 //0x24,越野跑
#define UTE_MODULE_SPORT_SKIING_ORDER                   0 //0x25,滑雪
#define UTE_MODULE_SPORT_ICE_HOCKEY_ORDER               0 //0x26,冰球
#define UTE_MODULE_SPORT_TAEKWONDO_ORDER                0 //0x27,跆拳道
#define UTE_MODULE_SPORT_VO2MAX_TEST_ORDER              0 //0x28,最大摄氧量测试
#define UTE_MODULE_SPORT_ROWING_MACHING_ORDER           0 //0x29,划船机
#define UTE_MODULE_SPORT_AIR_WALKER_ORDER               0 //0x2A,漫步机
#define UTE_MODULE_SPORT_HIKING_ORDER                   0 //0x2B,徒步
#define UTE_MODULE_SPORT_ATHLETICS_ORDER                0 //0x2C,田径
#define UTE_MODULE_SPORT_WAIST_TRAINING_ORDER           0 //0x2D,腰腹训练
#define UTE_MODULE_SPORT_KARATE_ORDER                   0 //0x2E,空手道
#define UTE_MODULE_SPORT_COOLDOWN_ORDER                 0 //0x2F,整理放松
#define UTE_MODULE_SPORT_CROSS_TRAINING_ORDER           0 //0x30,交叉训练
#define UTE_MODULE_SPORT_PILATES_ORDER                  0 //0x31,普拉提
#define UTE_MODULE_SPORT_CROSS_FIT_ORDER                0 //0x32,交叉配合
#define UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_ORDER      0 //0x33,功能性训练
#define UTE_MODULE_SPORT_PHYSICAL_TRAINING_ORDER        0 //0x34,体能训练
#define UTE_MODULE_SPORT_ARCHERY_ORDER                  0 //0x35,射箭
#define UTE_MODULE_SPORT_FLEXIBILITY_ORDER              0 //0x36,柔韧度
#define UTE_MODULE_SPORT_MIXED_CARDIO_ORDER             0 //0x37,混合有氧
#define UTE_MODULE_SPORT_LATIN_DANCE_ORDER              0 //0x38,拉丁舞
#define UTE_MODULE_SPORT_STREET_DANCE_ORDER             0 //0x39,街舞
#define UTE_MODULE_SPORT_KICKBOXING_ORDER               0 //0x3A,自由搏击
#define UTE_MODULE_SPORT_BARRE_ORDER                    0 //0x3B,芭蕾
#define UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_ORDER      0 //0x3C,澳式足球
#define UTE_MODULE_SPORT_MARTIAL_ARTS_ORDER             0 //0x3D,武术
#define UTE_MODULE_SPORT_STAIRS_ORDER                   0 //0x3E,爬楼
#define UTE_MODULE_SPORT_HANDBALL_ORDER                 0 //0x3F,手球
#define UTE_MODULE_SPORT_BOWLING_ORDER                  0 //0x40,保龄球
#define UTE_MODULE_SPORT_RACQUETBALL_ORDER              0 //0x41,壁球
#define UTE_MODULE_SPORT_CURLING_ORDER                  0 //0x42,冰壶
#define UTE_MODULE_SPORT_HUNTING_ORDER                  0//0x43,打猎
#define UTE_MODULE_SPORT_SNOWBOARDING_ORDER             0//0x44,单板滑雪
#define UTE_MODULE_SPORT_PLAY_ORDER                     0//0x45,休闲运动
#define UTE_MODULE_SPORT_AMERICAN_FOOTBALL_ORDER        0//0x46,美式橄榄球
#define UTE_MODULE_SPORT_HAND_CYCLING_ORDER             0//0x47,手摇车
#define UTE_MODULE_SPORT_FISHING_ORDER                  0 //0x48,钓鱼
#define UTE_MODULE_SPORT_DISC_SPORTS_ORDER              0 //0x49,飞盘
#define UTE_MODULE_SPORT_RUGGER_ORDER                   0//0x4A,橄榄球
#define UTE_MODULE_SPORT_GOLF_ORDER                     0 //0x4B,高尔夫
#define UTE_MODULE_SPORT_FOLK_DANCE_ORDER               0 //0x4C,民族舞
#define UTE_MODULE_SPORT_DOWNHILL_SKIING_ORDER          0//0x4D,高山滑雪
#define UTE_MODULE_SPORT_SNOW_SPORTS_ORDER              0//0x4E,雪上运动
#define UTE_MODULE_SPORT_MIND_BODY_ORDER                0//0x4F,舒缓冥想类运动
#define UTE_MODULE_SPORT_CORE_TRAINING_ORDER            0//0x50,核心训练
#define UTE_MODULE_SPORT_SKATING_ORDER                  0 //0x51,滑冰
#define UTE_MODULE_SPORT_FITNESS_GAMING_ORDER           0//0x52,健身游戏
#define UTE_MODULE_SPORT_AEROBICS_ORDER                 0 //0x53,健身操
#define UTE_MODULE_SPORT_GROUP_TRAINING_ORDER           0//0x54,团体操
#define UTE_MODULE_SPORT_KENDO_ORDER                    0//0x55,搏击操
#define UTE_MODULE_SPORT_LACROSSE_ORDER                 0 //0x56,长曲棍球
#define UTE_MODULE_SPORT_ROLLING_ORDER                  0//0x57,泡沫轴筋膜放松
#define UTE_MODULE_SPORT_WRESTLING_ORDER                0//0x58,摔跤
#define UTE_MODULE_SPORT_FENCING_ORDER                  0//0x59,击剑
#define UTE_MODULE_SPORT_SOFTBALL_ORDER                 0 //0x5A,垒球
#define UTE_MODULE_SPORT_SINGLE_BAR_ORDER               0//0x5B,单杠
#define UTE_MODULE_SPORT_PARALLEL_BARS_ORDER            0//0x5C,双杠
#define UTE_MODULE_SPORT_ROLLER_SKATING_ORDER           0//0x5D,轮滑
#define UTE_MODULE_SPORT_HULA_HOOP_ORDER                0 //0x5E,呼啦圈
#define UTE_MODULE_SPORT_DARTS_ORDER                    0 //0x5F,飞镖
#define UTE_MODULE_SPORT_PICKLEBALL_ORDER               0 //0x60,匹克球
#define UTE_MODULE_SPORT_HIIT_ORDER                     0//0x61,HIIT
#define UTE_MODULE_SPORT_SHOOTING_ORDER                 0//0x62,射击
#define UTE_MODULE_SPORT_JUDO_ORDER                     0//0x63,柔道
#define UTE_MODULE_SPORT_TRAMPOLINE_ORDER               0 //0x64,蹦床
#define UTE_MODULE_SPORT_SKATEBOARDING_ORDER            0 //0x65,滑板
#define UTE_MODULE_SPORT_HOVERBOARD_ORDER               0//0x66,平衡车
#define UTE_MODULE_SPORT_BLADING_ORDER                  0//0x67,溜旱冰
#define UTE_MODULE_SPORT_PARKOUR_ORDER                  0 //0x68,跑酷
#define UTE_MODULE_SPORT_DIVING_ORDER                   0//0x69,跳水
#define UTE_MODULE_SPORT_SURFING_ORDER                  0//0x6A,冲浪
#define UTE_MODULE_SPORT_SNORKELING_ORDER               0//0x6B,浮潜
#define UTE_MODULE_SPORT_PULL_UP_ORDER                  0 //0x6C,引体向上
#define UTE_MODULE_SPORT_PUSH_UP_ORDER                  0 //0x6D,俯卧撑
#define UTE_MODULE_SPORT_PLANKING_ORDER                 0 //0x6E,平板支撑
#define UTE_MODULE_SPORT_ROCK_CLIMBING_ORDER            0 //0x6F,攀岩
#define UTE_MODULE_SPORT_HIGHTJUMP_ORDER                0 //0x70,跳高
#define UTE_MODULE_SPORT_BUNGEE_JUMPING_ORDER           0//0x71,蹦极
#define UTE_MODULE_SPORT_LONGJUMP_ORDER                 0 //0x72,跳远
#define UTE_MODULE_SPORT_MARATHON_ORDER                 0//0x73,马拉松
#endif

#endif//_UTE_PROJECT_CONFIG_AB281_H_
