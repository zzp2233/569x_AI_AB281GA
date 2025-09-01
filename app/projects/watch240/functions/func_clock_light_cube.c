#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_project_config.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_WATCHS_LIGHT_CUBE_DIAL_SUPPORT

// 立方体
#define CUBE_RADIUS_MAX ((gui_image_get_size(tbl_menu_cube[0].res_addr).wid) / 2 )+15     // 切图正方形一半
#define CUBE_RADIUS_MIN ((gui_image_get_size(tbl_menu_cube[0].res_addr).wid) / 2 )     // 切图正方形一半
#define KALE_EDGE_SPACE 2                                                        // 边缘距离
#define KALE_ICON_SPACE 2                                                        // 图标距离
#define KALE_ICON_OUTER_SIZE (gui_image_get_size(tbl_menu_cube[0].res_addr).wid) // 外圈图标大小
#define KALE_ICON_INNER_SIZE (KALE_ICON_OUTER_SIZE / 5 * 2)                      // 内圈图标大小
#define KALE_REFRASH_EXPIRE 4                                                    // 刷新间隔 uint：ms
#define KALE_SWITCH_DR 2                                                         // 单次变更R值
#define KALE_ROTATE_ANGLE 25

static bool click = false;
enum
{
    BATTER_3D_CLICK_STA_NONE,   //None
    BATTER_3D_CLICK_STA_DEC,    //缩小
    BATTER_3D_CLICK_STA_INC,    //放大
    BATTER_3D_CLICK_STA_END,    //结束
    BATTER_3D_CLICK_STA_STOP,   //停顿
};


#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

static const compo_cube_item_t tbl_menu_cube[] =
{
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_FIND_PHONE_BIN, FUNC_FINDPHONE},
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_MUSIC_BIN, FUNC_BT},
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_SETTING_BIN, FUNC_SETTING},
#endif
#if 0//UTE_MODULE_SCREENS_ROTARY_MENUSTYLE_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_THEME_BIN, FUNC_MENUSTYLE},
#else
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_THEME_BIN, FUNC_STYLE},
#endif
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_VOICE_BIN, FUNC_VOICE},
#endif
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_WEATHER_BIN, FUNC_WEATHER},
#endif
};

// 光束图片
#define UTE_WATCHS_CUBE_LIGHT_PIC_ADDR UI_BUF_DIALPLATE_CUBE_LIGHT_LIGHT_00_BIN

// 光束数量
#define UTE_WATCHS_CUBE_LIGHT_ELE_CNT 12

// 时间数字字体
#define UTE_WATCHS_CUBE_TIME_NUM_FONT UI_BUF_0FONT_FONT_NUM_48_BIN
// 日期数字字体
#define UTE_WATCHS_CUBE_DATE_NUM_FONT UI_BUF_0FONT_FONT_NUM_32_BIN

// 时间小时数字位置
#define CUBE_HOUR_X         (GUI_SCREEN_CENTER_X - 40)
#define CUBE_HOUR_Y         (GUI_SCREEN_CENTER_Y - 100)
#define CUBE_HOUR_WIDTH     300
#define CUBE_HOUR_HEIGHT    70
// 时间:位置
#define CUBE_DOT_X          GUI_SCREEN_CENTER_X
#define CUBE_DOT_Y          (GUI_SCREEN_CENTER_Y - 104)
#define CUBE_DOT_WIDTH      300
#define CUBE_DOT_HEIGHT     70
// 时间分钟数字位置
#define CUBE_MIN_X          (GUI_SCREEN_CENTER_X + 40)
#define CUBE_MIN_Y          (GUI_SCREEN_CENTER_Y - 100)
#define CUBE_MIN_WIDTH      300
#define CUBE_MIN_HEIGHT     70
// 日期数字位置
#define CUBE_DATE_X          GUI_SCREEN_CENTER_X
#define CUBE_DATE_Y          (GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_X)
#define CUBE_DATE_WIDTH      300
#define CUBE_DATE_HEIGHT     70

#else

static const compo_cube_item_t tbl_menu_cube[] =
{
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_FIND_PHONE_BIN, FUNC_FINDPHONE},
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_MUSIC_BIN, FUNC_BT},
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_SETTING_BIN, FUNC_SETTING},
#endif
#if UTE_MODULE_SCREENS_ROTARY_MENUSTYLE_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_THEME_BIN, FUNC_MENUSTYLE},
#else
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_THEME_BIN, FUNC_STYLE},
#endif
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_VOICE_BIN, FUNC_VOICE},
#endif
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {UI_BUF_DIALPLATE_CUBE_LIGHT_MF_WEATHER_BIN, FUNC_WEATHER},
#endif
};

// 光束图片
#define UTE_WATCHS_CUBE_LIGHT_PIC_ADDR UI_BUF_DIALPLATE_CUBE_LIGHT_LIGHT_00_BIN

// 光束数量
#define UTE_WATCHS_CUBE_LIGHT_ELE_CNT 20

// 时间数字字体
#define UTE_WATCHS_CUBE_TIME_NUM_FONT UI_BUF_0FONT_FONT_NUM_48_BIN
// 日期数字字体
#define UTE_WATCHS_CUBE_DATE_NUM_FONT UI_BUF_0FONT_FONT_NUM_32_BIN

// 时间小时数字位置
#define CUBE_HOUR_X         (GUI_SCREEN_CENTER_X - 40)
#define CUBE_HOUR_Y         (GUI_SCREEN_CENTER_Y - 100)
#define CUBE_HOUR_WIDTH     300
#define CUBE_HOUR_HEIGHT    70
// 时间:位置
#define CUBE_DOT_X          GUI_SCREEN_CENTER_X
#define CUBE_DOT_Y          (GUI_SCREEN_CENTER_Y - 104)
#define CUBE_DOT_WIDTH      300
#define CUBE_DOT_HEIGHT     70
// 时间分钟数字位置
#define CUBE_MIN_X          (GUI_SCREEN_CENTER_X + 40)
#define CUBE_MIN_Y          (GUI_SCREEN_CENTER_Y - 100)
#define CUBE_MIN_WIDTH      300
#define CUBE_MIN_HEIGHT     70
// 日期数字位置
#define CUBE_DATE_X          GUI_SCREEN_CENTER_X
#define CUBE_DATE_Y          (GUI_SCREEN_CENTER_Y + GUI_SCREEN_CENTER_X)
#define CUBE_DATE_WIDTH      300
#define CUBE_DATE_HEIGHT     70

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    COMPO_ID_CUBE = 0x32,
    COMPO_ID_TIME_DOT,
};

const rect_t tbl_cube_time_hour_area =
{
    .x = CUBE_HOUR_X,
    .y = CUBE_HOUR_Y,
    .wid = CUBE_HOUR_WIDTH,
    .hei = CUBE_HOUR_HEIGHT,
};

const rect_t tbl_cube_time_dot_area =
{
    .x = CUBE_DOT_X,
    .y = CUBE_DOT_Y,
    .wid = CUBE_DOT_WIDTH,
    .hei = CUBE_DOT_HEIGHT,
};

const rect_t tbl_cube_time_min_area =
{
    .x = CUBE_MIN_X,
    .y = CUBE_MIN_Y,
    .wid = CUBE_MIN_WIDTH,
    .hei = CUBE_MIN_HEIGHT,
};

const rect_t tbl_cube_date_area =
{
    .x = CUBE_DATE_X,
    .y = CUBE_DATE_Y,
    .wid = CUBE_DATE_WIDTH,
    .hei = CUBE_DATE_HEIGHT,
};

// 点进图标进入应用
static void func_clock_light_cube_disk_icon_click(void)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_LIGHT_CUBE_INDEX)
    {
        return;
    }

    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    if(cube == NULL) return;
    point_t pt = ctp_get_sxy();

    int icon_idx = compo_cube_get_idx(cube, pt.x, pt.y);
    u8 func_sta;
    if (icon_idx < 0 || icon_idx >= CUBE_ITEM_CNT)
    {
        return;
    }
    // 根据图标索引获取应用ID
    func_sta = tbl_menu_cube[icon_idx].func_sta;

    // 切入应用
    if (func_sta > 0)
    {
        // func_switching(FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO, NULL);
        func_switch_to(func_sta, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
        // func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx; // 记住当前编号
    }
}

compo_form_t *func_clock_light_cube_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true); // 菜单一般创建在底层

    // 创建立方体菜单
    compo_cube_t *cube = compo_cube_create(frm, CUBE_RADIUS_MAX, tbl_menu_cube, CUBE_ITEM_CNT);
    compo_cube_set_type(cube, COMPO_CUBE_TYPE_POWER);
    compo_cube_add_element(cube, 0, UTE_WATCHS_CUBE_LIGHT_PIC_ADDR, UTE_WATCHS_CUBE_LIGHT_ELE_CNT);
    compo_cube_set_pos(cube, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20);
    compo_setid(cube, COMPO_ID_CUBE);

    // hour
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UTE_WATCHS_CUBE_TIME_NUM_FONT);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, tbl_cube_time_hour_area.x,tbl_cube_time_hour_area.y,tbl_cube_time_hour_area.wid,tbl_cube_time_hour_area.hei);
    compo_bonddata(txt, COMPO_BOND_HOUR);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    // dot
    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UTE_WATCHS_CUBE_TIME_NUM_FONT);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, tbl_cube_time_dot_area.x,tbl_cube_time_dot_area.y,tbl_cube_time_dot_area.wid,tbl_cube_time_dot_area.hei);
    compo_textbox_set(txt, ":");
    compo_setid(txt, COMPO_ID_TIME_DOT);

    // min
    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UTE_WATCHS_CUBE_TIME_NUM_FONT);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, tbl_cube_time_min_area.x,tbl_cube_time_min_area.y,tbl_cube_time_min_area.wid,tbl_cube_time_min_area.hei);
    compo_bonddata(txt, COMPO_BOND_MINUTE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    // date
    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UTE_WATCHS_CUBE_DATE_NUM_FONT);
    //    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 180, 300, 70);
    compo_textbox_set_location(txt, tbl_cube_date_area.x,tbl_cube_date_area.y,tbl_cube_date_area.wid,tbl_cube_date_area.hei);
    compo_bonddata(txt, COMPO_BOND_DATE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    // compo_cube_update(cube);
    return frm;
}

void func_clock_light_cube_process(void)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_LIGHT_CUBE_INDEX)
    {
        return;
    }
    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    if(cube != NULL)
    {
        compo_cube_move(cube);
    }
    static u32 tick = 0;
    static u8 sta = BATTER_3D_CLICK_STA_NONE; //0:None 1:开始缩小; 2:开始放大; 3:结束; 4:停顿
    u32 width_max = cube->ele[0].location.wid;//gui_image_get_size(tbl_menu_ele[0].res_addr).wid / 2;
    static u32 width;
    static u8 stop_cnt = 0;
    // printf("%s,click=%d,flag_move_auto=%d\n",__func__, click,cube->move_cb.flag_move_auto);
    // if (cube->move_cb.flag_move_auto==0)
    // {
    //     if(tick_check_expire(cube->move_cb.tick, 18))
    //     {
    //         s32 da;
    //         cube->move_cb.tick = tick_get();
    //         da = 20;
    //         compo_cube_roll(cube, da);
    //         compo_cube_update(cube);
    //     }
    // }

    if (click)
    {
        if (tick_check_expire(tick, 100))
        {
            tick = tick_get();

            switch (sta)
            {
                case BATTER_3D_CLICK_STA_NONE:
                    width = width_max;
                    if (cube->radius >= CUBE_RADIUS_MAX)
                    {
                        sta = BATTER_3D_CLICK_STA_DEC;
                    }
                    break;

                case BATTER_3D_CLICK_STA_DEC:
                    cube->radius--;
                    //1:x = CUBE_RADIUS_MIN * 1414/1000 : (s32)cube->radius * 1414 / 1000;
                    if ((((s32)cube->radius * 1414 / 1000) / (CUBE_RADIUS_MIN * 1414/1000)) <= width_max)
                    {
                        width--;
                        if (width < 0)
                        {
                            width = 0;
                        }
                    }
                    if (cube->radius <= CUBE_RADIUS_MIN)
                    {
                        width = 0;
                        sta = BATTER_3D_CLICK_STA_STOP;
                    }
                    break;

                case BATTER_3D_CLICK_STA_INC:
                    cube->radius++;

                    if ((((s32)cube->radius * 1414 / 1000) / (CUBE_RADIUS_MIN * 1414/1000)) <= width_max)
                    {
                        width++;
                        if (width > width_max)
                        {
                            width = width_max;
                        }
                    }

                    if (cube->radius >= CUBE_RADIUS_MAX)
                    {
                        width = width_max;
                        sta = BATTER_3D_CLICK_STA_END;
                    }
                    break;

                case BATTER_3D_CLICK_STA_END:
                    click = false;
                    sta = BATTER_3D_CLICK_STA_NONE;
                    break;

                case BATTER_3D_CLICK_STA_STOP:
                    stop_cnt++;
                    if (stop_cnt > 5)
                    {
                        sta = BATTER_3D_CLICK_STA_INC;
                        stop_cnt = 0;
                    }
                    break;
            }

            for (int i=0; i<CUBE_ITEM_CNT; i++)
            {
                widget_image3d_set_r(cube->item_img[i], cube->radius);
            }

            for (int i=0; i<CUBE_ITEM_ELE_CNT; i++)
            {
                if (cube->ele[i].img3d != NULL)
                {
                    widget_set_size(cube->ele[i].img3d, width, cube->ele[i].location.hei);
                }
            }
        }
    }

}

void func_3d_batter_click(void)
{
    if (click == false)
    {
        click = true;
    }
}
// 地图功能消息处理
bool func_clock_light_cube_message(size_msg_t msg)
{
    f_clock_t *f_clock = (f_clock_t *)func_cb.f_cb;
    bool flag_cube_touch = false;

    point_t pt = ctp_get_sxy();
    s16 cube_limit_x = (GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X/2);
    s16 cube_limit_y = (GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y/2);
    bool flag_cube_touch_x = (pt.x >= cube_limit_x) && (pt.x <= (cube_limit_x + GUI_SCREEN_CENTER_X));
    bool flag_cube_touch_y = (pt.y >= cube_limit_y) && (pt.y <= (cube_limit_y + GUI_SCREEN_CENTER_Y));

    if (sys_cb.dialplate_index == UTE_WATCHS_DIALPLATE_LIGHT_CUBE_INDEX && flag_cube_touch_x && flag_cube_touch_y)
    {
        compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
        if (msg == MSG_CTP_TOUCH && cube != NULL)
        {
            // 移动过程中，触屏停止。重新进入到开始拖动模式
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_DRAG);
            flag_cube_touch = true;
        }
        else if (msg >= MSG_CTP_SHORT_LEFT && msg <= MSG_CTP_SHORT_DOWN)
        {
            return true;
        }
    }

    static bool time_visible = 0;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_clock_light_cube_disk_icon_click(); // 单击图标
            func_3d_batter_click();
            flag_cube_touch = true;
            break;

#if !DRV_ENCODER_KEYS_WATCHMAIN_SCREEN_SWITCHOVER_SUPPORT
        case MSG_QDEC_FORWARD: // 向前滚动菜单
        {
            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            if(cube == NULL) return;
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_FORWARD);
            flag_cube_touch = true;
        }
        break;

        case MSG_QDEC_BACKWARD: // 向后滚动菜单
        {
            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            if(cube == NULL) return;
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_BACKWARD);
            flag_cube_touch = true;
        }
        break;
#endif

        case MSG_SYS_500MS: // 秒跳动处理
        {
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TIME_DOT);
            if(txt == NULL) return;
            compo_textbox_set_visible(txt, time_visible);
            time_visible ^= 1;
            // flag_cube_touch = true;
        }
        break;
    }

    return flag_cube_touch;
}

#endif
