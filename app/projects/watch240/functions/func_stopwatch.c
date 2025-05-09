#include "include.h"
#include "func.h"

#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT


#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define NUM_REC_COLOR               make_color(0xff, 0x83, 00)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_RECORD_VIEW = 1,       //查看记录
    COMPO_ID_BTN_RECORD,                //记录当前时间
    COMPO_ID_BTN_AFRESH,                //重新开始
    COMPO_ID_BTN_START_REC,             //开始计时
    //数字文本
    COMPO_ID_NUM_STOPWATCH_TIME,        //当前计时
    COMPO_ID_NUM_STOPWATCH_REC,         //记录数
};

typedef struct f_stopwatch_t_
{
    u8 min;                 //分
    u8 sec;                 //秒
    u16 msec;               //毫秒
} f_stopwatch_t;

typedef struct stopwatch_num_item_t_
{
    u32 res_addr;
    int num_cnt;
    u16 num_id;
    int val;
    s16 x;
    s16 y;
    bool zfill_en;
    bool visible_en;
} stopwatch_num_item_t;



static co_timer_t stopwatch_timer;

//创建秒表窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_stopwatch_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STOP_WATCH]);

    //新建按钮
    u32 res_addr;
    compo_button_t *btn;
    res_addr = sys_cb.stopwatch_sta ? UI_BUF_I330001_PUBLIC_PAUSED_BIN: UI_BUF_I330001_PUBLIC_PLAY_BIN;    //开始/暂停
    btn = compo_button_create_by_image(frm, res_addr);
    compo_setid(btn, COMPO_ID_BTN_START_REC);
    compo_button_set_pos(btn, 120, 240);
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_END_BIN);    //复位
    compo_setid(btn, COMPO_ID_BTN_AFRESH);
    compo_button_set_pos(btn, 46, 240);
    compo_button_set_visible(btn, sys_cb.stopwatch_total_msec > 0);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_JICI_BIN);    //计次
    compo_setid(btn, COMPO_ID_BTN_RECORD);
    compo_button_set_pos(btn, 194, 240);
    compo_button_set_visible(btn, sys_cb.stopwatch_total_msec > 0);
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_STOPWATCH_BG_BIN);    //计次详情
    compo_setid(btn, COMPO_ID_BTN_RECORD_VIEW);
    compo_button_set_pos(btn, 120, 85);
    compo_button_set_visible(btn, sys_cb.stopwatch_rec_cnt > 0);

    //创建数字文本
    compo_textbox_t *txt_num;
    char str_buff[9];
    u8 min, sec;
    u16 msec;
    min = ((sys_cb.stopwatch_total_msec / 1000) % 6000) / 60;
    sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    msec = sys_cb.stopwatch_total_msec % 1000;
    txt_num = compo_textbox_create(frm, 8);     //当前计时
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_set_pos(txt_num, GUI_SCREEN_CENTER_X/4, 110);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_48_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_align_center(txt_num, false);
    txt_num = compo_textbox_create(frm, 2);     //记录数
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_REC);
    compo_textbox_set_location(txt_num, 131, 85, 226, widget_text_get_height());
//    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_22_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d", sys_cb.stopwatch_rec_cnt);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_forecolor(txt_num, NUM_REC_COLOR);
    compo_textbox_set_visible(txt_num, sys_cb.stopwatch_rec_cnt > 0);

    min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    msec = sys_cb.stopwatch_total_msec % 1000;


    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
        compo_button_set_bgimg(btn_start, UI_BUF_I330001_PUBLIC_PLAY_GRAY_BIN);

        min  = 99;
        sec  = 59;
        msec = 999;
//        co_timer_del(&stopwatch_timer);
        //        memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
        //        sys_cb.stopwatch_rec_cnt = 0;
        //获取数字组件的地址
        compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
        memset(str_buff,0,sizeof(str_buff));
        snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
        compo_textbox_set(num_time, str_buff);
    }


    return frm;
}

//50ms秒表毫秒刷新回调函数（10ms误差大）
static void stopwatch_50ms_pro(co_timer_t *timer, void *param)
{
    static u32 real_total_msec;
//    if (sys_cb.stopwatch_sta && !sys_cb.gui_sleep_sta)      //休眠不计时
    if (sys_cb.stopwatch_sta)      //休眠不计时
    {
        if (sys_cb.stopwatch_total_msec == 0)
        {
            real_total_msec = 0;
        }
        real_total_msec += 50;
        sys_cb.stopwatch_total_msec = real_total_msec + get_random(50);    //防止百分秒永远是5的倍数
    }
}


//单击按钮
static void func_stopwatch_button_click(u32 key_flag)
{
    u32 res_addr;
    int id = ID_NULL;
    if (key_flag != 0)
    {
        id = key_flag;
    }
    else
    {
        id = compo_get_button_id();
    }
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    char str_buff[9];

    //获取数字组件的地址
    compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
    compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_t *num_rec = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_REC);
    compo_button_t *btn_afresh = compo_getobj_byid(COMPO_ID_BTN_AFRESH);
    compo_button_t *btn_record1 = compo_getobj_byid(COMPO_ID_BTN_RECORD_VIEW);
    compo_button_t *btn_record = compo_getobj_byid(COMPO_ID_BTN_RECORD);

    switch (id)
    {
        case COMPO_ID_BTN_RECORD_VIEW:
            if (sys_cb.stopwatch_rec_cnt)
            {
                func_cb.sta = FUNC_STOPWATCH_SUB_RECORD;
            }
            break;

        case COMPO_ID_BTN_RECORD:
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)return;
            //插入记录到头部
            if (sys_cb.stopwatch_sta)
            {
                sys_cb.stopwatch_rec_cnt += (sys_cb.stopwatch_rec_cnt < STOPWATCH_REC_NUM_MAX);
                for (u8 i = sys_cb.stopwatch_rec_cnt - 1; i > 0; i--)
                {
                    sys_cb.stopwatch_rec_view[i] = sys_cb.stopwatch_rec_view[i - 1];
                }
                sys_cb.stopwatch_rec_view[0] = sys_cb.stopwatch_total_msec;

                snprintf(str_buff, sizeof(str_buff), "%02d", sys_cb.stopwatch_rec_cnt);
                compo_textbox_set(num_rec, str_buff);
            }
            compo_textbox_set_visible(num_rec, sys_cb.stopwatch_rec_cnt > 0);
//            compo_button_set_visible(btn_record1, sys_cb.stopwatch_sta != 0);
            if(sys_cb.stopwatch_sta)
            {
                compo_button_set_visible(btn_record1, true);
            }

            break;

        case COMPO_ID_BTN_AFRESH:
            if (sys_cb.stopwatch_total_msec > 0)
            {
                sys_cb.stopwatch_sta = 0;
                co_timer_del(&stopwatch_timer);

                f_stopwatch->min = 0;
                f_stopwatch->sec = 0;
                f_stopwatch->msec = 0;

                memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_total_msec = 0;

                compo_textbox_set(num_time, "00:00.00");
                compo_textbox_set(num_rec, "0");
            }
            compo_button_set_visible(btn_record1, false);
            compo_textbox_set_visible(num_rec, false);
            compo_button_set_visible(btn_record, false);
            compo_button_set_visible(btn_afresh, false);
            res_addr = sys_cb.stopwatch_sta ? UI_BUF_I330001_PUBLIC_PAUSED_BIN: UI_BUF_I330001_PUBLIC_PLAY_BIN;
            compo_button_set_bgimg(btn_start, res_addr);
            break;

        case COMPO_ID_BTN_START_REC:
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100) return;

            sys_cb.stopwatch_sta = !sys_cb.stopwatch_sta;
            if (sys_cb.stopwatch_sta && sys_cb.stopwatch_total_msec == 0)
            {
                co_timer_set(&stopwatch_timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, stopwatch_50ms_pro, NULL);
            }
            res_addr = sys_cb.stopwatch_sta ? UI_BUF_I330001_PUBLIC_PAUSED_BIN: UI_BUF_I330001_PUBLIC_PLAY_BIN;
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
            {
                res_addr = UI_BUF_I330001_PUBLIC_PLAY_GRAY_BIN;
                sys_cb.stopwatch_sta = false;
            }
            compo_button_set_bgimg(btn_start, res_addr);
            compo_button_set_visible(btn_afresh, true);
            compo_button_set_visible(btn_record, true);
            break;
    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define NUM_REC_COLOR               make_color(0xff, 0x83, 00)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_RECORD_VIEW = 1,       //查看记录
    COMPO_ID_BTN_RECORD,                //记录当前时间
    COMPO_ID_BTN_AFRESH,                //重新开始
    COMPO_ID_BTN_START_REC,             //开始计时
    //数字文本
    COMPO_ID_NUM_STOPWATCH_TIME,        //当前计时
    COMPO_ID_NUM_STOPWATCH_REC,         //记录数
};

typedef struct f_stopwatch_t_
{
    u8 min;                 //分
    u8 sec;                 //秒
    u16 msec;               //毫秒
} f_stopwatch_t;

typedef struct stopwatch_num_item_t_
{
    u32 res_addr;
    int num_cnt;
    u16 num_id;
    int val;
    s16 x;
    s16 y;
    bool zfill_en;
    bool visible_en;
} stopwatch_num_item_t;

static co_timer_t stopwatch_timer;

//创建秒表窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_stopwatch_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STOP_WATCH]);

    //新建按钮
    u32 res_addr;
    compo_button_t *btn;
    res_addr = sys_cb.stopwatch_sta ? UI_BUF_I332001_PUBLIC_PAUSED_BIG_BIN: UI_BUF_I332001_PUBLIC_PLAY_BIG_00_BIN;    //开始/暂停
    btn = compo_button_create_by_image(frm, res_addr);
    compo_setid(btn, COMPO_ID_BTN_START_REC);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 98/2+230);
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_END_BIN);    //复位
    compo_setid(btn, COMPO_ID_BTN_AFRESH);
    compo_button_set_pos(btn, 64/2+47, 98/2+230);
    compo_button_set_visible(btn, sys_cb.stopwatch_total_msec > 0);

    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_JICI_BIN);    //计次
    compo_setid(btn, COMPO_ID_BTN_RECORD);
    compo_button_set_pos(btn, 64/2+249, 98/2+230);
    compo_button_set_visible(btn, sys_cb.stopwatch_rec_cnt > 0);
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_STOPWATCH_BG_BIN);    //计次详情
    compo_setid(btn, COMPO_ID_BTN_RECORD_VIEW);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 94);
    compo_button_set_visible(btn, sys_cb.stopwatch_rec_cnt > 0);

    //创建数字文本
    compo_textbox_t *txt_num;
    char str_buff[9];
    u8 min, sec;
    u16 msec;
    min = ((sys_cb.stopwatch_total_msec / 1000) % 6000) / 60;
    sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    msec = sys_cb.stopwatch_total_msec % 1000;
    txt_num = compo_textbox_create(frm, 8);     //当前计时
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_set_pos(txt_num, 63, 120);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_64_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_align_center(txt_num, false);
    txt_num = compo_textbox_create(frm, 2);     //记录数
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_REC);
    compo_textbox_set_pos(txt_num, 30/2+180, 91);
//    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_22_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d", sys_cb.stopwatch_rec_cnt);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_forecolor(txt_num, NUM_REC_COLOR);
    compo_textbox_set_visible(txt_num, sys_cb.stopwatch_rec_cnt > 0);

    min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    msec = sys_cb.stopwatch_total_msec % 1000;


    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
        compo_button_set_bgimg(btn_start, UI_BUF_I332001_PUBLIC_PLAY_BIG_01_BIN);

        min  = 99;
        sec  = 59;
        msec = 999;
//        co_timer_del(&stopwatch_timer);
        //        memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
        //        sys_cb.stopwatch_rec_cnt = 0;
        //获取数字组件的地址
        compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
        memset(str_buff,0,sizeof(str_buff));
        snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
        compo_textbox_set(num_time, str_buff);
    }


    return frm;
}

//50ms秒表毫秒刷新回调函数（10ms误差大）
static void stopwatch_50ms_pro(co_timer_t *timer, void *param)
{
    static u32 real_total_msec;
//    if (sys_cb.stopwatch_sta && !sys_cb.gui_sleep_sta)      //休眠不计时
    if (sys_cb.stopwatch_sta)      //休眠不计时
    {
        if (sys_cb.stopwatch_total_msec == 0)
        {
            real_total_msec = 0;
        }
        real_total_msec += 50;
        sys_cb.stopwatch_total_msec = real_total_msec + get_random(50);    //防止百分秒永远是5的倍数
    }
}


//单击按钮
static void func_stopwatch_button_click(u32 key_flag)
{
    u32 res_addr;
    int id = ID_NULL;
    if (key_flag != 0)
    {
        id = key_flag;
    }
    else
    {
        id = compo_get_button_id();
    }
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    char str_buff[9];

    //获取数字组件的地址
    compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
    compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_t *num_rec = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_REC);
    compo_button_t *btn_afresh = compo_getobj_byid(COMPO_ID_BTN_AFRESH);
    compo_button_t *btn_record1 = compo_getobj_byid(COMPO_ID_BTN_RECORD_VIEW);
    compo_button_t *btn_record = compo_getobj_byid(COMPO_ID_BTN_RECORD);

    switch (id)
    {
        case COMPO_ID_BTN_RECORD_VIEW:
            if (sys_cb.stopwatch_rec_cnt)
            {
                func_cb.sta = FUNC_STOPWATCH_SUB_RECORD;
            }
            break;

        case COMPO_ID_BTN_RECORD:
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)return;
            //插入记录到头部
            if (sys_cb.stopwatch_sta)
            {
                sys_cb.stopwatch_rec_cnt += (sys_cb.stopwatch_rec_cnt < STOPWATCH_REC_NUM_MAX);
                for (u8 i = sys_cb.stopwatch_rec_cnt - 1; i > 0; i--)
                {
                    sys_cb.stopwatch_rec_view[i] = sys_cb.stopwatch_rec_view[i - 1];
                }
                sys_cb.stopwatch_rec_view[0] = sys_cb.stopwatch_total_msec;

                snprintf(str_buff, sizeof(str_buff), "%02d", sys_cb.stopwatch_rec_cnt);
                compo_textbox_set(num_rec, str_buff);
            }
            compo_textbox_set_visible(num_rec, sys_cb.stopwatch_rec_cnt > 0);
//            compo_button_set_visible(btn_record1, sys_cb.stopwatch_sta != 0);
            if(sys_cb.stopwatch_sta)
            {
                compo_button_set_visible(btn_record1, true);
            }

            break;

        case COMPO_ID_BTN_AFRESH:
            if (sys_cb.stopwatch_total_msec > 0)
            {
                sys_cb.stopwatch_sta = 0;
                co_timer_del(&stopwatch_timer);

                f_stopwatch->min = 0;
                f_stopwatch->sec = 0;
                f_stopwatch->msec = 0;

                memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_total_msec = 0;

                compo_textbox_set(num_time, "00:00.00");
                compo_textbox_set(num_rec, "0");
            }
            compo_button_set_visible(btn_record1, false);
            compo_textbox_set_visible(num_rec, false);
            compo_button_set_visible(btn_record, false);
            res_addr = sys_cb.stopwatch_sta ? UI_BUF_I332001_PUBLIC_PAUSED_BIG_BIN: UI_BUF_I332001_PUBLIC_PLAY_BIG_00_BIN;
            compo_button_set_bgimg(btn_start, res_addr);
            break;

        case COMPO_ID_BTN_START_REC:
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100) return;

            sys_cb.stopwatch_sta = !sys_cb.stopwatch_sta;
            if (sys_cb.stopwatch_sta && sys_cb.stopwatch_total_msec == 0)
            {
                co_timer_set(&stopwatch_timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, stopwatch_50ms_pro, NULL);
            }
            res_addr = sys_cb.stopwatch_sta ? UI_BUF_I332001_PUBLIC_PAUSED_BIG_BIN: UI_BUF_I332001_PUBLIC_PLAY_BIG_00_BIN;
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
            {
                res_addr = UI_BUF_I332001_PUBLIC_PLAY_BIG_01_BIN;
                sys_cb.stopwatch_sta = false;
            }
            compo_button_set_bgimg(btn_start, res_addr);
            compo_button_set_visible(btn_afresh, true);
            compo_button_set_visible(btn_record, true);
            break;
    }

}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

//组件ID
enum
{
    COMPO_ID_NUM_STOPWATCH_TIME=1,        //当前计时
    COMPO_ID_BTN_RECORD_REST,             //记录数/重新开始
    COMPO_ID_BTN_PAUSE_PLAY,              //暂停/开始
    COMPO_ID_LISTBOX,
    COMPO_ID_LISTBOX_SHAPE,
};

typedef struct f_stopwatch_t_
{
    compo_listbox_t *listbox;
} f_stopwatch_t;

#define TO_BEGIN_MODE_PLAY_PIC       UI_BUF_I335001_2_STOPWATCH_ICON_START_208X52_X16_Y222_BIN
#define SART_MODE_PLAY_PIC           UI_BUF_I335001_2_STOPWATCH_PLAY_BIN
#define PAUSE_MODE_PAUSE_PIC         UI_BUF_I335001_2_STOPWATCH_SUSPENDED_BIN
#define PAUSE_MODE_RESET_PIC         UI_BUF_I335001_2_STOPWATCH_00_102X52_X16_Y222_BIN
#define SART_MODE_COUNT_PIC          UI_BUF_I335001_2_STOPWATCH_TIME_BIN
#define PAUSE_MODE_END_PIC           UI_BUF_I335001_2_STOPWATCH_GRAY_START_BIN

static co_timer_t stopwatch_timer;

static const compo_listbox_item_t tbl_stopwatch_list[STOPWATCH_REC_NUM_MAX+3];

static u32 stopwatch_text1_callback(u32 index)
{
    return COLOR_WHITE;
}

static u32 stopwatch_text2_callback(u32 index)
{
    return COLOR_GRAY;
}
static bool stopwatch_set_text_callback(u32 item_cnt, char* str_txt1, u16 str_txt1_len, char* str_txt2, u16 str_txt2_len, u16 index)
{
    u8 min = 0;
    u8 sec = 0;
    u16 msec = 0;
    if (index < item_cnt && index < sys_cb.stopwatch_rec_cnt)
    {
        min = ((sys_cb.stopwatch_rec_view[index] / 1000) / 60) % 100;
        sec = (sys_cb.stopwatch_rec_view[index] / 1000) % 60;
        msec = sys_cb.stopwatch_rec_view[index] % 1000;

        sprintf(str_txt1, "  %02d", index+1);
        sprintf(str_txt2, "%02d:%02d.%02d  ", min, sec, msec / 10);

        return true;
    }

    return false;
}

//创建秒表窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_stopwatch_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE );
    compo_form_set_title(frm, i18n[STR_STOP_WATCH]);

    u8 mode = 0;
    bool end_flag = false;

    u8 min, sec;
    u16 msec;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min = 99;
        sec = 59;
        msec = 99;
        end_flag = true;
    }
    else
    {
        min = ((sys_cb.stopwatch_total_msec / 1000) % 6000) / 60;
        sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
        msec = sys_cb.stopwatch_total_msec % 1000;
    }

    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_STOPWATCH_RECORD);
    compo_listbox_set_location(listbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+5,GUI_SCREEN_WIDTH,100);
    compo_listbox_set(listbox, tbl_stopwatch_list, sys_cb.stopwatch_rec_cnt+2);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_bgimg(listbox, UI_BUF_I335001_2_STOPWATCH_BG_240X34_X0_Y114_BIN);
    compo_listbox_set_text_modify_by_idx_callback2(listbox, stopwatch_set_text_callback);
    compo_listbox_set_text1_color_callback(listbox, stopwatch_text1_callback);
    compo_listbox_set_text2_color_callback(listbox, stopwatch_text2_callback);
    compo_listbox_set_focus_byidx(listbox, sys_cb.stopwatch_rec_cnt+1);
    compo_listbox_update(listbox);

    compo_shape_t *shape = compo_shape_create_for_page(frm, frm->page_body, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+5-51,230,1);
    compo_shape_set_color(shape,make_color(51,51,51));
    compo_setid(shape,COMPO_ID_LISTBOX_SHAPE);
    compo_shape_set_visible(shape,sys_cb.stopwatch_rec_cnt ? true : false);

    //创建数字文本
    compo_textbox_t *txt_num;
    char str_buff[10];
    txt_num = compo_textbox_create(frm, 8);     //当前计时
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_set_pos(txt_num, GUI_SCREEN_CENTER_X/4, sys_cb.stopwatch_rec_cnt ? 40 : 110);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_48_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_align_center(txt_num, false);

    compo_button_t *btn;
    u32 res_addr = (sys_cb.stopwatch_total_msec == 0) ? TO_BEGIN_MODE_PLAY_PIC : (end_flag ? PAUSE_MODE_END_PIC : sys_cb.stopwatch_sta ? PAUSE_MODE_PAUSE_PIC : SART_MODE_PLAY_PIC);
    btn = compo_button_create_by_image(frm, res_addr);//暂停/开始
    compo_setid(btn, COMPO_ID_BTN_PAUSE_PLAY);
    compo_button_set_pos(btn, (sys_cb.stopwatch_total_msec == 0) ? GUI_SCREEN_CENTER_X : GUI_SCREEN_WIDTH*3/4-5,
                         GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN).hei/2 - 20);

    res_addr = (sys_cb.stopwatch_sta ? SART_MODE_COUNT_PIC : PAUSE_MODE_RESET_PIC);//复位/计次
    btn = compo_button_create_page_by_image(frm,frm->page,res_addr);
    compo_setid(btn, COMPO_ID_BTN_RECORD_REST);
    compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN).hei/2 - 20);
    compo_button_set_visible(btn,!(sys_cb.stopwatch_total_msec == 0));
    return frm;
}

//50ms秒表毫秒刷新回调函数（10ms误差大）
static void stopwatch_50ms_pro(co_timer_t *timer, void *param)
{
    static u32 real_total_msec;
    if (sys_cb.stopwatch_sta)      //休眠不计时
    {
        if (sys_cb.stopwatch_total_msec == 0)
        {
            real_total_msec = 0;
        }
        real_total_msec += 50;
        sys_cb.stopwatch_total_msec = real_total_msec + get_random(50);    //防止百分秒永远是5的倍数
    }
}


//单击按钮
static void func_stopwatch_button_click(u32 key_flag)
{
    int id = ID_NULL;
    if (key_flag != 0)
    {
        id = key_flag;
    }
    else
    {
        id = compo_get_button_id();
    }

    if (id == 0)
    {
        return;
    }

    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PAUSE_PLAY);
    compo_button_t *btn_rest = compo_getobj_byid(COMPO_ID_BTN_RECORD_REST);
    compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
    compo_listbox_t *listbox  = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_shape_t   *shape    = compo_getobj_byid(COMPO_ID_LISTBOX_SHAPE);
    switch(id)
    {
        case COMPO_ID_BTN_RECORD_REST://记录数/重新开始
            if(sys_cb.stopwatch_sta && (sys_cb.stopwatch_total_msec / 1000 / 60 <= 99)) //计次
            {
                if(sys_cb.stopwatch_rec_cnt < STOPWATCH_REC_NUM_MAX)
                {
                    sys_cb.stopwatch_rec_cnt ++;
                    sys_cb.stopwatch_rec_view[sys_cb.stopwatch_rec_cnt-1] = sys_cb.stopwatch_total_msec;
                    compo_listbox_set(listbox, tbl_stopwatch_list, sys_cb.stopwatch_rec_cnt+2);
                    compo_listbox_set_text_modify_by_idx_callback2(listbox, stopwatch_set_text_callback);
                    compo_listbox_set_focus_byidx(listbox, sys_cb.stopwatch_rec_cnt+2);
                    compo_listbox_move_init_modify(listbox, 85, compo_listbox_gety_byidx(listbox,sys_cb.stopwatch_rec_cnt+1)+34);
                    compo_listbox_update(listbox);
                }
            }
            else//复位
            {
                compo_textbox_set(num_time, "00:00.00");
                co_timer_del(&stopwatch_timer);
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_total_msec = 0;
                compo_button_set_bgimg(btn_play,TO_BEGIN_MODE_PLAY_PIC);
                compo_button_set_visible(btn_rest,false);
                compo_button_set_pos(btn_play, GUI_SCREEN_WIDTH/2,GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN).hei/2 - 20);

                compo_listbox_set(listbox, tbl_stopwatch_list, sys_cb.stopwatch_rec_cnt+2);
                compo_listbox_set_text_modify_by_idx_callback2(listbox, stopwatch_set_text_callback);
                compo_listbox_set_focus_byidx(listbox, sys_cb.stopwatch_rec_cnt+1);
                compo_listbox_move_init_modify(listbox, 85, compo_listbox_gety_byidx(listbox,sys_cb.stopwatch_rec_cnt+1)+34);
                compo_listbox_update(listbox);
            }
            compo_shape_set_visible(shape,sys_cb.stopwatch_rec_cnt ? true : false);
            break;
        case COMPO_ID_BTN_PAUSE_PLAY://暂停/开始

            if (sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
            {
                return;
            }

            if(sys_cb.stopwatch_total_msec == 0)
            {
                sys_cb.stopwatch_sta = 1;
                compo_button_set_visible(btn_rest,true);
                compo_button_set_pos(btn_play,  GUI_SCREEN_WIDTH*3/4-5, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN).hei/2 - 20);
            }
            else
            {
                sys_cb.stopwatch_sta ^= 1;
            }
            if (sys_cb.stopwatch_sta && sys_cb.stopwatch_total_msec == 0)
            {
                co_timer_set(&stopwatch_timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, stopwatch_50ms_pro, NULL);
            }
            u32 res_addr = sys_cb.stopwatch_sta ? PAUSE_MODE_PAUSE_PIC : SART_MODE_PLAY_PIC;
            compo_button_set_bgimg(btn_play,res_addr);
            res_addr = sys_cb.stopwatch_sta ? SART_MODE_COUNT_PIC : PAUSE_MODE_RESET_PIC;
            compo_button_set_bgimg(btn_rest,res_addr);
            break;
        default :
            break;
    }

    compo_textbox_set_pos(num_time, GUI_SCREEN_CENTER_X/4, sys_cb.stopwatch_rec_cnt ? 40 : 110);
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

//组件ID
enum
{
    COMPO_ID_NUM_STOPWATCH_TIME=1,        //当前计时
    COMPO_ID_BTN_RECORD_REST,             //记录数/重新开始
    COMPO_ID_BTN_PAUSE_PLAY,              //暂停/开始
    COMPO_ID_LISTBOX,
};

typedef struct f_stopwatch_t_
{
    compo_listbox_t *listbox;
} f_stopwatch_t;

#define TO_BEGIN_MODE_PLAY_PIC       UI_BUF_I338001_STOPWATCH_STOPWATCH_START_BIN
#define SART_MODE_PLAY_PIC           UI_BUF_I338001_STOPWATCH_YELLOW_START_BIN
#define PAUSE_MODE_PAUSE_PIC         UI_BUF_I338001_STOPWATCH_YELLOW_STOP_BIN
#define PAUSE_MODE_RESET_PIC         UI_BUF_I338001_STOPWATCH_GRAY_AGIN_BIN
#define SART_MODE_COUNT_PIC          UI_BUF_I338001_STOPWATCH_STOPWATCH_BIN
#define PAUSE_MODE_END_PIC           UI_BUF_I338001_STOPWATCH_GRAY_YELLOW_START_BIN

static co_timer_t stopwatch_timer;

static const compo_listbox_item_t tbl_stopwatch_list[STOPWATCH_REC_NUM_MAX+4];

static u32 stopwatch_text1_callback(u32 index)
{
    return COLOR_WHITE;
}

static u32 stopwatch_text2_callback(u32 index)
{
    return COLOR_GRAY;
}
static bool stopwatch_set_text_callback(u32 item_cnt, char* str_txt1, u16 str_txt1_len, char* str_txt2, u16 str_txt2_len, u16 index)
{
    u8 min = 0;
    u8 sec = 0;
    u16 msec = 0;
    if (index < item_cnt && index < sys_cb.stopwatch_rec_cnt)
    {

        min = ((sys_cb.stopwatch_rec_view[index] / 1000) / 60) % 100;
        sec = (sys_cb.stopwatch_rec_view[index] / 1000) % 60;
        msec = sys_cb.stopwatch_rec_view[index] % 1000;
        // printf("index:%d  cnt:%d  min:%d sec:%d msec:%d\n",index,sys_cb.stopwatch_rec_cnt,min,sec,msec);

        sprintf(str_txt1, "  %02d", index+1);
        sprintf(str_txt2, "%02d:%02d.%02d  ", min, sec, msec / 10);

        return true;
    }

    return false;
}

//创建秒表窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_stopwatch_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE );
    compo_form_set_title(frm, i18n[STR_STOP_WATCH]);

    u8 mode = 0;
    bool end_flag = false;

    u8 min, sec;
    u16 msec;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min = 99;
        sec = 59;
        msec = 99;
        end_flag = true;
    }
    else
    {
        min = ((sys_cb.stopwatch_total_msec / 1000) % 6000) / 60;
        sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
        msec = sys_cb.stopwatch_total_msec % 1000;
    }

    // compo_shape_t *shape = compo_shape_create_for_page(frm, frm->page_body, COMPO_SHAPE_TYPE_RECTANGLE);
    // compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+5,GUI_SCREEN_WIDTH,120);
    // compo_shape_set_color(shape,COLOR_BLUE);

    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_STOPWATCH_RECORD);
    compo_listbox_set_location(listbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+5,GUI_SCREEN_WIDTH,80);
    compo_listbox_set(listbox, tbl_stopwatch_list, sys_cb.stopwatch_rec_cnt+4);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_bgimg(listbox, UI_BUF_I338001_STOPWATCH_LIST_BG_BIN);
    compo_listbox_set_text_modify_by_idx_callback2(listbox, stopwatch_set_text_callback);
    compo_listbox_set_text1_color_callback(listbox, stopwatch_text1_callback);
    compo_listbox_set_text2_color_callback(listbox, stopwatch_text2_callback);
    compo_listbox_set_focus_byidx(listbox, sys_cb.stopwatch_rec_cnt+2);
    compo_listbox_update(listbox);

    //创建数字文本
    compo_textbox_t *txt_num;
    char str_buff[10];
    txt_num = compo_textbox_create(frm, 8);     //当前计时
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_set_pos(txt_num, 63, sys_cb.stopwatch_rec_cnt ? 86-18 : 140-18);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_64_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_align_center(txt_num, false);

    compo_button_t *btn;
    u32 res_addr = (sys_cb.stopwatch_total_msec == 0) ? TO_BEGIN_MODE_PLAY_PIC : (end_flag ? PAUSE_MODE_END_PIC : sys_cb.stopwatch_sta ? PAUSE_MODE_PAUSE_PIC : SART_MODE_PLAY_PIC);
    btn = compo_button_create_by_image(frm, res_addr);//暂停/开始
    compo_setid(btn, COMPO_ID_BTN_PAUSE_PLAY);
    compo_button_set_pos(btn, (sys_cb.stopwatch_total_msec == 0) ? GUI_SCREEN_CENTER_X : 80/2+206,80/2+243);

    res_addr = (sys_cb.stopwatch_sta ? SART_MODE_COUNT_PIC : PAUSE_MODE_RESET_PIC);//复位/计次
    btn = compo_button_create_page_by_image(frm,frm->page,res_addr);
    compo_setid(btn, COMPO_ID_BTN_RECORD_REST);
    compo_button_set_pos(btn,80/2+74,80/2+243);
    compo_button_set_visible(btn,!(sys_cb.stopwatch_total_msec == 0));
    return frm;
}

//50ms秒表毫秒刷新回调函数（10ms误差大）
static void stopwatch_50ms_pro(co_timer_t *timer, void *param)
{
    static u32 real_total_msec;
    if (sys_cb.stopwatch_sta)      //休眠不计时
    {
        if (sys_cb.stopwatch_total_msec == 0)
        {
            real_total_msec = 0;
        }
        real_total_msec += 50;
        sys_cb.stopwatch_total_msec = real_total_msec + get_random(50);    //防止百分秒永远是5的倍数
    }
}


//单击按钮
static void func_stopwatch_button_click(u32 key_flag)
{
    int id = ID_NULL;
    if (key_flag != 0)
    {
        id = key_flag;
    }
    else
    {
        id = compo_get_button_id();
    }

    if (id == 0)
    {
        return;
    }

    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PAUSE_PLAY);
    compo_button_t *btn_rest = compo_getobj_byid(COMPO_ID_BTN_RECORD_REST);
    compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
    compo_listbox_t *listbox  = compo_getobj_byid(COMPO_ID_LISTBOX);
    switch(id)
    {
        case COMPO_ID_BTN_RECORD_REST://记录数/重新开始
            if(sys_cb.stopwatch_sta && (sys_cb.stopwatch_total_msec / 1000 / 60 <= 99)) //计次
            {
                if(sys_cb.stopwatch_rec_cnt < STOPWATCH_REC_NUM_MAX)
                {
                    sys_cb.stopwatch_rec_cnt ++;
                    // printf("cnt:%d\n",sys_cb.stopwatch_rec_cnt);
                    sys_cb.stopwatch_rec_view[sys_cb.stopwatch_rec_cnt-1] = sys_cb.stopwatch_total_msec;
                    // printf("yes_data\n");
                    compo_listbox_set(listbox, tbl_stopwatch_list, sys_cb.stopwatch_rec_cnt+4);
                    compo_listbox_set_text_modify_by_idx_callback2(listbox, stopwatch_set_text_callback);
                    compo_listbox_set_focus_byidx(listbox, sys_cb.stopwatch_rec_cnt+2);
                    compo_listbox_move_init_modify(listbox, 140, compo_listbox_gety_byidx(listbox,sys_cb.stopwatch_rec_cnt+3));
                    compo_listbox_update(listbox);
                }
            }
            else//复位
            {
                compo_textbox_set(num_time, "00:00.00");
                co_timer_del(&stopwatch_timer);
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_total_msec = 0;
                compo_button_set_bgimg(btn_play,TO_BEGIN_MODE_PLAY_PIC);
                compo_button_set_visible(btn_rest,false);
                compo_button_set_pos(btn_play, GUI_SCREEN_WIDTH/2,80/2+224);

                compo_listbox_set(listbox, tbl_stopwatch_list, sys_cb.stopwatch_rec_cnt+4);
                compo_listbox_set_text_modify_by_idx_callback2(listbox, stopwatch_set_text_callback);
                compo_listbox_set_focus_byidx(listbox, sys_cb.stopwatch_rec_cnt+2);
                compo_listbox_move_init_modify(listbox, 140, compo_listbox_gety_byidx(listbox,sys_cb.stopwatch_rec_cnt+3));
                compo_listbox_update(listbox);
            }
            break;
        case COMPO_ID_BTN_PAUSE_PLAY://暂停/开始

            if (sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
            {
                return;
            }

            if(sys_cb.stopwatch_total_msec == 0)
            {
                sys_cb.stopwatch_sta = 1;
                compo_button_set_visible(btn_rest,true);
                compo_button_set_pos(btn_play,   80/2+206,80/2+243);
            }
            else
            {
                sys_cb.stopwatch_sta ^= 1;
            }
            if (sys_cb.stopwatch_sta && sys_cb.stopwatch_total_msec == 0)
            {
                co_timer_set(&stopwatch_timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, stopwatch_50ms_pro, NULL);
            }
            u32 res_addr = sys_cb.stopwatch_sta ? PAUSE_MODE_PAUSE_PIC : SART_MODE_PLAY_PIC;
            compo_button_set_bgimg(btn_play,res_addr);
            res_addr = sys_cb.stopwatch_sta ? SART_MODE_COUNT_PIC : PAUSE_MODE_RESET_PIC;
            compo_button_set_bgimg(btn_rest,res_addr);
            break;
        default :
            break;
    }

    compo_textbox_set_pos(num_time, 63, sys_cb.stopwatch_rec_cnt ? 86-18 : 140-18);
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

#define NUM_REC_COLOR               make_color(0xff, 0x83, 00)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_RECORD_VIEW = 1,       //查看记录
    COMPO_ID_BTN_RECORD,                //记录当前时间
    COMPO_ID_BTN_AFRESH,                //重新开始
    COMPO_ID_BTN_START_REC,             //开始计时
    //数字文本
    COMPO_ID_NUM_STOPWATCH_TIME,        //当前计时
    COMPO_ID_NUM_STOPWATCH_REC,         //记录数
};

typedef struct f_stopwatch_t_
{
    u8 min;                 //分
    u8 sec;                 //秒
    u16 msec;               //毫秒
} f_stopwatch_t;

typedef struct stopwatch_num_item_t_
{
    u32 res_addr;
    int num_cnt;
    u16 num_id;
    int val;
    s16 x;
    s16 y;
    bool zfill_en;
    bool visible_en;
} stopwatch_num_item_t;

static co_timer_t stopwatch_timer;

//创建秒表窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_stopwatch_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STOP_WATCH]);

    //新建按钮
    u32 res_addr;
    compo_button_t *btn;
    res_addr = sys_cb.stopwatch_sta ? UI_BUF_I340001_PUBLIC_PAUSED_BIG_BIN: UI_BUF_I340001_PUBLIC_PLAY_BIG_00_BIN;    //开始/暂停
    btn = compo_button_create_by_image(frm, res_addr);
    compo_setid(btn, COMPO_ID_BTN_START_REC);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 98/2+230);
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_END_BIN);    //复位
    compo_setid(btn, COMPO_ID_BTN_AFRESH);
    compo_button_set_pos(btn, 76/2+34, 76/2+241);
    compo_button_set_visible(btn, sys_cb.stopwatch_total_msec > 0);

    btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_JICI_BIN);    //计次
    compo_setid(btn, COMPO_ID_BTN_RECORD);
    compo_button_set_pos(btn, 76/2+250, 76/2+241);
    compo_button_set_visible(btn, (sys_cb.stopwatch_sta || sys_cb.stopwatch_rec_cnt) > 0);  //秒表在运行的时候重新进入，计次按钮还是要显示出来
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_STOPWATCH_BG_BIN);    //计次详情
    compo_setid(btn, COMPO_ID_BTN_RECORD_VIEW);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 94);
    compo_button_set_visible(btn, sys_cb.stopwatch_rec_cnt > 0);

    //创建数字文本
    compo_textbox_t *txt_num;
    char str_buff[9];
    u8 min, sec;
    u16 msec;
    min = ((sys_cb.stopwatch_total_msec / 1000) % 6000) / 60;
    sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    msec = sys_cb.stopwatch_total_msec % 1000;
    txt_num = compo_textbox_create(frm, 8);     //当前计时
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_set_pos(txt_num, 63, 120);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_64_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_align_center(txt_num, false);
    txt_num = compo_textbox_create(frm, 2);     //记录数
    compo_setid(txt_num, COMPO_ID_NUM_STOPWATCH_REC);
    compo_textbox_set_pos(txt_num, 30/2+180, 91);
//    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_22_BIN);
    snprintf(str_buff, sizeof(str_buff), "%02d", sys_cb.stopwatch_rec_cnt);
    compo_textbox_set(txt_num, str_buff);
    compo_textbox_set_forecolor(txt_num, NUM_REC_COLOR);
    compo_textbox_set_visible(txt_num, sys_cb.stopwatch_rec_cnt > 0);

    min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    msec = sys_cb.stopwatch_total_msec % 1000;


    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
        compo_button_set_bgimg(btn_start, UI_BUF_I340001_PUBLIC_PLAY_BIG_01_BIN);

        min  = 99;
        sec  = 59;
        msec = 999;
//        co_timer_del(&stopwatch_timer);
        //        memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
        //        sys_cb.stopwatch_rec_cnt = 0;
        //获取数字组件的地址
        compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
        memset(str_buff,0,sizeof(str_buff));
        snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
        compo_textbox_set(num_time, str_buff);
    }


    return frm;
}

//50ms秒表毫秒刷新回调函数（10ms误差大）
static void stopwatch_50ms_pro(co_timer_t *timer, void *param)
{
    static u32 real_total_msec;
//    if (sys_cb.stopwatch_sta && !sys_cb.gui_sleep_sta)      //休眠不计时
    if (sys_cb.stopwatch_sta)      //休眠不计时
    {
        if (sys_cb.stopwatch_total_msec == 0)
        {
            real_total_msec = 0;
        }
        real_total_msec += 50;
        sys_cb.stopwatch_total_msec = real_total_msec + get_random(50);    //防止百分秒永远是5的倍数
    }
}


//单击按钮
static void func_stopwatch_button_click(u32 key_flag)
{
    u32 res_addr;
    int id = ID_NULL;
    if (key_flag != 0)
    {
        id = key_flag;
    }
    else
    {
        id = compo_get_button_id();
    }
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    char str_buff[9];

    //获取数字组件的地址
    compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
    compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
    compo_textbox_t *num_rec = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_REC);
    compo_button_t *btn_afresh = compo_getobj_byid(COMPO_ID_BTN_AFRESH);
    compo_button_t *btn_record1 = compo_getobj_byid(COMPO_ID_BTN_RECORD_VIEW);
    compo_button_t *btn_record = compo_getobj_byid(COMPO_ID_BTN_RECORD);

    switch (id)
    {
        case COMPO_ID_BTN_RECORD_VIEW:
            if (sys_cb.stopwatch_rec_cnt)
            {
                func_cb.sta = FUNC_STOPWATCH_SUB_RECORD;
            }
            break;

        case COMPO_ID_BTN_RECORD:
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)return;
            //插入记录到头部
            if (sys_cb.stopwatch_sta)
            {
                sys_cb.stopwatch_rec_cnt += (sys_cb.stopwatch_rec_cnt < STOPWATCH_REC_NUM_MAX);
                for (u8 i = sys_cb.stopwatch_rec_cnt - 1; i > 0; i--)
                {
                    sys_cb.stopwatch_rec_view[i] = sys_cb.stopwatch_rec_view[i - 1];
                }
                sys_cb.stopwatch_rec_view[0] = sys_cb.stopwatch_total_msec;

                snprintf(str_buff, sizeof(str_buff), "%02d", sys_cb.stopwatch_rec_cnt);
                compo_textbox_set(num_rec, str_buff);
            }
            compo_textbox_set_visible(num_rec, sys_cb.stopwatch_rec_cnt > 0);
//            compo_button_set_visible(btn_record1, sys_cb.stopwatch_sta != 0);
            if(sys_cb.stopwatch_sta)
            {
                compo_button_set_visible(btn_record1, true);
            }

            break;

        case COMPO_ID_BTN_AFRESH:
            if (sys_cb.stopwatch_total_msec > 0)
            {
                sys_cb.stopwatch_sta = 0;
                co_timer_del(&stopwatch_timer);

                f_stopwatch->min = 0;
                f_stopwatch->sec = 0;
                f_stopwatch->msec = 0;

                memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_rec_cnt = 0;
                sys_cb.stopwatch_total_msec = 0;

                compo_textbox_set(num_time, "00:00.00");
                compo_textbox_set(num_rec, "0");
            }
            compo_button_set_visible(btn_record1, false);
            compo_textbox_set_visible(num_rec, false);
            compo_button_set_visible(btn_record, false);
            res_addr = sys_cb.stopwatch_sta ? UI_BUF_I340001_PUBLIC_PAUSED_BIG_BIN: UI_BUF_I340001_PUBLIC_PLAY_BIG_00_BIN;
            compo_button_set_bgimg(btn_start, res_addr);
            break;

        case COMPO_ID_BTN_START_REC:
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100) return;

            sys_cb.stopwatch_sta = !sys_cb.stopwatch_sta;
            if (sys_cb.stopwatch_sta && sys_cb.stopwatch_total_msec == 0)
            {
                co_timer_set(&stopwatch_timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, stopwatch_50ms_pro, NULL);
            }
            res_addr = sys_cb.stopwatch_sta ? UI_BUF_I340001_PUBLIC_PAUSED_BIG_BIN: UI_BUF_I340001_PUBLIC_PLAY_BIG_00_BIN;
            if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
            {
                res_addr = UI_BUF_I340001_PUBLIC_PLAY_BIG_01_BIN;
                sys_cb.stopwatch_sta = false;
            }
            compo_button_set_bgimg(btn_start, res_addr);
            compo_button_set_visible(btn_afresh, true);
            compo_button_set_visible(btn_record, true);
            break;
    }

}

#else

#define NUM_REC_COLOR               make_color(0xff, 0x83, 00)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_RECORD_VIEW = 1,       //查看记录
    COMPO_ID_BTN_RECORD,                //记录当前时间
    COMPO_ID_BTN_AFRESH,                //重新开始
    COMPO_ID_BTN_START_REC,             //开始计时
    //数字文本
    COMPO_ID_NUM_STOPWATCH_TIME,        //当前计时
    COMPO_ID_NUM_STOPWATCH_REC,         //记录数
};

typedef struct f_stopwatch_t_
{
    u8 min;                 //分
    u8 sec;                 //秒
    u16 msec;               //毫秒
} f_stopwatch_t;

typedef struct stopwatch_num_item_t_
{
    u32 res_addr;
    int num_cnt;
    u16 num_id;
    int val;
    s16 x;
    s16 y;
    bool zfill_en;
    bool visible_en;
} stopwatch_num_item_t;

static co_timer_t stopwatch_timer;

//创建秒表窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_stopwatch_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//秒表功能事件处理
static void func_stopwatch_process(void)
{
#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    if (sys_cb.stopwatch_sta)
    {

        u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
        u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
        u16 msec = sys_cb.stopwatch_total_msec % 1000;
        char str_buff[9];

        if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
        {
//            printf("sec=%d\n",sys_cb.stopwatch_total_msec);
            compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
            compo_button_set_bgimg(btn_start, UI_BUF_I330001_PUBLIC_PLAY_GRAY_BIN);

            min  = 99;
            sec  = 59;
            msec = 999;
            co_timer_del(&stopwatch_timer);
//            memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
//            sys_cb.stopwatch_rec_cnt = 0;
//            sys_cb.stopwatch_total_msec = 0;
        }
        else
        {
            reset_sleep_delay_all();        //计时的时候不许休眠
        }

        f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;

        //获取数字组件的地址
        compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);

        if (f_stopwatch->msec != msec || f_stopwatch->sec != sec || f_stopwatch->min != min)
        {
            f_stopwatch->min = min;
            f_stopwatch->sec = sec;
            f_stopwatch->msec = msec;

            snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
            compo_textbox_set(num_time, str_buff);
        }
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    if (sys_cb.stopwatch_sta)
    {
        reset_sleep_delay_all();        //计时的时候不许休眠
        u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
        u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
        u16 msec = sys_cb.stopwatch_total_msec % 1000;
        char str_buff[9];

        if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
        {
//            printf("sec=%d\n",sys_cb.stopwatch_total_msec);
            compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
            compo_button_set_bgimg(btn_start, UI_BUF_I332001_PUBLIC_PLAY_BIG_01_BIN);

            min  = 99;
            sec  = 59;
            msec = 999;
            co_timer_del(&stopwatch_timer);
//            memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
//            sys_cb.stopwatch_rec_cnt = 0;
//            sys_cb.stopwatch_total_msec = 0;
        }

        f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;

        //获取数字组件的地址
        compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);

        if (f_stopwatch->msec != msec || f_stopwatch->sec != sec || f_stopwatch->min != min)
        {
            f_stopwatch->min = min;
            f_stopwatch->sec = sec;
            f_stopwatch->msec = msec;

            snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
            compo_textbox_set(num_time, str_buff);
        }
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    if (sys_cb.stopwatch_sta)
    {
        reset_sleep_delay_all();        //计时的时候不许休眠
        u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
        u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
        u16 msec = sys_cb.stopwatch_total_msec % 1000;
        char str_buff[9];

        if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
        {
//            printf("sec=%d\n",sys_cb.stopwatch_total_msec);
            compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START_REC);
            compo_button_set_bgimg(btn_start, UI_BUF_I340001_PUBLIC_PLAY_BIG_01_BIN);

            min  = 99;
            sec  = 59;
            msec = 999;
            co_timer_del(&stopwatch_timer);
//            memset(sys_cb.stopwatch_rec_view, 0, sizeof(sys_cb.stopwatch_rec_view));
//            sys_cb.stopwatch_rec_cnt = 0;
//            sys_cb.stopwatch_total_msec = 0;
        }

        f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;

        //获取数字组件的地址
        compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);

        if (f_stopwatch->msec != msec || f_stopwatch->sec != sec || f_stopwatch->min != min)
        {
            f_stopwatch->min = min;
            f_stopwatch->sec = sec;
            f_stopwatch->msec = msec;

            snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
            compo_textbox_set(num_time, str_buff);
        }
    }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    compo_textbox_t *num_time = compo_getobj_byid(COMPO_ID_NUM_STOPWATCH_TIME);
    if (sys_cb.stopwatch_sta)
    {
        u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
        u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
        u16 msec = sys_cb.stopwatch_total_msec % 1000;
        char str_buff[10];
        if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
        {
            min  = 99;
            sec  = 59;
            msec = 999;
            co_timer_del(&stopwatch_timer);
            sys_cb.stopwatch_sta = false;
            compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PAUSE_PLAY);
            compo_button_t *btn_rest = compo_getobj_byid(COMPO_ID_BTN_RECORD_REST);
            compo_button_set_bgimg(btn_play,PAUSE_MODE_END_PIC);
            compo_button_set_bgimg(btn_rest,PAUSE_MODE_RESET_PIC);
        }
        else
        {
            reset_sleep_delay_all();        //计时的时候不许休眠
        }

        snprintf(str_buff, sizeof(str_buff), "%02d:%02d.%02d", min, sec, msec / 10);
        compo_textbox_set(num_time, str_buff);

    }

    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    if(f_stopwatch->listbox)
    {
        compo_listbox_move(f_stopwatch->listbox);
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//秒表功能消息处理
static void func_stopwatch_message(size_msg_t msg)
{
#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
//            func_stopwatch_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_stopwatch_button_click(0);
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case KU_BACK:
            func_stopwatch_button_click(COMPO_ID_BTN_START_REC);
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_stopwatch->listbox;
    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_stopwatch_button_click(0);
            break;
        case KU_BACK:
            func_stopwatch_button_click(COMPO_ID_BTN_PAUSE_PLAY);
            break;
        default:
            func_message(msg);
            break;
    }
#elif  GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
//            func_stopwatch_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_stopwatch_button_click(0);
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case KU_BACK:
            func_stopwatch_button_click(COMPO_ID_BTN_START_REC);
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            break;

        default:
            func_message(msg);
            break;
    }

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//进入秒表功能
static void func_stopwatch_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_stopwatch_t));
    func_cb.frm_main = func_stopwatch_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    f_stopwatch->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_stopwatch->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    // compo_listbox_move_init(listbox);
    compo_listbox_move_init_modify(listbox, 85, compo_listbox_gety_byidx(listbox,sys_cb.stopwatch_rec_cnt+1)+34);
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    f_stopwatch->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_stopwatch->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    // compo_listbox_move_init(listbox);
    compo_listbox_move_init_modify(listbox, 140, compo_listbox_gety_byidx(listbox,sys_cb.stopwatch_rec_cnt+3));
#endif
}

//退出秒表功能
static void func_stopwatch_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_STOPWATCH;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_stopwatch_t *f_stopwatch = (f_stopwatch_t *)func_cb.f_cb;
    if(f_stopwatch->listbox != NULL)
    {
        func_free(f_stopwatch->listbox->mcb);
    }
#endif
}

//秒表功能
void func_stopwatch(void)
{
    printf("%s\n", __func__);
    func_stopwatch_enter();
    while (func_cb.sta == FUNC_STOPWATCH)
    {
        func_stopwatch_process();
        func_stopwatch_message(msg_dequeue());
    }
    func_stopwatch_exit();
}
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
