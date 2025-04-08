#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_project_config.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT

// 蝴蝶表盘
#define BTF_MIN_WING 20      // 翅膀挥动最小幅度
#define BTF_MAX_WING 25      // 翅膀挥动最大幅度
#define BTF_MOVE_MAX_WING 35 // 飞舞时翅膀挥动最大幅度

#define BTF_Y_POS 30                            // 主体相对于屏幕中心Y的偏移
#define BTF_LIGHT_NUM 20                        // 左+右边光晕泡泡数量
#define BTF_STATIC_FLAP_EXPIRE 5                // 静态挥动翅膀频率,ms
#define BTF_MOVE_FLAP_EXPIRE 2                  // 静态挥动翅膀频率,ms
#define BTF_LEFT_EDGE GUI_SCREEN_WIDTH / 3      // 点击范围左边距判断
#define BTF_RIGHT_EDGE GUI_SCREEN_WIDTH / 3 * 2 // 点击范围右边距判断

enum
{
    COMPO_ID_BTFLY = 0x32,
    COMPO_ID_TIME_DOT,
    COMPO_ID_L_LIGHT1,
    COMPO_ID_L_LIGHT2,
    COMPO_ID_L_LIGHT3,
    COMPO_ID_L_LIGHT4,
    COMPO_ID_L_LIGHT5,
    COMPO_ID_L_LIGHT6,
    COMPO_ID_L_LIGHT7,
    COMPO_ID_L_LIGHT8,
    COMPO_ID_L_LIGHT9,
    COMPO_ID_L_LIGHT10,
    COMPO_ID_R_LIGHT1,
    COMPO_ID_R_LIGHT2,
    COMPO_ID_R_LIGHT3,
    COMPO_ID_R_LIGHT4,
    COMPO_ID_R_LIGHT5,
    COMPO_ID_R_LIGHT6,
    COMPO_ID_R_LIGHT7,
    COMPO_ID_R_LIGHT8,
    COMPO_ID_R_LIGHT9,
    COMPO_ID_R_LIGHT10,
};

// 触摸某一个区域触发飞舞
void func_clock_butterfly_click(void)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_BTF_INDEX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

    s32 sx, sy, x, y;
    compo_butterfly_t *btfly = compo_getobj_byid(COMPO_ID_BTFLY);
    ctp_get_cur_point(&sx, &sy, &x, &y);
    if (sx < BTF_LEFT_EDGE)
    {
        // 左边
        compo_butterfly_move_control(btfly, COMPO_BUTTERFLY_MOVE_CMD_LEFT);
    }
    else if (sx > BTF_RIGHT_EDGE)
    {
        compo_butterfly_move_control(btfly, COMPO_BUTTERFLY_MOVE_CMD_RIGHT);
    }
    else
    {
        compo_butterfly_move_control(btfly, COMPO_BUTTERFLY_MOVE_CMD_LOOP);
    }
}

// 蝴表盘
compo_form_t *func_clock_butterfly_form_create(void)
{
    tft_set_temode(0);
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_butterfly_res_t btf_res;
    btf_res.res_body = UI_BUF_DIALPLATE_BUTTERFLY_BODY_BIN;
    btf_res.res_wing_down = UI_BUF_DIALPLATE_BUTTERFLY_DOWN_BIN;
    btf_res.res_wing_up = UI_BUF_DIALPLATE_BUTTERFLY_UP_BIN;

    compo_butterfly_t *btfly = compo_butterfly_create(frm, &btf_res);
    compo_butterfly_set_pos(btfly, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + BTF_Y_POS);
    widget_image3d_set_rotation_center(btfly->img_body,gui_image_get_size(UI_BUF_DIALPLATE_BUTTERFLY_BODY_BIN).wid/2, gui_image_get_size(UI_BUF_DIALPLATE_BUTTERFLY_BODY_BIN).hei/2);
    widget_set_pos(btfly->img_body, GUI_SCREEN_CENTER_X+4, GUI_SCREEN_CENTER_Y);
    compo_setid(btfly, COMPO_ID_BTFLY);

    // 新建光晕泡泡
    for (u8 i = 0; i < BTF_LIGHT_NUM; i++)
    {
        compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_DIALPLATE_BUTTERFLY_LIGHT_BIN);
        compo_setid(pic, COMPO_ID_L_LIGHT1 + i);
        compo_picturebox_set_visible(pic, false);
        widget_set_top(pic->img, true);
    }

    // 新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 40, GUI_SCREEN_CENTER_Y - 120, 300, 70);
    compo_bonddata(txt, COMPO_BOND_HOUR);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 40, GUI_SCREEN_CENTER_Y - 120, 300, 70);
    compo_bonddata(txt, COMPO_BOND_MINUTE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 70, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 60, 290, 70);
    compo_bonddata(txt, COMPO_BOND_DATE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 120, 260, 70);
    compo_textbox_set(txt, ":");
    compo_setid(txt, COMPO_ID_TIME_DOT);

    return frm;
}

void func_clock_butterfly_set_light_visible(bool visible)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_BTF_INDEX)
    {
        return;
    }

    sys_cb.dialplate_btf_ready = visible;
    compo_picturebox_t *pic;
    for (u8 i = 0; i < BTF_LIGHT_NUM; i++)
    {
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + i);
        if (compo_picturebox_get_visible(pic))
        {
            compo_picturebox_set_visible(pic, visible);
        }
    }
}

void func_clock_butterfly_process_do(void)
{
    compo_butterfly_t *btfly = compo_getobj_byid(COMPO_ID_BTFLY);

    static s8 direction = -1;
    static u32 ticks = 0;

    static u32 ticks_light = 0;
    static s16 btfly_wind_angle = BTF_MAX_WING;
    u8 expire = BTF_STATIC_FLAP_EXPIRE;
    u8 max_wing = BTF_MAX_WING;

    compo_picturebox_t *pic;

    // 移动挥动翅膀加速
    if (compo_butterfly_get_sta(btfly) == COMPO_BUTTERFLY_STA_MOVE)
    {
        expire = BTF_MOVE_FLAP_EXPIRE;
        max_wing = BTF_MOVE_MAX_WING;
    }
    else
    {
        if (btfly_wind_angle > max_wing)
        {
            btfly_wind_angle = max_wing;
        }
    }

    // 挥动翅膀处理
    if (tick_check_expire(ticks, expire))
    {
        ticks = tick_get();
        if (btfly_wind_angle == BTF_MAX_WING)
        {
            direction = 1;
        }
        else if (btfly_wind_angle == max_wing)
        {
            direction = -1;
        }
        btfly_wind_angle += direction;
        compo_butterfly_flap(btfly, btfly_wind_angle);
        compo_butterfly_update(btfly);

        u8 index = get_random(BTF_LIGHT_NUM >> 1);
        s32 rota = compo_butterfly_get_rotation(btfly);
        s16 z = compo_butterfly_get_z(btfly);
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + index);

        if (compo_picturebox_get_visible(pic) == false)
        {
            s16 x = 100 + get_random(70);
            s16 y = -get_random(200 + rota / 10);
            compo_picturebox_set_rotation_center(pic, x, y);
            compo_picturebox_set_rotation(pic, rota);
            compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 170 + BTF_Y_POS + z * 30 / 100);
            compo_picturebox_set_size(pic, 1, 1);
            compo_picturebox_set_alpha(pic, 0);
            compo_picturebox_set_visible(pic, true);
        }

        index = get_random(BTF_LIGHT_NUM >> 1) + (BTF_LIGHT_NUM >> 1);
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + index);
        if (compo_picturebox_get_visible(pic) == false)
        {
            s16 x = -100 - get_random(70);
            s16 y = -get_random(200 + rota / 10);
            compo_picturebox_set_rotation_center(pic, x, y);
            compo_picturebox_set_rotation(pic, rota);
            compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 170 + BTF_Y_POS + z * 30 / 100);
            compo_picturebox_set_size(pic, 1, 1);
            compo_picturebox_set_alpha(pic, 0);
            compo_picturebox_set_visible(pic, true);
        }
    }

    compo_butterfly_move(btfly);

    // 光晕处理
    if (tick_check_expire(ticks_light, expire * 2))
    {
        ticks_light = tick_get();
        for (u8 i = 0; i < BTF_LIGHT_NUM; i++)
        {
            pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + i);
            if (compo_picturebox_get_visible(pic) == true)
            {
                u8 alpha = compo_picturebox_get_alpha(pic);
                alpha = !alpha ? 204 : alpha;
                alpha -= 5;
                s16 size = 50 * (255 - alpha) / 255;
                if (size >= 10)
                {
                    compo_picturebox_set_size(pic, size, size);
                    compo_picturebox_set_alpha(pic, alpha);
                }

                s32 rota = compo_butterfly_get_rotation(btfly);
                compo_picturebox_set_rotation(pic, rota);
                if (alpha < 30)
                {
                    compo_picturebox_set_visible(pic, false);
                }
            }
        }
    }
}

// 地图功能事件处理
void func_clock_butterfly_process(void)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_BTF_INDEX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

    func_clock_butterfly_process_do();
}

// 地图功能消息处理
bool func_clock_butterfly_message(size_msg_t msg)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_BTF_INDEX)
    {
        return false;
    }

    bool intercept_msg = false;
    static bool time_visible = 0;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_clock_butterfly_click();
            intercept_msg = true;
            break;

        case MSG_SYS_500MS: // 秒跳动处理
        {
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TIME_DOT);
            compo_textbox_set_visible(txt, time_visible);
            time_visible ^= 1;
            // intercept_msg = true;
        }
        break;

    }

    return intercept_msg;
}

#endif
