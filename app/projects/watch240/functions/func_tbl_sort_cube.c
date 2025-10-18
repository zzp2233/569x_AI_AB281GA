#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_TBL_SORT_CUBE_SUPPORT

typedef struct f_tbl_sort_cube_t_
{

} f_tbl_sort_cube_t;

enum
{
    COMPO_ID_CUBE = 1,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
const static compo_cube_item_t cube_item_tbl_zh[] =
{
    {UI_BUF_I341001_CUBE_ZH_00_ACTIVITY_BIN, FUNC_ACTIVITY},
    {UI_BUF_I341001_CUBE_ZH_01_HEART_BIN, FUNC_HEARTRATE},
    {UI_BUF_I341001_CUBE_ZH_02_SPO2_BIN, FUNC_BLOOD_OXYGEN},
    {UI_BUF_I341001_CUBE_ZH_03_PRESSURE_BIN, FUNC_PRESSURE},
    {UI_BUF_I341001_CUBE_ZH_04_SLEEP_BIN, FUNC_SLEEP},
    {UI_BUF_I341001_CUBE_ZH_05_WEATHER_BIN, FUNC_WEATHER},
    {UI_BUF_I341001_CUBE_ZH_06_MUSIC_BIN, FUNC_BT},
    {UI_BUF_I341001_CUBE_ZH_07_CALL_BIN, FUNC_CALL},
    {UI_BUF_I341001_CUBE_ZH_08_SPORT_BIN, FUNC_SPORT},
};

const static compo_cube_item_t cube_item_tbl_en[] =
{
    {UI_BUF_I341001_CUBE_EN_00_ACTIVITY_BIN, FUNC_ACTIVITY},
    {UI_BUF_I341001_CUBE_EN_01_HEART_BIN, FUNC_HEARTRATE},
    {UI_BUF_I341001_CUBE_EN_02_SPO2_BIN, FUNC_BLOOD_OXYGEN},
    {UI_BUF_I341001_CUBE_EN_03_PRESSURE_BIN, FUNC_PRESSURE},
    {UI_BUF_I341001_CUBE_EN_04_SLEEP_BIN, FUNC_SLEEP},
    {UI_BUF_I341001_CUBE_EN_05_WEATHER_BIN, FUNC_WEATHER},
    {UI_BUF_I341001_CUBE_EN_06_MUSIC_BIN, FUNC_BT},
    {UI_BUF_I341001_CUBE_EN_07_CALL_BIN, FUNC_CALL},
    {UI_BUF_I341001_CUBE_EN_08_SPORT_BIN, FUNC_SPORT},
};
#else
const static compo_cube_item_t cube_item_tbl_zh[] =
{
    {NULL, FUNC_ACTIVITY},
    {NULL, FUNC_HEARTRATE},
    {NULL, FUNC_BLOOD_OXYGEN},
    {NULL, FUNC_PRESSURE},
    {NULL, FUNC_SLEEP},
    {NULL, FUNC_WEATHER},
};

const static compo_cube_item_t cube_item_tbl_en[] =
{
    {NULL, FUNC_ACTIVITY},
    {NULL, FUNC_HEARTRATE},
    {NULL, FUNC_BLOOD_OXYGEN},
    {NULL, FUNC_PRESSURE},
    {NULL, FUNC_SLEEP},
    {NULL, FUNC_WEATHER},
};
#endif

static compo_cube_item_t cube_item_tbl_curr[CUBE_ITEM_CNT];

#define CUBE_ITEM_TBL_CNT (sizeof(cube_item_tbl_zh) / sizeof(cube_item_tbl_zh[0]))

// 创建一级卡片魔方窗体
compo_form_t *func_tbl_sort_cube_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    // 创建背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(0x22, 0x22, 0x22));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
#if UTE_DRV_AMOLED_S368X448_GC3A71_193WG2014V1_QSPI_SUPPORT
    compo_shape_set_radius(rect, 74);
#else
    compo_shape_set_radius(rect, UTE_MODULE_WATCHONLINNE_R_VALUE);
#endif

    compo_cube_item_t *cube_item_tbl_tmp = cube_item_tbl_en;
    if (uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
    {
        cube_item_tbl_tmp = cube_item_tbl_zh;
    }

    // 用于标记cube_item_tbl_tmp中哪些项已被使用
    bool used_items[CUBE_ITEM_TBL_CNT] = {false};
    u8 curr_cnt = 0;

    // 首先从历史任务中填充（最多6个）
    int history_count = latest_task_count();
    for (int i = 0; i < history_count && curr_cnt < 6; i++)
    {
        u8 func_id = latest_task_get(i);

        // 在cube_item_tbl_tmp中查找对应的项
        for (int j = 0; j < CUBE_ITEM_TBL_CNT; j++)
        {
            if (cube_item_tbl_tmp[j].func_sta == func_id)
            {
                cube_item_tbl_curr[curr_cnt] = cube_item_tbl_tmp[j];
                used_items[j] = true; // 标记为已使用
                curr_cnt++;
                break;
            }
        }
    }

    // 如果数量不足6个，用cube_item_tbl_tmp中的剩余项填充
    for (int i = 0; i < CUBE_ITEM_TBL_CNT && curr_cnt < 6; i++)
    {
        if (!used_items[i])
        {
            // 检查该项是否已经在cube_item_tbl_curr中，避免重复
            bool is_duplicate = false;
            for (int k = 0; k < curr_cnt; k++)
            {
                if (cube_item_tbl_curr[k].func_sta == cube_item_tbl_tmp[i].func_sta)
                {
                    is_duplicate = true;
                    break;
                }
            }

            // 如果该项未被使用且不重复
            if (!is_duplicate)
            {
                cube_item_tbl_curr[curr_cnt] = cube_item_tbl_tmp[i];
                curr_cnt++;
            }
        }
    }

    compo_cube_t *cube = compo_cube_create(frm, (gui_image_get_size(cube_item_tbl_curr[0].res_addr).wid) / 2, cube_item_tbl_curr, CUBE_ITEM_CNT);
    compo_cube_set_pos(cube, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(cube, COMPO_ID_CUBE);

    return frm;
}

// 一级卡片魔方功能事件处理
static void func_tbl_sort_cube_process(void)
{
    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    if(cube != NULL)
    {
        compo_cube_move(cube);
    }
    func_process();
}

static void func_tbl_sort_cube_icon_click(void)
{
    compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
    if (cube == NULL)
        return;
    point_t pt = ctp_get_sxy();

    int icon_idx = compo_cube_get_idx(cube, pt.x, pt.y);
    u8 func_sta;
    if (icon_idx < 0 || icon_idx >= CUBE_ITEM_CNT)
    {
        return;
    }
    // 根据图标索引获取应用ID
    func_sta = cube_item_tbl_curr[icon_idx].func_sta;

    // 切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

// 一级卡片魔方功能消息处理
static void func_tbl_sort_cube_message(size_msg_t msg)
{
    point_t pt = ctp_get_sxy();
    s16 cube_limit_x = (GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2);
    s16 cube_limit_y = (GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 2);
    bool flag_cube_touch_x = (pt.x >= cube_limit_x) && (pt.x <= (cube_limit_x + GUI_SCREEN_CENTER_X));
    bool flag_cube_touch_y = (pt.y >= cube_limit_y) && (pt.y <= (cube_limit_y + GUI_SCREEN_CENTER_Y));

    if (flag_cube_touch_x && flag_cube_touch_y)
    {
        compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
        if (msg == MSG_CTP_TOUCH && cube != NULL)
        {
            // 移动过程中，触屏停止。重新进入到开始拖动模式
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_DRAG);
        }
        else if (msg >= MSG_CTP_SHORT_LEFT && msg <= MSG_CTP_SHORT_DOWN)
        {
            return true;
        }
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_tbl_sort_cube_icon_click(); // 单击图标
            break;

#if !DRV_ENCODER_KEYS_WATCHMAIN_SCREEN_SWITCHOVER_SUPPORT
        case MSG_QDEC_FORWARD: // 向前滚动菜单
        {
            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            if (cube == NULL)
                return;
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_FORWARD);
            flag_cube_touch = true;
        }
        break;

        case MSG_QDEC_BACKWARD: // 向后滚动菜单
        {
            compo_cube_t *cube = compo_getobj_byid(COMPO_ID_CUBE);
            if (cube == NULL)
                return;
            compo_cube_move_control(cube, COMPO_CUBE_MOVE_CMD_BACKWARD);
            flag_cube_touch = true;
        }
        break;
#endif

        default:
            func_message(msg);
            break;
    }
}

// 进入一级卡片魔方功能
static void func_tbl_sort_cube_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_tbl_sort_cube_t));
    func_cb.frm_main = func_tbl_sort_cube_form_create();
}

// 退出一级卡片魔方功能
static void func_tbl_sort_cube_exit(void)
{
    func_cb.last = FUNC_TBL_SORT_CUBE;
}

// 一级卡片魔方功能
void func_tbl_sort_cube(void)
{
    printf("%s\n", __func__);
    func_tbl_sort_cube_enter();
    while (func_cb.sta == FUNC_TBL_SORT_CUBE)
    {
        func_tbl_sort_cube_process();
        func_tbl_sort_cube_message(msg_dequeue());
    }
    func_tbl_sort_cube_exit();
}

#endif
