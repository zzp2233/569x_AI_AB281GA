#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

typedef struct f_sav_t_
{
    u8 vol_value;
    u8 shk_value;
} f_sav_t;

enum
{
    //数字
    COMPO_ID_NUM_DISP_ONE = 1,
    COMPO_ID_NUM_DISP_TWS,

    //按键
    COMPO_ID_BIN_REDUCE_ONE,
    COMPO_ID_BIN_INCREASE_ONE,
    COMPO_ID_BIN_REDUCE_TWS,
    COMPO_ID_BIN_INCREASE_TWS,
    COMPO_ID_BIN_REDUCE_THR,
    COMPO_ID_BIN_INCREASE_THR,
    //图像
    COMPO_ID_PIC_LEVEL1,            //音量目前等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
    COMPO_ID_PIC_LEVEL6,
    COMPO_ID_PIC_LEVEL7,
    COMPO_ID_PIC_LEVEL8,
    COMPO_ID_PIC_LEVEL9,
    COMPO_ID_PIC_LEVEL10,
    COMPO_ID_PIC_LEVEL11,
    COMPO_ID_PIC_LEVEL12,
    COMPO_ID_PIC_LEVEL13,
    COMPO_ID_PIC_LEVEL14,
    COMPO_ID_PIC_LEVEL15,
};

#define SAV_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_sav_disp_btn_item) / sizeof(tbl_sav_disp_btn_item[0])))

typedef struct sav_disp_btn_item_t_
{
    u16 btn_id;
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} sav_disp_btn_item_t;

//按钮item，创建时遍历一下
static const sav_disp_btn_item_t tbl_sav_disp_btn_item[] =
{
    {  COMPO_ID_BIN_REDUCE_ONE,            35,      112,   40,  40},
    {  COMPO_ID_BIN_INCREASE_ONE,          205,     112,   40,  40},
    {  COMPO_ID_BIN_REDUCE_TWS,            35,      200,   40,  40},
    {  COMPO_ID_BIN_INCREASE_TWS,          205,     200,   40,  40},
    {  COMPO_ID_BIN_REDUCE_THR,            35,      276,   40,  40},
    {  COMPO_ID_BIN_INCREASE_THR,          205,     276,   40,  40},
};

typedef struct sav_disp_pic_item_t_
{
    u32 res_addr;
    u32 res_addr_bg;
    u16 pic_id;
    s16 x;
    s16 y;
    bool visible_en;
} sav_disp_pic_item_t;

#define SAV_DISP_PIC_ITEM_CNT                       ((int)(sizeof(tbl_sav_disp_pic_item) / sizeof(tbl_sav_disp_pic_item[0])))

//图片item，创建时遍历一下
static const sav_disp_pic_item_t tbl_sav_disp_pic_item[] =
{
    {UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREEN_BIN,   UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREY_BIN,    COMPO_ID_PIC_LEVEL1,          68,     112},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL2,          68+21,    112},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL3,          68+21*2,    112},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL4,          68+21*3,    112},
    {UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREY_BIN,   COMPO_ID_PIC_LEVEL5,          68+21*4,    112},
    {UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREEN_BIN,   UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREY_BIN,    COMPO_ID_PIC_LEVEL6,          68,     200},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL7,          68+21,    200},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL8,          68+21*2,    200},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL9,          68+21*3,    200},
    {UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREY_BIN,   COMPO_ID_PIC_LEVEL10,         68+21*4,    200},
//    {UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREEN_BIN,   UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREY_BIN,    COMPO_ID_PIC_LEVEL11,         68,     276},
//    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL12,         68+21,    276},
//    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL13,         68+21*2,    276},
//    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL14,         68+21*3,    276},
//    {UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREY_BIN,   COMPO_ID_PIC_LEVEL15,         68+21*4,    276},
};

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    int idx=0;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_SAV]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_SETTINGS_SOUND_BTN_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X ,tbl_sav_disp_btn_item[0].y );

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_SETTINGS_SOUND_BTN_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X ,tbl_sav_disp_btn_item[2].y );


    for( idx=0; idx<SAV_DISP_BTN_ITEM_CNT; idx++)
    {
        compo_button_t * btn = compo_button_create(frm);
        compo_button_set_location(btn, tbl_sav_disp_btn_item[idx].x,tbl_sav_disp_btn_item[idx].y, tbl_sav_disp_btn_item[idx].w,tbl_sav_disp_btn_item[idx].h);
        compo_setid(btn,tbl_sav_disp_btn_item[idx].btn_id);
    }

    for( idx=0; idx<SAV_DISP_PIC_ITEM_CNT; idx++)
    {
        picbox = compo_picturebox_create(frm, tbl_sav_disp_pic_item[idx].res_addr_bg);
        compo_picturebox_set_pos(picbox,tbl_sav_disp_pic_item[idx].x+10 ,tbl_sav_disp_pic_item[idx].y );
        compo_setid(picbox,tbl_sav_disp_pic_item[idx].pic_id);
    }


    //创建文本
    compo_textbox_t *txt_voice = compo_textbox_create(frm, 4);
    compo_textbox_set_align_center(txt_voice, false);
    compo_textbox_set_pos(txt_voice, 22, 62);
    compo_textbox_set(txt_voice, i18n[STR_MEDIA_VOL]);

    compo_textbox_t *txt_call = compo_textbox_create(frm, 4);
    compo_textbox_set_align_center(txt_call, false);
    compo_textbox_set_pos(txt_call, 22, 148);
    compo_textbox_set(txt_call, i18n[STR_CALL_VOL]);

    compo_textbox_t *txt_shake = compo_textbox_create(frm, 2);
    compo_textbox_set_align_center(txt_shake, false);
    compo_textbox_set_pos(txt_shake, 22, 232);
    compo_textbox_set(txt_shake, i18n[STR_SHK]);






#if UTE_DRV_MOTOR_PWM_MODE_SUPPORT

#else
    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 260, GUI_SCREEN_WIDTH, 70);
    compo_shape_set_color(shape, COLOR_BLACK);
    //COLOR_WHITE
//    COLOR_BLACK
#endif

    return frm;
}

//声音与振动事件处理
static void func_set_sub_sav_process(void)
{
    func_process();
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{

    for(int idx=0; idx<10; idx++)
    {
        compo_picturebox_t *picbox = compo_getobj_byid(tbl_sav_disp_pic_item[idx].pic_id);

        if(idx<= 4)
        {
            if(sys_cb.vol/(4*(idx+1))){
                compo_picturebox_set(picbox,tbl_sav_disp_pic_item[idx].res_addr);
            }else{
                compo_picturebox_set(picbox,tbl_sav_disp_pic_item[idx].res_addr_bg);
            }
        }
        else if(idx<= 9)
        {
            if(sys_cb.hfp_vol/(4*(idx+1-5))){
                compo_picturebox_set(picbox,tbl_sav_disp_pic_item[idx].res_addr);
            }else{
                compo_picturebox_set(picbox,tbl_sav_disp_pic_item[idx].res_addr_bg);
            }
        }

    }
}

//单击按钮
static void func_sav_button_click(void)
{
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BIN_REDUCE_ONE:       //音频音量
            if (sys_cb.vol > 0 && sys_cb.vol <= VOL_CHANGE*5)
            {
                sys_cb.vol -= VOL_CHANGE;
            }
            break;

        case COMPO_ID_BIN_INCREASE_ONE:
            if (sys_cb.vol >= 0 && sys_cb.vol < VOL_CHANGE*5)
            {
                sys_cb.vol += VOL_CHANGE;
            }
            break;

        case COMPO_ID_BIN_REDUCE_TWS:     //通话音量
            if (sys_cb.hfp_vol > 0 && sys_cb.hfp_vol <= VOL_CHANGE*5)
            {
                sys_cb.hfp_vol -= VOL_CHANGE;
            }
            break;

        case COMPO_ID_BIN_INCREASE_TWS:   //通话音量
            if (sys_cb.hfp_vol >= 0 && sys_cb.hfp_vol < VOL_CHANGE*5)
            {
                sys_cb.hfp_vol += VOL_CHANGE;
            }
            break;

        case COMPO_ID_BIN_REDUCE_THR:     //马达等级
            if (sys_cb.motor_level > 0 && sys_cb.motor_level <= MOTOR_MAX_LEVEL)
            {
                sys_cb.motor_level -= 1;
            }
            break;

        case COMPO_ID_BIN_INCREASE_THR:   //马达等级
            if (sys_cb.motor_level >= 0 && sys_cb.motor_level <= 6)
            {
                sys_cb.motor_level += 1;
            }
            break;

        default:
            break;
    }
    func_set_sub_sav_disp();
}

//声音与振动功能消息处理
static void func_set_sub_sav_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sav_button_click();
            break;

        default:
            func_message(msg);
            break;
    }

}

//进入声音与振动功能
static void func_set_sub_sav_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sav_t));
    func_cb.frm_main = func_set_sub_sav_form_create();

    f_sav_t *sav = (f_sav_t *)func_cb.f_cb;
    sav->shk_value = COMPO_ID_NUM_DISP_ONE;
    sav->vol_value = COMPO_ID_NUM_DISP_ONE;
    func_set_sub_sav_disp();
}

//退出声音与振动功能
static void func_set_sub_sav_exit(void)
{
    func_cb.last = FUNC_SET_SUB_SAV;
}

//声音与振动功能
void func_set_sub_sav(void)
{
    printf("%s\n", __func__);
    func_set_sub_sav_enter();
    while (func_cb.sta == FUNC_SET_SUB_SAV)
    {
        func_set_sub_sav_process();
        func_set_sub_sav_message(msg_dequeue());
    }
    func_set_sub_sav_exit();
}
