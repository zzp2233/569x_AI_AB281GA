#include "include.h"
#include "func.h"
#include "func_bt.h"
#include "ute_module_call.h"

typedef struct f_bt_call_t_
{
    bool sta;                   //0:呼出(outgoing); 1:通话(call);

    u16 times;                  //通话秒数
    char call_time_str[10];     //通话计时字符串
    u32 clcc_tick;              //主动查询号码计时
    u32 exit_tick;              //页面退出计时
    bool call_mute_flag;
    char pbap_result_Name[50];//存放来电与接听联系人名字
    char tmp_pbap_result_Name[50];
    u8  page_num;               //0:通话  1：声音设置
    bool touch_flag;
    s8   level_value;
    s32 old_dy;
} f_bt_call_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define TXT_X_MIN 20

enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
    COMPO_ID_TXT_IN_CALL,
    COMPO_ID_BIN_SOUND,
    COMPO_ID_PIC_VOLUME,
    COMPO_ID_BTN_INCREASE,
    COMPO_ID_BTN_REDUCE,
};

static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING)
    {
        func_directly_back_to();
    }
}

void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = uteModuleCallGetCallingTimeSecond();
#else
    u16 call_times = bt_cb.times;
#endif

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void  func_bt_call_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_HEART_RATE]);
    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    // compo_textbox_set_align_center(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
//    compo_textbox_set_autosize(time_txt, true);
    compo_setid(time_txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(time_txt, COLOR_GREEN);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 240);

    //mic
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_JINGYIN00_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X*2/3, 240);

    //sound
    btn =compo_button_create_by_image (frm,UI_BUF_I330001_CALL_CALLING_SOUND_BIN);
    compo_setid(btn, COMPO_ID_BIN_SOUND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X*2/3,240);

    ///////////////////////////////////////第二页
    widget_page_t *page = widget_page_create(frm->page);///创建页码页面
    widget_page_set_client(page, 0, 0);

    //音量
    compo_picturebox_t *volume_pic = compo_picturebox_create_for_page(frm,frm->page, UI_BUF_I330001_CALL_CALLING_PROGRESS_BAR_BIN);
    compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
    compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_WIDTH, 144);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);

    //新建按钮
    btn = compo_button_create_page_by_image(frm,frm->page, UI_BUF_I330001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn,39+GUI_SCREEN_WIDTH,164);

    btn = compo_button_create_page_by_image(frm,frm->page, UI_BUF_I330001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  201+GUI_SCREEN_WIDTH,164);


    return frm;
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);
    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    // compo_textbox_set_align_center(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-5, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_IN_CALL]);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);
    // compo_textbox_set_align_center(txt, false);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    // compo_textbox_set_pos(txt,txt_x,138-widget_text_get_max_height()/2);
//    printf("x:%d leng:%d\n",txt_x,txt_leng);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 240);

//    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_JINGYIN00_BIN);
//    compo_setid(btn, COMPO_ID_BTN_MIC);
//    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X*2/3, 240);

    return frm;
}
static void func_bt_call_disp_Refresh(void)
{
    compo_picturebox_t *pic_level;
    pic_level = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(pic_level,sys_cb.hfp_vol-1,15);
}
AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false)
    {
        //销毁窗体
        if (func_cb.frm_main != NULL)
        {
            compo_form_destroy(func_cb.frm_main);
            memset(f_bt_call->pbap_result_Name, '\0', sizeof(f_bt_call->pbap_result_Name));
        }
        // printf("call_yes\n");
        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta)                                                              //号码未通知, 主动查询
    {
        if (tick_check_expire(f_bt_call->clcc_tick, 500))
        {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)      //退出通话页面
    {
        func_bt_call_back_to();
    }
    else
#endif
        if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500))          //强制退出, 防呆
        {
            printf("call reject, force exit!\n");
            sys_cb.reject_tick = tick_get();
            f_bt_call->exit_tick = 0;
            func_bt_call_back_to();
        }
}


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
static void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_call_up_date_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

//    printf("name:%s name:%s\n",f_bt_call->pbap_result_Name , sys_cb.pbap_result_Name);

    if(strcmp(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        // s16 txt_leng;
        // s16 txt_x=0;
        memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, sizeof(f_bt_call->pbap_result_Name));

        memset(f_bt_call->tmp_pbap_result_Name, '\0', sizeof(f_bt_call->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_call->tmp_pbap_result_Name, sizeof(f_bt_call->tmp_pbap_result_Name));


        printf("tmp_pbap_result_Name [%s]\n", f_bt_call->tmp_pbap_result_Name);
        compo_textbox_t *name_txt     = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_call->tmp_pbap_result_Name);

        // txt_leng = widget_text_get_area(name_txt->txt).wid;
        // txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
        // if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
        // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);
    }
}
void func_bt_call_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    func_bt_call_up_date_process();
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
    if(f_bt_call->page_num==1)
    {
        func_bt_call_disp_Refresh();
    }
}

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
bool mic_eq_drc_test = false;
#endif

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
//        printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            f_bt_call->exit_tick = tick_get();
            break;
        case COMPO_ID_BTN_MIC:
//        printf("COMPO_ID_BTN_REJECT\n");
            f_bt_call->call_mute_flag ^=1;
            compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MIC);

            if(f_bt_call->call_mute_flag)
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = false;
                }
                else
                {
                    audio_path_exit(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_exit(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I330001_CALL_CALLING_JINGYIN01_BIN);
            }
            else
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = true;
                }
                else
                {
                    audio_path_init(AUDIO_PATH_BTMIC);
                    audio_path_start(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_init(AUDIO_PATH_BTMIC);
                audio_path_start(AUDIO_PATH_BTMIC);
                bt_sco_pcm_set_dump_pass_cnt(5);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I330001_CALL_CALLING_JINGYIN00_BIN);
            }
            break;
        case COMPO_ID_BIN_SOUND:
            widget_page_set_client(func_cb.frm_main->page,-GUI_SCREEN_WIDTH, 0);
            f_bt_call->page_num =1 ;
            break;
        case COMPO_ID_BTN_REDUCE:
            if(sys_cb.hfp_vol>1)
            {
                printf("11111down");
                bt_volume_down();
            }
            func_bt_call_disp_Refresh();
            break;

        case COMPO_ID_BTN_INCREASE:
            bt_volume_up();
            printf("11111up");
            func_bt_call_disp_Refresh();
            break;
        default:
            break;
    }

}

#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

#define TXT_X_MIN 20
#define SPACING_COUNT (-(GUI_SCREEN_HEIGHT/8))

enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
    COMPO_ID_TXT_IN_CALL,
    COMPO_ID_BTN_VOLUME,//音量
    COMPO_ID_PIC_VOLUME,//音量等级
    COMPO_ID_COLOR_BLACK,//刷黑
};
static s16 level_old = 0;
static bool call_volume_flag = false;

static void func_bt_call_volume_handle(void)
{
    f_bt_call_t *f_bt_call_volume = (f_bt_call_t *)func_cb.f_cb;
    if(f_bt_call_volume->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_bt_call_volume->touch_flag = ctp_get_dxy(&dx, &dy);//上一个条件默认是true,在这里重新获取状态以及更新
        if(f_bt_call_volume->touch_flag == 0) //未按下，将记录系统通话音量
        {
            level_old = sys_cb.hfp_vol;
            f_bt_call_volume->level_value = sys_cb.hfp_vol;
        }
        if(f_bt_call_volume->touch_flag && dy != 0)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+level_old;
            if(level_data<1)
                level_data=1;
            else if(level_data>15)
                level_data=15;

            if(level_data != f_bt_call_volume->level_value)
            {
                if(level_data < f_bt_call_volume->level_value)
                {
                    bt_volume_down();
                    f_bt_call_volume->level_value = level_data;
                }
                else if(level_data > f_bt_call_volume->level_value)
                {
                    bt_volume_up();
                    f_bt_call_volume->level_value = level_data;
                }
            }
        }
        else
        {
            printf("%s\n",__func__);
            // f_bt_call_volume->level_old = f_bt_call_volume->level_value;
        }
    }
    compo_picturebox_t *volume_pic = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);
}
static void func_bt_call_volume_pic_update(void)
{
    compo_picturebox_t *volume_pic = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_COLOR_BLACK);
    if(call_volume_flag)
    {
        compo_listbox_set_visible(volume_pic,true);
        compo_listbox_set_visible(shape,true);
    }
    else
    {
        compo_listbox_set_visible(volume_pic,false);
        compo_listbox_set_visible(shape,false);
    }
}
static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING)
    {
        func_directly_back_to();
    }
}

void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = uteModuleCallGetCallingTimeSecond();
#else
    u16 call_times = bt_cb.times;
#endif

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void  func_bt_call_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);

//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_HEART_RATE]);
    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_setid(time_txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(time_txt, COLOR_WHITE);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_01_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 240);

    //静音按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_03_LOUDSPEAKER_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, 166+60/2, 240);

    //音量按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_00_SOUND_BIN);
    compo_setid(btn, COMPO_ID_BTN_VOLUME);
    compo_button_set_pos(btn, 14+60/2, 240);

    //刷黑
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape, COMPO_ID_COLOR_BLACK);
    compo_shape_set_color(shape, COLOR_BLACK);
    //音量
    compo_picturebox_t *volume_pic = compo_picturebox_create(frm, UI_BUF_I335001_CALL_20_ICON_VOLUME_68X200_X86_Y40_BIN);
    compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
    compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);

    func_bt_call_volume_pic_update();

    return frm;
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);
    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-5, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_IN_CALL]);
    compo_textbox_set_forecolor(txt, COLOR_WHITE);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_01_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 240);

    //静音按钮
    // btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_03_LOUDSPEAKER_BIN);
    // compo_setid(btn, COMPO_ID_BTN_MIC);
    // compo_button_set_pos(btn, 166+60/2, 240);

    //音量按钮
    // btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_00_SOUND_BIN);
    // // compo_setid(btn, COMPO_ID_BTN_VOLUME);
    // compo_button_set_pos(btn, 14+60/2, 240);


    return frm;
}

AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false)
    {
        //销毁窗体
        if (func_cb.frm_main != NULL)
        {
            compo_form_destroy(func_cb.frm_main);
            memset(f_bt_call->pbap_result_Name, '\0', sizeof(f_bt_call->pbap_result_Name));
        }
        // printf("call_yes\n");
        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta)                                                              //号码未通知, 主动查询
    {
        if (tick_check_expire(f_bt_call->clcc_tick, 500))
        {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)      //退出通话页面
    {
        func_bt_call_back_to();
    }
    else
#endif
        if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500))          //强制退出, 防呆
        {
            printf("call reject, force exit!\n");
            sys_cb.reject_tick = tick_get();
            f_bt_call->exit_tick = 0;
            func_bt_call_back_to();
        }
}


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
static void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_call_up_date_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

//    printf("name:%s name:%s\n",f_bt_call->pbap_result_Name , sys_cb.pbap_result_Name);

    if(strcmp(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        // s16 txt_leng;
        // s16 txt_x=0;
        memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, sizeof(f_bt_call->pbap_result_Name));

        memset(f_bt_call->tmp_pbap_result_Name, '\0', sizeof(f_bt_call->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_call->tmp_pbap_result_Name, sizeof(f_bt_call->tmp_pbap_result_Name));


        printf("tmp_pbap_result_Name [%s]\n", f_bt_call->tmp_pbap_result_Name);
        compo_textbox_t *name_txt     = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_call->tmp_pbap_result_Name);

        // txt_leng = widget_text_get_area(name_txt->txt).wid;
        // txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
        // if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
        // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);
    }
}
void func_bt_call_process(void)
{
    if(call_volume_flag)
    {
        func_bt_call_volume_handle();
    }
    func_bt_call_up_date_process();
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
}

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
bool mic_eq_drc_test = false;
#endif

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
//        printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            f_bt_call->exit_tick = tick_get();
            break;
        case COMPO_ID_BTN_MIC:
//        printf("COMPO_ID_BTN_REJECT\n");
            f_bt_call->call_mute_flag ^=1;
            compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MIC);

            if(f_bt_call->call_mute_flag)
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = false;
                }
                else
                {
                    audio_path_exit(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_exit(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_02_LOUDSPEAKER_BIN);
            }
            else
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = true;
                }
                else
                {
                    audio_path_init(AUDIO_PATH_BTMIC);
                    audio_path_start(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_init(AUDIO_PATH_BTMIC);
                audio_path_start(AUDIO_PATH_BTMIC);
                bt_sco_pcm_set_dump_pass_cnt(5);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_03_LOUDSPEAKER_BIN);
            }
            break;
        case COMPO_ID_BTN_VOLUME:           //进入音量设置界面
            if(sys_cb.incall_flag)
            {
                call_volume_flag = true;
                func_bt_call_volume_pic_update();
            }
            break;
        default:
            break;
    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define TXT_X_MIN 20

enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
    COMPO_ID_TXT_IN_CALL,
    COMPO_ID_BIN_SOUND,
    COMPO_ID_PIC_VOLUME,
    COMPO_ID_BTN_INCREASE,
    COMPO_ID_BTN_REDUCE,
};

static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING)
    {
        func_directly_back_to();
    }
}

void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = uteModuleCallGetCallingTimeSecond();;
#else
    u16 call_times = bt_cb.times;
#endif

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void  func_bt_call_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 83, GUI_SCREEN_WIDTH/1.4, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_setid(time_txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(time_txt, COLOR_GREEN);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 279);

    //mic
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_JINGYIN00_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, 79,279);

    //sound
    btn =compo_button_create_by_image (frm,UI_BUF_I332001_CALL_CALLING_SOUND_BIN);
    compo_setid(btn, COMPO_ID_BIN_SOUND);
    compo_button_set_pos(btn, 281,279);

///////////////////////////////////////第二页
    widget_page_t *page = widget_page_create(frm->page);///创建页码页面
    widget_page_set_client(page, 0, 0);

    //音量
    compo_picturebox_t *volume_pic = compo_picturebox_create_for_page(frm,frm->page, UI_BUF_I332001_CALL_CALLING_PROGRESS_BAR_BIN);
    compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
    compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_WIDTH, 44+144);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);

    //新建按钮
    btn = compo_button_create_page_by_image(frm,frm->page, UI_BUF_I332001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 62/2+41+GUI_SCREEN_WIDTH,62/2+159);

    btn = compo_button_create_page_by_image(frm,frm->page, UI_BUF_I332001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  62/2+257+GUI_SCREEN_WIDTH,62/2+159);
    return frm;
}
static void func_bt_call_disp_Refresh(void)
{
    compo_picturebox_t *pic_level;
    pic_level = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(pic_level,sys_cb.hfp_vol-1,15);
}
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 83, GUI_SCREEN_WIDTH/1.4, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_setid(txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);
    compo_textbox_set(txt, i18n[STR_IN_CALL]);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 279);

    return frm;
}

AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false)
    {
        //销毁窗体
        if (func_cb.frm_main != NULL)
        {
            compo_form_destroy(func_cb.frm_main);
            memset(f_bt_call->pbap_result_Name, '\0', sizeof(f_bt_call->pbap_result_Name));
        }

        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta)                                                              //号码未通知, 主动查询
    {
        if (tick_check_expire(f_bt_call->clcc_tick, 500))
        {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)      //退出通话页面
    {
        func_bt_call_back_to();
    }
    else
#endif
        if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500))          //强制退出, 防呆
        {
            printf("call reject, force exit!\n");
            sys_cb.reject_tick = tick_get();
            f_bt_call->exit_tick = 0;
            func_bt_call_back_to();
        }
}


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
static void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_call_up_date_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

//    printf("name:%s name:%s\n",f_bt_call->pbap_result_Name , sys_cb.pbap_result_Name);

    if(strcmp(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        s16 txt_leng;
        s16 txt_x=0;
        memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, sizeof(f_bt_call->pbap_result_Name));

        memset(f_bt_call->tmp_pbap_result_Name, '\0', sizeof(f_bt_call->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_call->tmp_pbap_result_Name, sizeof(f_bt_call->tmp_pbap_result_Name));


//        printf("tmp_pbap_result_Name [%s]\n", f_bt_call->tmp_pbap_result_Name);
        compo_textbox_t *name_txt     = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_call->tmp_pbap_result_Name);
    }
}
void func_bt_call_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    func_bt_call_up_date_process();
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
    if(f_bt_call->page_num==1)
    {
        func_bt_call_disp_Refresh();
    }
}

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
bool mic_eq_drc_test = false;
#endif

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
//        printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            f_bt_call->exit_tick = tick_get();
            break;
        case COMPO_ID_BTN_MIC:
//        printf("COMPO_ID_BTN_REJECT\n");
            f_bt_call->call_mute_flag ^=1;
            compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MIC);

            if(f_bt_call->call_mute_flag)
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = false;
                }
                else
                {
                    audio_path_exit(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_exit(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I332001_CALL_CALLING_JINGYIN01_BIN);
            }
            else
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = true;
                }
                else
                {
                    audio_path_init(AUDIO_PATH_BTMIC);
                    audio_path_start(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_init(AUDIO_PATH_BTMIC);
                audio_path_start(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I332001_CALL_CALLING_JINGYIN00_BIN);
            }
            break;
        case COMPO_ID_BIN_SOUND:
            widget_page_set_client(func_cb.frm_main->page,-GUI_SCREEN_WIDTH, 0);
            f_bt_call->page_num =1 ;
            break;
        case COMPO_ID_BTN_REDUCE:
            if(sys_cb.hfp_vol>1)
                bt_volume_down();
            func_bt_call_disp_Refresh();
            break;

        case COMPO_ID_BTN_INCREASE:
            bt_volume_up();
            func_bt_call_disp_Refresh();
            break;

        default:
            break;
    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
#define TXT_X_MIN 20

enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
    COMPO_ID_BTN_SOUD,
    COMPO_ID_SOUD_SHAPE,
    COMPO_ID_BTN_SOUD_SHAPE,
    COMPO_ID_TXT_IN_CALL,
};

static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING)
    {
        func_directly_back_to();
    }
}
void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = uteModuleCallGetCallingTimeSecond();;
#else
    u16 call_times = bt_cb.times;
#endif

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void  func_bt_call_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

#define val_shape_y  45
#define val_shepe_x  135
#define val_shape_h  270
#define val_shepe_w  90
#define make_pic_hei(val)   (val*val_shape_h/15)
#define make_pic_y(hei)     (val_shape_h-hei+val_shape_y)

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{
//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 83, GUI_SCREEN_WIDTH/1.4, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_setid(time_txt, COMPO_ID_TXT_TIME);
    // compo_textbox_set_forecolor(time_txt, COLOR_GREEN);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 80/2+234);

    //mic
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_MICROPHONE_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, 80/2+232,80/2+234);

    //声音
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_SOUND_BIN);
    compo_setid(btn, COMPO_ID_BTN_SOUD);
    compo_button_set_pos(btn, 80/2+48,80/2+234);

    compo_shape_t * val_bg = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(val_bg,-GUI_SCREEN_WIDTH+val_shepe_x,val_shape_y,val_shepe_w,val_shape_h);
    compo_shape_set_color(val_bg,make_color(50, 50, 50));
    compo_shape_set_radius(val_bg, 20);
    widget_set_align_center(val_bg->rect, false);

    s16 shape_y = make_pic_y(make_pic_hei(sys_cb.hfp_vol));
    s16 shape_h = make_pic_hei(sys_cb.hfp_vol);
    compo_shape_t * val = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(val,-GUI_SCREEN_WIDTH+val_shepe_x,shape_y,val_shepe_w,shape_h);
    compo_shape_set_radius(val, 20);
    widget_set_align_center(val->rect, false);
    compo_setid(val, COMPO_ID_SOUD_SHAPE);

    btn = compo_button_create_by_image(frm, 0);
    compo_setid(btn, COMPO_ID_BTN_SOUD_SHAPE);
    compo_button_set_location(btn, -GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,val_shepe_w,val_shape_h);

    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_VOLUME_BIN);
    compo_button_set_pos(btn, -GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X,32/2+262);

    return frm;
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 83, GUI_SCREEN_WIDTH/1.4, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_setid(txt, COMPO_ID_TXT_TIME);
    // compo_textbox_set_forecolor(txt, COLOR_GREEN);
    compo_textbox_set(txt, i18n[STR_IN_CALL]);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 80/2+234);

    return frm;
}

AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false)
    {
        //销毁窗体
        if (func_cb.frm_main != NULL)
        {
            compo_form_destroy(func_cb.frm_main);
            memset(f_bt_call->pbap_result_Name, '\0', sizeof(f_bt_call->pbap_result_Name));
        }

        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta)                                                              //号码未通知, 主动查询
    {
        if (tick_check_expire(f_bt_call->clcc_tick, 500))
        {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)      //退出通话页面
    {
        func_bt_call_back_to();
    }
    else
#endif
        if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500))          //强制退出, 防呆
        {
            printf("call reject, force exit!\n");
            sys_cb.reject_tick = tick_get();
            f_bt_call->exit_tick = 0;
            func_bt_call_back_to();
        }
}

// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
static void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_call_up_date_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;


    if(f_bt_call->touch_flag)
    {
#define lever_data (270/15)
        s32 dx,dy;
        f_bt_call->touch_flag = ctp_get_dxy(&dx, &dy);//上一个条件默认是true,在这里重新获取状态以及更新
        if(f_bt_call->touch_flag)
        {
            s8 level_value = sys_cb.hfp_vol + (-dy/lever_data)+f_bt_call->level_value;
            // printf("level:%d  hfp_vol:%d\n",level_value,sys_cb.hfp_vol);
            if(level_value > sys_cb.hfp_vol && sys_cb.hfp_vol<15)
            {
                bt_volume_up();
                f_bt_call->level_value --;
            }
            else if(level_value < sys_cb.hfp_vol && sys_cb.hfp_vol>1)
            {
                bt_volume_down();
                f_bt_call->level_value ++;
            }
        }
        else
        {
            f_bt_call->level_value = 0;
        }
    }

    s16 shape_y = make_pic_y(make_pic_hei(sys_cb.hfp_vol));
    s16 shape_h = make_pic_hei(sys_cb.hfp_vol);
    compo_shape_t * val = compo_getobj_byid(COMPO_ID_SOUD_SHAPE);
    if(val != NULL)
    {
        compo_shape_set_location(val,-GUI_SCREEN_WIDTH+val_shepe_x,shape_y,val_shepe_w,shape_h);
    }

    if(strcmp(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        s16 txt_leng;
        s16 txt_x=0;
        memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, sizeof(f_bt_call->pbap_result_Name));

        memset(f_bt_call->tmp_pbap_result_Name, '\0', sizeof(f_bt_call->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_call->tmp_pbap_result_Name, sizeof(f_bt_call->tmp_pbap_result_Name));
        compo_textbox_t *name_txt     = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_call->tmp_pbap_result_Name);
    }
}
void func_bt_call_process(void)
{
    func_bt_call_up_date_process();
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
}

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
bool mic_eq_drc_test = false;
#endif

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
//        printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            f_bt_call->exit_tick = tick_get();
            break;
        case COMPO_ID_BTN_MIC:
//        printf("COMPO_ID_BTN_REJECT\n");
            f_bt_call->call_mute_flag ^=1;
            compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MIC);

            if(f_bt_call->call_mute_flag)
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = false;
                }
                else
                {
                    audio_path_exit(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_exit(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I338001_11_CALL_MICROPHONE_01_BIN);
            }
            else
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = true;
                }
                else
                {
                    audio_path_init(AUDIO_PATH_BTMIC);
                    audio_path_start(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_init(AUDIO_PATH_BTMIC);
                audio_path_start(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I338001_11_CALL_MICROPHONE_BIN);
            }
            break;
        case COMPO_ID_BTN_SOUD:
            widget_page_set_client(func_cb.frm_main->page_body, GUI_SCREEN_WIDTH, 0);
            f_bt_call->page_num =1 ;
            break;
        default:
            break;
    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define TXT_X_MIN 20

enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
    COMPO_ID_TXT_IN_CALL,
    COMPO_ID_BIN_SOUND,
    COMPO_ID_PIC_VOLUME,
    COMPO_ID_BTN_INCREASE,
    COMPO_ID_BTN_REDUCE,
};

static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING)
    {
        func_directly_back_to();
    }
}

void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = uteModuleCallGetCallingTimeSecond();;
#else
    u16 call_times = bt_cb.times;
#endif

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void  func_bt_call_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 83, GUI_SCREEN_WIDTH/1.4, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_setid(time_txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(time_txt, COLOR_GREEN);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 279);

    //mic
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_CALL_CALLING_JINGYIN00_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, 34+76/2,241+76/2);

    //sound
    btn =compo_button_create_by_image (frm,UI_BUF_I340001_CALL_CALLING_SOUND_BIN);
    compo_setid(btn, COMPO_ID_BIN_SOUND);
    compo_button_set_pos(btn, 250+76/2,241+76/2);

///////////////////////////////////////第二页
    widget_page_t *page = widget_page_create(frm->page);///创建页码页面
    widget_page_set_client(page, 0, 0);

    //音量
    compo_picturebox_t *volume_pic = compo_picturebox_create_for_page(frm,frm->page, UI_BUF_I340001_CALL_CALLING_PROGRESS_BAR_BIN);
    compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
    compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_WIDTH, 44+144);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);

    //新建按钮
    btn = compo_button_create_page_by_image(frm,frm->page, UI_BUF_I340001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 62/2+41+GUI_SCREEN_WIDTH,62/2+159);

    btn = compo_button_create_page_by_image(frm,frm->page, UI_BUF_I340001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  62/2+257+GUI_SCREEN_WIDTH,62/2+159);
    return frm;
}
static void func_bt_call_disp_Refresh(void)
{
    compo_picturebox_t *pic_level;
    pic_level = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(pic_level,sys_cb.hfp_vol-1,15);
}
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 83, GUI_SCREEN_WIDTH/1.4, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_setid(txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);
    compo_textbox_set(txt, i18n[STR_IN_CALL]);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 279);

    return frm;
}

AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false)
    {
        //销毁窗体
        if (func_cb.frm_main != NULL)
        {
            compo_form_destroy(func_cb.frm_main);
            memset(f_bt_call->pbap_result_Name, '\0', sizeof(f_bt_call->pbap_result_Name));
        }

        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta)                                                              //号码未通知, 主动查询
    {
        if (tick_check_expire(f_bt_call->clcc_tick, 500))
        {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)      //退出通话页面
    {
        func_bt_call_back_to();
    }
    else
#endif
        if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500))          //强制退出, 防呆
        {
            printf("call reject, force exit!\n");
            sys_cb.reject_tick = tick_get();
            f_bt_call->exit_tick = 0;
            func_bt_call_back_to();
        }
}


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
static void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_call_up_date_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

//    printf("name:%s name:%s\n",f_bt_call->pbap_result_Name , sys_cb.pbap_result_Name);

    if(strcmp(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        s16 txt_leng;
        s16 txt_x=0;
        memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, sizeof(f_bt_call->pbap_result_Name));

        memset(f_bt_call->tmp_pbap_result_Name, '\0', sizeof(f_bt_call->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_call->tmp_pbap_result_Name, sizeof(f_bt_call->tmp_pbap_result_Name));


//        printf("tmp_pbap_result_Name [%s]\n", f_bt_call->tmp_pbap_result_Name);
        compo_textbox_t *name_txt     = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_call->tmp_pbap_result_Name);
    }
}
void func_bt_call_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    func_bt_call_up_date_process();
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
    if(f_bt_call->page_num==1)
    {
        func_bt_call_disp_Refresh();
    }
}

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
bool mic_eq_drc_test = false;
#endif

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
//        printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            f_bt_call->exit_tick = tick_get();
            break;
        case COMPO_ID_BTN_MIC:
//        printf("COMPO_ID_BTN_REJECT\n");
            f_bt_call->call_mute_flag ^=1;
            compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MIC);

            if(f_bt_call->call_mute_flag)
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = false;
                }
                else
                {
                    audio_path_exit(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_exit(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I340001_CALL_CALLING_JINGYIN01_BIN);
            }
            else
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = true;
                }
                else
                {
                    audio_path_init(AUDIO_PATH_BTMIC);
                    audio_path_start(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_init(AUDIO_PATH_BTMIC);
                audio_path_start(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I340001_CALL_CALLING_JINGYIN00_BIN);
            }
            break;
        case COMPO_ID_BIN_SOUND:
            widget_page_set_client(func_cb.frm_main->page,-GUI_SCREEN_WIDTH, 0);
            f_bt_call->page_num =1 ;
            break;
        case COMPO_ID_BTN_REDUCE:
            if(sys_cb.hfp_vol>1)
                bt_volume_down();
            func_bt_call_disp_Refresh();
            break;

        case COMPO_ID_BTN_INCREASE:
            bt_volume_up();
            func_bt_call_disp_Refresh();
            break;

        default:
            break;
    }

}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#define TXT_X_MIN 20
#define SPACING_COUNT (-(GUI_SCREEN_HEIGHT/8))

enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
    COMPO_ID_TXT_IN_CALL,
    COMPO_ID_BTN_VOLUME,//音量
    COMPO_ID_PIC_VOLUME,//音量等级
    COMPO_ID_COLOR_BLACK,//刷黑
};
static s16 level_old = 0;
static bool call_volume_flag = false;

static void func_bt_call_volume_handle(void)
{
    f_bt_call_t *f_bt_call_volume = (f_bt_call_t *)func_cb.f_cb;
    if(f_bt_call_volume->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_bt_call_volume->touch_flag = ctp_get_dxy(&dx, &dy);//上一个条件默认是true,在这里重新获取状态以及更新
        if(f_bt_call_volume->touch_flag == 0) //未按下，将记录系统通话音量
        {
            level_old = sys_cb.hfp_vol;
            f_bt_call_volume->level_value = sys_cb.hfp_vol;
        }
        if(f_bt_call_volume->touch_flag && dy != 0)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+level_old;
            if(level_data<1)
                level_data=1;
            else if(level_data>15)
                level_data=15;

            if(level_data != f_bt_call_volume->level_value)
            {
                if(level_data < f_bt_call_volume->level_value)
                {
                    bt_volume_down();
                    f_bt_call_volume->level_value = level_data;
                }
                else if(level_data > f_bt_call_volume->level_value)
                {
                    bt_volume_up();
                    f_bt_call_volume->level_value = level_data;
                }
            }
        }
        else
        {
            printf("%s\n",__func__);
            // f_bt_call_volume->level_old = f_bt_call_volume->level_value;
        }
    }
    compo_picturebox_t *volume_pic = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);
}
static void func_bt_call_volume_pic_update(void)
{
    compo_picturebox_t *volume_pic = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_COLOR_BLACK);
    if(call_volume_flag)
    {
        compo_listbox_set_visible(volume_pic,true);
        compo_listbox_set_visible(shape,true);
    }
    else
    {
        compo_listbox_set_visible(volume_pic,false);
        compo_listbox_set_visible(shape,false);
    }
}
static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING)
    {
        func_directly_back_to();
    }
}

void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = uteModuleCallGetCallingTimeSecond();
#else
    u16 call_times = bt_cb.times;
#endif

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void  func_bt_call_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);

//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_HEART_RATE]);
    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 74+30/2, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_setid(time_txt, COMPO_ID_TXT_TIME);
    compo_textbox_set_forecolor(time_txt, COLOR_WHITE);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_01_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 156+54/2);

    //静音按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_03_LOUDSPEAKER_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, 166+60/2, 156+54/2);

    //音量按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_00_SOUND_BIN);
    compo_setid(btn, COMPO_ID_BTN_VOLUME);
    compo_button_set_pos(btn, 14+60/2, 156+54/2);

    //刷黑
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape, COMPO_ID_COLOR_BLACK);
    compo_shape_set_color(shape, COLOR_BLACK);
    //音量
    compo_picturebox_t *volume_pic = compo_picturebox_create(frm, UI_BUF_I342001_11_CALL_ICON_VOLUME_BIN);
    compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
    compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_cut(volume_pic,sys_cb.hfp_vol-1,15);

    func_bt_call_volume_pic_update();

    return frm;
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{

//    bt_pbap_lookup_number(hfp_get_last_call_number(0));
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);
    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 74+30/2, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_IN_CALL]));
    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-5, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6+16, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_IN_CALL]);
    compo_textbox_set_forecolor(txt, COLOR_WHITE);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_01_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 156+54/2);

    //静音按钮
    // btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_03_LOUDSPEAKER_BIN);
    // compo_setid(btn, COMPO_ID_BTN_MIC);
    // compo_button_set_pos(btn, 166+60/2, 240);

    //音量按钮
    // btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_09_CALLING_ICON_PIC60X60_X14_90_166_Y202_00_SOUND_BIN);
    // // compo_setid(btn, COMPO_ID_BTN_VOLUME);
    // compo_button_set_pos(btn, 14+60/2, 240);


    return frm;
}

AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false)
    {
        //销毁窗体
        if (func_cb.frm_main != NULL)
        {
            compo_form_destroy(func_cb.frm_main);
            memset(f_bt_call->pbap_result_Name, '\0', sizeof(f_bt_call->pbap_result_Name));
        }
        // printf("call_yes\n");
        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta)                                                              //号码未通知, 主动查询
    {
        if (tick_check_expire(f_bt_call->clcc_tick, 500))
        {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)      //退出通话页面
    {
        func_bt_call_back_to();
    }
    else
#endif
        if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500))          //强制退出, 防呆
        {
            printf("call reject, force exit!\n");
            sys_cb.reject_tick = tick_get();
            f_bt_call->exit_tick = 0;
            func_bt_call_back_to();
        }
}


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
static void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_call_up_date_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

//    printf("name:%s name:%s\n",f_bt_call->pbap_result_Name , sys_cb.pbap_result_Name);

    if(strcmp(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        // s16 txt_leng;
        // s16 txt_x=0;
        memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, sizeof(f_bt_call->pbap_result_Name));

        memset(f_bt_call->tmp_pbap_result_Name, '\0', sizeof(f_bt_call->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_call->tmp_pbap_result_Name, sizeof(f_bt_call->tmp_pbap_result_Name));


        printf("tmp_pbap_result_Name [%s]\n", f_bt_call->tmp_pbap_result_Name);
        compo_textbox_t *name_txt     = compo_getobj_byid(COMPO_ID_TXT_NAME);
        compo_textbox_set(name_txt, f_bt_call->tmp_pbap_result_Name);

        // txt_leng = widget_text_get_area(name_txt->txt).wid;
        // txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
        // if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
        // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);
    }
}
void func_bt_call_process(void)
{
    if(call_volume_flag)
    {
        func_bt_call_volume_handle();
    }
    func_bt_call_up_date_process();
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
}

#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
bool mic_eq_drc_test = false;
#endif

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:
//        printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            f_bt_call->exit_tick = tick_get();
            break;
        case COMPO_ID_BTN_MIC:
//        printf("COMPO_ID_BTN_REJECT\n");
            f_bt_call->call_mute_flag ^=1;
            compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_MIC);

            if(f_bt_call->call_mute_flag)
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = false;
                }
                else
                {
                    audio_path_exit(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_exit(AUDIO_PATH_BTMIC);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I342001_11_CALL_BUTTON_PRESS_MICROPHONE01_BIN);
            }
            else
            {
#if EQ_DBG_IN_UART || EQ_DBG_IN_SPP
                if (xcfg_cb.eq_dgb_uart_en || xcfg_cb.eq_dgb_uart_en)
                {
                    mic_eq_drc_test = true;
                }
                else
                {
                    audio_path_init(AUDIO_PATH_BTMIC);
                    audio_path_start(AUDIO_PATH_BTMIC);
                }
#else
                audio_path_init(AUDIO_PATH_BTMIC);
                audio_path_start(AUDIO_PATH_BTMIC);
                bt_sco_pcm_set_dump_pass_cnt(5);
#endif
                compo_button_set_bgimg(btn,UI_BUF_I342001_11_CALL_BUTTON_PRESS_MICROPHONE00_BIN);
            }
            break;
        case COMPO_ID_BTN_VOLUME:           //进入音量设置界面
            if(sys_cb.incall_flag)
            {
                call_volume_flag = true;
                func_bt_call_volume_pic_update();
            }
            break;
        default:
            break;
    }

}

#else
compo_form_t *func_bt_outgoing_form_create(void)
{
}
void func_bt_call_process(void)
{}
void  func_bt_call_number_update(void)
{}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//蓝牙音乐消息处理
static void func_bt_call_message(size_msg_t msg)
{
#if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT \
     || GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT \
     || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT \
     || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT)
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
            if(f_bt_call->touch_flag == false && compo_get_button_id() == COMPO_ID_BTN_SOUD_SHAPE)
            {
                f_bt_call->touch_flag = true;
            }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
            f_bt_call->touch_flag = true;
#endif
            break;
        case KU_BACK:
            if(bt_get_call_indicate() == BT_CALL_INCOMING)
            {
                bt_call_answer_incoming();                  //接听第2路通话
            }
            else
            {
//                bt_call_terminate();                        //挂断当前通话
            }
            break;

        case KU_RIGHT:
            bt_volume_up();
            break;

        case KU_LEFT:
            bt_volume_down();
            break;

        case MSG_SYS_1S:
            func_message(MSG_SYS_1S);
            if (f_bt_call->sta == true)
            {
#if !CALL_MGR_EN
                // f_bt_call->times++;
#endif
                bt_incall_time_update();
            }
            break;

        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
            if(!call_volume_flag)//音量设置页面状态，true:显示，false:不显示
#endif
            {
                func_bt_call_click();                           //单击按钮
            }
            break;

        case EVT_CALL_NUMBER_UPDATE:
            func_bt_call_number_update();
            break;

        case MSG_SYS_500MS:
            reset_sleep_delay_all();                           //来电不休眠
            break;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
        case MSG_CTP_LONG_RIGHT:                               //退出音量设置页面
            if(call_volume_flag)
            {
                call_volume_flag = false;
                func_bt_call_volume_pic_update();
            }
            break;
#endif
        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
        case MSG_CTP_SHORT_LEFT:
            break;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
        case MSG_CTP_SHORT_RIGHT:
            if(f_bt_call->page_num==1)
            {
                f_bt_call->page_num=0;
                widget_page_set_client(func_cb.frm_main->page,0, 0);
            }
            break;
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
        case MSG_CTP_SHORT_RIGHT:
            if(f_bt_call->page_num==1)
            {
                f_bt_call->page_num=0;
                widget_page_set_client(func_cb.frm_main->page_body,0, 0);
            }
            break;
#endif
        default:
            func_message(msg);
            break;
    }
#endif
}

void func_bt_call_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_bt_call_t));
    func_cb.frm_main = func_bt_outgoing_form_create();
    func_bt_call_number_update();
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    // memcpy(f_bt_call->pbap_result_Name, sys_cb.pbap_result_Name, 50);
    memset(f_bt_call->pbap_result_Name,0, sizeof(f_bt_call->pbap_result_Name));
//   printf("name:%s  name:%s\n",sys_cb.pbap_result_Name,f_bt_call->pbap_result_Name);

    f_bt_call->call_mute_flag =false;
    func_cb.mp3_res_play = func_bt_mp3_res_play;
    bsp_bt_call_enter();
    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
}

void func_bt_call_exit(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    call_volume_flag = false;//音量设置页面标志复位
#endif
    bsp_bt_call_exit();
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    if (func_cb.sta == FUNC_SCAN)
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
    {
        while (sys_cb.sco_state == true)          //等待SCO完全退出, 支付宝和扫一扫界面都使用二维码，二维码与通话复用
        {
            WDT_CLR();
        }
    }

    func_cb.last = FUNC_BT_CALL;
}

void func_bt_call(void)
{
    u16 interval = 0, latency = 0, tout = 0;

    printf("%s\n", __func__);

    if (ble_is_connect() && (ble_get_conn_interval() < 400))
    {
        interval = ble_get_conn_interval();
        latency = ble_get_conn_latency();
        tout = ble_get_conn_timeout();
        ble_update_conn_param(480, 0, 500);
    }
    func_bt_call_enter();
    while (func_cb.sta == FUNC_BT_CALL)
    {
        func_bt_call_process();
        func_bt_call_message(msg_dequeue());
    }
    func_bt_call_exit();

    if (bt_cb.disp_status != BT_STA_INCALL)
    {
        if (interval | latency | tout)
        {
            ble_update_conn_param(interval, latency, tout);
        }
    }
}
