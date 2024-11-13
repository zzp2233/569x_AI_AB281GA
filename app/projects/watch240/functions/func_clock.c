#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

extern u16 func_clock_preview_get_type(void);

//立方体图标列表(固定6项)
static const compo_cube_item_t tbl_menu_cube[] =
{
    {UI_BUF_DIALPLATE_CUBE_CALL_BIN,             FUNC_CALL},
    {UI_BUF_DIALPLATE_CUBE_HEART_RATE_BIN,       FUNC_HEARTRATE},
    {UI_BUF_DIALPLATE_CUBE_MUSIC_BIN,            FUNC_BT},
    {UI_BUF_DIALPLATE_CUBE_SLEEP_BIN,            FUNC_SLEEP},
    {UI_BUF_DIALPLATE_CUBE_SPORT_BIN,            FUNC_SPORT},
    {UI_BUF_DIALPLATE_CUBE_STEP_BIN,             FUNC_SLEEP},
};


//蝴蝶表盘
#define BTF_MIN_WING                20                      //翅膀挥动最小幅度
#define BTF_MAX_WING                25                      //翅膀挥动最大幅度
#define BTF_MOVE_MAX_WING           35                      //飞舞时翅膀挥动最大幅度

#define BTF_Y_POS                   30                      //主体相对于屏幕中心Y的偏移
#define BTF_LIGHT_NUM               20                      //左+右边光晕泡泡数量
#define BTF_STATIC_FLAP_EXPIRE      5                       //静态挥动翅膀频率,ms
#define BTF_MOVE_FLAP_EXPIRE        2                       //静态挥动翅膀频率,ms
#define BTF_LEFT_EDGE               GUI_SCREEN_WIDTH / 3    //点击范围左边距判断
#define BTF_RIGHT_EDGE              GUI_SCREEN_WIDTH / 3 * 2//点击范围右边距判断

//立方体
#define CUBE_RADIUS                 (gui_image_get_size(tbl_menu_cube[0].res_addr).wid)/2          //切图正方形一半
#define KALE_EDGE_SPACE             2                                                              //边缘距离
#define KALE_ICON_SPACE             2                                                              //图标距离
#define KALE_ICON_OUTER_SIZE        (gui_image_get_size(tbl_menu_cube[0].res_addr).wid)            //外圈图标大小
#define KALE_ICON_INNER_SIZE        (KALE_ICON_OUTER_SIZE / 5 * 2)                                 //内圈图标大小
#define KALE_REFRASH_EXPIRE         4                                                              //刷新间隔 uint：ms
#define KALE_SWITCH_DR              2                                                              //单次变更R值
#define KALE_ROTATE_ANGLE           25


#define DIALPLATE_NUM               (sizeof(dialplate_info) / sizeof(u32))
#define DIALPLATE_BTF_IDX           DIALPLATE_NUM - 1        //蝴蝶表盘默认最后一个
#define DIALPLATE_CUBE_IDX          DIALPLATE_NUM - 2        //立方体表盘默认倒数第二个

const u32 dialplate_info[] =
{
    UI_BUF_DIALPLATE_1_BIN,
#if !SECURITY_TRANSITCODE_EN
//    UI_BUF_DIALPLATE_2_BIN,
    UI_BUF_DIALPLATE_5_BIN,
    UI_BUF_DIALPLATE_6_BIN,
//    UI_BUF_DIALPLATE_7_BIN,
    UI_BUF_DIALPLATE_4_BIN,
#endif
    UI_BUF_DIALPLATE_CUBE_BIN,
    UI_BUF_DIALPLATE_BTF_BIN,

};

//表盘快捷按钮编号表 对应表盘工具
const u8 quick_btn_tbl[] =
{
    [0]     = FUNC_NULL,                    //空
    [1]     = FUNC_HEARTRATE,               //心率
    [2]     = FUNC_BT,                      //音乐
    [3]     = FUNC_ALARM_CLOCK,             //闹钟
    [4]     = FUNC_BLOOD_OXYGEN,            //血氧
    [5]     = FUNC_NULL,                    //血糖
    [6]     = FUNC_BLOOD_PRESSURE,          //血压
    [7]     = FUNC_BREATHE,                 //呼吸
    [8]     = FUNC_CALCULATOR,              //计算器

    [9]     = FUNC_CAMERA,                  //相机
    [10]    = FUNC_TIMER,                   //定时器
    [11]    = FUNC_SLEEP,                   //睡眠
    [12]    = FUNC_STOPWATCH,               //秒表
    [13]    = FUNC_WEATHER,                 //天气
    [14]    = FUNC_GAME,                    //游戏
    [15]    = FUNC_STYLE,                   //菜单风格
    [16]    = FUNC_ALTITUDE,                //海拔
    [17]    = FUNC_MAP,                     //地图
    [18]    = FUNC_MESSAGE,                 //消息
    [19]    = FUNC_SCAN,                    //扫一扫
    [20]    = FUNC_VOICE,                   //语音助手
#if SECURITY_PAY_EN
    [21]    = FUNC_ALIPAY,                  //支付宝
#else
    [21]    = FUNC_NULL,                    //支付宝
#endif // SECURITY_PAY_EN
    [22]    = FUNC_COMPASS,                 //指南针
    [23]    = FUNC_ADDRESS_BOOK,            //电话簿
    [24]    = FUNC_SPORT,                   //运动
    [25]    = FUNC_CALL,                    //电话
    [26]    = FUNC_FINDPHONE,               //寻找手机
    [27]    = FUNC_CALENDAER,               //日历
    [28]    = FUNC_ACTIVITY,                //活动记录
    [29]    = FUNC_FLASHLIGHT,              //手电筒
    [30]    = FUNC_SETTING,                 //设置
};

enum
{
    COMPO_ID_BTFLY = 1,
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

    COMPO_ID_CUBE,

};

int compo_get_animation_id(void);
void compo_animation_manual_next(compo_animation_t *animation);
void func_switch_to(u8 sta, u16 switch_mode);

u8 func_clock_get_max_dialplate_num(void)
{
    return (sizeof(dialplate_info) / sizeof(u32));
}

u32 func_clock_get_dialplate_info(u8 index)
{
    return dialplate_info[index];
}

//点进图标进入应用
static void func_clock_cube_disk_icon_click(void)
{
    if (sys_cb.dialplate_index != DIALPLATE_CUBE_IDX)
    {
        return;
    }

    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    point_t pt = ctp_get_sxy();

    int icon_idx = compo_cube_get_idx(cube, pt.x, pt.y);
    u8 func_sta;
    if (icon_idx < 0 || icon_idx >= CUBE_ITEM_CNT)
    {
        return;
    }
    //根据图标索引获取应用ID
    func_sta = tbl_menu_cube[icon_idx].func_sta;

    //切入应用
    if (func_sta > 0)
    {
//        func_switching(FUNC_SWITCH_FADE_OUT | FUNC_SWITCH_AUTO, NULL);
        func_switch_to(func_sta, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
        func_cb.sta = func_sta;
        func_cb.menu_idx = icon_idx;                //记住当前编号
    }
}

//触摸某一个区域触发飞舞
void func_clock_butterfly_click(void)
{
    if (sys_cb.dialplate_index != DIALPLATE_BTF_IDX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

    s32 sx, sy, x, y;
    compo_butterfly_t *btfly = compo_getobj_byid(COMPO_ID_BTFLY);
    ctp_get_cur_point(&sx, &sy, &x, &y);
    if (sx < BTF_LEFT_EDGE)       //左边
    {
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

//单击按钮
static void func_clock_button_click(void)
{
    u16 btn_id = compo_get_button_id();
    u16 animation_id = compo_get_animation_id();
    if (btn_id)
    {
        func_switch_to(quick_btn_tbl[btn_id], FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
    }
    else if (animation_id)
    {
        compo_animation_t *animation = compo_getobj_byid(animation_id);
        if (animation->bond_data == COMPO_BOND_IMAGE_CLICK)
        {
            compo_animation_manual_next(animation);
        }
        else if (animation->bond_data == COMPO_BOND_ANIMATION_AREA_CLICK)
        {
            compo_animation_click_set_vis(animation);
        }
    }
    func_clock_butterfly_click();
    func_clock_cube_disk_icon_click();                //单击图标
}

//立方体表盘
compo_form_t *func_clock_cube_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);       //菜单一般创建在底层

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_DIALPLATE_CUBE_BG_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20);

    //创建立方体菜单
    compo_cube_t *cube = compo_cube_create(frm, CUBE_RADIUS, tbl_menu_cube, CUBE_ITEM_CNT);
    compo_cube_set_pos(cube, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20);
    compo_setid(cube, COMPO_ID_CUBE);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_46_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_HOUR);


    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_46_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_MINUTE);

    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 180, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 140, 300, 70);
    compo_bonddata(txt, COMPO_BOND_DATE);

    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_46_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 105, 300, 70);
    compo_textbox_set(txt, ":");
    compo_setid(txt, COMPO_ID_TIME_DOT);

    //compo_cube_update(cube);
    return frm;
}

//蝴表盘
compo_form_t *func_clock_butterfly_form_create(void)
{
    tft_set_temode(0);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_butterfly_res_t btf_res;
    btf_res.res_body = UI_BUF_DIALPLATE_BUTTERFLY_BODY_BIN;
    btf_res.res_wing_down = UI_BUF_DIALPLATE_BUTTERFLY_DOWN_BIN;
    btf_res.res_wing_up = UI_BUF_DIALPLATE_BUTTERFLY_UP_BIN;

    compo_butterfly_t *btfly = compo_butterfly_create(frm, &btf_res);
    compo_butterfly_set_pos(btfly, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + BTF_Y_POS);
    compo_setid(btfly, COMPO_ID_BTFLY);

    //新建光晕泡泡
    for(u8 i=0; i<BTF_LIGHT_NUM; i++)
    {
        compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_DIALPLATE_BUTTERFLY_LIGHT_BIN);
        compo_setid(pic, COMPO_ID_L_LIGHT1 + i);
        compo_picturebox_set_visible(pic, false);
        widget_set_top(pic->img, true);
    }

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_46_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 50, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_HOUR);


    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_46_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 50, GUI_SCREEN_CENTER_Y - 100, 300, 70);
    compo_bonddata(txt, COMPO_BOND_MINUTE);

    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 70, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 30, 300, 70);
    compo_bonddata(txt, COMPO_BOND_DATE);

    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_46_BIN);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 140, 300, 70);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 100, 260, 70);
    compo_textbox_set(txt, ":");
    compo_setid(txt, COMPO_ID_TIME_DOT);

    return frm;
}


compo_form_t *func_clock_form_create(void)
{
    compo_form_t *frm;
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        frm = func_clock_butterfly_form_create();
    }
    else if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
    {
        frm = func_clock_cube_form_create();
    }
    else
    {
        u32 base_addr = dialplate_info[sys_cb.dialplate_index];
        u16 compo_num = bsp_uitool_header_phrase(base_addr);
        if (!compo_num)
        {
            halt(HALT_GUI_DIALPLATE_HEAD);
        }

        frm = compo_form_create(true);
        bsp_uitool_create(frm, base_addr, compo_num);
        return frm;
    }


    return frm;
}

void func_clock_butterfly_set_light_visible(bool visible)
{
    if (sys_cb.dialplate_index != DIALPLATE_BTF_IDX)
    {
        return;
    }

    sys_cb.dialplate_btf_ready = visible;
    compo_picturebox_t *pic;
    for(u8 i=0; i<BTF_LIGHT_NUM; i++)
    {
        pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + i);
        if (compo_picturebox_get_visible(pic))
        {
            compo_picturebox_set_visible(pic, visible);
        }
    }
}

void func_clock_butterfly_process(void)
{
    if (sys_cb.dialplate_index != DIALPLATE_BTF_IDX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

    compo_butterfly_t *btfly = compo_getobj_byid(COMPO_ID_BTFLY);

    static s8 direction = -1;
    static u32 ticks = 0;

    static u32 ticks_light = 0;
    static s16 btfly_wind_angle = BTF_MAX_WING;
    u8 expire = BTF_STATIC_FLAP_EXPIRE;
    u8 max_wing = BTF_MAX_WING;

    compo_picturebox_t *pic;

    //移动挥动翅膀加速
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

    //挥动翅膀处理
    if (tick_check_expire(ticks, expire))
    {
        ticks = tick_get();
        if (btfly_wind_angle == BTF_MAX_WING)
        {
            direction = 1;
        }
        else if(btfly_wind_angle == max_wing)
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

    //光晕处理
    if (tick_check_expire(ticks_light, expire * 2))
    {
        ticks_light = tick_get();
        for(u8 i=0; i<BTF_LIGHT_NUM; i++)
        {
            pic = compo_getobj_byid(COMPO_ID_L_LIGHT1 + i);
            if (compo_picturebox_get_visible(pic) == true)
            {
                u8 alpha = compo_picturebox_get_alpha(pic);
                alpha = !alpha ? 204 :alpha;
                alpha -= 5;
                s16 size = 50 *(255 - alpha) / 255;
                if (size >= 10)
                {
                    compo_picturebox_set_size(pic, size,size);
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

//子功能公共事件处理
void func_clock_sub_process(void)
{
    func_process();                                     //刷新UI
}

//子功能公共消息处理
void func_clock_sub_message(size_msg_t msg)
{
    func_message(msg);
}

//时钟表盘功能事件处理
static void func_clock_process(void)
{
    if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
    {
        compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
        compo_cube_move(cube);
    }
    func_process();                                  //刷新UI
}

//static void func_clock_swith(bool dir)
//{
//    if (dir) {
//        sys_cb.dialplate_index++;
//        if (sys_cb.dialplate_index > DIALPLATE_NUM - 1) {
//            sys_cb.dialplate_index = 0;
//        }
//    } else {
//        if (sys_cb.dialplate_index == 0) {
//            sys_cb.dialplate_index = DIALPLATE_NUM - 1;
//        } else {
//            sys_cb.dialplate_index--;
//        }
//    }
//    printf("dialplate index:%d\n", sys_cb.dialplate_index);
//    compo_form_destroy(func_cb.frm_main);
//    func_cb.frm_main = func_clock_form_create();
//}

//时钟表盘功能消息处理
static void func_clock_message(size_msg_t msg)
{

    point_t pt = ctp_get_sxy();
    s16 cube_limit_x = (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid) / 2;
    s16 cube_limit_y = (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid) / 2;
    bool flag_cube_touch_x = (pt.x >= cube_limit_x) && (pt.x <= (cube_limit_x + gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid));
    bool flag_cube_touch_y = (pt.y >= cube_limit_y) && (pt.y <= (cube_limit_y + gui_image_get_size(UI_BUF_DIALPLATE_CUBE_BG_BIN).wid));


    if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX && flag_cube_touch_x && flag_cube_touch_y)
    {
        if (msg == MSG_CTP_TOUCH)
        {
            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            //移动过程中，触屏停止。重新进入到开始拖动模式
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_DRAG);
        }
        else if (msg >= MSG_CTP_SHORT_LEFT && msg <= MSG_CTP_SHORT_DOWN)
        {
            return;
        }

    }
    static bool time_visible = 0;
    switch (msg)
    {
        case MSG_CTP_SHORT_UP:
//        func_clock_sub_pullup();                //上拉菜单
            func_clock_butterfly_set_light_visible(false);
            func_switch_to(FUNC_CARD, FUNC_SWITCH_MENU_PULLUP_UP);  //上拉卡片界面
            if (func_cb.sta == FUNC_CLOCK)
            {
                func_clock_butterfly_set_light_visible(true);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
//        func_clock_sub_side();                  //右拉边菜单
            func_switch_to(FUNC_SIDEBAR, FUNC_SWITCH_LR_ZOOM_RIGHT);  //右滑界面
            break;

        case MSG_CTP_SHORT_DOWN:
            func_clock_sub_dropdown();              //下拉菜单
            break;

        case MSG_CTP_CLICK:
            func_clock_button_click();
            break;

        case MSG_SYS_500MS:
            //秒跳动处理

            if (sys_cb.dialplate_index >= DIALPLATE_CUBE_IDX)
            {
                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TIME_DOT);
                compo_textbox_set_visible(txt, time_visible);
                time_visible ^= 1;
            }

            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
            {
                compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
                compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_FORWARD);
            }
            break;

        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
            if (sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
            {
                compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
                compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_BACKWARD);
            }
            break;

        case MSG_CTP_LONG:
            if (func_clock_preview_get_type() == PREVIEW_ROTARY_STYLE)
            {
                func_cb.sta = FUNC_CLOCK_PREVIEW;
            }
            else
            {
                func_switch_to(FUNC_CLOCK_PREVIEW, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);                    //切换回主时钟
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入时钟表盘功能
static void func_clock_enter(void)
{
    uteModuleGuiCommonGetCurrWatchIndex(&sys_cb.dialplate_index);
    func_cb.f_cb = func_zalloc(sizeof(f_clock_t));
    func_cb.frm_main = func_clock_form_create();
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        sys_cb.dialplate_btf_ready = true;
        tft_set_temode(0);
    }
}

//退出时钟表盘功能
static void func_clock_exit(void)
{
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        sys_cb.dialplate_btf_ready = false;
        tft_set_temode(DEFAULT_TE_MODE);
    }
    func_cb.last = FUNC_CLOCK;
}

//时钟表盘功能
void func_clock(void)
{
    printf("%s\n", __func__);
    func_clock_enter();
    while (func_cb.sta == FUNC_CLOCK)
    {
        func_clock_process();
        func_clock_message(msg_dequeue());
    }
    func_clock_exit();
}
