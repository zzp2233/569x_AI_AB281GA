#include "include.h"
#include "func.h"
#include "ute_module_notify.h"
#include "func_cover.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
enum
{
    COMPO_ID_CARD_FIRST=1,
    COMPO_ID_CARD_MAX_LAST=UTE_MODULE_NOTIFY_SAVE_CNT,
    COMPO_ID_ALL_DEL_BTN,
};
typedef struct f_message_detail_t_
{
    page_tp_move_t *ptm;
    u8 msg_num;
    bool flag_drag;                 //开始拖动
    bool flag_init;
} f_message_detail_t;

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

typedef struct
{
    u8 hour;
    u8 am_pm;
} message1_12hour_convert_t;

message1_12hour_convert_t message1_convert_to_12hour(u8 time)
{
    u8 am_pm = (time >= 12) ? 2 : 1;    //2 PM, 1 AM
    message1_12hour_convert_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (time == 0)
        {
            hour12.hour = 12;
        }
        else if (time > 12)
        {
            hour12.hour = time - 12;
        }
        else
        {
            hour12.hour = time;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = time;
    hour12.am_pm = 0;
    return hour12;
}

typedef struct f_message_detail_card_t_
{
    s32  pic_x;
    s32  pic_y;
    s32  pic_w;
    s32  pic_h;
    s32  time_x;
    s32  time_y;
    s32  time_w;
    s32  time_h;
    s32  msg_x;
    s32  msg_y;
    s32  msg_w;
    s32  msg_h;
} f_message_detail_card_t;

static const f_message_detail_card_t message_card[]=
{
    {
        .pic_x  = -136,    .pic_y  = -54,   .pic_w  = 44,    .pic_h  = 44,
        .time_x = 0,       .time_y = -68,   .time_w = 158,   .time_h = 34,
        .msg_x  = -156,    .msg_y  = -5,    .msg_w  = 310,   .msg_h  = 82,
    },
};

static u32 func_message_detail_card_get_icon(u8 type)
{
    if (type < func_cover_get_detail_msg_cnt())
    {
        return func_cover_get_detail_msg_ui(type);
    }
    return 0;
}

void companyDuplicates(uint8_t srcNum, uint8_t *srcArray, uint8_t srcSize, uint8_t *dstArray, uint8_t *dstSize)
{
    *dstSize = 0; // 初始化目标数组的大小为 0
    memset(dstArray, 0, UTE_MODULE_NOTIFY_SAVE_CNT);

    for (uint8_t i = 0; i < srcSize; i++)
    {
        if (srcNum == srcArray[i])
        {
            dstArray[(*dstSize)++] = i;
        }
    }
}

extern uint8_t comArray_val;

uint8_t sametype_count = 0;
uint8_t msgtypeShow[UTE_MODULE_NOTIFY_SAVE_CNT] = {0};

extern const f_cover_detail_msg_item_t tbl_cover_detail_msg_item[];

//创建消息窗体
compo_form_t *func_message_detail_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape = compo_shape_create_for_page(frm, frm->page, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_BLACK);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MESSAGE]);
    compo_textbox_t *title = compo_textbox_create_for_page(frm, frm->page, strlen(i18n[STR_MESSAGE]));
    compo_textbox_set_align_center(title, false);
    compo_textbox_set_location(title, FORM_TITLE_LEFT, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 4 - FORM_TITLE_LEFT, 50);
    compo_textbox_set(title, i18n[STR_MESSAGE]);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);  //获取系统时间
    char time_buff[20] = {0};
    message1_12hour_convert_t timeHour = message1_convert_to_12hour(time.hour);
    if (timeHour.am_pm > 0)
    {
        snprintf(time_buff, sizeof(time_buff), "%s%02d:%02d", timeHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM], timeHour.hour, time.min);
    }
    else
    {
        snprintf(time_buff, sizeof(time_buff), "%02d:%02d", timeHour.hour, time.min);
    }
    title = compo_textbox_create_for_page(frm, frm->page, 20);
    compo_textbox_set_location(title, FORM_TITLE_RIGHT + 20, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12, FORM_TITLE_HEIGHT);
    compo_textbox_set_align_center(title, false);
    compo_textbox_set_multiline(title, false);
    compo_textbox_set(title, time_buff);

    u8 msgtype_num[UTE_MODULE_NOTIFY_SAVE_CNT] = {0};
    char time_buf[30];
    u8 msg_num = uteModuleNotifyGetTotalNotifyCnt();

    if(msg_num == 0)
    {
        //创建无消息界面
        compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I341001_14_INFORMATION_MESSAGE_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 139+58);

        compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_MSG]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 294 + 20, 300, 40);
        compo_textbox_set(txt, i18n[STR_NO_MSG]);

        return frm;
    }

    if(msg_num>COMPO_ID_CARD_MAX_LAST)
    {
        msg_num = COMPO_ID_CARD_MAX_LAST;
    }

    ute_module_notify_data_t *ute_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
    uteModuleNotifyGetData(ute_msg);
    ute_module_systemtime_time_t time_data;
    uteModuleSystemtimeGetTime(&time_data);//获取系统时间

    for(int i=0; i<msg_num; i++)
    {
        msgtype_num[i] = ute_msg->historyNotify[i].type;
    }

    sametype_count = 0;         //用于计算界面长度

    companyDuplicates(comArray_val, &msgtype_num[0], msg_num, msgtypeShow, &sametype_count);

    for (u8 u = 0; u < sametype_count; u++)
    {
        uint8_t hour=ute_msg->historyNotify[msgtypeShow[u]].hour;/*!系统时间，24小时格式的小时格式，数值为0~23 */
        uint8_t min =ute_msg->historyNotify[msgtypeShow[u]].min ;/*!系统时间，分钟，数值为0~59 */
        uint8_t str_am[30];
        memset(str_am, 0, sizeof(str_am));
        if(uteModuleSystemtime12HOn())
        {
            if(hour<12)
            {
                memcpy(str_am, i18n[STR_AM], strlen(i18n[STR_AM])+1);
            }
            else
            {
                memcpy(str_am, i18n[STR_PM], strlen(i18n[STR_AM])+1);
            }
            hour %= 12;
            if(hour==0)
            {
                hour = 12;
            }
        }

        memset(time_buf,0,sizeof(time_buf));
        if(time_data.year != ute_msg->historyNotify[msgtypeShow[u]].year || time_data.month != ute_msg->historyNotify[msgtypeShow[u]].month)
        {
            if (time_data.year != ute_msg->historyNotify[msgtypeShow[u]].year)
            {
                snprintf(time_buf,sizeof(time_buf), "%d/%d/%d", //record_tbl[index].callTime.year,
                         ute_msg->historyNotify[msgtypeShow[u]].year,
                         ute_msg->historyNotify[msgtypeShow[u]].month,
                         ute_msg->historyNotify[msgtypeShow[u]].day);
            }
            else
            {
                snprintf(time_buf,sizeof(time_buf), "%d/%d", //record_tbl[index].callTime.year,
                         ute_msg->historyNotify[msgtypeShow[u]].month,
                         ute_msg->historyNotify[msgtypeShow[u]].day);
            }
        }
        else if(time_data.day > ute_msg->historyNotify[msgtypeShow[u]].day && time_data.month == ute_msg->historyNotify[msgtypeShow[u]].month)
        {
            snprintf(time_buf,sizeof(time_buf), "%d/%d", //record_tbl[index].callTime.year,
                     ute_msg->historyNotify[msgtypeShow[u]].month,
                     ute_msg->historyNotify[msgtypeShow[u]].day);
        }
        else
        {
            snprintf(time_buf,sizeof(time_buf), "%02d:%02d %s", //record_tbl[index].callTime.year,
                     hour,min,str_am);
        }

        char* msg = (char*)ute_msg->historyNotify[msgtypeShow[u]].content;
        compo_cardbox_t *cardbox = compo_cardbox_create(frm, 0, 2, 2, 342, 166);
        compo_setid(cardbox, COMPO_ID_CARD_FIRST + msgtypeShow[u]);
        compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, 72 + 83 + 180*u);

        compo_cardbox_icon_set_pos(cardbox, 0, 0, 0);
        compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_14_INFORMATION_BG_BIN);

        compo_cardbox_icon_set_location(cardbox, 1, message_card[0].pic_x, message_card[0].pic_y, message_card[0].pic_w, message_card[0].pic_h);
        compo_cardbox_icon_set(cardbox, 1, func_message_detail_card_get_icon(ute_msg->historyNotify[msgtypeShow[u]].type));

        compo_cardbox_text_set_location(cardbox, 0, message_card[0].time_x, message_card[0].time_y, message_card[0].time_w, message_card[0].time_h);
        widget_set_align_center(cardbox->text[0], false);
        widget_text_set_color(cardbox->text[0], make_color(0x94, 0x94, 0x94));
        widget_text_set_right_align(cardbox->text[0], true);
        compo_cardbox_text_set(cardbox, 0, time_buf);

        compo_cardbox_text_set_location(cardbox, 1, message_card[0].msg_x, message_card[0].msg_y, message_card[0].msg_w, message_card[0].msg_h);
        widget_set_align_center(cardbox->text[1], false);
        compo_cardbox_text_set(cardbox, 1, msg);
        widget_text_set_wordwrap(cardbox->text[1], true);
        widget_text_set_ellipsis(cardbox->text[1], true);

        uint8_t type = ute_msg->historyNotify[msgtypeShow[u]].type;
        compo_textbox_t *textbox = compo_textbox_create_for_page(frm, cardbox->page, strlen(i18n[tbl_cover_detail_msg_item[type].str_idx]));
        compo_textbox_set_location(textbox, message_card[0].pic_x + 28, message_card[0].pic_y - 16, 110, 48);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set(textbox, i18n[tbl_cover_detail_msg_item[type].str_idx]);
    }

    //创建按钮
    compo_button_t* btn = compo_button_create_by_image(frm, UI_BUF_I341001_14_INFORMATION_DEL_BIN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, (sametype_count == 1) ? 388 : 88 + 40 + 179*(sametype_count));
    compo_setid(btn, COMPO_ID_ALL_DEL_BTN);

    ab_free(ute_msg);

    return frm;
}


static void func_message_detail_card_init()
{
    f_message_detail_t *f_message_detail = (f_message_detail_t *)func_cb.f_cb;

    f_message_detail->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));

    u8 msg_num = uteModuleNotifyGetTotalNotifyCnt();
    f_message_detail->msg_num = msg_num;

    uint16_t page_height = sametype_count*179 + 62 + 80;
    if(page_height < GUI_SCREEN_HEIGHT)
    {
        page_height = 0;
    }

    f_message_detail->flag_init = false;
    f_message_detail->flag_drag = false;

    page_move_info_t info =
    {
        .title_used = true,
        .page_size = page_height,
        .page_count = 1,
        .jump_perc = 0,
        .quick_jump_perc = 40,
        .up_over_perc   = 1,
        .down_over_perc = 1,
    };
    compo_page_move_init(f_message_detail->ptm, func_cb.frm_main->page_body, &info);
}

#else
compo_form_t *func_message_detail_form_create(void)
{
};
static void func_message_detail_card_init()
{
}
#endif
//更新消息卡片
static void func_message_detail_card_update()
{
    f_message_detail_t *f_message_detail = (f_message_detail_t *)func_cb.f_cb;

    if(uteModuleNotifyIsNewNotifyDisplay())
    {
        uteModuleNotifySetNewNotifyFlag(!uteModuleNotifyIsNewNotifyDisplay());
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    else if (f_message_detail->msg_num != uteModuleNotifyGetTotalNotifyCnt())
    {
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
}

//获取点击的卡片组件id
static u16 func_message_detail_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<COMPO_ID_CARD_MAX_LAST; i++)
    {
        id = COMPO_ID_CARD_FIRST + i;
        cardbox = compo_getobj_byid(id);
        if(cardbox!=NULL)
        {
            rect = compo_cardbox_get_absolute(cardbox);
            if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
            {
                ret = id;
                break;
            }
        }
    }
    return ret;
}
static void func_message_detail_click(void)
{
    u8 id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_ALL_DEL_BTN:            //清除消息
            uteModuleNotifyDelMultiHistoryData(msgtypeShow, sametype_count);
            func_switch_to(FUNC_MESSAGE, 0);
//            uteModuleNotifyDelAllHistoryData(true);
            break;
        default:
        {
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            point_t pt = ctp_get_sxy();
            u16 compo_id = func_message_detail_card_get_btn_id(pt);
            if (compo_id < COMPO_ID_CARD_FIRST || compo_id > COMPO_ID_CARD_MAX_LAST)
            {
                return;
            }
            compo_id-=1;
            char time_buf[60];
            ute_module_notify_data_t *ute_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
            uteModuleNotifyGetData(ute_msg);
            ute_module_systemtime_time_t time_data;
            uteModuleSystemtimeGetTime(&time_data);//获取系统时间

            uint8_t hour=ute_msg->historyNotify[compo_id].hour;/*!系统时间，24小时格式的小时格式，数值为0~23 */
            uint8_t min =ute_msg->historyNotify[compo_id].min ;/*!系统时间，分钟，数值为0~59 */
            uint8_t str_am[30];
            memset(str_am,0,sizeof(str_am));
            if(uteModuleSystemtime12HOn())
            {
                if(hour<12)
                {
                    memcpy(str_am,i18n[STR_AM],strlen(i18n[STR_AM])+1);
                }
                else
                {
                    memcpy(str_am,i18n[STR_PM],strlen(i18n[STR_AM])+1);
                }
                hour %= 12;
                if(hour==0)
                {
                    hour = 12;
                }
            }

            memset(time_buf,0,sizeof(time_buf));
            if(time_data.year != ute_msg->historyNotify[compo_id].year || time_data.month != ute_msg->historyNotify[compo_id].month)
            {
                if (time_data.year != ute_msg->historyNotify[compo_id].year)
                {
                    snprintf(time_buf,sizeof(time_buf), "%d/%d/%d", //record_tbl[index].callTime.year,
                             ute_msg->historyNotify[compo_id].year,
                             ute_msg->historyNotify[compo_id].month,
                             ute_msg->historyNotify[compo_id].day);
                }
                else
                {
                    snprintf(time_buf,sizeof(time_buf), "%d/%d", //record_tbl[index].callTime.year,
                             ute_msg->historyNotify[compo_id].month,
                             ute_msg->historyNotify[compo_id].day);
                }
            }
            else if(time_data.day > ute_msg->historyNotify[compo_id].day && time_data.month == ute_msg->historyNotify[compo_id].month)
            {
                sprintf((char*)time_buf, "%d/%d", //record_tbl[index].callTime.year,
                        ute_msg->historyNotify[compo_id].month,
                        ute_msg->historyNotify[compo_id].day);
            }
            else
            {
                sprintf((char*)time_buf, "%02d:%02d %s", //record_tbl[index].callTime.year,
                        hour,min,str_am);
            }

            char* msg = (char*)ute_msg->historyNotify[compo_id].content;
            sys_cb.msg_index = ute_msg->historyNotify[compo_id].type;
            int res = msgbox(msg, i18n[func_cover_get_str_idx(MSGBOX_MSG_TYPE_DETAIL)], time_buf, MSGBOX_MODE_BTN_DELETE, MSGBOX_MSG_TYPE_DETAIL);
            if (res == MSGBOX_RES_DELETE)
            {
                uteModuleNotifySetDisplayIndex(compo_id);
                uteModuleNotifyDelAllHistoryData(false);
                if (sametype_count == 1)
                {
                    func_switch_to(FUNC_MESSAGE, 0);
                }
            }
            ab_free(ute_msg);
#endif
        }
        break;
    }
}

//消息功能事件处理
static void func_message_detail_process(void)
{
    f_message_detail_t *f_message_detail = (f_message_detail_t *)func_cb.f_cb;
    compo_page_move_process(f_message_detail->ptm);
    func_message_detail_card_update();
    func_process();
}

//消息功能消息处理
static void func_message_detail_message(size_msg_t msg)
{
    f_message_detail_t *f_message_detail = (f_message_detail_t *)func_cb.f_cb;

    if (f_message_detail->flag_drag)
    {
        evt_message(msg);  //拖动中，只处理部分消息
        return;
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_message_detail_click();
            break;

        case MSG_CTP_TOUCH:
            if(uteModuleNotifyGetTotalNotifyCnt())
            {
                compo_page_move_touch_handler(f_message_detail->ptm);
            }
            break;

        case MSG_CTP_LONG:
            break;

        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CTP_SHORT_RIGHT:
            func_switch_to(FUNC_MESSAGE, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);
            break;

        default:
            func_message(msg);
            break;
    }
}
//进入消息功能
static void func_message_detail_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_message_detail_t));
    func_cb.frm_main = func_message_detail_form_create();
    func_message_detail_card_init();
}

//退出消息功能
static void func_message_detail_exit(void)
{
    f_message_detail_t *f_msg = (f_message_detail_t *)func_cb.f_cb;
    if (sys_cb.refresh_language_flag)
    {
        if(f_msg->ptm != NULL)
        {
            func_free(f_msg->ptm);
        }
        return;
    }

    //func_cb.last = FUNC_MESSAGE_DETAIL;
    task_stack_remove(FUNC_MESSAGE_DETAIL);

    if(f_msg->ptm != NULL)
    {
        func_free(f_msg->ptm);
    }
}

//消息功能
void func_message_detail_info(void)
{
    printf("%s\n", __func__);
    func_message_detail_enter();
    while (func_cb.sta == FUNC_MESSAGE_DETAIL)
    {
        func_message_detail_process();
        func_message_detail_message(msg_dequeue());
    }
    func_message_detail_exit();
}

#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
