#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_compass_t_ {
    s16 angle;
    s16 pos;
} f_compass_t;

//组件ID
enum{
    //按键
    COMPO_ID_COMPASS = 1,
    COMPO_ID_COMPASS_TEXT,
    COMPO_ID_COMPASS_DIR_TEXT,
};

typedef struct {
    char dir_str;
    u16 dir_num;
} compass_t;

const compass_t compass_table[5] = {
    {'N', 45},
    {'E', 135},
    {'S', 225},
    {'W', 315},
    {'N', 360},
};

void func_compass_dir_show(compo_textbox_t *text, bool force)
{
    f_compass_t *f_compass = (f_compass_t *)func_cb.f_cb;
    static u8 last_i = 0;

    u8 i = 0;
    for(;i<4;i++) {
        if ((f_compass->pos / 10) < compass_table[i].dir_num) {
            break;
        }
    }

    if ((last_i == i) && !force) {
        return;
    }

    last_i = i;

    char dbuf[2];
    memset(dbuf, 0, 2);
    sprintf(dbuf, "%c", compass_table[i].dir_str);
//    printf("[%d]:%c\n", i, compass_table[i].dir_str);
    compo_textbox_set(text, dbuf);
}

void func_compass_num_show(compo_textbox_t *text)
{
    f_compass_t *f_compass = (f_compass_t *)func_cb.f_cb;
    static u16 last_pos = 0;

    if ((last_pos / 10) == (f_compass->pos / 10)) {

        return;
    }

    last_pos = f_compass->pos;

    char tbuf[10];
    memset(tbuf, 0, 10);
    sprintf(tbuf, "~%d°", (f_compass->pos / 10));
    compo_textbox_set(text, tbuf);
}

//创建指南针窗体
compo_form_t *func_compass_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_COMPASS]);

    f_compass_t *f_compass = (f_compass_t *)func_cb.f_cb;
    f_compass->angle = 0;
    f_compass->pos = 1800;

    area_t area_bg = gui_image_get_size(UI_BUF_COMPASS_BG_BIN);
    area_t area_point = gui_image_get_size(UI_BUF_COMPASS_POINT_BIN);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_COMPASS_BG_BIN);
    compo_setid(pic, COMPO_ID_COMPASS);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X);
    compo_picturebox_set_rotation_center(pic, area_bg.wid / 2, area_bg.hei / 2);
    compo_picturebox_set_rotation(pic, f_compass->angle);

    pic = compo_picturebox_create(frm, UI_BUF_COMPASS_POINT_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, area_point.wid / 2);

    compo_textbox_t *text = compo_textbox_create(frm, 8);
    compo_setid(text, COMPO_ID_COMPASS_TEXT);
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 35, 200, 70);
    compo_textbox_set_font(text, UI_BUF_0FONT_FONT_NUM_46_BIN);
    func_compass_num_show(text);

    compo_textbox_t *dir_text = compo_textbox_create(frm, 2);
    compo_setid(dir_text, COMPO_ID_COMPASS_DIR_TEXT);
    compo_textbox_set_location(dir_text, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 35, 100, 70);
    compo_textbox_set_font(dir_text, UI_BUF_0FONT_FONT_NUM_46_BIN);
    func_compass_dir_show(dir_text, true);

    tft_set_temode(0);

    return frm;
}

void func_compass_demo_process(void)
{
    f_compass_t *f_compass = (f_compass_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (tick_check_expire(ticks, 10)) {
        ticks = tick_get();
        f_compass->angle++;
        f_compass->pos--;
        if (f_compass->pos < 0) {
            f_compass->pos = 3599;
        }
        compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_COMPASS);
        compo_picturebox_set_rotation(pic, f_compass->angle);

        compo_textbox_t *text = compo_getobj_byid(COMPO_ID_COMPASS_TEXT);
        func_compass_num_show(text);

        text = compo_getobj_byid(COMPO_ID_COMPASS_DIR_TEXT);
        func_compass_dir_show(text, false);

        if (f_compass->angle >= 3600) {
            f_compass->angle = 0;
        }
    }
}

//指南针功能事件处理
static void func_compass_process(void)
{
    func_compass_demo_process();
    func_process();
}

//指南针功能消息处理
static void func_compass_message(size_msg_t msg)
{
    switch (msg) {
    case MSG_CTP_CLICK:
        break;

    case MSG_CTP_SHORT_UP:
        break;

    case MSG_CTP_SHORT_DOWN:
        break;

    case MSG_CTP_LONG:
        break;

    default:
        func_message(msg);
        break;
    }
}

//进入指南针功能
static void func_compass_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_compass_t));
    func_cb.frm_main = func_compass_form_create();
}

//退出指南针功能
static void func_compass_exit(void)
{
    tft_set_temode(DEFAULT_TE_MODE);
    func_cb.last = FUNC_COMPASS;
}

//指南针功能
void func_compass(void)
{
    printf("%s\n", __func__);
    func_compass_enter();
    while (func_cb.sta == FUNC_COMPASS) {
        func_compass_process();
        func_compass_message(msg_dequeue());
    }
    func_compass_exit();
}
