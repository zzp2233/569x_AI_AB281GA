#include "include.h"
#include "func.h"

#define TRACE_EN 1
#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
// 组件ID
enum
{
    COMPO_ID_BTN_CONFIRM = 1,
    COMPO_ID_TXT_NUM_TOP,
    COMPO_ID_TXT_NUM_MIDDLE,
    COMPO_ID_TXT_NUM_BOTTOM
};

#define TXT_SPACING    45
#define CENTER_TXT_Y   130 / 2
#define TXT_X          GUI_SCREEN_CENTER_X

typedef struct target_puffs_sub_set_t_
{
    bool touch_flag;
    s32 move_dy;
    s32 move_dy_data;
    u8 current_index;  // 当前选中数字在数组中的索引
    u16 target_puffs[3];
} target_puffs_sub_set_t;

// 目标口数数组
static const u16 target_puffs_data[] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 600, 700, 800, 900, 1000};
#define TARGET_PUFFS_DATA_SIZE (sizeof(target_puffs_data) / sizeof(target_puffs_data[0]))

// 创建目标口数选择窗体
compo_form_t *func_target_puffs_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_TARGET_PUFFS]);

    target_puffs_sub_set_t *f_target_puffs = (target_puffs_sub_set_t *)func_cb.f_cb;
    f_target_puffs->current_index = 3;  // 初始选中200（数组索引3对应200）
    for (int i = 0; i < 3; i++)
    {
        f_target_puffs->target_puffs[i] = target_puffs_data[f_target_puffs->current_index + i - 1];
        compo_textbox_t *txt = compo_textbox_create(frm, 4);
        compo_setid(txt, COMPO_ID_TXT_NUM_TOP + i);
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set_pos(txt, TXT_X, GUI_SCREEN_CENTER_Y + TXT_SPACING * (i - 1) - 15);
        char buf[5];
        snprintf(buf, sizeof(buf), "%d", f_target_puffs->target_puffs[i]);
        compo_textbox_set(txt, buf);
    }

    compo_button_t *btn_confirm = compo_button_create_by_image(frm, UI_BUF_I330001_SET_PUFFS_OK_BIN);
    compo_setid(btn_confirm, COMPO_ID_BTN_CONFIRM);
    compo_button_set_pos(btn_confirm, 120, 260);
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_SET_PUFFS_OVERLAY_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-15);

    return frm;
}

#define HALF_SCREEN_HEIGHT (GUI_SCREEN_HEIGHT / 2)

// 滑动处理
static void func_target_puffs_sub_set_move(void)
{
    target_puffs_sub_set_t *f_target_puffs = (target_puffs_sub_set_t *)func_cb.f_cb;

    if (f_target_puffs->touch_flag)
    {
        s32 dx, dy;

        f_target_puffs->touch_flag = ctp_get_dxy(&dx, &dy);

        if (f_target_puffs->touch_flag)
        {
            f_target_puffs->move_dy_data -= dy;

            // 计算滑动距离是否超过半个屏幕
            if (abs(f_target_puffs->move_dy_data) >= HALF_SCREEN_HEIGHT)
            {
                int offset = (f_target_puffs->move_dy_data > 0)? 1 : -1;
                int new_index = f_target_puffs->current_index + offset;
                new_index = CLAMP(new_index, 0, TARGET_PUFFS_DATA_SIZE - 1);
                printf("[MOVE] new_index=%d, offset=%d\n", new_index, offset);

                for (int i = 0; i < 3; i++)
                {
                    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TXT_NUM_TOP + i);
                    int data_index = new_index - 1 + i;
                    if (data_index < 0 || data_index >= TARGET_PUFFS_DATA_SIZE)
                    {
                        compo_textbox_set(txt, "");
                    }
                    else
                    {
                        char buf[5];
                        snprintf(buf, sizeof(buf), "%d", target_puffs_data[data_index]);
                        printf("[BUF] i=%d, data_index=%d, buf='%s'\n", i, data_index, buf);
                        compo_textbox_set(txt, buf);
                    }
                }

                f_target_puffs->current_index = new_index;
                f_target_puffs->move_dy_data = 0;
            }
        }
        else
        {
            f_target_puffs->move_dy_data = 0;
            for (int i = 0; i < 3; i++)
            {
                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TXT_NUM_TOP + i);
                int data_index = f_target_puffs->current_index - 1 + i;
                if (data_index < 0 || data_index >= TARGET_PUFFS_DATA_SIZE)
                {
                    compo_textbox_set(txt, "");
                }
                else
                {
                    char buf[5];
                    snprintf(buf, sizeof(buf), "%d", target_puffs_data[data_index]);
                    compo_textbox_set(txt, buf);
                }
            }
        }
    }
}
// 单击按钮处理
static void func_target_puffs_sub_set_button_click(void)
{
    int id = compo_get_button_id();
    if (id == COMPO_ID_BTN_CONFIRM)
    {
        target_puffs_sub_set_t *f_target_puffs = (target_puffs_sub_set_t *)func_cb.f_cb;
        int selected_index = f_target_puffs->current_index;
        if (selected_index >= 0 && selected_index < TARGET_PUFFS_DATA_SIZE)
        {
            u16 selected_puffs = target_puffs_data[selected_index];
            ecig_set_target_puffs(selected_puffs);
            printf("Set target puffs: %d\n",selected_puffs);
            func_cb.sta = FUNC_SET_PUFFS;
        }
    }
}

// 目标口数选择功能消息处理
static void func_target_puffs_sub_set_message(size_msg_t msg)
{
    target_puffs_sub_set_t *f_target_puffs = (target_puffs_sub_set_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_target_puffs_sub_set_button_click();
            break;
        case MSG_CTP_TOUCH:
            f_target_puffs->touch_flag = true;
            break;
        default:
            func_message(msg);
            break;
    }
}

// 进入目标口数
static void func_target_puffs_sub_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(target_puffs_sub_set_t));
    func_cb.frm_main = func_target_puffs_form_create();
}

// 退出目标口数
static void func_target_puffs_sub_set_exit(void)
{
    func_cb.last = FUNC_TARGET_PUFFS;
}

// 目标口数功能
void func_target_puffs(void)
{
    printf("%s\n", __func__);
    func_target_puffs_sub_set_enter();
    while (func_cb.sta == FUNC_TARGET_PUFFS)
    {
        func_target_puffs_sub_set_move();
        func_target_puffs_sub_set_message(msg_dequeue());
        func_process();
    }
    func_target_puffs_sub_set_exit();
}
