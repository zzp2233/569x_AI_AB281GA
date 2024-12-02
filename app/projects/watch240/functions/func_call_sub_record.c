#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_module_charencode.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define CALL_LIST_CNT                       ((int)(sizeof(tbl_record_list) / sizeof(tbl_record_list[0])))
#define CALL_TYPE_ICON_AREA                 (gui_image_get_size(UI_BUF_I330001_CALL_CALL_LOG_INCOMING_CALL_BIN))                  //通话记录类型图标显示区域, 如果图片输入0表示图标所占区域为0

enum
{
    COMPO_ID_LISTBOX = 1,
    COMPO_ID_COVER_PIC,
    COMPO_ID_COVER_TXT,
};

typedef struct f_call_list_t_
{
    compo_listbox_t *listbox;
    u32 tick;

} f_call_list_t;

static const compo_listbox_item_t tbl_record_list[UTE_MODULE_CALL_RECORDS_MAX_COUNT] = {0};
static ute_module_call_records_t* record_tbl = NULL;            //通话记录数据
static u16 record_cnt = 0;                                       //通话记录个数

//设置已接、未接电话颜色
static u32 call_record_set_text1_callback(u32 index)
{
    switch (record_tbl[index].callType)
    {
        case CALL_MISSED:                   //未接电话时要显示的图标
            return  make_color(255, 69, 31);

        case CALL_RECEIVED:                 //接听电话时要显示的图标
            return  COLOR_WHITE;

        case CALL_DIALED:                   //拨出电话时要显示的图标
            return  COLOR_WHITE;

        default:
            break;
    }

    return COLOR_WHITE;
}

//单个图标设置回调函数
static u32 call_record_set_icon_callback(u32 index)
{
    switch (record_tbl[index].callType)
    {
        case CALL_MISSED:                   //未接电话时要显示的图标
            return  UI_BUF_I330001_CALL_CALL_LOG_INCOMING_CALL_BIN;//UI_BUF_ICON_CALL_BIN;

        case CALL_RECEIVED:                 //接听电话时要显示的图标
            return  UI_BUF_I330001_CALL_CALL_LOG_DEENERGIZE_BIN;

        case CALL_DIALED:                   //拨出电话时要显示的图标
            return  UI_BUF_I330001_CALL_CALL_LOG_DEENERGIZE_BIN;

        default:
            break;

    }

    return 0;

}

//更新通话记录回调函数
static bool call_record_update_callback(u32 item_cnt, char* str_txt1, u16 str_txt1_len, char* str_txt2, u16 str_txt2_len, u16 index)
{
    if (index < item_cnt && index < record_cnt)
    {
        //需要使用静态数组作为中间变量，以防占用堆栈内存
        //中间缓存大小要比实际获取的名字要大
        //方便再接受到的名字过长时，方便后面使用uteModuleCharencodeGetUtf8String转换的时候可以自动加入省略号
        static u8 str_txt2_time[30] = {0};
        memset(str_txt2_time, 0, sizeof(str_txt2_time));

//        printf("len [%d,%d]\n", str_txt1_len, str_txt2_len);
        if (record_tbl[index].nameUnicodeLen == 0)
        {
            if (str_txt1_len > record_tbl[index].numberAsciiLen)
            {
                str_txt1_len = record_tbl[index].numberAsciiLen;
            }
            memcpy(str_txt1, record_tbl[index].numberAscii, str_txt1_len);
//            printf("####[%d,%d,%d]->[%s] [%s]\n", record_tbl[index].nameUnicodeLen, record_tbl[index].numberAsciiLen, str_txt1_len, str_txt1, record_tbl[index].numberAscii);
        }
        else
        {
            if (str_txt1_len > record_tbl[index].nameUnicodeLen)
            {
                str_txt1_len = record_tbl[index].nameUnicodeLen;
            }
            memcpy(str_txt1, record_tbl[index].nameUnicode, str_txt1_len);
        }

        if (str_txt2_len > sizeof(str_txt2_time))
        {
            str_txt2_len = sizeof(str_txt2_time);
        }

        //"2024-11-11 09:10:50"
        sprintf((char*)str_txt2_time, "%02d/%02d %02d:%02d:%02d", //record_tbl[index].callTime.year,
                record_tbl[index].callTime.month,
                record_tbl[index].callTime.day,
                record_tbl[index].callTime.hour,
                record_tbl[index].callTime.min,
                record_tbl[index].callTime.sec);
        memcpy(str_txt2, str_txt2_time, str_txt2_len);
//        printf("####[%s]\n", str_txt2_time);
        return true;
    }

    return false;
}

static u8 func_call_sub_record_update(void)
{
    u8 cnt = uteModuleCallGetCallRecordsSize(NULL);
    if (record_cnt != cnt)
    {
        record_cnt = cnt;
        printf("%s:%d\n", __func__, record_cnt);
        if (record_tbl != NULL)
        {
            ab_free(record_tbl);
            record_tbl = NULL;
        }
        record_tbl = ab_zalloc(sizeof(ute_module_call_records_t)*record_cnt);
        if (record_tbl != NULL)
        {
            printf("update record\n");
            uteModuleCallGetCallRecordsSize(record_tbl);
            return true;
        }
    }

    return false;
}

//创建通话记录窗体
compo_form_t *func_call_sub_record_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CALL_RECENT]);

    //创建无消息界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I330001_CALL_NO_LOG_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_COVER_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_NO_CALL_RECORD]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*3/4, GUI_SCREEN_WIDTH / 2, widget_text_get_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_NO_CALL_RECORD]);
    compo_setid(txt, COMPO_ID_COVER_TXT);


    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_TWO_TEXT);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    //更新最近通话
    func_call_sub_record_update();
    if (record_cnt)
    {
        compo_listbox_set(listbox, tbl_record_list, (record_cnt < 2) ? 2 : record_cnt);
    }
    compo_listbox_set_icon_area(listbox, CALL_TYPE_ICON_AREA);
    compo_listbox_set_text_modify_by_idx_callback2(listbox, call_record_update_callback);
    compo_listbox_set_icon_callback(listbox, call_record_set_icon_callback);
    compo_listbox_set_text1_color_callback(listbox, call_record_set_text1_callback);
//    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    compo_textbox_set_visible(txt, record_cnt > 0 ? false : true);
    compo_picturebox_set_visible(pic, record_cnt > 0 ? false : true);

    return frm;
}

//点进图标进入应用
static void func_call_sub_record_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= record_cnt)
    {
        return;
    }

    if (bt_is_connected())
    {
        memset(sys_cb.outgoing_number, 0, 16);
        for(u8 i=0; i<16; i++)
        {
            sys_cb.outgoing_number[i] = record_tbl[icon_idx].numberAscii[i];
        }
        bt_call_redial_number();
    }

}

//通话记录功能事件处理
static void func_set_sub_record_list_process(void)
{
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;


    if (tick_check_expire(f_call->tick, 1000))
    {
        f_call->tick = tick_get();
        //更新联系人
        u8 sta = func_call_sub_record_update();
        if (sta)
        {
            compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
            compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
            if (record_cnt)
            {
                compo_listbox_set_visible(f_call->listbox, true);
                compo_listbox_set(f_call->listbox, tbl_record_list, (record_cnt < 2) ? 2 : record_cnt);
                compo_listbox_set_icon_area(f_call->listbox, CALL_TYPE_ICON_AREA);
                compo_listbox_move_init_modify(f_call->listbox, 127-30, compo_listbox_gety_byidx(f_call->listbox, (record_cnt - 2 > 0) ? record_cnt - 2 : 1));
                compo_listbox_update_with_text_scroll_rst(f_call->listbox);
                compo_listbox_set_bgimg(f_call->listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
                compo_listbox_set_focus_byidx(f_call->listbox, 1);

                compo_listbox_update(f_call->listbox);
            }
            else
            {
                compo_listbox_set_visible(f_call->listbox, false);
                compo_listbox_set(f_call->listbox, tbl_record_list, record_cnt);
            }
            compo_textbox_set_visible(txt, record_cnt > 0 ? false : true);
            compo_picturebox_set_visible(pic, record_cnt > 0 ? false : true);
        }
    }


    compo_listbox_move(f_call->listbox);
    compo_listbox_update(f_call->listbox);

    func_process();
}

//通话记录功能消息处理
static void func_set_sub_record_list_message(size_msg_t msg)
{
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;

    if (record_cnt)
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            if (record_cnt)
            {
                func_call_sub_record_icon_click();
            }
            break;


        case MSG_CTP_LONG:
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {

            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            if (func_cb.flag_sort)
            {
                func_backing_to();
            }
            else
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_LEFT:
            if (func_cb.flag_sort)
            {

            }
            else
            {
                func_message(msg);
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入通话记录功能
static void func_call_sub_record_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_call_list_t));
    func_cb.frm_main = func_call_sub_record_form_create();

    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    f_call->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_call->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(f_call->listbox, 127-30, compo_listbox_gety_byidx(f_call->listbox, (record_cnt - 2 > 0) ? record_cnt - 2 : 1));
    func_cb.enter_tick = tick_get();


}

//退出通话记录功能
static void func_call_sub_record_exit(void)
{
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;

    if (record_tbl != NULL)
    {
        ab_free(record_tbl);
        record_tbl = NULL;
    }
    record_cnt = 0;

    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_CALL_SUB_RECORD;
}

//通话记录功能
void func_call_sub_record(void)
{
    printf("%s\n", __func__);
    func_call_sub_record_enter();
    while (func_cb.sta == FUNC_CALL_SUB_RECORD)
    {
        func_set_sub_record_list_process();
        func_set_sub_record_list_message(msg_dequeue());
    }
    func_call_sub_record_exit();
}
