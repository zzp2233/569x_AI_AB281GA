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

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    COMPO_ID_CARD_FIRST=1,
    COMPO_ID_CARD_MAX_LAST=10,
    COMPO_ID_ALL_DEL_BTN,
};
typedef struct f_message_t_
{
    page_tp_move_t *ptm;
} f_message_t;
typedef struct f_message_card_t_
{
    s32  rect_x;
    s32  rect_y;
    s32  rect_w;
    s32  rect_h;
    s32  rect_r;
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
} f_message_card_t;

static const f_message_card_t message_card[]=
{
    {
        .rect_x=0,     .rect_y=0,    .rect_w=228,  .rect_h=94, .rect_r=10,
        .pic_x=-84,    .pic_y=-46,   .pic_w=32,    .pic_h=32,
        .time_x=-18,   .time_y=-40,  .time_w=118,  .time_h=25,
        .msg_x=-104,   .msg_y=-10,   .msg_w=205,   .msg_h=55,
    },
};

static u32 func_message_card_get_icon(u8 type)
{
    if (type < func_cover_get_detail_msg_cnt())
    {
        return func_cover_get_detail_msg_ui(type);
    }
    return 0;
}
//创建消息窗体
compo_form_t *func_message_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MESSAGE]);

    char time_buf[30];
    u8 msg_num = uteModuleNotifyGetTotalNotifyCnt();

    if(msg_num == 0)
    {
        //创建无消息界面
        compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I330001_NOTIFICATION_NO_DATA_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X);

        compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_MSG]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 184, 230, 30);
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
        uint8_t hour=ute_msg->historyNotify[i].hour;/*!系统时间，24小时格式的小时格式，数值为0~23 */
        uint8_t min =ute_msg->historyNotify[i].min ;/*!系统时间，分钟，数值为0~59 */
        uint8_t *str_am = (uint8_t *)ab_zalloc(sizeof(uint8_t));
        memset(str_am,0,sizeof(str_am));
        if(uteModuleSystemtime12HOn())
        {
            if(hour<=12 && hour!=0)
            {
                memcpy(&str_am[0],i18n[STR_AM],strlen(i18n[STR_AM])+1);
            }
            else
            {
                memcpy(&str_am[0],i18n[STR_PM],strlen(i18n[STR_AM])+1);
            }
            hour %= 12;
            if(hour==0)
            {
                hour = 12;
            }
        }

        memset(time_buf,0,sizeof(time_buf));
        if(time_data.year != ute_msg->historyNotify[i].year || time_data.month != ute_msg->historyNotify[i].month)
        {
            sprintf((char*)time_buf, "%04d/%02d/%02d", //record_tbl[index].callTime.year,
                    ute_msg->historyNotify[i].year,
                    ute_msg->historyNotify[i].month,
                    ute_msg->historyNotify[i].day);
        }
        else if(time_data.day > ute_msg->historyNotify[i].day && time_data.month == ute_msg->historyNotify[i].month)
        {
            sprintf((char*)time_buf, "%02d/%02d", //record_tbl[index].callTime.year,
                    ute_msg->historyNotify[i].month,
                    ute_msg->historyNotify[i].day);
        }
        else
        {
            sprintf((char*)time_buf, "%02d:%02d %s", //record_tbl[index].callTime.year,
                    hour,min,str_am);
        }

        char* msg = (char*)ute_msg->historyNotify[i].content;

        compo_cardbox_t *cardbox = compo_cardbox_create(frm,1,1,2,228,130);
        compo_setid(cardbox,COMPO_ID_CARD_FIRST+i);
        compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,109+133*i);

        compo_cardbox_rect_set_location(cardbox,0,message_card[0].rect_x,message_card[0].rect_y,message_card[0].rect_w,message_card[0].rect_h,message_card[0].rect_r);
        compo_cardbox_rect_set_color(cardbox,0, make_color(41,41,41));

        compo_cardbox_icon_set_location(cardbox,0,message_card[0].pic_x,message_card[0].pic_y,message_card[0].pic_w,message_card[0].pic_h);
        compo_cardbox_icon_set(cardbox, 0, func_message_card_get_icon(ute_msg->historyNotify[i].type));

        compo_cardbox_text_set_location(cardbox,0,message_card[0].time_x,message_card[0].time_y,message_card[0].time_w,message_card[0].time_h);
        widget_set_align_center(cardbox->text[0],false);
        widget_text_set_color(cardbox->text[0],make_color(0x94,0x94,0x94));
        compo_cardbox_text_set(cardbox,0,time_buf);
        widget_text_set_right_align(cardbox->text[0],true);

        compo_cardbox_text_set_location(cardbox,1,message_card[0].msg_x,message_card[0].msg_y,message_card[0].msg_w,message_card[0].msg_h);
        widget_set_align_center(cardbox->text[1],false);
        compo_cardbox_text_set(cardbox,1,msg);
        widget_text_set_wordwrap(cardbox->text[1], true);
        widget_text_set_ellipsis(cardbox->text[1], true);
    }
    printf("11111111111111111111111111\n");
    //创建按钮
    compo_button_t* btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,105+(133*msg_num));
    compo_setid(btn, COMPO_ID_ALL_DEL_BTN);

    //创建删除按钮文本
    compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR_ALL]));
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,105+(133*msg_num),200,62);
    compo_textbox_set(text, i18n[STR_CLEAR_ALL]);

    ab_free(ute_msg);

    return frm;
}


static void func_message_card_init()
{
    f_message_t *f_message = (f_message_t *)func_cb.f_cb;
    f_message->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));

    u8 msg_num = uteModuleNotifyGetTotalNotifyCnt();

    uint16_t page_height = msg_num*133+109;
    if(page_height<GUI_SCREEN_HEIGHT)
    {
        page_height = GUI_SCREEN_HEIGHT;
    }

    page_move_info_t info =
    {
        .title_used = true,
        .page_size = page_height,
        .page_count = 1,
        .jump_perc = 0,
        .quick_jump_perc = 3,
        .up_over_perc   = 1,
        .down_over_perc = 1,
    };
    compo_page_move_init(f_message->ptm, func_cb.frm_main->page_body, &info);
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    COMPO_ID_CARD_FIRST=1,
    COMPO_ID_CARD_MAX_LAST=10,
    COMPO_ID_ALL_DEL_BTN,
};
typedef struct f_message_t_
{
    u8 refresh_data;
    page_tp_move_t *ptm;
} f_message_t;
typedef struct f_message_card_t_
{
    s32  rect_x;
    s32  rect_y;
    s32  rect_w;
    s32  rect_h;
    s32  rect_r;
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
} f_message_card_t;

static const f_message_card_t message_card[]=
{
    {
        .rect_x=0,     .rect_y=0,    .rect_w=284,  .rect_h=116, .rect_r=20,
        .pic_x=-84,    .pic_y=-58,   .pic_w=46,    .pic_h=46,
        .time_x=5,     .time_y=-51,  .time_w=120,  .time_h=30,
        .msg_x=-119,   .msg_y=-20,   .msg_w=250,   .msg_h=72,
    },
};

static u32 func_message_card_get_icon(u8 type)
{
    if (type < func_cover_get_detail_msg_cnt())
    {
        return func_cover_get_detail_msg_ui(type);
    }
    return 0;
}
//创建消息窗体
compo_form_t *func_message_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MESSAGE]);

    char time_buf[30];
    u8 msg_num = uteModuleNotifyGetTotalNotifyCnt();

    if(msg_num == 0)
    {
        //创建无消息界面
        compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I332001_NOTIFICATION_NO_DATA_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X);

        compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_MSG]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 212+16, 230, 30);
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
        memset(time_buf,0,sizeof(time_buf));
        if(time_data.year != ute_msg->historyNotify[i].year || time_data.month != ute_msg->historyNotify[i].month)
        {
            sprintf((char*)time_buf, "%04d/%02d/%02d", //record_tbl[index].callTime.year,
                    ute_msg->historyNotify[i].year,
                    ute_msg->historyNotify[i].month,
                    ute_msg->historyNotify[i].day);
        }
        else if(time_data.day > ute_msg->historyNotify[i].day && time_data.month == ute_msg->historyNotify[i].month)
        {
            sprintf((char*)time_buf, "%02d/%02d", //record_tbl[index].callTime.year,
                    ute_msg->historyNotify[i].month,
                    ute_msg->historyNotify[i].day);
        }
        else
        {
            sprintf((char*)time_buf, "%02d:%02d", //record_tbl[index].callTime.year,
                    ute_msg->historyNotify[i].hour,
                    ute_msg->historyNotify[i].min);
        }

        char* msg = (char*)ute_msg->historyNotify[i].content;

        compo_cardbox_t *cardbox = compo_cardbox_create(frm,1,1,2,284,173);
        compo_setid(cardbox,COMPO_ID_CARD_FIRST+i);
        compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,141+176*i);

        compo_cardbox_rect_set_location(cardbox,0,message_card[0].rect_x,message_card[0].rect_y,message_card[0].rect_w,message_card[0].rect_h,message_card[0].rect_r);
        compo_cardbox_rect_set_color(cardbox,0, make_color(41,41,41));

        compo_cardbox_icon_set_location(cardbox,0,message_card[0].pic_x,message_card[0].pic_y,message_card[0].pic_w,message_card[0].pic_h);
        compo_cardbox_icon_set(cardbox, 0, func_message_card_get_icon(ute_msg->historyNotify[i].type));

        compo_cardbox_text_set_location(cardbox,0,message_card[0].time_x,message_card[0].time_y,message_card[0].time_w,message_card[0].time_h);
        widget_set_align_center(cardbox->text[0],false);
        widget_text_set_color(cardbox->text[0],make_color(0x94,0x94,0x94));
        compo_cardbox_text_set(cardbox,0,time_buf);
        widget_text_set_right_align(cardbox->text[0],true);

        compo_cardbox_text_set_location(cardbox,1,message_card[0].msg_x,message_card[0].msg_y,message_card[0].msg_w,message_card[0].msg_h);
        widget_set_align_center(cardbox->text[1],false);
        compo_cardbox_text_set(cardbox,1,msg);
        widget_text_set_wordwrap(cardbox->text[1], true);
        widget_text_set_ellipsis(cardbox->text[1], true);
    }

    //创建按钮
    compo_button_t* btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,105+(176*msg_num));
    compo_setid(btn, COMPO_ID_ALL_DEL_BTN);

    //创建删除按钮文本
    compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR_ALL]));
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,105+(176*msg_num),200,62);
    compo_textbox_set(text, i18n[STR_CLEAR_ALL]);

    ab_free(ute_msg);

    return frm;
}


static void func_message_card_init()
{
    f_message_t *f_message = (f_message_t *)func_cb.f_cb;
    f_message->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));

    u8 msg_num = uteModuleNotifyGetTotalNotifyCnt();
    f_message->refresh_data = msg_num;

    uint16_t page_height = msg_num*176+141;
    if(page_height<GUI_SCREEN_HEIGHT)
    {
        page_height = GUI_SCREEN_HEIGHT;
    }

    page_move_info_t info =
    {
        .title_used = true,
        .page_size = page_height,
        .page_count = 1,
        .jump_perc = 0,
        .quick_jump_perc = 3,
        .up_over_perc   = 1,
        .down_over_perc = 1,
    };
    compo_page_move_init(f_message->ptm, func_cb.frm_main->page_body, &info);
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//更新消息卡片
static void func_message_card_update()
{
    f_message_t *f_message = (f_message_t *)func_cb.f_cb;

    if(uteModuleNotifyIsNewNotifyDisplay())
    {
        uteModuleNotifySetNewNotifyFlag(!uteModuleNotifyIsNewNotifyDisplay());
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
}

//获取点击的卡片组件id
static u16 func_message_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<=COMPO_ID_CARD_MAX_LAST; i++)
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
static void func_message_click(void)
{
    u8 id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_ALL_DEL_BTN:            //清除消息
            uteModuleNotifyDelAllHistoryData(true);
            break;
        default:
        {
            point_t pt = ctp_get_sxy();
            u16 compo_id = func_message_card_get_btn_id(pt);
            if (compo_id < COMPO_ID_CARD_FIRST || compo_id > COMPO_ID_CARD_MAX_LAST)
            {
                return;
            }
            compo_id-=1;
            char time_buf[30];
            ute_module_notify_data_t *ute_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
            uteModuleNotifyGetData(ute_msg);
            ute_module_systemtime_time_t time_data;
            if(time_data.year != ute_msg->historyNotify[compo_id].year || time_data.month != ute_msg->historyNotify[compo_id].month)
            {
                sprintf((char*)time_buf, "%04d/%02d/%02d", //record_tbl[index].callTime.year,
                        ute_msg->historyNotify[compo_id].year,
                        ute_msg->historyNotify[compo_id].month,
                        ute_msg->historyNotify[compo_id].day);
            }
            else if(time_data.day > ute_msg->historyNotify[compo_id].day && time_data.month == ute_msg->historyNotify[compo_id].month)
            {
                sprintf((char*)time_buf, "%02d/%02d", //record_tbl[index].callTime.year,
                        ute_msg->historyNotify[compo_id].month,
                        ute_msg->historyNotify[compo_id].day);
            }
            else
            {
                sprintf((char*)time_buf, "%02d:%02d", //record_tbl[index].callTime.year,
                        ute_msg->historyNotify[compo_id].hour,
                        ute_msg->historyNotify[compo_id].min);
            }
            char* msg = (char*)ute_msg->historyNotify[compo_id].content;
            sys_cb.msg_index = ute_msg->historyNotify[compo_id].type;
            int res = msgbox(msg, NULL, time_buf, MSGBOX_MODE_BTN_DELETE, MSGBOX_MSG_TYPE_DETAIL);
            if (res == MSGBOX_RES_DELETE)
            {
                uteModuleNotifySetDisplayIndex(compo_id);
                uteModuleNotifyDelAllHistoryData(false);
            }
            ab_free(ute_msg);
        }
        break;
    }
}

//消息功能事件处理
static void func_message_process(void)
{
    f_message_t *f_message = (f_message_t *)func_cb.f_cb;
    compo_page_move_process(f_message->ptm);
    func_message_card_update();
    func_process();
}

//消息功能消息处理
static void func_message_message(size_msg_t msg)
{
    f_message_t *f_message = (f_message_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_message_click();
            break;

        case MSG_CTP_TOUCH:
            if(uteModuleNotifyGetTotalNotifyCnt())
            {
                compo_page_move_touch_handler(f_message->ptm);
            }
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入消息功能
static void func_message_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_message_t));
    func_cb.frm_main = func_message_form_create();
    func_message_card_init();
}

//退出消息功能
static void func_message_exit(void)
{
    f_message_t *f_msg = (f_message_t *)func_cb.f_cb;
    if (func_cb.left_sta == FUNC_MESSAGE)
    {
        if (sys_cb.refresh_language_flag == false)
        {
            func_cb.last = FUNC_MESSAGE;
            func_cb.left_sta = FUNC_NULL;
            task_stack_init();  //任务堆栈
            latest_task_init(); //最近任务
        }
    }
    else
    {
        func_cb.last = FUNC_MESSAGE;
        func_cb.left_sta = FUNC_NULL;
    }
    func_free(f_msg->ptm);
}

//消息功能
void func_message_info(void)
{
    printf("%s\n", __func__);
    func_message_enter();
    while (func_cb.sta == FUNC_MESSAGE)
    {
        func_message_process();
        func_message_message(msg_dequeue());
    }
    func_message_exit();
}

#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
