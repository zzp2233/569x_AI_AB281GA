#include "include.h"

#if UTE_MODULE_SCREENS_MINI_CARD_SUPPORT

#include "ute_module_weather.h"
#include "ute_module_systemtime.h"
#include "ute_module_sport.h"
#include "ute_module_timezonesetting.h"
#include "ute_module_sleep.h"
#include "ute_module_heart.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_emotionPressure.h"
#include "ute_module_menstrualcycle.h"
#include "ute_module_charencode.h"
#include "ute_module_music.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#define CARD_START_POS_Y        (96+CARD_HEIGHT/2)
#define CARD_WIDTH      348
#define CARD_HEIGHT     158
#define CARD_GAP        14

#define CARD_SCALE_START_POS            (CARD_START_POS_Y+CARD_HEIGHT + 40)//350                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             (CARD_SCALE_START_POS+70)//400                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 156 * 2)                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 156)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              0X25                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               0X10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）
#define PRO_X(a)    ((a) - CARD_GAP - CARD_WIDTH / 2)
#define PRO_Y(a)    ((a) - CARD_START_POS_Y - CARD_HEIGHT / 2)

#define CARD_EDIT_HEI                   90

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - (((func_cb.mini_sort_cnt == 1) ? 1 : (func_cb.mini_sort_cnt+1)) * CARD_HEIGHT + CARD_GAP + 5 * func_cb.mini_sort_cnt))                    //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + ((func_cb.mini_sort_cnt == 1) ? 0 : 30))                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - ((func_cb.mini_sort_cnt == 1) ? 0 : 78))                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 5                                                                      //拖动松手后的速度(惯性)

#define VOICE_POS_Y        8+76/2
#define VOICE_PIC_WID      76
#define VOICE_PIC_HEI      76

#define CLICK_ICON_AREA_SIZE        20
#endif
enum
{
    COMPO_ID_BTN_VOICE = 1,
    COMPO_ID_PIC_VOICE,
    COMPO_ID_BTN_EDIT,

    COMPO_ID_CARD_START,
    //不可再CARD START 后面添加任何COMPO_ID, 要添加就再前面添加！！！
};


#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

//计时状态
enum
{
    TIMER_STA_IDLE,
    TIMER_STA_WORKING,
    TIMER_STA_PAUSE,
    TIMER_STA_DONE,
    TIMER_STA_RESET,
};

typedef struct f_mini_compo_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s8 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 tick1;
    bool womenFlag;
} f_mini_compo_card_t;

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_alarm_hour_format_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (hour24 == 0)
        {
            hour12.hour = 12;
        }
        else if (hour24 > 12)
        {
            hour12.hour = hour24 - 12;
        }
        else
        {
            hour12.hour = hour24;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = hour24;
    hour12.am_pm = 0;
    return hour12;
}

static const int press_str[] =
{
    STR_RELAXED, // 轻松
    STR_NORMAL, // 正常
    STR_MEDIUM, // 中等
    STR_HIGH, // 偏高
};

void removeElement(u8 *arr, u8 *size, u8 func)
{
    bool haveBeen = false;
    if (*size == 1 && arr[0] == func)
    {
        arr[0] = FUNC_ACTIVITY;
        return;
    }
    for (u8 i = 0; i < *size; i++)
    {
        if (arr[i] == func)
        {
            haveBeen = true;
        }
        if (haveBeen)
        {
            arr[i] = arr[i + 1];
        }
    }

    if (haveBeen)
    {
        (*size)--;
    }
}

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
extern void func_get_world_time(ute_module_systemtime_time_t *buffer,ute_module_timezonesetting_data_t worldClockData,uint8_t index);
extern compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM];
static u16 music_len = 0;
static void func_mini_compo_card_create_o_update(compo_form_t* frm, bool is_create)
{
    ute_module_sports_record_data_t sportsHistoryRecordData;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    ute_module_timezonesetting_data_t worldClockData;
    uteModuleTimeZoneSettingGetData(&worldClockData);
//    printf("%s create\n", __func__);
    if (is_create == false)
    {
        if (func_cb.sta != FUNC_MINI_COMPO_CARD)
        {
            return;
        }
    }
    compo_cardbox_t* card = NULL;
    char tmp[50] = {0};
    memset(tmp, 0, sizeof(tmp));
    for (int i=0; i<func_cb.mini_sort_cnt; i++)
    {
        switch (func_cb.mini_tbl_sort[func_cb.mini_sort_cnt-1-i])
        {
            case FUNC_ACTIVITY:
            {
                //用户数据接口
                u16 kcal_value = uteModuleSportGetCurrDayKcalData();         //卡路里（热量）
                uint32_t step_value = 0;         //步数
                u16 timer_value = uteModuleSportGetCurrDayExerciseData() / 60;        //运动时间
                uteModuleSportGetCurrDayStepCnt(&step_value,NULL,NULL);
                ute_module_target_notify_data_t targetNotifyData;
                uteModuleSportGetTodayTargetNotifyData(&targetNotifyData);
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 7, 3, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_icon_set(card, 0, UI_BUF_I341001_5_ACTIVITY_KCAL_BIN);
                compo_cardbox_icon_set_location(card, 0, 17+34/2-CARD_WIDTH/2, 14+34/2-CARD_HEIGHT/2, 34, 34);

                compo_cardbox_icon_set(card, 1, UI_BUF_I341001_5_ACTIVITY_STEPS_BIN);
                compo_cardbox_icon_set_location(card, 1, 17+34/2-CARD_WIDTH/2, 60+34/2-CARD_HEIGHT/2, 34, 34);

                compo_cardbox_icon_set(card, 2, UI_BUF_I341001_5_ACTIVITY_DIS_BIN);
                compo_cardbox_icon_set_location(card, 2, 17+34/2-CARD_WIDTH/2, 106+34/2-CARD_HEIGHT/2, 34, 34);

                compo_cardbox_icon_set_location(card, 3, 191+140/2-CARD_WIDTH/2, 7+140/2-CARD_HEIGHT/2, 140, 140);
                compo_cardbox_icon_set(card, 3, UI_BUF_I341001_23_WIDGETS_ACTIVITY_BG_BIN);

                u8 pic_dis = (targetNotifyData.todayKcalTarget!=0 )? \
                             (kcal_value * 100 /targetNotifyData.todayKcalTarget /10) : 0;
                if(pic_dis>10)pic_dis=10;
                compo_cardbox_icon_set(card, 4, UI_BUF_I341001_23_WIDGETS_ACTIVITY_KCAL_GIF_BIN);
                compo_cardbox_icon_cut(card, 4, pic_dis, 11);
                compo_cardbox_icon_set_pos(card, 4, 191+140/2-CARD_WIDTH/2, 7+140/2-CARD_HEIGHT/2);

                pic_dis = uteModuleSportGetStepsTargetCnt()!=0 ? (step_value *100/(uteModuleSportGetStepsTargetCnt())/10 ) : 0;
                if(pic_dis>10)pic_dis=10;
                compo_cardbox_icon_set(card, 5, UI_BUF_I341001_23_WIDGETS_ACTIVITY_STEPS_GIF_BIN);
                compo_cardbox_icon_cut(card, 5, pic_dis, 11);
                compo_cardbox_icon_set_pos(card, 5, 191+140/2-CARD_WIDTH/2, 7+140/2-CARD_HEIGHT/2);

                pic_dis = (targetNotifyData.exerciseTimeTargetSetMin!=0 )? \
                          (timer_value * 100 /targetNotifyData.exerciseTimeTargetSetMin /10) : 0;
                if(pic_dis>10)pic_dis=10;
                compo_cardbox_icon_set(card, 6, UI_BUF_I341001_23_WIDGETS_ACTIVITY_TIMES_GIF_BIN);
                compo_cardbox_icon_cut(card, 6, pic_dis, 11);
                compo_cardbox_icon_set_pos(card, 6, 191+140/2-CARD_WIDTH/2, 7+140/2-CARD_HEIGHT/2);

//                if (kcal_value > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", kcal_value);
                }
//                else
//                {
//                    memset(tmp, 0, sizeof(tmp));
//                    snprintf(tmp, sizeof(tmp), "--");
//                }
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 61-CARD_WIDTH/2, 14-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, tmp);
                compo_cardbox_text_set_forecolor(card, 0, COLOR_RED);

//                if (step_value > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", step_value);
                }
//                else
//                {
//                    memset(tmp, 0, sizeof(tmp));
//                    snprintf(tmp, sizeof(tmp), "--");
//                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_location(card, 1, 61-CARD_WIDTH/2, 60-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);
                compo_cardbox_text_set_forecolor(card, 1, COLOR_GREEN);

//                if (timer_value > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", timer_value);
                }
//                else
//                {
//                    memset(tmp, 0, sizeof(tmp));
//                    snprintf(tmp, sizeof(tmp), "--");
//                }
                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, 61-CARD_WIDTH/2, 106-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 2, tmp);
                compo_cardbox_text_set_forecolor(card, 2, COLOR_BLUE);
            }
            break;

            case FUNC_BT:
            {
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 4, 1, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);

                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_icon_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT);
                compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_MUSIC_BG_BIN);

                compo_cardbox_text_set_align_center(card, 0, true);
                compo_cardbox_text_set_location(card, 0, 0, 40-CARD_HEIGHT/2, CARD_WIDTH - 20, 48);

                if (ble_is_connect() || bt_a2dp_profile_completely_connected())
                {
                    uint16_t title_size_leng = 0;
                    uint16_t artist_size_leng = 0;
                    char title_buf[UTE_MUSIC_TITLE_MAX_SIZE] = {0};
                    char artist_buf[UTE_MUSIC_ARTLIST_MAX_SIZE] = {0};
                    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf, &title_size_leng);
                    uteModuleMusicGetPlayerArtistSize((uint8_t *)artist_buf, &artist_size_leng);

                    widget_text_t* txt = card->text[0];
                    if (txt && (music_len != title_size_leng))
                    {
                        music_len = title_size_leng;
                        card->roll_cb[0].mode = TEXT_AUTOROLL_MODE_NULL;
                        widget_text_set_client(txt,0, 0);
                    }

                    if (title_size_leng == 0)
                    {
                        if (uteModuleMusicGetPlayerPaused())
                        {
                            compo_cardbox_text_set(card, 0, i18n[STR_NOT_PLAYED]);
                        }
                        else
                        {
                            compo_cardbox_text_set(card, 0, i18n[STR_PLAYING]);
                        }
                    }
                    else
                    {
                        const u16 tmp_size = UTE_MUSIC_TITLE_MAX_SIZE + UTE_MUSIC_ARTLIST_MAX_SIZE + 2;
                        char *tmp_title = ab_zalloc(tmp_size);
                        if (tmp_title && artist_size_leng)
                        {
                            snprintf(tmp_title, tmp_size, "%s %s", title_buf, artist_buf);
                            compo_cardbox_text_set(card, 0, tmp_title);
                        }
                        else
                        {
                            compo_cardbox_text_set(card, 0, title_buf);
                        }
                        ab_free(tmp_title);
                    }
                }
                else
                {
                    card->roll_cb[0].mode = TEXT_AUTOROLL_MODE_NULL;
                    compo_cardbox_text_set(card, 0, i18n[STR_VOICE_BT_NOT_CONNECT]);
                }

                compo_cardbox_icon_set_location(card, 1, 0, 74+72/2-CARD_HEIGHT/2, 72, 72);
                if(ble_is_connect() || bt_a2dp_profile_completely_connected())
                {
                    compo_cardbox_icon_set(card, 1, !uteModuleMusicGetPlayerPaused() ? UI_BUF_I341001_23_WIDGETS_MUSIC_PLAY_BIN : UI_BUF_I341001_23_WIDGETS_MUSIC_PAUSE_BIN);
                    widget_set_alpha(card->icon[0],0xff);
                    widget_set_alpha(card->icon[1],0xff);
                    widget_set_alpha(card->icon[2],0xff);
                    widget_set_alpha(card->icon[3],0xff);
                }
                else
                {
                    compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_MUSIC_PAUSE_BIN);
                    widget_set_alpha(card->icon[0],120);
                    widget_set_alpha(card->icon[1],120);
                    widget_set_alpha(card->icon[2],120);
                    widget_set_alpha(card->icon[3],120);
                }

                compo_cardbox_icon_set_location(card, 2, 52+40/2-CARD_WIDTH/2, 90+40/2-CARD_HEIGHT/2, 40, 40);
                compo_cardbox_icon_set(card, 2, UI_BUF_I341001_23_WIDGETS_MUSIC_PREVIOUS_SONG_BIN);

                compo_cardbox_icon_set_location(card, 3, 256+40/2-CARD_WIDTH/2, 90+40/2-CARD_HEIGHT/2, 40, 40);
                compo_cardbox_icon_set(card, 3, UI_BUF_I341001_23_WIDGETS_MUSIC_NEXT_SONG_BIN);
            }
            break;

            case FUNC_HEARTRATE:
            {
                //用户数据接口
                ute_module_heart_test_last_info_t lastInfo;
                uteModuleHeartGetLastTestInfo(&lastInfo);
                u16 heart_rate_value = lastInfo.lastValue;

                heart_rate_value = (heart_rate_value == 0xff) ? 0 : heart_rate_value;
                u16 heart_rate_max = uteModuleHeartGetMaxHeartValue();
                heart_rate_max = (heart_rate_max == 0xff) ? 0 : heart_rate_max;
                u16 heart_rate_min = uteModuleHeartGetMinHeartValue();
                heart_rate_min = (heart_rate_min == 0xff) ? 0 : heart_rate_min;
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 3, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);


                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set_forecolor(card, 0, COLOR_RED);
                compo_cardbox_text_set(card, 0, i18n[STR_HEART_RATE]);

                if (heart_rate_value > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", heart_rate_value);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 60-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);

                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, (24 + widget_text_get_area(card->text[1]).wid) - CARD_WIDTH/2, 70-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 2, i18n[STR_PER_MINUTE]);

                if (heart_rate_max > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", heart_rate_max);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 3, false);
                compo_cardbox_text_set_location(card, 3, 41 - CARD_WIDTH/2, 109-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 3, tmp);
                compo_cardbox_text_set_forecolor(card, 3,make_color(0x66,0x66,0x66));
                if (heart_rate_min > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", heart_rate_min);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 4, false);
                compo_cardbox_text_set_location(card, 4, 125-CARD_WIDTH/2, 109-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 4, tmp);
                compo_cardbox_text_set_forecolor(card, 4,make_color(0x66,0x66,0x66));

                compo_cardbox_icon_set_location(card, 0, 19+18/2-CARD_WIDTH/2, 116+18/2-CARD_HEIGHT/2, 18, 18);
                compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_MAX_BIN);

                compo_cardbox_icon_set_location(card, 1, 103+18/2-CARD_WIDTH/2, 116+18/2-CARD_HEIGHT/2, 18, 18);
                compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_MIN_BIN);

                compo_cardbox_icon_set_location(card, 2, 261+68/2-CARD_WIDTH/2, 71+68/2-CARD_HEIGHT/2, 68, 68);
                compo_cardbox_icon_set(card, 2, UI_BUF_I341001_23_WIDGETS_RETRY_BIN);

            }
            break;

            case FUNC_BLOOD_OXYGEN:
            {
                if(is_create)
                {
                    uint8_t oxygen_date[24]= {0};
                    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧
                }
                ute_module_bloodoxygen_test_last_info_t lastInfo;
                uteModuleBloodoxygenGetLastTestInfo(&lastInfo);
                int oxy_value = lastInfo.lastValue;
                oxy_value = (oxy_value == 0xff) ? 0 : oxy_value;
                int oxy_max = 0;
                int oxy_min = 0;
                uteModuleBloodoxygenGetMinMaxValue(&oxy_min,&oxy_max);
                oxy_min = (oxy_min == 0xff) ? 0 : oxy_min;
                oxy_max = (oxy_max == 0xff) ? 0 : oxy_max;
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 3, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set_forecolor(card, 0, COLOR_RED);
                compo_cardbox_text_set(card, 0, i18n[STR_BLOOD_OXYGEN]);

                if (oxy_value > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", oxy_value);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 60-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);

                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, (18 + widget_text_get_area(card->text[1]).wid) - CARD_WIDTH/2, 70-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 2, "%");

                if (oxy_max > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", oxy_max);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 3, false);
                compo_cardbox_text_set_location(card, 3, 41 - CARD_WIDTH/2, 109-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 3, tmp);
                compo_cardbox_text_set_forecolor(card, 3,make_color(0x66,0x66,0x66));

                if (oxy_min > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", oxy_min);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 4, false);
                compo_cardbox_text_set_location(card, 4, 125-CARD_WIDTH/2, 109-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 4, tmp);
                compo_cardbox_text_set_forecolor(card, 4,make_color(0x66,0x66,0x66));

                compo_cardbox_icon_set_location(card, 0, 19+18/2-CARD_WIDTH/2, 116+18/2-CARD_HEIGHT/2, 18, 18);
                compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_MAX_BIN);

                compo_cardbox_icon_set_location(card, 1, 103+18/2-CARD_WIDTH/2, 116+18/2-CARD_HEIGHT/2, 18, 18);
                compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_MIN_BIN);

                compo_cardbox_icon_set_location(card, 2, 261+68/2-CARD_WIDTH/2, 71+68/2-CARD_HEIGHT/2, 68, 68);
                compo_cardbox_icon_set(card, 2, UI_BUF_I341001_23_WIDGETS_RETRY_BIN);
            }
            break;

            case FUNC_PRESSURE:
            {
                if(is_create)
                {
                    uint8_t dateBuf[24];
                    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(dateBuf, 24);
                }
                ute_module_emotion_pressure_test_last_info_t lastInfo;
                uteModuleEmotionPressureGetLastTestInfo(&lastInfo);

                u16 pressure_value = lastInfo.lastPressureValue;
                pressure_value = (pressure_value == 0xff) ? 0 : pressure_value;
                u16 pressure_max = uteModuleEmotionPressureGetEmotionPressureMaxValue();
                pressure_max = (pressure_max == 0xff) ? 0 : pressure_max;
                u16 pressure_min = uteModuleEmotionPressureGetEmotionPressureMinValue();
                pressure_min = (pressure_min == 0xff) ? 0 : pressure_min;
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 3, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set_forecolor(card, 0, make_color(0,255,218));
                compo_cardbox_text_set(card, 0, i18n[STR_STRESS]);

                if (pressure_value > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", pressure_value);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 60-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);

                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, (28 + widget_text_get_area(card->text[1]).wid) - CARD_WIDTH/2, 68-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                u8 index = (pressure_value < 80) ? pressure_value / 30 : 3;
                compo_cardbox_text_set(card, 2, i18n[press_str[index]]);
                compo_cardbox_text_set_visible(card, 2,pressure_value ? true : false);
                if (pressure_max > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", pressure_max);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 3, false);
                compo_cardbox_text_set_location(card, 3, 41 - CARD_WIDTH/2, 109-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 3, tmp);
                compo_cardbox_text_set_forecolor(card, 3,make_color(0x66,0x66,0x66));
                if (pressure_min > 0)
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", pressure_min);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "--");
                }
                compo_cardbox_text_set_align_center(card, 4, false);
                compo_cardbox_text_set_location(card, 4, 125-CARD_WIDTH/2, 109-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 4, tmp);
                compo_cardbox_text_set_forecolor(card, 4,make_color(0x66,0x66,0x66));

                compo_cardbox_icon_set_location(card, 0, 19+18/2-CARD_WIDTH/2, 116+18/2-CARD_HEIGHT/2, 18, 18);
                compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_MAX_BIN);

                compo_cardbox_icon_set_location(card, 1, 103+18/2-CARD_WIDTH/2, 116+18/2-CARD_HEIGHT/2, 18, 18);
                compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_MIN_BIN);
                compo_cardbox_icon_set_location(card, 2, 261+68/2-CARD_WIDTH/2, 71+68/2-CARD_HEIGHT/2, 68, 68);
                compo_cardbox_icon_set(card, 2, UI_BUF_I341001_23_WIDGETS_RETRY_BIN);
            }
            break;

            case FUNC_ALARM_CLOCK:
            {
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 1+7, 4, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_forecolor(card, 0, make_color(0, 223, 235));
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_ALARM_CLOCK]);

                u8 index =  uteModuleSystemtimeAlarmGetNextIndex();
                if (uteModuleSystemtimeGetAlarmTotalCnt() > 0)
                {
                    u8 hour = uteModuleSystemtimeGetAlarmHour(index);
                    func_alarm_hour_format_t clock_time = func_alarm_convert_to_12hour(hour);
                    hour = clock_time.hour;
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%02d:%02d", hour, uteModuleSystemtimeGetAlarmMin(index));
                    compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_38_BIN);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 65-CARD_HEIGHT/2, CARD_WIDTH/2, uteModuleSystemtimeGetAlarmTotalCnt() ? 38 : widget_text_get_max_height());
                    compo_cardbox_text_set(card, 1, tmp);

                    memset(tmp,0,sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%s", clock_time.am_pm ? (clock_time.am_pm ==1 ? i18n[STR_AM] : i18n[STR_PM]) : " ");
                    compo_cardbox_text_set_align_center(card, 3, false);
                    compo_cardbox_text_set_location(card, 3, 28-CARD_WIDTH/2 + 100, 65-CARD_HEIGHT/2, CARD_WIDTH/2, uteModuleSystemtimeGetAlarmTotalCnt() ? 38 : widget_text_get_max_height());
                    compo_cardbox_text_set(card, 3, tmp);
                }
                else
                {
                    compo_cardbox_text_set_font(card, 1, 0);
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), i18n[STR_NO_CLOCK]);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 65-CARD_HEIGHT/2, CARD_WIDTH/2, uteModuleSystemtimeGetAlarmTotalCnt() ? 38 : widget_text_get_max_height());
                    compo_cardbox_text_set(card, 1, tmp);
                    compo_cardbox_text_set(card, 3, NULL);
                }


                const uint32_t clock_off_day[14]=
                {
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_01_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_02_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_03_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_04_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_05_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_06_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_00_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_08_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_09_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_10_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_11_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_12_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_13_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_07_BIN,
                };
                const uint32_t clock_on_day[14]=
                {
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_01_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_02_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_03_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_04_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_05_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_06_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_00_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_08_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_09_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_10_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_11_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_12_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_13_BIN,
                    UI_BUF_I341001_20_ALARM_CLOCK_STATE_07_BIN,
                };
                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_forecolor(card, 2,make_color(0x66,0x66,0x66));
                compo_cardbox_text_set_location(card, 2, 18-CARD_WIDTH/2, 112-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                if (uteModuleSystemtimeGetAlarmTotalCnt() > 0)
                {
                    if (uteModuleSystemtimeGetAlarmCycle(index) & BIT(7))
                    {
                        memset(tmp, 0, sizeof(tmp));
                        snprintf(tmp, sizeof(tmp), "%s", i18n[STR_ONCE]);
                        compo_cardbox_text_set(card, 2, tmp);
                        compo_cardbox_text_set_visible(card, 2, true);
                    }
                    else if (uteModuleSystemtimeGetAlarmCycle(index) == 0x7f)
                    {
                        memset(tmp, 0, sizeof(tmp));
                        snprintf(tmp, sizeof(tmp), "%s", i18n[STR_EVERY_DAY]);
                        compo_cardbox_text_set(card, 2, tmp);
                        compo_cardbox_text_set_visible(card, 2, true);
                    }
                    else
                    {
                        compo_cardbox_text_set_visible(card, 2, false);
                        for (u8 j = 0; j < 7; j++)
                        {
                            u8 temp = (j + 1) % 7;
                            compo_cardbox_icon_set_location(card, j, 18+32/2-CARD_WIDTH/2+38*j, 112+32/2-CARD_HEIGHT/2, 32, 32);
                            if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID || uteModuleSystemtimeReadLanguage() == TRADITIONAL_CHINESE_ID)
                            {
                                compo_cardbox_icon_set(card, j, (uteModuleSystemtimeGetAlarmCycle(index) & BIT(temp)) ? clock_on_day[j] : clock_off_day[j]);
                            }
                            else
                            {
                                compo_cardbox_icon_set(card, j, (uteModuleSystemtimeGetAlarmCycle(index) & BIT(temp)) ? clock_on_day[j + 7] : clock_off_day[j + 7]);
                            }
                        }
                    }
                    compo_cardbox_icon_set_location(card, 7, 272+58/2-CARD_WIDTH/2, 62+34/2-CARD_HEIGHT/2, 58, 34);
                    compo_cardbox_icon_set(card, 7, uteModuleSystemtimeGetAlarmEnableState(index) ? UI_BUF_I341001_23_WIDGETS_SWITCH_OPEN_BUTTON_BIN : UI_BUF_I341001_23_WIDGETS_SWITCH_CLOSE_BUTTON_BIN);
                }
                else
                {
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%s >",i18n[STR_GO_ADD]);
                    compo_cardbox_text_set(card, 2, tmp);
                    compo_cardbox_text_set_visible(card, 2, true);
                    for(u8 j = 0; j < 8; j++)
                    {
                        compo_cardbox_icon_set(card,j,NULL);
                    }
                }


            }
            break;

            case FUNC_WEATHER:
            {

                ute_module_weather_data_t weater_data;
                ute_display_ctrl_t displayInfo;
                uteModuleWeatherGetData(&weater_data);
                uteModuleGuiCommonGetDisplayInfo(&displayInfo); //获取温度
                if(displayInfo.isFahrenheit)    //是否为华氏度
                {
                    weater_data.fristDayCurrTemperature= weater_data.fristDayCurrTemperature*9/5+32;
                    /*pcm 2022-09-19 */
                    if(weater_data.fristDayCurrTemperature<(-99))
                    {
                        weater_data.fristDayCurrTemperature=-99;
                    }
                    weater_data.dayTemperatureMax[0]= weater_data.dayTemperatureMax[0]*9/5+32;
                    weater_data.dayTemperatureMin[0]= weater_data.dayTemperatureMin[0]*9/5+32;
                    if(weater_data.dayTemperatureMax[0]<(-99))  weater_data.dayTemperatureMax[0]=-99;
                    if(weater_data.dayTemperatureMin[0]<(-99))  weater_data.dayTemperatureMin[0]=-99;
                }
                int tempe_value = weater_data.fristDayCurrTemperature;
                int tempe_max = weater_data.dayTemperatureMax[0];
                int tempe_min = weater_data.dayTemperatureMin[0];

                card = (is_create == true) ? compo_cardbox_create(frm, 1, 1, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_forecolor(card, 0, make_color(67, 203, 248));
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_WEATHER]);

                memset(tmp, 0, sizeof(tmp));
                if (weater_data.DayWeather[0] != WEATHER_TYPE_UNKNOWN)
                {
                    snprintf(tmp, sizeof(tmp), "%d", tempe_value);
                }
                else
                {
                    snprintf(tmp, sizeof(tmp), "--");
                }
                u8 index = weater_data.DayWeather[0] >> 8;
                if (index >= WEATHER_TYPE_NIGHT)
                {
                    index = WEATHER_TYPE_UNKNOWN;
                }
                if(uteModuleSystemtimeIsNight()) //是否为夜间
                {
                    switch(index)
                    {
                        case WEATHER_TYPE_SUNNY:
                            index = WEATHER_TYPE_NIGHT;
                            break;
                        case WEATHER_TYPE_OVERCAST:
                            index = WEATHER_TYPE_NIGHT + 1;
                            break;
                        case WEATHER_TYPE_SHOWER_RAIN:
                        case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                        case WEATHER_TYPE_RAIN_SNOW:         //雨夹雪
                        case WEATHER_TYPE_LIGHT_RAIN:        //小雨
                        case WEATHER_TYPE_HEAVY_RAIN:        //大雨
                            index = WEATHER_TYPE_NIGHT + 2;
                            break;
                    }
                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 60-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);

                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, (20 + widget_text_get_area(card->text[1]).wid) - CARD_WIDTH/2, 66-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                if(displayInfo.isFahrenheit)    //是否为华氏度
                {
                    compo_cardbox_text_set(card, 2, "°F");
                }
                else
                {
                    compo_cardbox_text_set(card, 2, "°C");
                }

                memset(tmp, 0, sizeof(tmp));
                if (weater_data.DayWeather[0] != WEATHER_TYPE_UNKNOWN)
                {
                    if(displayInfo.isFahrenheit)    //是否为华氏度
                    {
                        snprintf(tmp, sizeof(tmp), "%d/%d°F", tempe_max, tempe_min);
                    }
                    else
                    {
                        snprintf(tmp, sizeof(tmp), "%d/%d°C", tempe_max, tempe_min);
                    }
                }
                else
                {
                    if(displayInfo.isFahrenheit)    //是否为华氏度
                    {
                        snprintf(tmp, sizeof(tmp), "--/--°F");
                    }
                    else
                    {
                        snprintf(tmp, sizeof(tmp), "--/--°C");
                    }
                }
                compo_cardbox_text_set_align_center(card, 3, false);
                compo_cardbox_text_set_location(card, 3, 18 - CARD_WIDTH/2, 112-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 3, tmp);
                compo_cardbox_text_set_forecolor(card, 3,make_color(0x66,0x66,0x66));

                const u8 weather_list[] =
                {
                    STR_UNKNOWN,       // 未知
                    STR_SUNNY,         // 晴天
                    STR_CLOUDY,        // 阴天
                    STR_SUN_CLOUDY,    // 晴间多云
                    STR_MODERATE_RAIN, // 阵雨
                    STR_RAINY_SHOWERS, // 雷阵雨
                    STR_SLEET,         // 雨夹雪
                    STR_DRIZZLE,       // 小雨
                    STR_HEAVY_RAIN,    // 大雨
                    STR_SNOW_WEAK,     // 小雪
                    STR_SAND_AND_DUST, // 沙尘暴
                    STR_HAZE,          // 雾霾
                    STR_WINDY,         // 大风
                    STR_SNOW_WEAK,     // 小雪
                    STR_SNOW_HAV,      // 大雪
                    STR_SUNNY,         // 晴天
                    STR_SUN_CLOUDY,    // 晴间多云
                    STR_MODERATE_RAIN, // 阵雨夜
                };
                //compo_cardbox_text_set_align_center(card, 4, false);
                compo_cardbox_text_set_forecolor(card, 4, make_color(0x66,0x66,0x66));
                widget_text_set_right_align(card->text[4], true);
                compo_cardbox_text_set(card, 4, i18n[weather_list[index]]);
                area_t area_css = widget_text_get_area(card->text[4]);
                u8 move_x = (area_css.wid < 40) ?  15 : 0;
                compo_cardbox_text_set_location(card, 4, 250 - CARD_WIDTH/2 - move_x, 140-CARD_HEIGHT/2, CARD_WIDTH/2, widget_text_get_max_height());
                if (area_css.wid > CARD_WIDTH/2)
                {
                    widget_text_set_right_align(card->text[4], false);
                    compo_cardbox_text_set(card, 4, i18n[weather_list[index]]);
                }
                const u32 future_weather_icon[] =
                {
                    UI_BUF_I341001_12_WEATHER_WEATHER_00_UNKNOWEN_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_01_SUNNY_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_03_OVERCAST_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_02_CLOUDY_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_04_SHOWER_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_05_T_STORM_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_06_SLEET_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_07_LIGHT_RAIN_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_08_HEAVY_RAIN_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_09_SNOW_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_10_SAND_STORM_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_11_HAZE_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_12_WINDY_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_13_MINOR_SNOW_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_14_HEAVY_SNOW_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_15_NIGHT_SUNNY_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_16_NIGHT_CLOUDY_BIN,
                    UI_BUF_I341001_12_WEATHER_WEATHER_17_NIGHT_RAIN_BIN
                };
                compo_cardbox_icon_set(card, 0, future_weather_icon[index]);
                compo_cardbox_icon_set_location(card, 0, 288+40/2-CARD_WIDTH/2, 61+40/2-CARD_HEIGHT/2, 40, 40);
            }
            break;

            case FUNC_SLEEP:
            {
                static ute_module_sleep_display_data_t sleep_data;
                if(is_create == true)
                {
                    memset(&sleep_data, 0, sizeof(ute_module_sleep_display_data_t));
                    uteModuleSleepGetCurrDayDataDisplay(&sleep_data);
                }

                card = (is_create == true) ? compo_cardbox_create(frm, 1, 4, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_text_set_forecolor(card, 0, make_color(192,67,255));
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_SLEEP]);

                if(!sleep_data.totalSleepMin) ///是否有睡眠时长
                {
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 64-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                    compo_cardbox_text_set(card, 1,  i18n[STR_NODATA]);

                    compo_cardbox_icon_set_location(card, 0, 16+315/2-CARD_WIDTH/2, 118+20/2-CARD_HEIGHT/2, 316, 20);
                    compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_SLEEP_BG_BIN);
                }
                else
                {

                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", sleep_data.totalSleepMin / 60);
                    compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                    compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 64-CARD_HEIGHT/2, CARD_WIDTH/2,60);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set(card, 1,tmp);

                    compo_cardbox_text_set_align_center(card, 2, false);
                    compo_cardbox_text_set_location(card, 2, 18-CARD_WIDTH/2 + widget_text_get_area(card->text[1]).wid, 70-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                    compo_cardbox_text_set(card, 2,  i18n[STR_HOUR]);

                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%d", sleep_data.totalSleepMin % 60);
                    compo_cardbox_text_set_font(card, 3, UI_BUF_0FONT_FONT_NUM_48_BIN);
                    compo_cardbox_text_set_location(card, 3, 18-CARD_WIDTH/2 +  widget_text_get_area(card->text[1]).wid\
                                                    + widget_text_get_area(card->text[2]).wid, 64-CARD_HEIGHT/2, CARD_WIDTH/2, 60);
                    compo_cardbox_text_set_align_center(card, 3, false);
                    compo_cardbox_text_set(card, 3,tmp);

                    compo_cardbox_text_set_align_center(card, 4, false);
                    compo_cardbox_text_set_location(card, 4, 18-CARD_WIDTH/2 + widget_text_get_area(card->text[1]).wid + widget_text_get_area(card->text[2]).wid + \
                                                    widget_text_get_area(card->text[3]).wid, 70-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                    compo_cardbox_text_set(card, 4,  i18n[STR_MINUTE]);

                    u8 deep = sleep_data.deepSleepMin * 100 / sleep_data.totalSleepMin;
                    u8 light = sleep_data.lightSleepMin * 100 / sleep_data.totalSleepMin;
                    u8 rem = sleep_data.RemSleepMin * 100 / sleep_data.totalSleepMin;
                    u8 wake = sleep_data.wakeSleepMin * 100 / sleep_data.totalSleepMin;
                    u8 crc = deep + wake + rem + light;

                    deep = (sleep_data.deepSleepMin > 0 && 316 * deep / crc < 3) ? 3 : deep;
                    light = (sleep_data.lightSleepMin > 0 && 316 * light / crc < 3) ? 3 : light;
                    rem = (sleep_data.RemSleepMin > 0 && 316 * rem / crc < 3) ? 3 : rem;
                    wake = (sleep_data.wakeSleepMin > 0 && 316 * wake / crc < 3) ? 3 : wake;
                    crc = deep + wake + rem + light;

                    compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_SLEEP_00_BIN);
                    compo_cardbox_icon_set_location(card, 0, PRO_X(30 + 316 * deep / crc / 2), 118+20/2-CARD_HEIGHT/2, 316 * deep / crc, 20);

                    compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_SLEEP_01_BIN);
                    compo_cardbox_icon_set_location(card, 1, PRO_X(30 + 316 * deep / crc + 2 + 316 * light / crc / 2), 118+20/2-CARD_HEIGHT/2, 316 * light / crc, 20);

                    compo_cardbox_icon_set(card, 2, UI_BUF_I341001_23_WIDGETS_SLEEP_02_BIN);
                    compo_cardbox_icon_set_location(card, 2, PRO_X(30 + 316 * (deep + light) / crc + 4 + 316 * rem / crc / 2), 118+20/2-CARD_HEIGHT/2, 316 * rem / crc, 20);

                    compo_cardbox_icon_set(card, 3, UI_BUF_I341001_23_WIDGETS_SLEEP_03_BIN);
                    compo_cardbox_icon_set_location(card, 3, PRO_X(30 + 316 * (deep + light + rem) / crc + 6 + 316 * wake / crc / 2), 118+20/2-CARD_HEIGHT/2, 316 * wake / crc, 20);
                }
            }
            break;

            case FUNC_SPORT_HISTORY:
            {
                uteModuleSportsHistoryRecordGetData(&sportsHistoryRecordData);
                u8 sport_type = sportsHistoryRecordData.oneHistoryRecordData[0].sportsType;
                u8 sport_time_hour = sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.hour;

                card = (is_create == true) ? compo_cardbox_create(frm, 1, 0, 3, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_text_set_forecolor(card, 0, make_color(160,254,0));
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_EXERCISE_RECODE]);

                memset(tmp, 0, sizeof(tmp));
                if (sport_type)
                {
                    snprintf(tmp, sizeof(tmp), "%s", i18n[tbl_sport_list[sport_type].str_idx]);
                }
                else
                {
                    snprintf(tmp, sizeof(tmp), i18n[STR_RECORD]);
                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, (sport_type ? 70 : 80)-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);
                memset(tmp, 0, sizeof(tmp));
                if (sport_type)
                {
                    if(sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.day == time.day)
                    {
                        u8 sportsDateHour = sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.hour;
                        if (time.is12HourTime)
                        {
                            if (sportsDateHour > 12)
                            {
                                sportsDateHour = sportsDateHour - 12;
                            }
                            if (sportsDateHour == 0)
                            {
                                sportsDateHour = 12;
                            }
                        }
                        snprintf(tmp, sizeof(tmp), "%02d:%02d%s", sportsDateHour, \
                                 sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.min,time.is12HourTime ?(sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.hour < 12 ? i18n[STR_AM] : i18n[STR_PM]):' ');
                    }
                    else
                    {
                        snprintf(tmp, sizeof(tmp), "%02d/%02d", sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.month, sportsHistoryRecordData.oneHistoryRecordData[0].startSportTime.day);
                    }
                    compo_cardbox_text_set_visible(card, 2, true);
                }
                else
                {
                    compo_cardbox_text_set_visible(card, 2, false);
                }
                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, 18-CARD_WIDTH/2, 112-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 2, tmp);
                compo_cardbox_text_set_forecolor(card, 2,make_color(0x66,0x66,0x66));

            }
            break;

            case FUNC_WOMEN_HEALTH:
            {

                ute_menstrual_cycle_param_t periodParam;
                uteModuleMenstrualCycleGetParam(&periodParam);

                u8 women_state = 0;
                if (uteModuleMenstrualCycleIsOpen())
                {
                    women_state = uteModuleMenstrualCycleGetStatusByTime(time.year,time.month,time.day);
                }
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 2, 3, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
                //                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);

                compo_cardbox_text_set_forecolor(card, 0, make_color(246,34,199));
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_WOMEN_HEALTH]);

                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%s", i18n[STR_CURRENT_CYCLE]);
                compo_cardbox_text_set_align_center(card, 2, false);
                compo_cardbox_text_set_location(card, 2, 18-CARD_WIDTH/2, 112-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 2, tmp);
                compo_cardbox_text_set_forecolor(card, 2,make_color(0x66,0x66,0x66));

                memset(tmp, 0, sizeof(tmp));
                if (periodParam.keepDays == 0 || periodParam.lastTimestamp[3] == 0 || periodParam.cycleDays < 28 || !uteModuleMenstrualCycleIsOpen())
                {
                    snprintf(tmp, sizeof(tmp), i18n[STR_RECORD]);
                    compo_cardbox_text_set_visible(card, 2, false);
                    compo_cardbox_icon_set_visible(card, 0, false);
                    compo_cardbox_icon_set_visible(card, 1, true);
                    compo_cardbox_icon_set_pos(card, 1, PRO_X(272 + 68 / 2), 20);
                    compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_WOMAN_EDIT_BIN);
                }
                else
                {
                    compo_cardbox_icon_set_visible(card, 1, false);
                    compo_cardbox_icon_set_visible(card, 0, true);
                    compo_cardbox_text_set_visible(card, 2, true);
                    u32 state_index[3] =
                    {
                        UI_BUF_I341001_26_WOMEN_HEALTH_00_BIN,//经期
                        UI_BUF_I341001_26_WOMEN_HEALTH_01_BIN,//安全期
                        UI_BUF_I341001_26_WOMEN_HEALTH_02_BIN,//易孕期
                    };
                    switch(women_state)
                    {
                        case 1:
                            snprintf(tmp, sizeof(tmp), "%s", i18n[STR_MENSTRUAL_CYCLE]);
                            compo_cardbox_icon_set(card, 0,state_index[0]);
                            break;
                        case 2:
                        case 3:
                            snprintf(tmp, sizeof(tmp), "%s", i18n[STR_PREGNANCY]);
                            compo_cardbox_icon_set(card, 0,state_index[2]);
                            break;
                        case 4:
                        case 5:
                            snprintf(tmp, sizeof(tmp), "%s", i18n[STR_SAFE_PERIOD]);
                            compo_cardbox_icon_set(card, 0,state_index[1]);
                            break;
                    }
                    compo_cardbox_icon_set_location(card, 0, PRO_X(252 + 78 / 2), 20,78,78);

                }
                compo_cardbox_text_set_align_center(card, 1, false);
                compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, (uteModuleMenstrualCycleIsOpen() ? 68 : 80)-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 1, tmp);
            }
            break;

            case FUNC_STOPWATCH:
            {
                uint8_t stopmin = 0;
                uint8_t stopsec = 0;
                uint16_t stopmsec = 0;
                if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
                {
                    stopmin = 99;
                    stopsec = 59;
                    stopmsec = 999;
                    sys_cb.stopwatch_sta = 0;
                }
                else
                {
                    stopmin = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
                    stopsec = (sys_cb.stopwatch_total_msec / 1000) % 60;
                    stopmsec = sys_cb.stopwatch_total_msec % 1000;
                }

                card = (is_create == true) ? compo_cardbox_create(frm, 1, 2, 2, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_forecolor(card, 0, make_color(255, 177, 0));
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH/2, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_STOP_WATCH]);
                if(sys_cb.stopwatch_sta)
                {
                    compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_38_BIN);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 91-CARD_WIDTH/2, 92-CARD_HEIGHT/2, CARD_WIDTH, 48);

                    compo_cardbox_icon_set_location(card, 1, 18 + 68/2-CARD_WIDTH/2, 72+68/2-CARD_HEIGHT/2, 68, 68);
                    compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_STOPWATCH_CANCEL_BIN);
                }
                else
                {
                    compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 82-CARD_HEIGHT/2, CARD_WIDTH,60);
                    compo_cardbox_icon_set(card, 1, NULL);
                }
                memset(tmp, 0, sizeof(tmp));
                snprintf(tmp, sizeof(tmp), "%02d:%02d.%02d",stopmin,stopsec,stopmsec / 10);
                compo_cardbox_text_set(card, 1, tmp);
                u32 res_addr = (sys_cb.stopwatch_sta ? UI_BUF_I341001_23_WIDGETS_STOPWATCH_PLAY_BIN : UI_BUF_I341001_23_WIDGETS_STOPWATCH_PAUSE_BIN);//复位/计次
                compo_cardbox_icon_set_location(card, 0, 261+68/2-CARD_WIDTH/2, 72+68/2-CARD_HEIGHT/2, 68, 68);
                compo_cardbox_icon_set(card, 0, res_addr);
            }

            break;

            case FUNC_TIMER:
            {
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 4, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//                compo_cardbox_rect_set_color(card, 0, COLOR_GRAY);
                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_forecolor(card, 0, make_color(34, 152, 255));
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_TIMER]);
                if(sys_cb.timer_sta == TIMER_STA_IDLE)
                {
                    compo_cardbox_text_set_font(card, 1, 0);
                    compo_cardbox_text_set_align_center(card, 1, true);
                    for (int i=0; i<4; i++)
                    {
                        compo_cardbox_icon_set_location(card, i, 17+68/2+80*i-CARD_WIDTH/2, 71+68/2-CARD_HEIGHT/2, 68, 68);
                        u32 index = uteModuleSystemtimeCompareLanguage(CHINESE_LANGUAGE_ID) ? UI_BUF_I341001_23_WIDGETS_TIMER_ZH_BIN : UI_BUF_I341001_23_WIDGETS_TIMER_EN_BIN;
                        compo_cardbox_icon_set(card, i, index);
                    }
                    char temp[4] = {1,2,5,10};
                    for(int i=1; i<5; i++)
                    {
                        memset(tmp, 0, sizeof(tmp));
                        compo_cardbox_text_set_location(card, i, 12+68/2+80*(i-1)-CARD_WIDTH/2, 62+68/2-CARD_HEIGHT/2, 68, widget_text_get_height() * 2);
                        snprintf(tmp, sizeof(tmp), " %d", temp[i - 1]);
                        compo_cardbox_text_set(card, i, tmp);
                    }
                }
                else
                {
                    for (int i=0; i<4; i++)
                    {
                        compo_cardbox_icon_set_location(card, i, 17+68/2+80*i-CARD_WIDTH/2, 71+68/2-CARD_HEIGHT/2, 68, 68);
                        compo_cardbox_icon_set(card, i, NULL);
                    }
                    for(int i=1; i<5; i++)
                    {
                        compo_cardbox_text_set_location(card, i, 17+68/2+80*(i-1)-CARD_WIDTH/2, 71+68/2-CARD_HEIGHT/2, 68, widget_text_get_max_height());
                        compo_cardbox_text_set(card, i, NULL);
                    }
                    compo_cardbox_icon_set_location(card, 1, 18 + 68/2-CARD_WIDTH/2, 72+68/2-CARD_HEIGHT/2, 68, 68);
                    compo_cardbox_icon_set(card, 1, UI_BUF_I341001_23_WIDGETS_STOPWATCH_CANCEL_BIN);

                    u8 hour = SEC_TO_HOUR(sys_cb.timer_left_sec);
                    u8 min = SEC_TO_MIN(sys_cb.timer_left_sec);
                    u8 sec = SEC_TO_SEC(sys_cb.timer_left_sec);
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%02d:%02d:%02d",hour,min,sec);
                    compo_cardbox_text_set_font(card, 1, UI_BUF_0FONT_FONT_NUM_38_BIN);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 91-CARD_WIDTH/2, 92-CARD_HEIGHT/2, CARD_WIDTH, 48);
                    compo_cardbox_text_set(card, 1, tmp);
                    u32 res_addr = sys_cb.timer_sta == TIMER_STA_WORKING ? UI_BUF_I341001_23_WIDGETS_STOPWATCH_PLAY_BIN: UI_BUF_I341001_23_WIDGETS_STOPWATCH_PAUSE_BIN;

                    compo_cardbox_icon_set_location(card, 0, 261+68/2-CARD_WIDTH/2, 72+68/2-CARD_HEIGHT/2, 68, 68);
                    compo_cardbox_icon_set(card, 0, res_addr);
                }
            }
            break;

            case FUNC_WORLD_CLOCK:
                card = (is_create == true) ? compo_cardbox_create(frm, 1, 1, 5, CARD_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);

                compo_cardbox_rect_set_location(card, 0, 0, 0, CARD_WIDTH, CARD_HEIGHT, 38);
                compo_cardbox_text_set_align_center(card, 0, false);
                compo_cardbox_text_set_forecolor(card, 0, make_color(72, 222, 255));
                compo_cardbox_text_set_location(card, 0, 18-CARD_WIDTH/2, 18-CARD_HEIGHT/2, CARD_WIDTH, 48);
                compo_cardbox_text_set(card, 0, i18n[STR_WORLD_CLOCK]);

                if(worldClockData.number)
                {
                    ute_module_systemtime_time_t get_time;
                    func_get_world_time(&get_time,worldClockData,worldClockData.number - 1);
                    memset(tmp, 0, sizeof(tmp));
                    snprintf(tmp, sizeof(tmp), "%02d:%02d", get_time.hour, get_time.min);
                    compo_cardbox_text_set_font(card,1, UI_BUF_0FONT_FONT_NUM_48_BIN);
                    compo_cardbox_text_set(card,1,tmp);
                    widget_text_t* txt = card->text[1];
                    if (txt)
                    {
                        card->roll_cb[1].mode = TEXT_AUTOROLL_MODE_NULL;
                        widget_text_set_client(txt, 0, 0);
                    }
                    compo_cardbox_text_set_location(card,1,18-CARD_WIDTH/2,- 20,150,50);
                    compo_cardbox_text_set_align_center(card,1, false);

                    compo_cardbox_text_set(card,2,worldClockData.info[worldClockData.number - 1].countryNameU8);
                    compo_cardbox_text_set_forecolor(card, 2,make_color(0x66,0x66,0x66));
                    if(worldClockData.number == 1)
                    {
                        compo_cardbox_text_set_location(card,2,18-CARD_WIDTH/2,26,CARD_WIDTH - 36,48);
                        compo_cardbox_text_set_align_center(card,2, false);
                        area_t txt_leng = widget_text_get_area(card->text[2]);
                        if(txt_leng.wid < CARD_WIDTH - 36)
                        {
                            widget_text_t* txt = card->text[2];
                            if (txt)
                            {
                                card->roll_cb[2].mode = TEXT_AUTOROLL_MODE_NULL;
                                widget_text_set_client(txt, 0, 0);
                            }
                        }
                        for(u8 i = 3; i < 5; i++)
                        {
                            compo_cardbox_text_set(card,i,NULL);
                        }
                        compo_cardbox_icon_set(card, 0,NULL);
                    }
                    else
                    {
                        compo_cardbox_text_set_align_center(card,2, true);
                        compo_cardbox_text_set_location(card,2,PRO_X(90),46,150,48);

                        memset(&get_time,0,sizeof(ute_module_systemtime_time_t));
                        func_get_world_time(&get_time,worldClockData,worldClockData.number - 2);

                        snprintf(tmp, sizeof(tmp), "%02d:%02d", get_time.hour, get_time.min);
                        compo_cardbox_text_set_location(card,3,PRO_X(220),- 20,150,50);
                        compo_cardbox_text_set_font(card,3, UI_BUF_0FONT_FONT_NUM_48_BIN);
                        compo_cardbox_text_set_align_center(card,3, false);
                        compo_cardbox_text_set(card,3,tmp);

                        compo_cardbox_text_set(card,4,worldClockData.info[worldClockData.number - 2].countryNameU8);
                        compo_cardbox_text_set_location(card,4,PRO_X(268),46,150,48);
                        compo_cardbox_text_set_forecolor(card, 4,make_color(0x66,0x66,0x66));

                        compo_cardbox_icon_set_location(card, 0, 0, 20, 2, 64);
                        compo_cardbox_icon_set(card, 0, UI_BUF_I341001_23_WIDGETS_LINE_WORLD_CLOCK_BIN);
                    }
                }
                else
                {
                    compo_cardbox_text_set_font(card,1, 0);
                    compo_cardbox_text_set(card, 1, i18n[STR_APP_ADD]);
                    compo_cardbox_text_set_align_center(card, 1, false);
                    compo_cardbox_text_set_location(card, 1, 18-CARD_WIDTH/2, 80-CARD_HEIGHT/2, CARD_WIDTH - 36, 48);
                    for(u8 i = 2; i < 5; i++)
                    {
                        compo_cardbox_text_set(card,i,NULL);
                    }
                    compo_cardbox_icon_set(card, 0,NULL);
                }

                break;
        }

        if (card != NULL && is_create == true)
        {
            compo_setid(card, COMPO_ID_CARD_START+i);
        }
    }
}

static void func_mini_compo_card_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL )
    {
        switch (compo->id)
        {
            case COMPO_ID_BTN_VOICE:
                x = GUI_SCREEN_CENTER_X;
                y = VOICE_POS_Y;
                w = VOICE_PIC_WID;
                h = VOICE_PIC_HEI;
                card_y = VOICE_POS_Y;
                card_id = 0;
                break;
            case COMPO_ID_PIC_VOICE:
                x = GUI_SCREEN_CENTER_X;
                y = VOICE_POS_Y;
                w = VOICE_PIC_WID;
                h = VOICE_PIC_HEI;
                card_y = VOICE_POS_Y;
                card_id = 1;
                break;
            case COMPO_ID_BTN_EDIT:
                x = GUI_SCREEN_CENTER_X;
                card_y = y = CARD_START_POS_Y + (CARD_HEIGHT + CARD_GAP) * (func_cb.mini_sort_cnt-1) + CARD_HEIGHT/2 + gui_image_get_size(UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_ICON_BTN_CUSTOM_BIN).hei/2 + CARD_GAP;
                w = gui_image_get_size(UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_ICON_BTN_CUSTOM_BIN).wid;
                h = gui_image_get_size(UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_ICON_BTN_CUSTOM_BIN).hei;
                card_id = func_cb.mini_sort_cnt+1;
                break;
            default:
                if (compo->id >= COMPO_ID_CARD_START && compo->id < COMPO_ID_CARD_START+func_cb.mini_sort_cnt)   //卡片组件
                {
                    card_id = compo->id - COMPO_ID_CARD_START;
                    x = GUI_SCREEN_CENTER_X;
                    card_y = y = CARD_START_POS_Y + (CARD_HEIGHT + CARD_GAP) * (func_cb.mini_sort_cnt-1-card_id);
//                    printf("[update] id[%d], xy[%d,%d]\n", card_id, x, y);
                    w = CARD_WIDTH;
                    h = CARD_HEIGHT;
                    card_id += 2;
                }
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_mini_compo_card_t *f_mini_compo_card = (f_mini_compo_card_t*)func_cb.f_cb;
            f_mini_compo_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }
        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_location(pic, x, y, w, h);
//            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_BUTTON)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_location(btn, x, y, w, h);
//            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_button_set_visible(btn, false);
            }
            else
            {
                compo_button_set_visible(btn, true);
            }
        }


        compo = compo_get_next(compo);          //遍历组件
    }

}

compo_form_t* func_mini_compo_card_form_create(void)
{
    tft_set_temode(0);

    compo_form_t* frm = compo_form_create(true);

    compo_button_t* edit_btn = compo_button_create_by_image(frm, UI_BUF_I341001_23_WIDGETS_EDIT_BIN);
//    compo_button_set_pos(edit_btn, GUI_SCREEN_CENTER_X, CARD_START_POS_Y + (CARD_HEIGHT + CARD_GAP) * (func_cb.mini_sort_cnt-1) + CARD_HEIGHT/2);
    compo_setid(edit_btn, COMPO_ID_BTN_EDIT);

    func_mini_compo_card_create_o_update(frm, true);

    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_VOICE_ASSISTANT_BIN);
//    compo_picturebox_set_location(pic, GUI_SCREEN_CENTER_X, VOICE_POS_Y, VOICE_PIC_WID, VOICE_PIC_HEI);
    compo_setid(pic, COMPO_ID_PIC_VOICE);
    compo_button_t* voice_btn = compo_button_create(frm);
//    compo_button_set_location(voice_btn, GUI_SCREEN_CENTER_X, VOICE_POS_Y, VOICE_PIC_WID, VOICE_PIC_HEI);
    compo_setid(voice_btn, COMPO_ID_BTN_VOICE);

    func_mini_compo_card_update(0, true);

    return frm;
}


static s16 func_mini_compo_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//获取点击的卡片组件id
static u16 func_mini_compo_card_get_btn_id(point_t pt)
{
    u16 id = ID_NULL;
    component_t *compo = (component_t *)compo_pool_get_bottom();
    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t* card = (compo_cardbox_t*)compo;
            rect_t rect = compo_cardbox_get_absolute(card);
            if (compo_cardbox_get_visible(card) && abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
            {
                id = card->id;
            }
        }

        if (compo->type <= COMPO_TYPE_PROGRESSBAR && compo->type > COMPO_TYPE_NONE)
        {
            compo = compo_get_next(compo);          //遍历组件
        }
        else
        {
            return ID_NULL;
        }
    }
    return id;
}
bool start_testing = false;
bool mini_comp_timer_start = false;
static void func_mini_compo_card_click(void)
{
    int btn_id = compo_get_button_id();
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_mini_compo_card_get_btn_id(pt);
    //printf("click compo_id:%d\n", compo_id);

    if (compo_id != ID_NULL)
    {
        compo_cardbox_t* card = compo_getobj_byid(compo_id);
        u8 card_sort_idx = (func_cb.mini_sort_cnt - 1) - (compo_id - COMPO_ID_CARD_START);
        //printf("idx[%d] sta[%d]\n", card_sort_idx, func_cb.mini_tbl_sort[card_sort_idx]);

        switch (func_cb.mini_tbl_sort[card_sort_idx])
        {
            case FUNC_ACTIVITY:
                func_jump = FUNC_ACTIVITY;
                break;

            case FUNC_BT:
            {
                rect_t play_rect = compo_cardbox_get_icon_absolute(card, 1);
                rect_t pre_rect = compo_cardbox_get_icon_absolute(card, 2);
                rect_t next_rect = compo_cardbox_get_icon_absolute(card, 3);
                if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    //播放音乐
                    //printf("FUNC_BT play btn click\n");
                    if(ble_is_connect() || bt_a2dp_profile_completely_connected())
                    {
                        uteModuleMusicCtrlPaused(false);
                    }
                }
                else if (abs_s(pt.x - pre_rect.x) * 2 <= (pre_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - pre_rect.y) * 2 <= (pre_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    //上一首
                    //printf("FUNC_BT pre btn click\n");
                    if(ble_is_connect() || bt_a2dp_profile_completely_connected())
                    {
                        uteModuleMusicCtrl(false, true, false);
                    }
                }
                else if (abs_s(pt.x - next_rect.x) * 2 <= (next_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - next_rect.y) * 2 <= (next_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    //下一首
                    //printf("FUNC_BT pre btn click\n");
                    if(ble_is_connect() || bt_a2dp_profile_completely_connected())
                    {
                        uteModuleMusicCtrl(true, true, false);
                    }
                }
                else
                {
                    func_jump = FUNC_BT;
                }
            }
            break;

            case FUNC_HEARTRATE:
            {
                rect_t messure_rect = compo_cardbox_get_icon_absolute(card, 2);
                if (abs_s(pt.x - messure_rect.x) * 2 <= (messure_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - messure_rect.y) * 2 <= (messure_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    uteModuleHeartStartSingleTesting(TYPE_HEART);
                    start_testing = true;
                }
                func_jump = FUNC_HEARTRATE;

            }
            break;

            case FUNC_BLOOD_OXYGEN:
            {
                rect_t messure_rect = compo_cardbox_get_icon_absolute(card, 2);
                if (abs_s(pt.x - messure_rect.x) * 2 <= (messure_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - messure_rect.y) * 2 <= (messure_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                    start_testing = true;
                }
                func_jump = FUNC_BLOOD_OXYGEN;

            }
            break;

            case FUNC_PRESSURE:
            {
                rect_t messure_rect = compo_cardbox_get_icon_absolute(card, 2);
                if (abs_s(pt.x - messure_rect.x) * 2 <= (messure_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - messure_rect.y) * 2 <= (messure_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    uteModuleEmotionPressureStartSingleTesting(false);
                    start_testing = true;
                }
                func_jump = FUNC_PRESSURE;
            }
            break;

            case FUNC_ALARM_CLOCK:
            {
                u8 index = uteModuleSystemtimeAlarmGetNextIndex();
                rect_t alarm_isopen = compo_cardbox_get_icon_absolute(card, 7);
                if (abs_s(pt.x - alarm_isopen.x) * 2 <= (alarm_isopen.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - alarm_isopen.y) * 2 <= (alarm_isopen.hei + CLICK_ICON_AREA_SIZE))
                {
                    uteModuleSystemtimeEnableAlarm(index, !uteModuleSystemtimeGetAlarmEnableState(index));
                }
                else
                {
                    func_jump = FUNC_ALARM_CLOCK;
                }
            }
            break;

            case FUNC_WEATHER:
                func_jump = FUNC_WEATHER;
                break;

            case FUNC_SLEEP:
                func_jump = FUNC_SLEEP;
                break;

            case FUNC_SPORT_HISTORY:
                func_jump = FUNC_SPORT_HISTORY;
                break;

            case FUNC_WOMEN_HEALTH:
                func_jump = FUNC_WOMEN_HEALTH;
                break;

            case FUNC_STOPWATCH:
            {
                extern co_timer_t stopwatch_timer;
                rect_t stopwatch_rect = compo_cardbox_get_icon_absolute(card, 0);
                rect_t stopwatch_rest = compo_cardbox_get_icon_absolute(card, 1);
                if (abs_s(pt.x - stopwatch_rect.x) * 2 <= (stopwatch_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - stopwatch_rect.y) * 2 <= (stopwatch_rect.hei + CLICK_ICON_AREA_SIZE))
                {
                    u32 res_addr = 0;
                    if(sys_cb.stopwatch_sta)
                    {
                        res_addr = UI_BUF_I341001_23_WIDGETS_STOPWATCH_PAUSE_BIN;
                        sys_cb.stopwatch_sta = 0;
                    }
                    else
                    {
                        res_addr = UI_BUF_I341001_23_WIDGETS_STOPWATCH_PLAY_BIN;
                        sys_cb.stopwatch_sta = 1;
                        if (sys_cb.stopwatch_sta && sys_cb.stopwatch_total_msec == 0)
                        {
                            extern void stopwatch_50ms_pro(co_timer_t *timer, void *param);
                            co_timer_set(&stopwatch_timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, stopwatch_50ms_pro, NULL);
                        }
                    }
                    compo_cardbox_icon_set(card, 0, res_addr);

                }
                else if (abs_s(pt.x - stopwatch_rest.x) * 2 <= (stopwatch_rest.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - stopwatch_rest.y) * 2 <= (stopwatch_rest.hei + CLICK_ICON_AREA_SIZE))
                {
                    if(sys_cb.stopwatch_sta)
                    {
                        co_timer_del(&stopwatch_timer);
                        sys_cb.stopwatch_rec_cnt = 0;
                        sys_cb.stopwatch_total_msec = 0;
                        sys_cb.stopwatch_sta = 0;
                    }
                }
                else
                {
                    func_jump = FUNC_STOPWATCH;
                }
            }
            break;

            case FUNC_TIMER:
            {
                rect_t time_rect = compo_cardbox_get_icon_absolute(card, 0);
                rect_t time_rest = compo_cardbox_get_icon_absolute(card, 1);
                rect_t time5 = compo_cardbox_get_icon_absolute(card, 2);
                rect_t time10 = compo_cardbox_get_icon_absolute(card, 3);

                extern u32 rtccnt_tmp;
                extern co_timer_t timer_timer;
                if (abs_s(pt.x - time_rect.x) * 2 <= (time_rect.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - time_rect.y) * 2 <= (time_rect.hei + CLICK_ICON_AREA_SIZE))
                {

                    if(sys_cb.timer_sta != TIMER_STA_IDLE)
                    {
                        if (sys_cb.timer_sta == TIMER_STA_WORKING)
                        {
                            sys_cb.timer_sta = TIMER_STA_PAUSE;
                        }
                        else if (sys_cb.timer_sta == TIMER_STA_DONE)
                        {
                            sys_cb.timer_start_rtc = compo_cb.rtc_cnt;
                            sys_cb.timer_sta = TIMER_STA_WORKING;
                        }
                        else
                        {
                            if (sys_cb.timer_sta == TIMER_STA_RESET)
                            {
                                sys_cb.timer_start_rtc = compo_cb.rtc_cnt;
                            }
                            sys_cb.timer_sta = TIMER_STA_WORKING;
                        }
                    }
                    else
                    {
                        sys_cb.timer_left_sec = sys_cb.timer_total_sec = 1 * 60;
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                        mini_comp_timer_start = true;
                        func_jump = FUNC_TIMER;
                    }
                }
                else if(abs_s(pt.x - time_rest.x) * 2 <= (time_rest.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - time_rest.y) * 2 <= (time_rest.hei + CLICK_ICON_AREA_SIZE))
                {
                    if(sys_cb.timer_sta != TIMER_STA_IDLE)
                    {
                        co_timer_del(&timer_timer);
                        sys_cb.timer_sta = TIMER_STA_IDLE;
                        sys_cb.timer_left_sec = sys_cb.timer_total_sec = 0;
                        rtccnt_tmp = 0;
                    }
                    else
                    {
                        sys_cb.timer_left_sec = sys_cb.timer_total_sec = 2 * 60;
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                        mini_comp_timer_start = true;
                        func_jump = FUNC_TIMER;
                    }
                }
                else if(abs_s(pt.x - time5.x) * 2 <= (time5.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - time5.y) * 2 <= (time5.hei + CLICK_ICON_AREA_SIZE))
                {
                    if(sys_cb.timer_sta == TIMER_STA_IDLE)
                    {
                        sys_cb.timer_left_sec = sys_cb.timer_total_sec = 5 * 60;
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                        mini_comp_timer_start = true;
                    }
                    func_jump = FUNC_TIMER;
                }
                else if(abs_s(pt.x - time10.x) * 2 <= (time10.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - time10.y) * 2 <= (time10.hei + CLICK_ICON_AREA_SIZE))
                {
                    if(sys_cb.timer_sta == TIMER_STA_IDLE)
                    {
                        sys_cb.timer_left_sec = sys_cb.timer_total_sec = 10 * 60;
                        sys_cb.timer_sta = TIMER_STA_WORKING;
                        mini_comp_timer_start = true;
                    }
                    func_jump = FUNC_TIMER;
                }
                else
                {
                    func_jump = FUNC_TIMER;
                }
            }
            break;

            case FUNC_WORLD_CLOCK:
                func_jump = FUNC_WORLD_CLOCK;
                break;
        }

    }
    else if (btn_id != ID_NULL)
    {
        switch (btn_id)
        {
            case COMPO_ID_BTN_VOICE:
                //printf("COMPO_ID_BTN_VOICE\n");
                func_jump = FUNC_VOICE;
                break;

            case COMPO_ID_BTN_EDIT:
                //printf("COMPO_ID_BTN_EDIT\n");
                func_jump = FUNC_QUICK_CARD_SETTING;
                break;

            default:
                break;
        }
    }

    if (func_jump != FUNC_NULL)
    {
        // func_switch_to(func_jump, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);  //切换动画
        uteTaskGuiStartScreen(func_jump, 0, __func__);
//        func_cb.sta = func_jump;  //直接跳转
    }
}

//设置页面偏移位置
static void func_mini_compo_card_set_offs(s16 ofs_y)
{
    f_mini_compo_card_t *f_mini_compo_card = (f_mini_compo_card_t*)func_cb.f_cb;

    f_mini_compo_card->ofs_y = ofs_y;
    func_mini_compo_card_update(f_mini_compo_card->ofs_y, false);
}

static void func_mini_compo_card_process(void)
{
    f_mini_compo_card_t *f_mini_compo_card = (f_mini_compo_card_t*)func_cb.f_cb;

//    ute_personal_info_t personal;
//    uteApplicationCommonGetPersonalInfo(&personal);
    if (!uteModuleMenstrualCycleIsOpen() && f_mini_compo_card->womenFlag)
    {
        msg_enqueue(MSG_CHECK_LANGUAGE);
        return;
    }

    for (int i=0; i<func_cb.mini_sort_cnt; i++)
    {
        compo_cardbox_t *card = compo_getobj_byid(COMPO_ID_CARD_START + i);
        compo_cardbox_text_scroll_process(card, true);
    }

    if (f_mini_compo_card->flag_drag)
    {
        s32 dx, dy;
        f_mini_compo_card->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_mini_compo_card->flag_drag)
        {
            //拖动页面
            f_mini_compo_card->ofs_y_drag = func_mini_compo_card_limit_y(f_mini_compo_card->ofs_y + dy);
            func_mini_compo_card_update(f_mini_compo_card->ofs_y_drag, false);
            // uteModuleHeartStopSingleTesting(TYPE_HEART);
            // if (uteModuleBloodoxygenIsTesting())
            // {
            //     uteModuleBloodoxygenStopSingleTesting();///关闭测试
            // }
            // if(uteModuleEmotionPressureIsTesting())
            // {
            //     uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_INITIATIVE_QUIT);
            // }
        }
        else
        {
            //抬手
            f_mini_compo_card->ofs_y = f_mini_compo_card->ofs_y_drag;
            s16 last_dy = ctp_get_last_dxy().y;
            s16 to_y = f_mini_compo_card->ofs_y + (last_dy * DRAG_AUTO_SPEED);
//            f_mini_compo_card->moveto_y =

            if (to_y >= DRAG_Y_MAX)
            {
                f_mini_compo_card->moveto_y = DRAG_Y_MAX;  //回弹效果
            }
            else if (to_y <= DRAG_Y_MIN)
            {
                f_mini_compo_card->moveto_y = DRAG_Y_MIN;
            }
            else
            {
                f_mini_compo_card->moveto_y = to_y;//f_mini_compo_card->ofs_y;
            }

            f_mini_compo_card->flag_move_auto = (func_cb.mini_sort_cnt == 1) ? false : true;
            f_mini_compo_card->tick = tick_get();
        }
    }
    else if (f_mini_compo_card->flag_move_auto)
    {
        //自动移动
        if (f_mini_compo_card->ofs_y == f_mini_compo_card->moveto_y)
        {
            if (f_mini_compo_card->ofs_y < SPRING_Y_MIN)
            {
                f_mini_compo_card->moveto_y = SPRING_Y_MIN;
            }
            else if (f_mini_compo_card->ofs_y > SPRING_Y_MAX)
            {
                f_mini_compo_card->moveto_y = SPRING_Y_MAX;
            }
            else
            {
                f_mini_compo_card->flag_move_auto = false;            //移动完成
                f_mini_compo_card->moveto_card = f_mini_compo_card->focus_card;
            }
        }
        else if (tick_check_expire(f_mini_compo_card->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            //printf("[%d]--move_to->[%d] [%d,%d]\n", f_mini_compo_card->ofs_y, f_mini_compo_card->moveto_y, SPRING_Y_MIN, SPRING_Y_MAX);
            s16 dy;
            f_mini_compo_card->tick = tick_get();
            dy = f_mini_compo_card->moveto_y - f_mini_compo_card->ofs_y;
            if (dy > 0)
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy > FOCUS_AUTO_STEP)
                {
                    dy = FOCUS_AUTO_STEP;
//                } else {
//                    dy = 1;
                }
            }
            else if (dy < 0)
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy < -FOCUS_AUTO_STEP)
                {
                    dy = -FOCUS_AUTO_STEP;
//                } else {
//                    dy = -1;
                }
            }
            func_mini_compo_card_set_offs(f_mini_compo_card->ofs_y + dy);
        }
    }
    if (tick_check_expire(f_mini_compo_card->tick1, 100))
    {
        f_mini_compo_card->tick1 = tick_get();
        func_mini_compo_card_create_o_update(func_cb.frm_main, false);
    }
    func_process();
}

static void func_mini_compo_card_message(size_msg_t msg)
{
    f_mini_compo_card_t *f_mini_compo_card = (f_mini_compo_card_t*)func_cb.f_cb;

    if (f_mini_compo_card->flag_drag)
    {
        switch (msg)
        {
            case MSG_CTP_CLICK:
                func_mini_compo_card_click();
                break;

            case MSG_CTP_SHORT_LEFT:
                func_switch_to(FUNC_CLOCK, sys_cb.switch_lr_left_mode_choose);
                break;
            case KU_BACK:
            case KU_DELAY_BACK:
                func_switch_to(FUNC_CLOCK,sys_cb.switch_lr_left_mode_choose);
                return;
                //func_switch_to_clock();                     //切换回主时钟
                break;
            default:
                evt_message(msg);       //拖动中，只处理部分消息
                break;
        }
        return;
    }
    else if (f_mini_compo_card->flag_move_auto)
    {
        switch (msg)
        {
            case MSG_CTP_TOUCH:   //自动移动过程中，触屏停止
                f_mini_compo_card->flag_drag = func_cb.mini_sort_cnt == 1 ? false : true;
                f_mini_compo_card->flag_move_auto = false;
                break;

            case MSG_QDEC_FORWARD:  //向前滚动菜单
                if (++f_mini_compo_card->moveto_card > f_mini_compo_card->focus_card)
                {
//                f_mini_compo_card->moveto_y = func_clock_sub_card_align_by_idx(f_mini_compo_card->moveto_card);
                }
                break;

            case MSG_QDEC_BACKWARD:
                if (--f_mini_compo_card->moveto_card < f_mini_compo_card->focus_card)
                {
//                f_mini_compo_card->moveto_y = func_clock_sub_card_align_by_idx(f_mini_compo_card->moveto_card);
                }
                break;

            case MSG_CTP_SHORT_LEFT:
                func_switch_to(FUNC_CLOCK, sys_cb.switch_lr_left_mode_choose);
                break;

            case MSG_CTP_CLICK:
                func_mini_compo_card_click();
                break;

            case KU_BACK:
            case KU_DELAY_BACK:
                func_switch_to(FUNC_CLOCK,sys_cb.switch_lr_left_mode_choose);
                return;
                break;

            default:
                evt_message(msg);
                break;
        }
        return;
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_mini_compo_card_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
//            if (msg == MSG_CTP_SHORT_DOWN && ctp_get_sxy().y < EXIT_Y_MAX) {   //下滑返回到时钟主界面
//                func_clock_sub_card_switch_to_clock(false);
//            } else {
            f_mini_compo_card->flag_drag = func_cb.mini_sort_cnt == 1 ? false : true;
            f_mini_compo_card->flag_move_auto = false;
//            }
            break;

        case MSG_CTP_SHORT_LEFT:
            func_switch_to(FUNC_CLOCK, sys_cb.switch_lr_left_mode_choose);
            break;
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_FORWARD:
            f_mini_compo_card->flag_move_auto = true;
            if (++f_mini_compo_card->moveto_card > f_mini_compo_card->focus_card)
            {
//                f_mini_compo_card->moveto_y = func_clock_sub_card_align_by_idx(f_mini_compo_card->moveto_card);
            }
            TRACE("focus_card[%d] to_y[%d]\n", f_mini_compo_card->focus_card, f_mini_compo_card->moveto_y);
            break;

        case MSG_QDEC_BACKWARD:
            f_mini_compo_card->flag_move_auto = true;
            if (--f_mini_compo_card->moveto_card < f_mini_compo_card->focus_card)
            {
//                f_mini_compo_card->moveto_y = func_clock_sub_card_align_by_idx(f_mini_compo_card->moveto_card);
            }
            TRACE("focus_card[%d] to_y[%d]\n", f_mini_compo_card->focus_card, f_mini_compo_card->moveto_y);
            break;

        case MSG_SYS_500MS:
#if BT_ID3_TAG_EN
            if (bt_a2dp_profile_completely_connected())
            {
                bt_music_paly_status_info();
            }
#endif
            break;

        case KU_BACK:
        case KU_DELAY_BACK:
            func_switch_to(FUNC_CLOCK,sys_cb.switch_lr_left_mode_choose);
            return;

        default:
            func_message(msg);
            break;
    }

}
#endif
static void func_mini_compo_card_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_mini_compo_card_t));

    f_mini_compo_card_t *f_mini_compo_card = (f_mini_compo_card_t*)func_cb.f_cb;

//    ute_personal_info_t personal;
//    uteApplicationCommonGetPersonalInfo(&personal);
    if (!uteModuleMenstrualCycleIsOpen())
    {
        removeElement(func_cb.mini_tbl_sort, &func_cb.mini_sort_cnt, FUNC_WOMEN_HEALTH);
        uteModuleGuiCommonSaveMiniCardTblSort(func_cb.mini_tbl_sort, func_cb.mini_sort_cnt);
    }
    for(u8 k = 0; k < func_cb.mini_sort_cnt; k++)
    {
        if (func_cb.mini_tbl_sort[k] == FUNC_WOMEN_HEALTH)
        {
            f_mini_compo_card->womenFlag = true;
        }
        else
        {
            f_mini_compo_card->womenFlag = false;
        }
    }
    func_cb.frm_main = func_mini_compo_card_form_create();

    tft_set_temode(0);
}

static void func_mini_compo_card_exit(void)
{
    func_cb.last = FUNC_MINI_COMPO_CARD;
    tft_set_temode(DEFAULT_TE_MODE);
}

void func_mini_compo_card(void)
{
    printf("%s\n", __func__);
    func_mini_compo_card_enter();
    while (func_cb.sta == FUNC_MINI_COMPO_CARD)
    {
        func_mini_compo_card_process();
        func_mini_compo_card_message(msg_dequeue());
    }
    func_mini_compo_card_exit();
}

#endif // UTE_MODULE_SCREENS_MINI_CARD_SUPPORT
