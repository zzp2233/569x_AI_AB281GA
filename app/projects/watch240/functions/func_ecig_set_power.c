#include "include.h"
#include "func.h"
#define TRACE_EN 1
#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

// 组件ID
#define BTN_REST_DISP_TIME 300 // 按钮释放时间 ms 级别
#define MIN_POWER_SINGLE 10    // 单发最小功率
#define MAX_POWER_SINGLE 18    // 单发最大功率
#define MIN_POWER_DOUBLE 20    // 双发最小功率
#define MAX_POWER_DOUBLE 30    // 双发最大功率
#define INDEX_MAX_SINGLE 8     // 单发最大 index
#define INDEX_MAX_DOUBLE 10    // 双发最大 index


void save_power_and_index(int power, int index);
void load_power_and_index(int *power, int *index);

enum
{
    COMPO_ID_BTN_ECIG_SET_D_D,  // 单发
    COMPO_ID_BTN_ECIG_SET_S_D,  // 双发
    COMPO_ID_BTN_ECIG_SET_D_D1, // 单发按钮
    COMPO_ID_BTN_ECIG_SET_S_D1, // 双发按钮
    COMPO_ID_BTN_ECIG_BG_S_D,   // 背景双
    COMPO_ID_BTN_ECIG_BG_D_D,   // 背景单
    COMPO_ID_BTN_ECIG_BG_NUM,   // 单进度条
    COMPO_ID_BTN_ECIG_BG1_NUM,  // 双进度条
    COMPO_ID_BTN_ECIG_SET_ADD,  // 加功按钮
    COMPO_ID_BTN_ECIG_SET_DEL,  // 减功按钮
    COMPO_ID_BTN_ECIG_SET_ADD1, // 加功率
    COMPO_ID_BTN_ECIG_SET_DEL1, // 减功率
};

typedef struct f_setpower_t_
{
    uint32_t tick;
    int current_index; // 当前进度条
    int current_power; // 当前功率值
} f_setpower_t;

static int stored_power = 15;
static int stored_index = 15 - MIN_POWER_SINGLE;

// 保存功率和进度条信息
void save_power_and_index(int power, int index)
{
    stored_power = power;
    stored_index = index;
    ecig_set_power(power);
}

// 加载功率和进度条信息
void load_power_and_index(int *power, int *index)
{
    *power = stored_power;
    *index = stored_index;
    if ((get_gear_func() == 0 && *power < MIN_POWER_SINGLE) ||
        (get_gear_func() == 1 && *power < MIN_POWER_DOUBLE))
    {
        *power = get_gear_func() == 0 ? 15 : 25;
        *index = *power - (get_gear_func() == 0 ? MIN_POWER_SINGLE : MIN_POWER_DOUBLE);
    }
}

// 创建功率窗体
compo_form_t *func_ecig_set_power_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_BG_DANG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_D_D);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_BG_SHUANG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_S_D);

    compo_button_t *btn1 = compo_button_create(frm);
    compo_button_set_location(btn1, 43, 46, 50, 50);
    compo_setid(btn1, COMPO_ID_BTN_ECIG_SET_D_D);
    compo_button_set_visible(btn1, get_gear_func() == 0 ? true : false);

    compo_button_t *btn2 = compo_button_create(frm);
    compo_button_set_location(btn2, 197, 46, 50, 50);
    compo_setid(btn2, COMPO_ID_BTN_ECIG_SET_S_D);
    compo_button_set_visible(btn2, get_gear_func() == 1 ? true : false);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_01_BIN);
    compo_picturebox_set_pos(picbox, 43, 46);
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_D_D1);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_01_BIN);
    compo_picturebox_set_pos(picbox, 197, 46);
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_S_D1);

    int total_cnt = (get_gear_func() == 0) ? 9 : 11;
    int power, index;
    load_power_and_index(&power, &index);
    f_setpower->current_index = index;
    f_setpower->current_power = power;

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_DANG_FA_POINT_BIN);
    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);
    compo_picturebox_set_pos(picbox, 121, 165);
    compo_picturebox_set_visible(picbox, get_gear_func() == 0 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG_NUM);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_SHUANG_FA_POINT_BIN);

    if (get_gear_func() == 1 && f_setpower->current_index > INDEX_MAX_DOUBLE)
    {
        f_setpower->current_index = INDEX_MAX_DOUBLE;
    }
    compo_picturebox_cut(picbox, f_setpower->current_index, total_cnt);
    compo_picturebox_set_pos(picbox, 121, 165);
    compo_picturebox_set_visible(picbox, get_gear_func() == 1 ? true : false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_BG1_NUM);

    compo_button_t *btn3 = compo_button_create(frm); // ＋
    compo_button_set_location(btn3, 184, 247, 50, 50);
    compo_button_set_visible(btn3, true);
    compo_setid(btn3, COMPO_ID_BTN_ECIG_SET_ADD);

    compo_button_t *btn4 = compo_button_create(frm); // 减
    compo_button_set_location(btn4, 56, 247, 50, 50);
    compo_button_set_visible(btn4, true);
    compo_setid(btn4, COMPO_ID_BTN_ECIG_SET_DEL);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_BTN_JIA_BIN);
    compo_picturebox_set_pos(picbox, 184, 247);
    compo_picturebox_set_visible(picbox, false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_ADD1);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_POWER1_BTN_JIAN_BIN);
    compo_picturebox_set_pos(picbox, 56, 247);
    compo_picturebox_set_visible(picbox, false);
    compo_setid(picbox, COMPO_ID_BTN_ECIG_SET_DEL1);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_SINGLE]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,33-10, 35,43,45);
    // compo_textbox_set_pos(txt_onoff, 30, 90-17);
    compo_textbox_set(txt, i18n[STR_SINGLE]);

    txt = compo_textbox_create(frm, strlen(i18n[STR_DUAL]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 187-10, 35,43,45);
    // compo_textbox_set_pos(txt_onoff, 30, 90-17);
    compo_textbox_set(txt, i18n[STR_DUAL]);
    return frm;
}

// 触摸按钮效果处理
static void func_ecig_set_power_button_touch_handle(void)
{
    int id = compo_get_button_id();
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;
    compo_button_t *btn_s = (compo_button_t *)compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D);
    compo_button_t *btn_d = (compo_button_t *)compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_D_D);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D1);
    compo_picturebox_t *picbox1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_D_D1);
    compo_picturebox_t *picbox2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_NUM);
    compo_picturebox_t *picbox3 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG1_NUM);
    compo_picturebox_t *picbox4 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);
    compo_picturebox_t *picbox5 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_D_D);
    compo_picturebox_t *picbox_add = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_ADD1);
    compo_picturebox_t *picbox_del = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_DEL1);
    switch (id)
    {
        case COMPO_ID_BTN_ECIG_SET_S_D:
            test_2st_gear_func();
            compo_button_set_visible(btn_d, false); // 隐藏单发按钮
            compo_button_set_visible(btn_s, true);  // 显示双发按钮
            compo_picturebox_set_visible(picbox1, false);
            compo_picturebox_set_visible(picbox, true);
            compo_picturebox_set_visible(picbox2, false);
            compo_picturebox_set_visible(picbox3, true);
            compo_picturebox_set_visible(picbox5, false);
            compo_picturebox_set_visible(picbox4, true);
            f_setpower->current_index = 25 - MIN_POWER_DOUBLE;
            if (f_setpower->current_index > INDEX_MAX_DOUBLE)
            {
                f_setpower->current_index = INDEX_MAX_DOUBLE;
            }
            f_setpower->current_power = 25;
            compo_picturebox_cut(picbox3, f_setpower->current_index, 11);

            printf("111Switched to Double mode: total_cnt = 11, current_index = %d, current_power = %d\n",
                   f_setpower->current_index, f_setpower->current_power);
            break;
        case COMPO_ID_BTN_ECIG_SET_D_D:
            test_1st_gear_func();
            compo_button_set_visible(btn_s, false); // 隐藏双发按钮
            compo_button_set_visible(btn_d, true);  // 显示单发按钮
            compo_picturebox_set_visible(picbox, false);
            compo_picturebox_set_visible(picbox1, true);
            compo_picturebox_set_visible(picbox3, false);
            compo_picturebox_set_visible(picbox2, true);
            compo_picturebox_set_visible(picbox4, false);
            compo_picturebox_set_visible(picbox5, true);
            f_setpower->current_index = 15 - MIN_POWER_SINGLE;
            f_setpower->current_power = 15;
            compo_picturebox_cut(picbox2, f_setpower->current_index, 9);
            ecig_set_power(f_setpower->current_power);
            printf("222Switched to Single mode: total_cnt = 9, current_index = %d, current_power = %d\n",
                   f_setpower->current_index, f_setpower->current_power);
            break;
        case COMPO_ID_BTN_ECIG_SET_ADD:
            if (get_gear_func() == 0)
            {
                if (f_setpower->current_index < INDEX_MAX_SINGLE)
                {
                    f_setpower->current_index++;
                    f_setpower->current_power++;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 9);
                }
            }
            else
            {
                if (f_setpower->current_index < INDEX_MAX_DOUBLE)
                {
                    f_setpower->current_index++;
                    f_setpower->current_power++;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_cut(picbox3, f_setpower->current_index, 11);
                }
            }

            printf("333Power increased: Mode %s, total_cnt = %d, current_index = %d, current_power = %d\n",
                   get_gear_func() == 0 ? "Single" : "Double",
                   get_gear_func() == 0 ? 9 : 11, f_setpower->current_index, f_setpower->current_power);
            break;

        case COMPO_ID_BTN_ECIG_SET_DEL:
            if (get_gear_func() == 0)
            {
                if (f_setpower->current_index > 0)
                {
                    f_setpower->current_index--;
                    f_setpower->current_power--;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_cut(picbox2, f_setpower->current_index, 9);
                }
            }
            else
            {
                if (f_setpower->current_index > 0)
                {
                    f_setpower->current_index--;
                    f_setpower->current_power--;
                    ecig_set_power(f_setpower->current_power);
                    compo_picturebox_cut(picbox3, f_setpower->current_index, 11);
                }
            }

            printf("444Power decreased: Mode %s, total_cnt = %d, current_index = %d, current_power = %d\n",
                   get_gear_func() == 0 ? "Single" : "Double",
                   get_gear_func() == 0 ? 9 : 11, f_setpower->current_index, f_setpower->current_power);
            break;

        default:
            break;
    }
    save_power_and_index(f_setpower->current_power, f_setpower->current_index);
}

static void func_ecig_set_power_button_release_handle()
{
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;

    s32 dx, dy;
    compo_picturebox_t *picbox_add = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_ADD1);
    compo_picturebox_t *picbox_del = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_DEL1);

    if (f_setpower->tick <= tick_get() - BTN_REST_DISP_TIME)
    {
        compo_picturebox_set_visible(picbox_add, false);
        compo_picturebox_set_visible(picbox_del, false);
    }

    int id = compo_get_button_id();
    if (id == 0 || !ctp_get_dxy(&dx, &dy))
        return;

    f_setpower->tick = tick_get();

    switch (id)
    {
        case COMPO_ID_BTN_ECIG_SET_ADD:
            compo_picturebox_set_visible(picbox_add, true);
            break;
        case COMPO_ID_BTN_ECIG_SET_DEL:
            compo_picturebox_set_visible(picbox_del, true);
            break;
    }
}

// 功率功能消息处理
static void func_ecig_set_power_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_ecig_set_power_button_touch_handle();
            break;
        // case MSG_CTP_SHORT_LEFT:
        // case MSG_CTP_SHORT_UP:
        // case MSG_CTP_SHORT_RIGHT:
        // case MSG_CTP_SHORT_DOWN:
        //     // func_menu_sub_drag_move();
        //     break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

// 功率功能事件处理
static void func_ecig_set_power_process(void)
{
    func_ecig_set_power_button_release_handle();
    func_process();
}

// 进入功率功能
static void func_ecig_set_power_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_setpower_t));
    f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;
    if (f_setpower)
    {
        memset(f_setpower, 0, sizeof(f_setpower_t)); // 显式初始化内存
        load_power_and_index(&f_setpower->current_power, &f_setpower->current_index);
    }
    func_cb.frm_main = func_ecig_set_power_form_create();
}

// 退出功率功能
static void func_ecig_set_power_exit(void)
{
    if (func_cb.f_cb != NULL)
    {
        f_setpower_t *f_setpower = (f_setpower_t *)func_cb.f_cb;
        save_power_and_index(f_setpower->current_power, f_setpower->current_index);
        func_free(func_cb.f_cb);
        func_cb.f_cb = NULL;
    }
    func_cb.last = FUNC_ECIG_SET_POWER;
}

// 功率功能
void func_ecig_set_power(void)
{
    printf("%s\n", __func__);
    func_ecig_set_power_enter();
    while (func_cb.sta == FUNC_ECIG_SET_POWER)
    {
        func_ecig_set_power_process();
        func_ecig_set_power_message(msg_dequeue());
    }
    func_ecig_set_power_exit();
}
