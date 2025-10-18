#include "include.h"
#include "func.h"
#include "ute_module_gui_common.h"
#include "ute_module_watchonline.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

// 组件ID
enum
{
    // 列表(横向)
    COMPO_ID_ROTARY = 1,
};

typedef struct f_sort_preview_t_
{
    compo_rotary_t *rotary;
} f_sort_preview_t;

typedef struct f_sort_preview_info_t_
{
    u16 func;
    u32 pic_addr;
} f_sort_preview_info_t;

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
const static f_sort_preview_info_t f_sort_preview_tbl_zh[] =
{
    {FUNC_ACTIVITY, UI_BUF_I341001_ROTARY_ZH_148X180_00_ACTIVITY_BIN},
    {FUNC_HEARTRATE, UI_BUF_I341001_ROTARY_ZH_148X180_01_HEART_BIN},
    {FUNC_BLOOD_OXYGEN, UI_BUF_I341001_ROTARY_ZH_148X180_02_SPO2_BIN},
    {FUNC_PRESSURE, UI_BUF_I341001_ROTARY_ZH_148X180_03_PRESSURE_BIN},
    {FUNC_SLEEP, UI_BUF_I341001_ROTARY_ZH_148X180_04_SLEEP_BIN},
    {FUNC_WEATHER, UI_BUF_I341001_ROTARY_ZH_148X180_05_WEATHER_BIN},
    {FUNC_BT, UI_BUF_I341001_ROTARY_ZH_148X180_06_MUSIC_BIN},
    {FUNC_CALL, UI_BUF_I341001_ROTARY_ZH_148X180_07_CALL_BIN},
    {FUNC_SPORT, UI_BUF_I341001_ROTARY_ZH_148X180_08_SPORT_BIN},
};

const static f_sort_preview_info_t f_sort_preview_tbl_en[] =
{
    {FUNC_ACTIVITY, UI_BUF_I341001_ROTARY_EN_148X180_00_ACTIVITY_BIN},
    {FUNC_HEARTRATE, UI_BUF_I341001_ROTARY_EN_148X180_01_HEART_BIN},
    {FUNC_BLOOD_OXYGEN, UI_BUF_I341001_ROTARY_EN_148X180_02_SPO2_BIN},
    {FUNC_PRESSURE, UI_BUF_I341001_ROTARY_EN_148X180_03_PRESSURE_BIN},
    {FUNC_SLEEP, UI_BUF_I341001_ROTARY_EN_148X180_04_SLEEP_BIN},
    {FUNC_WEATHER, UI_BUF_I341001_ROTARY_EN_148X180_05_WEATHER_BIN},
    {FUNC_BT, UI_BUF_I341001_ROTARY_EN_148X180_06_MUSIC_BIN},
    {FUNC_CALL, UI_BUF_I341001_ROTARY_EN_148X180_07_CALL_BIN},
    {FUNC_SPORT, UI_BUF_I341001_ROTARY_EN_148X180_08_SPORT_BIN},
};
#else
const func_switch_mode_t func_switch_mode_rotary[] =
{
    {NULL, 0},
};
const static f_sort_preview_info_t f_sort_preview_tbl_en[] =
{
    {NULL, 0},
};
#endif

#define FUNC_SORT_CNT (sizeof(f_sort_preview_tbl_zh) / sizeof(f_sort_preview_tbl_zh[0]))

compo_rotary_item_t screen_tbl_sort[FUNC_SORT_CNT] = {0};

// 创建快捷卡片预览窗体
compo_form_t *func_sort_preview_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    // 获取快捷卡片预览图集合

    u8 sort_cnt = (func_cb.sort_cnt - 1) > FUNC_SORT_CNT ? FUNC_SORT_CNT : (func_cb.sort_cnt - 1);

    for (uint8_t i = 0; i < sort_cnt; i++)
    {
        u8 target_func = func_cb.tbl_sort[i + 1]; //第一个是表盘，跳过
        for (u8 j = 0; j < FUNC_SORT_CNT; j++)
        {
            if (uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
            {
                if (f_sort_preview_tbl_zh[j].func == target_func)
                {
                    screen_tbl_sort[i].res_addr = f_sort_preview_tbl_zh[j].pic_addr;
                    break;
                }
            }
            else
            {
                if (f_sort_preview_tbl_en[j].func == target_func)
                {
                    screen_tbl_sort[i].res_addr = f_sort_preview_tbl_en[j].pic_addr;
                    break;
                }
            }
        }
    }


    // 创建背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(0x22, 0x22, 0x22));
#if UTE_DRV_AMOLED_S368X448_GC3A71_193WG2014V1_QSPI_SUPPORT
    compo_shape_set_radius(rect, 74);
#else
    compo_shape_set_radius(rect, UTE_MODULE_WATCHONLINNE_R_VALUE);
#endif
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    // 创建转盘组件
    compo_rotary_t *rotary = compo_rotary_create(frm, screen_tbl_sort, sort_cnt);
    compo_setid(rotary, COMPO_ID_ROTARY);
    compo_rotary_update(rotary);

    return frm;
}

// 快捷卡片预览功能事件处理
static void func_sort_preview_process(void)
{
    f_sort_preview_t *f_sort_preview = (f_sort_preview_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_sort_preview->rotary;
    compo_rotary_move(rotary);
    func_process();

    if (compo_rotary_get_sta(rotary) == COMPO_ROTARY_STA_EXIT)
    {
        func_cb.sta = func_cb.tbl_sort[compo_rotary_get_idx(rotary) + 1]; //第一个是表盘，跳过
    }
}

// 快捷卡片预览功能消息处理
static void func_sort_preview_message(size_msg_t msg)
{
    f_sort_preview_t *f_sort_preview = (f_sort_preview_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_sort_preview->rotary;

    switch (msg)
    {
        case KU_BACK:
        case KU_DELAY_BACK:
            func_directly_back_to();
            break;
        default:
            break;
    }

    if (compo_rotary_message(rotary, msg))
    {
        return;
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
        {
            compo_rotary_move_control(rotary, COMPO_ROTARY_MOVE_CMD_EXITING);
        }
        break;

        default:
            func_message(msg);
            break;
    }
}

// 进入快捷卡片预览功能
static void func_sort_preview_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sort_preview_t));
    func_cb.frm_main = func_sort_preview_form_create();
    tft_set_temode(0);
    f_sort_preview_t *f_sort_preview = (f_sort_preview_t *)func_cb.f_cb;
    f_sort_preview->rotary = compo_getobj_byid(COMPO_ID_ROTARY);
    compo_rotary_t *rotary = f_sort_preview->rotary;
    if (rotary->type != COMPO_TYPE_ROTARY)
    {
        halt(HALT_GUI_COMPO_ROTARY_TYPE);
    }
    compo_rotary_set_rotation_byidx(rotary, 0);
    compo_rotary_update(rotary);
    if (func_cb.last == FUNC_CLOCK)
    {
        compo_rotary_move_control(rotary, COMPO_ROTARY_MOVE_CMD_ENTERING); // 入场
    }
}

// 退出快捷卡片预览功能
static void func_sort_preview_exit(void)
{
    tft_set_temode(DEFAULT_TE_MODE);
    func_cb.last = FUNC_SWITCH_MODE_ROTARY;
}

// 快捷卡片预览功能
void func_sort_preview(void)
{
    printf("%s\n", __func__);
    func_sort_preview_enter();
    while (func_cb.sta == FUNC_SWITCH_MODE_ROTARY)
    {
        func_sort_preview_process();
        func_sort_preview_message(msg_dequeue());
    }
    func_sort_preview_exit();
}