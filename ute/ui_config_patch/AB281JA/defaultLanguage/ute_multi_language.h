/**
*@file
*@brief utf8 words list
*@details
*@author
*@date   2025-06-13 15:59:19
*@version  UTE WordListTool,Version 2.2.1.0
*/

#ifndef _UTE_MULTI_LANGUAGE_H_
#define _UTE_MULTI_LANGUAGE_H_

#include <include.h>

enum
{
    STR_NULL, //
    STR_MESSAGE, // 通知
    STR_PHONE, // 通话
    STR_SPORTS, // 运动
    STR_SPORT_RUN, // 户外跑
    STR_SPORT_RIDE_BIKE, // 骑行
    STR_SPORT_JUMP_ROPE, // 跳绳
    STR_SPORT_SWIMMING, // 游泳
    STR_SPORT_BADMINTON, // 羽毛球
    STR_SPORT_TABLE_TENNIS, // 乒乓球
    STR_SPORT_TENNIS, // 网球
    STR_SPORT_CLIMBING, // 登山
    STR_SPORT_WALKING, // 健走
    STR_SPORT_BASKETBALL, // 篮球
    STR_SPORT_FOOTBALL, // 足球
    STR_SPORT_BASEBALL, // 棒球
    STR_SPORT_VOLLEYBALL, // 排球
    STR_SPORT_CRICKET, // 板球
    STR_SPORT_RUGBY, // 橄榄球
    STR_SPORT_HOCKEY, // 曲棍球
    STR_SPORT_DANCE, // 跳舞
    STR_SPORT_SPINNING, // 动感单车
    STR_SPORT_YOGA, // 瑜伽
    STR_SPORT_SIT_UP, // 仰卧起坐
    STR_SPORT_TREADMILL, // 室内跑
    STR_SPORT_GYMNASTICS, // 体操
    STR_SPORT_BOATING, // 划船
    STR_SPORT_JUMPING_JACK, // 开合跳
    STR_SPORT_FREE_TRAINING, // 自由训练
    STR_SPORT_EXIT_MSG1, // 运动结束
    STR_SPORT_EXIT_MSG2, // 本次运动数据过少，不会记录数据，是否结束？
    STR_STEP, // 步
    STR_SLEEP, // 睡眠
    STR_MUSIC, // 音乐
    STR_HEART_RATE, // 心率
    STR_BLOOD_PRESSURE, // 血压
    STR_WEATHER, // 天气
    STR_BREATHE_TRAIN, // 呼吸训练
    STR_BREATHE_INHALE, // 吸气
    STR_BREATHE_EXHALE, // 呼气
    STR_EVREY_DAY_ACTIVITY, // 每日活动
    STR_CALCULATOR, // 计算器
    STR_ALARM_CLOCK, // 闹钟
    STR_ALARM_CLOCK_EDIT, // 编辑闹钟
    STR_ALARM_CLOCK_REPEAT, // 重复
    STR_ALARM_CLOCK_SET, // 设置时间
    STR_TIMER, // 计时器
    STR_STOP_WATCH, // 秒表
    STR_STOP_WATCH_RECORD, // 秒表记录
    STR_CAMERA, // 遥控拍照
    STR_VOICE_ASSISTANT, // 语音助手
    STR_FIND_PHONE, // 找手机
    STR_GAME, // 游戏
    STR_SETTING, // 设置
    STR_SETTING_LIGHT, // 亮度调节
    STR_SETTING_DOUSING, // 亮屏时长
    STR_LIGHT_ALWAYS, // 常亮
    STR_SETTING_UP, // 抬腕亮屏
    STR_SETTING_DISTURD, // 勿扰模式
    STR_DISTURD_ALL, // 全天开启
    STR_DISTURD_TIM, // 定时开启
    STR_DISTURD_TIM_CTT, // 开启后，来电和通知静音且不振动(闹钟除外)，抬腕不亮屏，是否开启？
    STR_DISTURD_TIM_START, // 开始时间
    STR_DISTURD_TIM_END, // 结束时间
    STR_VOL, // 声音
    STR_SHK, // 振动
    STR_MEDIA_VOL, // 媒体音量
    STR_CALL_VOL, // 通话音量
    STR_SETTING_LANGUAGE, // 语言选择
    STR_BEATHER_DURATION, // 呼吸时长
    STR_CUSTOM_TIME, // 时间
    STR_SETTING_CALENDAR, // 日历
    STR_SETTING_PASSWORD, // 密码
    STR_SETTING_ABOUT, // 关于
    STR_SETTING_RESTART, // 重启
    STR_SETTING_RSTFY, // 恢复出厂
    STR_SETTING_OFF, // 关机
    STR_CALL_RECENT, // 通话记录
    STR_CALL_LINK, // 联系人
    STR_CALL_DIAL, // 拨号盘
    STR_CANCEL, // 取消
    STR_FLASHLIGHT, // 手电筒
    STR_STYLE, // 主题
    STR_VOICE, // 语音助手
    STR_CALENDAR, // 日历
    STR_LATEST_APP, // 最近应用
    STR_CALORIE, // 热量
    STR_STEPS, // 步数
    STR_DISTANCE, // 距离
    STR_CELSIUS, // ℃
    STR_FAHRENHEIT, // ℉
    STR_YEAR, // 年
    STR_MON, // 月
    STR_MDAY, // 日
    STR_SUNDAY, // 周日
    STR_MONDAY, // 周一
    STR_TUESDAY, // 周二
    STR_WEDNESDAY, // 周三
    STR_THURSDAY, // 周四
    STR_FRIDAY, // 周五
    STR_SATURDAY, // 周六
    STR_CLOUDY, // 多云
    STR_SUNNY, // 晴
    STR_SNOWY, // 雪
    STR_RAINY, // 雨
    STR_OVERCAST, // 阴
    STR_SAND_AND_DUST, // 沙尘
    STR_WINDY, // 大风
    STR_HAZE, // 雾霾
    STR_KCAL, // 千卡
    STR_KM, // 公里
    STR_MILE, // 英里
    STR_HOUR, // 时
    STR_MIN, // 分
    STR_SEC, // 秒
    STR_WEAR_CHECK, // 请系紧佩戴，确保手表背面紧贴手腕，并保持静止。
    STR_UNKNOWN, // 未知
    STR_COMPO_SELECT, // 添加组件
    STR_STYLE_GRID_1, // 网格
    STR_STYLE_HONEYCOMB, // 蜂窝
    STR_STYLE_LIST_1, // 列表
    STR_STYLE_SKYRER, // 天圆地方
    STR_STYLE_WATERFALL, // 瀑布
    STR_DEV_NEME, // 设备名称
    STR_BLE_MAC, // Mac地址
    STR_SYS_VERSION, // 版本号
    STR_SLOW, // 缓慢
    STR_FASTER, // 稍快
    STR_OK, // 确定
    STR_SETTING_MODE, // 呼吸节奏
    STR_DRIZZLE, // 小雨
    STR_MODERATE_RAIN, // 阵雨
    STR_HEAVY_RAIN, // 大雨
    STR_SLEET, // 雨夹雪
    STR_RAINY_SHOWERS, // 雷阵雨
    STR_NO_MSG, // 无通知
    STR_SET_STEP_COUNT, // 步数
    STR_SET_DISTANCE, // 距离
    STR_GOAL_ACHIEVE, // 目标达成
    STR_SEDENTARY_REMIND, // 起来活动一下吧
    STR_DEEP_SLEEP, // 深睡
    STR_LIGHT_SLEEP, // 浅睡
    STR_ALWAKE_SLEEP, // 清醒
    STR_EYE_MOVE_SLEEP, // 快速眼动
    STR_CONNECT_BLUETOOTH, // 蓝牙未连接
    STR_PER_MINUTE, // 次/分
    STR_CUSTOM, // 自定义
    STR_ONCE, // 单次
    STR_EVERY_DAY, // 每天
    STR_NO_CLOCK, // 无闹钟
    STR_SURE_OFF, // 确认关机?
    STR_SURE_RESET, // 恢复出厂设置将清空数据，是否继续？
    STR_SURE_REBOOT, // 确认重启？
    STR_FLY_BIRD, // 飞鸟游戏
    STR_IN_CALL, // 正在呼出...
    STR_CALL_ME, // 来电
    STR_VOICE_SPEAKER, // 倾听中...
    STR_VOICE_BT_NOT_CONNECT, // 未连接
    STR_VOICE_SIRI_START, // 点击屏幕开始对话
    STR_ADDRESS_BOOK_SYNC, // 请前往APP设置SOS号码
    STR_AM, // 上午
    STR_PM, // 下午
    STR_NO_CALL_RECORD, // 没有通话记录
    STR_HIGHEST, // 最高
    STR_LOWSET, // 最低
    STR_SLIDE_REST, // 滑动重启
    STR_SLIDE_OFF, // 滑动关机
    STR_SLIDE_CALL, // 滑动呼叫
    STR_NO_DATA, // 无数据
    STR_BLOOD_OXYGEN_UINT, // 血氧饱和度
    STR_GONG_GE, // 宫格
    STR_SPHERE, // 球体
    STR_CHECKERBOARD, // 棋盘
    STR_HALO, // 光环
    STR_SIX_PALACE_GRID, // 六宫格
    STR_DEL, // 删除
    STR_CLEAR, // 清除
    STR_CLEAR_ALL, // 全部清除
    STR_DIAL_SWICTH, // 更换表盘
    STR_QRCODE, // 二维码
    STR_START, // 开始
    STR_STOP, // 停止
    STR_CLICK_OPEN, // 点击屏幕打开
    STR_CALENDAR_ONE, // 一
    STR_CALENDAR_TWO, // 二
    STR_CALENDAR_THREE, // 三
    STR_CALENDAR_HOUR, // 四
    STR_CALENDAR_FIVE, // 五
    STR_CALENDAR_SIX, // 六
    STR_CALENDAR_SEVEN, // 日
    STR_MORE, // 更多
    STR_COMMON_APP, // 常用应用
    STR_APP_DOWNLOAD, // 扫码下载APP或扫码配对
    STR_BLOOD_OXYGEN, // 血氧
    STR_ALTITUDE, // 海拔
    STR_SOOTHING, // 舒缓
    STR_PLEASE_WEAR, // 请佩戴手表测量
    STR_CLICK_START_PIC, // 点击开始拍照
    STR_SPORT_FINISH_APP, // 更多详情请前往APP查看
    STR_SPORT_ORDER, // 次
    STR_PAUSE, // 暂停
    STR_CLOSE, // 关闭
    STR_MIN_JOINT, // ##分钟
    STR_SECOND_JOINT, // ##秒
    STR_ADD_CLOCK, // 添加闹钟
    STR_TO_DAY, // 今天
    STR_END, // 结束
    STR_NO_GET_WEATHER, // 未获取到天气，请连接APP后重试。
    STR_CONTINUE, // 继续
    STR_UPGRADING, // 升级中...
    STR_UPDATED, // 升级成功
    STR_UPDATE_FAILED, // 升级失败
    STR_APP_AGIAN, // 请连接App重试
    STR_WATCH_RESTART, // 手表即将进入重启
    STR_UP_WATCHING, // 表盘正在传输中，请保持APP和设备连接
    STR_NO_CONTACTS, // 没有联系人
    STR_BREATHE_FINISH, // 训练完成
    STR_BREATHE_TIME, // 训练时长
    STR_LOW_BATTERY, // 低电量
    STR_TIMER_FINIFH, // 计时结束
    STR_INS, // 同步中...
    STR_SYNC_SUC, // 同步成功
    STR_SYNC_FAIL, // 同步失败
    STR_MUTE, // 静音
    STR_WOMEN_HEALTH, // 女性健康
    STR_PLEASE_APP_DATA, // 请前往APP同步数据
    STR_MENSTRUAL_CYCLE, // 月经期
    STR_PREGNANCY, // 易孕期
    STR_SAFE_PERIOD, // 安全期
    STR_HEART_HIGHT, // 心率过高
    STR_HEART_LOW, // 心率过低
    STR_PACE, // 配速
    STR_KILOMETRE, // 千米
    STR_STRESS, // 压力
    STR_EMOTION, // 情绪
    STR_TOOL_BOX, // 工具箱
    STR_SOS, // SOS
    STR_RESTING_HEART, // 静息心率
    STR_WEEK7_HEART, // 最近7天平均静息心率
    STR_HEART_ABOUT, // 静息心率是指在清醒、不活动的安静状态下，每分钟的心跳次数，是衡量心脏健康的重要指标之一。可选择在清晨清醒后安静状态下测量，理想的静息心率是50~80次/分钟。
    STR_OXYGEN_ABOUT_1, // 血氧饱和度(SpO2)是血液中氧合血红蛋(HbO2)的百分比，即血液中氧气的浓度，是维持人体正常机能的重要生理参数。
    STR_OXYGEN_ABOUT_2, // 测量须知:1.本设备支持测量范围为70%-100%2.请确保手表佩戴在距离腕骨一指处，不要太靠下，并系紧表带。3.测量过程中请尽可能保持静止，保持表盘朝上。4.佩戴方式错误、测量过程中的晃动、双臂垂在两侧、手臂毛发、纹身或处于寒冷环境等因素影响测量精准度可能会下降。5.测量结果仅供参考，不做为医疗诊断依据。
    STR_DO_DELAY_CLOCK, // 是否删除闹钟？
    STR_ADD_CLOCK_NO_MORE, // 当前添加闹钟个数已达上限
    STR_STEP_DETAILS, // 步数详情
    STR_WEEK_STEP, // 本周步数
    STR_SYSTEM_SET, // 系统设置
    STR_UV, // 紫外线
    STR_UV_VERY_WEAK, // 很弱
    STR_UV_WEAK, // 弱
    STR_UV_MODERATE, // 中等
    STR_UV_STRONG, // 强
    STR_UV_VERY_STRONG, // 很强
    STR_HUMIDITY, // 湿度
    STR_FUTURE_WEATHER, // 未来天气
    STR_CALL_CLOSED_IS_OPEN, // 呼叫开关已禁用，您想打开它吗？
    STR_WATCHFACE_AND_THEME, // 表盘&主题
    STR_DIAL_SOS, // 拨打SOS？
    STR_LOW_BATTERY_MODE, // 电量剩余##%，是否开启省电模式？
    STR_SILENT_MODE_PROMAT, // 在静音模式下，来电无铃声提醒（闹钟除外）。
    STR_BREATHE_EXPLAIN, // 请保持静止，将注意力集中在呼吸上。
    STR_MOOD_EXPLAIN1, // 压力在心理学上指精神上束缚和紧张的感受，适当压力有助于提高工作和学习的效率，压力过大则会影响到身心健康。压力指标是通过心率变异性(HRV)的数值变化进行计算，用来评定身体状态改变的指标。
    STR_POSITIVE, // 积极
    STR_CALM, // 平和
    STR_NEGATIVE, // 消极
    STR_MOOD_EXPLAIN2, // 测量通知 1. 设备支持的测量范围:正静和负。
    STR_PRESSURE_EXPLAIN1, // 压力在心理学上指精神上束缚和紧张的感受，适当压力有助于提高工作和学习的效率，压力过大则会影响到身心健康。压力指标是通过心率变异性(HRV)的数值变化进行计算，用来评定身体状态改变的指标。
    STR_PRESSURE_EXPLAIN2, // 1.本设备支持测量范围为1-1002.请确保手环佩戴在距离腕骨一指处，不要太靠下，并系紧表带。3.测量过程中请尽可能保持静止，保持表盘朝上。4.处于运动状态和佩戴部位频繁活动时，会影响测量的准确性。5.测量结果仅供参考，不作为医疗诊断依据。
    STR_RELAXED, // 轻松
    STR_NORMAL, // 正常
    STR_MEDIUM, // 中等
    STR_HIGH, // 偏高
    STR_VBRATION, // 震动强度
    STR_SOUND_AND_VIBRATION, // 声音和震动
    STR_HIGH_1, // 高
    STR_MIDDLE, // 中
    STR_LOW, // 低
    STR_TESTING, // 测量中...
    STR_STATE_TEST, // 开始测量
    STR_BLE_SUCCESSFUL, // 蓝牙连接成功
    STR_BLE_OFF, // 手机蓝牙已断开
    STR_LANGUAGE_CN, // 简体中文
    STR_LANGUAGE_ENG, // English
    STR_LANGUAGE_FN, // Français
    STR_LANGUAGE_RU, // Русский
    STR_LANGUAGE_JP, // 日語
    STR_ITALY, // Italiano
    STR_GERMAN, // Deutsch
    STR_ARABIC, // العربية
    STR_TRADITIONAL_CHINESE, // 繁体中文
    STR_THAI, // ภาษาไทย
    STR_SPANISH, // Español
    STR_TURKEY, // Türkçe
    STR_PORTUGUESE, // Português
    STR_INDONESUAN, // Indonesia
    STR_VIETNAMESE, // Tiếng Việt
    STR_MALAYSIA, // Melayu
};

extern const char * const *i18n;
extern const char * const i18n_zh_rcn[];
extern const char * const i18n_en_rus[];
extern const char * const i18n_ru_rru[];
extern const char * const i18n_tr_rtr[];
extern const char * const i18n_ms_rmy[];
extern const char * const i18n_de_rde[];
extern const char * const i18n_es_res[];
extern const char * const i18n_fr_rfr[];
extern const char * const i18n_in_rid[];
extern const char * const i18n_it_rit[];
extern const char * const i18n_pt_rpt[];
extern const char * const i18n_th_rth[];
extern const char * const i18n_vi_rvn[];

#endif // _UTE_MULTI_LANGUAGE_H_
