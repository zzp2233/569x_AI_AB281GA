#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "api_gui.h"
#include "../../../../ute/module/smoke/ute_module_smoke.h"

// 基础配置宏
#define POWER_LEVEL_COUNT 30 // 最大功率等级
//#define BAR_COUNT 19         // 柱形总数,最新定义放在ute_module_smoke.h
#define BAR_SPACING 20       // 柱形间距（像素）
#define VISIBLE_BARS 6       // 一屏可见柱形数量

#define BASE_ID_LIGHT_PIC 100 // 柱形图ID基值
#define BASE_ID_LIGHT_BTN 200 // 按钮ID基值
#define BASE_ID_TXT_NUM 300   // 文本框ID基值

// 组件ID枚举
enum
{
    COMPO_ID_LIGHT_PIC = 1,
    COMPO_ID_LIGHT_BTN,
    COMPO_ID_TXT_NUM,
};

// 单个柱形状态结构体
typedef struct bar_state_t_
{
    bool touch_flag; // 触摸标志
    s8 level_old;    // 触摸起始等级
    s8 level_value;  // 当前等级
} bar_state_t;

// 页面总状态结构体
typedef struct f_ecig_setpower_t_
{
    bool touch_flag;
    s32 move_offset;
    s32 move_offset_old;
    s8 level_old;
    s8 level_value;
    bar_state_t bars[BAR_COUNT];
    page_tp_move_t *ptm; // 分页滚动控制块
} f_ecig_setpower_t;

// 柱形尺寸与位置宏
#define val_shape_y 80
#define val_shepe_x 15
#define val_shape_h 138
#define val_shepe_w 20

// 滚动与布局计算宏
#define BAR_TOTAL_WIDTH (val_shepe_w + BAR_SPACING)
#define VISIBLE_WIDTH (VISIBLE_BARS * BAR_TOTAL_WIDTH)
#define TOTAL_WIDTH (BAR_COUNT * BAR_TOTAL_WIDTH)
#define make_pic_hei(val) (val * val_shape_h / POWER_LEVEL_COUNT)
#define make_pic_y(hei) (val_shape_h - hei + val_shape_y)
#define SPACING_COUNT (-val_shape_h / POWER_LEVEL_COUNT)
#define get_bar_x(index, ptm) (val_shepe_x + index * BAR_TOTAL_WIDTH + ptm->move_offset)

/**
 * 创建功率设置页面的窗体与所有UI组件
 */
compo_form_t *func_ecig_setpower_form_create(void)
{
    char txt_buf[5] = {0};
    compo_form_t *frm = compo_form_create(true);

    f_ecig_setpower_t *f_ecig_setpower = (f_ecig_setpower_t *)func_cb.f_cb;

    for (int i = 0; i < BAR_COUNT; i++)
    {

        // 从power_seq数组加载初始值，如果没有有效值则使用默认值5
        s8 initial_level = (uteModuleSmokeData.power_seq[i] >= 5 && uteModuleSmokeData.power_seq[i] <= POWER_LEVEL_COUNT) ?
                           uteModuleSmokeData.power_seq[i] : 5;

        f_ecig_setpower->bars[i].level_old = initial_level;
        f_ecig_setpower->bars[i].level_value = initial_level;
        f_ecig_setpower->bars[i].touch_flag = false;
        s8 level_data = initial_level;  // 使用加载的值而不是固定的5

        // 同步到power_seq数组（确保数据一致性）
        uteModuleSmokeData.power_seq[i] = level_data;

        // 计算初始位置
        s16 shape_hei = make_pic_hei(level_data);
        s16 shape_y = make_pic_y(shape_hei);
        s16 bar_x = get_bar_x(i, f_ecig_setpower->ptm);

        // 创建背景柱形
        compo_shape_t *bg_bar = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        if (bg_bar != NULL)
        {
            compo_shape_set_color(bg_bar, make_color(33, 75, 76));
            s16 bg_hei = make_pic_hei(POWER_LEVEL_COUNT);
            s16 bg_y = make_pic_y(bg_hei);
            compo_shape_set_location(bg_bar, bar_x, bg_y + 5, val_shepe_w, bg_hei);
            compo_shape_set_radius(bg_bar, 0);
            widget_set_align_center(bg_bar->rect, false);
        }

        // 创建柱形图标
        compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_SETPOWER_00_32X17_BIN);
        if (picbox != NULL)
        {
            compo_picturebox_set_pos(picbox, bar_x + 10, val_shape_y + 150);
            compo_picturebox_cut(picbox, i, 25);
        }

        // 创建可调节柱形
        compo_shape_t *adj_bar = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        if (adj_bar != NULL)
        {
            compo_shape_set_color(adj_bar, make_color(31, 185, 207));
            compo_shape_set_location(adj_bar, bar_x, shape_y + 5, val_shepe_w, shape_hei);
            compo_shape_set_radius(adj_bar, 0);
            widget_set_align_center(adj_bar->rect, false);
            compo_setid(adj_bar, BASE_ID_LIGHT_PIC + i);
        }

        // 创建触摸按钮
        compo_button_t *btn = compo_button_create_by_image(frm, 0);
        if (btn != NULL)
        {
            compo_setid(btn, BASE_ID_LIGHT_BTN + i);
            compo_button_set_location(btn, bar_x + val_shepe_w / 2, val_shape_y + val_shape_h / 2,
                                      val_shepe_w, val_shape_h);
            compo_button_set_visible(btn, true);
        }

        // 创建数值文本框
        compo_textbox_t *textbox_num = compo_textbox_create(frm, 3);
        if (textbox_num != NULL)
        {
            compo_textbox_set_font(textbox_num, UI_BUF_0FONT_FONT_NUM_22_BIN);
            compo_textbox_set_align_center(textbox_num, false);
            compo_textbox_set_location(textbox_num, bar_x - 2, val_shape_y - 30, 30, 20);
            snprintf(txt_buf, sizeof(txt_buf), "%02d", level_data);
            compo_textbox_set(textbox_num, txt_buf);
            compo_setid(textbox_num, BASE_ID_TXT_NUM + i);
        }
    }

    return frm;
}

/**
 * 通过按钮ID获取柱形索引
 */
static int get_touched_bar_index(u32 btn_id)
{
    if (btn_id >= BASE_ID_LIGHT_BTN && btn_id < BASE_ID_LIGHT_BTN + BAR_COUNT)
    {
        return btn_id - BASE_ID_LIGHT_BTN;
    }
    return -1;
}

/**
 * 判断是否有柱形被触摸
 */
static bool is_any_bar_touched(f_ecig_setpower_t *f_ecig_setpower)
{
    if (f_ecig_setpower == NULL)
        return false;
    for (int i = 0; i < BAR_COUNT; i++)
    {
        if (f_ecig_setpower->bars[i].touch_flag)
        {
            return true;
        }
    }
    return false;
}

/**
 * 处理柱形滚动与高度更新
 */
static void func_ecig_setpower_disp_move_handle(void)
{
    f_ecig_setpower_t *f_ecig_setpower = (f_ecig_setpower_t *)func_cb.f_cb;

    // 处理水平分页滚动
    compo_page_move_process(f_ecig_setpower->ptm);

    // 处理柱形垂直滑动调节
    for (int i = 0; i < BAR_COUNT; i++)
    {
        if (f_ecig_setpower->bars[i].touch_flag)
        {
            s32 dx = 0, dy = 0;
            f_ecig_setpower->bars[i].touch_flag = ctp_get_dxy(&dx, &dy);

            if (f_ecig_setpower->bars[i].touch_flag)
            {
                // 计算当前等级
                s8 level_data = (dy / SPACING_COUNT) + f_ecig_setpower->bars[i].level_old;
                level_data = (level_data < 5) ? 5 : (level_data > POWER_LEVEL_COUNT) ? POWER_LEVEL_COUNT
                             : level_data;

                // 更新柱形显示
                if (level_data != f_ecig_setpower->bars[i].level_value)
                {
                    f_ecig_setpower->bars[i].level_value = level_data;
                    uteModuleSmokeData.power_seq[i] = f_ecig_setpower->bars[i].level_value;
                    s16 shape_hei = make_pic_hei(level_data);
                    s16 shape_y = make_pic_y(shape_hei);
                    s16 bar_x = val_shepe_x + i * BAR_TOTAL_WIDTH;

                    compo_shape_t *adj_bar = (compo_shape_t *)compo_getobj_byid(BASE_ID_LIGHT_PIC + i);
                    if (adj_bar != NULL)
                    {
                        compo_shape_set_location(adj_bar, bar_x, shape_y + 5, val_shepe_w, shape_hei);
                    }

                    //保存数据
                    uteModuleSmokeDataSaveConfig();

                    // 更新数值显示
                    char txt_buf[5] = {0};
                    snprintf(txt_buf, sizeof(txt_buf), "%02d", level_data);
                    compo_textbox_t *textbox_num = (compo_textbox_t *)compo_getobj_byid(BASE_ID_TXT_NUM + i);
                    if (textbox_num != NULL)
                    {
                        compo_textbox_set(textbox_num, txt_buf);
                    }
                }
            }
            else
            {
                // 触摸结束更新旧等级
                f_ecig_setpower->bars[i].level_old = f_ecig_setpower->bars[i].level_value;
            }
        }
    }
}

static void func_ecig_setpower_process(void)
{
    func_ecig_setpower_disp_move_handle();
    func_process();
}

/**
 * 页面消息处理函数
 */
static void func_ecig_setpower_message(size_msg_t msg)
{
    f_ecig_setpower_t *f_ecig_setpower = (f_ecig_setpower_t *)func_cb.f_cb;
    if (f_ecig_setpower == NULL || f_ecig_setpower->ptm == NULL)
    {

        return;
    }

    switch (msg)
    {
        case MSG_CTP_TOUCH:
        {
            // 无柱形被触摸时处理页面滚动或新触摸
            if (!is_any_bar_touched(f_ecig_setpower))
            {
                u32 btn_id = compo_get_button_id();
                int bar_idx = get_touched_bar_index(btn_id);

                // 触摸有效柱形按钮
                if (bar_idx != -1 && !f_ecig_setpower->bars[bar_idx].touch_flag)
                {
                    s32 start_x, start_y, cur_x, cur_y;
                    if (ctp_get_cur_point(&start_x, &start_y, &cur_x, &cur_y))
                    {
                        // 计算柱形边界
                        s16 bar_x = get_bar_x(bar_idx, f_ecig_setpower->ptm);
                        s32 shape_left = bar_x;
                        s32 shape_right = bar_x + val_shepe_w;
                        s32 shape_top = val_shape_y;
                        s32 shape_bottom = val_shape_y + val_shape_h;

                        // 检查触摸点是否在柱形范围内
                        bool in_range = (cur_x >= shape_left && cur_x <= shape_right) &&
                                        (cur_y >= shape_top && cur_y <= shape_bottom);

                        if (in_range)
                        {
                            f_ecig_setpower->bars[bar_idx].touch_flag = true;
                        }
                    }
                }
                else
                {
                    // 处理页面滚动触摸
                    compo_page_move_touch_handler(f_ecig_setpower->ptm);
                }
            }
            break;
        }
        case MSG_CTP_CLICK:

            break;
        default:

            func_message(msg);
            break;
    }
}

/**
 * 页面进入初始化
 */
static void func_ecig_setpower_enter(void)
{
    // 分配页面状态结构体内存
    func_cb.f_cb = func_zalloc(sizeof(f_ecig_setpower_t));
    f_ecig_setpower_t *f_ecig_setpower = (f_ecig_setpower_t *)func_cb.f_cb;
    if (f_ecig_setpower == NULL)
    {

        return;
    }

    // 创建主窗体
    func_cb.frm_main = func_ecig_setpower_form_create();

    // 初始化分页滚动控制块
    f_ecig_setpower->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    if (f_ecig_setpower->ptm == NULL)
    {

        return;
    }

    // 计算总页数（向上取整）
    u16 page_count = (BAR_COUNT + VISIBLE_BARS - 1) / VISIBLE_BARS;

    // 配置滚动信息
    page_move_info_t info =
    {
        .dir = 1,
        .page_size = TOTAL_WIDTH,   // 整个内容宽度作为一个大页面
        .page_count = 1,            // 只有一个页面
        .jump_perc = 0,             // 无自动对齐
        .quick_jump_perc = 0,       // 无快速跳转
        .up_over_perc = 10,
        .down_over_perc = 10,
        .down_spring_perc = 5,
        .title_used = false,
        .anim_time_ms = 200
    };

    // 初始化滚动控制
    compo_page_move_init(f_ecig_setpower->ptm, func_cb.frm_main->page_body, &info);

    // 关键修复：设置最大滚动偏移（防止过度滚动）
    s16 max_scroll = -(TOTAL_WIDTH - VISIBLE_WIDTH + val_shepe_x);
    f_ecig_setpower->ptm->down_border_offset = max_scroll;
    f_ecig_setpower->ptm->down_spring_offset = max_scroll;  // 关键：设置回弹边界
}

static void func_ecig_setpower_exit(void)
{
    f_ecig_setpower_t *f_ecig_setpower = (f_ecig_setpower_t *)func_cb.f_cb;
    if (f_ecig_setpower == NULL)
        return;

    // 释放滚动控制块内存
    if (f_ecig_setpower->ptm)
    {
        func_free(f_ecig_setpower->ptm);
        f_ecig_setpower->ptm = NULL;
    }
    uteModuleSmokeDataSaveConfig();
    func_cb.last = FUNC_ECIG_SETPOWER;

    func_free(func_cb.f_cb);
    func_cb.f_cb = NULL;
}

/**
 * 功率设置页面主函数
 */
void func_ecig_setpower(void)
{
    printf("%s \n", __func__);
    func_ecig_setpower_enter();

    while (func_cb.sta == FUNC_ECIG_SETPOWER)
    {
        func_ecig_setpower_process();
        func_ecig_setpower_message(msg_dequeue());
    }

    func_ecig_setpower_exit();
}
