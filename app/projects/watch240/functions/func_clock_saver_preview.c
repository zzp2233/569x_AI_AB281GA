#include "include.h"
#include "func.h"
#include "func_clock_saver.h"
#include "ute_module_gui_common.h"
#include "ute_module_watchonline.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (UTE_MODULE_WATCH_PREVIEW_STYLE >= PREVIEW_ROWBOX_STYLE && UTE_MODULE_WATCH_PREVIEW_STYLE <= PREVIEW_ROTARY_STYLE)
#define CUR_PREVIEW_SAVER_STYLE UTE_MODULE_WATCH_PREVIEW_STYLE
#else
#define CUR_PREVIEW_SAVER_STYLE PREVIEW_ROTARY_STYLE
#endif

//根据表盘的基地址获取对应的预览图
u32 func_clock_saver_preview_get_addr(u32 base_addr)
{
    u32 user_addr = base_addr;
#if UTE_MODULE_CUSTOM_WATCHONLINE_UITOOL_SUPPORT
    user_addr += sizeof(watchConfig_t);
#endif
    uitool_header_t uitool_header;
    os_spiflash_read(&uitool_header, user_addr, UITOOL_HEADER);
    for(u16 i=0; i<uitool_header.num; i++)
    {
        uitool_res_t uitool_res = {0};
        os_spiflash_read(&uitool_res, user_addr + UITOOL_HEADER + i * UITOOL_RES_HEADER, UITOOL_RES_HEADER);
        u32 res_addr = user_addr + uitool_res.res_addr;
        //预览图
        if (uitool_res.res_type == UITOOL_TYPE_IMAGE && uitool_res.bond_type == COMPO_BOND_IMAGE_CLOCK_PREVIEW)
        {
            return res_addr;
        }
    }
    return 0;
}

//获取表盘预览类型
u16 func_clock_saver_preview_get_type(void)
{
    return CUR_PREVIEW_SAVER_STYLE;
}

#if (CUR_PREVIEW_SAVER_STYLE == PREVIEW_ROTARY_STYLE)

/********************************************************************
 * 注意：使用该效果的表盘预览图在生成时需要使能旋转压缩
*********************************************************************/


//组件ID
enum
{
    //列表(横向)
    COMPO_ID_ROTARY = 1,
    COMPO_ID_PIC_LOCK,
};

typedef struct f_clock_preview_t_
{
    compo_rotary_t *rotary;
} f_clock_preview_t;

static compo_rotary_item_t prev_dial_item[STACKLIST_PIC_COUNT_MAX] = {0};

//创建表盘预览窗体
compo_form_t *func_clock_saver_preview_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //获取表盘预览图集合
    for (u8 i = 0; i < UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX; i++)
    {
        prev_dial_item[i].res_addr = func_clock_saver_preview_get_addr(func_clock_saver_get_saver_info_addr(i));
        prev_dial_item[i].str_idx = STR_NULL;
    }

    //创建背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(24, 24, 32));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    //创建转盘组件
    compo_rotary_t *rotary = compo_rotary_create(frm, prev_dial_item, UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX);
    compo_setid(rotary, COMPO_ID_ROTARY);
    compo_rotary_update(rotary);

    return frm;
}

//表盘预览功能事件处理
static void func_clock_saver_preview_process(void)
{
    f_clock_preview_t *f_clock_preview = (f_clock_preview_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_clock_preview->rotary;
    compo_rotary_move(rotary);
    func_process();
}

//表盘预览功能消息处理
static void func_clock_saver_preview_message(size_msg_t msg)
{
    f_clock_preview_t *f_clock_preview = (f_clock_preview_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_clock_preview->rotary;

    switch (msg)
    {
        case KU_BACK:
        case KU_DELAY_BACK:
            uteModuleGuiCommonSetCurrSreenSaverIndex(compo_rotary_get_idx(rotary));
            uteModuleGuiCommonGoBackLastScreen();
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
            uteModuleGuiCommonSetCurrSreenSaverIndex(compo_rotary_get_idx(rotary));
            uteModuleGuiCommonGoBackLastScreen();
        }
        break;

        default:
            func_message(msg);
            break;
    }
}

//进入表盘预览功能
static void func_clock_saver_preview_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_preview_t));
    func_cb.frm_main = func_clock_saver_preview_form_create();
    tft_set_temode(0);
    f_clock_preview_t *f_clock_preview = (f_clock_preview_t *)func_cb.f_cb;
    f_clock_preview->rotary = compo_getobj_byid(COMPO_ID_ROTARY);
    compo_rotary_t *rotary = f_clock_preview->rotary;
    if (rotary->type != COMPO_TYPE_ROTARY)
    {
        halt(HALT_GUI_COMPO_ROTARY_TYPE);
    }
    uint8_t dialplate_index = 0;
    uteModuleGuiCommonGetCurrScreenSaverIndex(&dialplate_index);
    compo_rotary_set_rotation_byidx(rotary, sys_cb.dialplate_index);
    compo_rotary_update(rotary);
}

//退出表盘预览功能
static void func_clock_saver_preview_exit(void)
{
    tft_set_temode(DEFAULT_TE_MODE);
    func_cb.last = FUNC_CLOCK_SAVER_PREVIEW;
    task_stack_pop();
}

//表盘预览功能
void func_clock_saver_preview(void)
{
    printf("%s\n", __func__);
    func_clock_saver_preview_enter();
    while (func_cb.sta == FUNC_CLOCK_SAVER_PREVIEW)
    {
        func_clock_saver_preview_process();
        func_clock_saver_preview_message(msg_dequeue());
    }
    func_clock_saver_preview_exit();
}

#elif (CUR_PREVIEW_SAVER_STYLE == PREVIEW_ROWBOX_STYLE)

// 组件ID
enum
{
    // 列表(横向)
    COMPO_ID_ROWBOX = 1,
    COMPO_ID_PIC_LOCK,
    COMPO_ID_PIC_DOT_0,
    COMPO_ID_PIC_DOT_1,
    COMPO_ID_PIC_DOT_2,
    COMPO_ID_PIC_DOT_3,
    COMPO_ID_PIC_DOT_4,
    COMPO_ID_PIC_DOT_5,
    COMPO_ID_PIC_DOT_6,
    COMPO_ID_PIC_DOT_7,
    COMPO_ID_PIC_DOT_8,
    COMPO_ID_PIC_DOT_9,
    COMPO_ID_PIC_DOT_10
};

typedef struct f_clock_preview_t_
{

} f_clock_preview_t;

// 创建表盘预览窗体
compo_form_t *func_clock_saver_preview_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    // 创建背景
    // compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    // compo_shape_set_color(rect, make_color(24, 24, 32));
    // compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    // 设置标题栏
    //  compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE);
    //  compo_form_set_title(frm, i18n[STR_DIAL_SWICTH]);
    //  compo_form_set_title_center(frm, true);

    // 创建预览图
    compo_rowbox_t *rowbox = compo_rowbox_create(frm);
    compo_rowbox_set_style(rowbox, COMPO_ROWBOX_STYLE_PREVIEW); // 设置预览样式，用于表盘时需要在set前调用

    u32 *dialplate_info = func_clock_saver_get_saver_info_array();
    compo_rowbox_set(rowbox, dialplate_info, UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX);
    compo_setid(rowbox, COMPO_ID_ROWBOX);
    compo_rowbox_cycle_en(rowbox, true); // 循环滚动

    uint8_t dialplate_index = 0;
    uteModuleGuiCommonGetCurrScreenSaverIndex(&dialplate_index);

    compo_rowbox_set_focus_byidx(rowbox, dialplate_index);
    compo_rowbox_set_margin(rowbox, 1, 6, (UTE_MODULE_WATCHONLINNE_R_VALUE / 2 - 1), make_color(0x22, 0x22, 0x22));
    compo_rowbox_update(rowbox);

#ifdef UI_BUF_DIALPLATE_DOT_GREY_BIN
    area_t img_area;
    img_area = gui_image_get_size(UI_BUF_DIALPLATE_DOT_GREY_BIN);
    uint16_t total_width = img_area.wid * UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX;
    uint16_t start_center_x = (GUI_SCREEN_WIDTH - total_width) / 2 + img_area.wid / 2;
    compo_picturebox_t *picbox = NULL;
    uint8_t dot_img_id = COMPO_ID_PIC_DOT_0;
    for (uint8_t i = 0; i < UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX; i++)
    {
        if (i == dialplate_index)
        {
            picbox = compo_picturebox_create(frm, UI_BUF_DIALPLATE_DOT_WHITE_BIN);
        }
        else
        {
            picbox = compo_picturebox_create(frm, UI_BUF_DIALPLATE_DOT_GREY_BIN);
        }
        compo_picturebox_set_pos(picbox, start_center_x + i * img_area.wid, GUI_SCREEN_HEIGHT - img_area.hei);
        compo_setid(picbox, dot_img_id++);
    }
#endif

    return frm;
}

// 表盘预览功能事件处理
static void func_clock_saver_preview_process(void)
{
    compo_rowbox_t *rowbox = compo_getobj_byid(COMPO_ID_ROWBOX);
    compo_rowbox_move(rowbox);

#ifdef UI_BUF_DIALPLATE_DOT_WHITE_BIN
    u8 dialplate_max_num = func_clock_get_max_dialplate_num();
    compo_picturebox_t *picbox = NULL;
    for (uint8_t i = 0; i < dialplate_max_num; i++)
    {
        picbox = compo_getobj_byid(COMPO_ID_PIC_DOT_0 + i);
        if (picbox)
        {
            compo_picturebox_set(picbox, UI_BUF_DIALPLATE_DOT_GREY_BIN);
        }
    }
    picbox = compo_getobj_byid(COMPO_ID_PIC_DOT_0 + rowbox->img_idx[IMG_MAX / 2]);
    if (picbox)
    {
        compo_picturebox_set(picbox, UI_BUF_DIALPLATE_DOT_WHITE_BIN);
    }
#endif

    func_process();
}

// 表盘预览功能消息处理
static void func_clock_saver_preview_message(size_msg_t msg)
{
    compo_rowbox_t *rowbox = compo_getobj_byid(COMPO_ID_ROWBOX);
    compo_rowbox_move_cb_t *mcb = rowbox->mcb;
    int sel_idx; // 选择表盘的idx

    switch (msg)
    {
        case MSG_CTP_CLICK:
            sel_idx = compo_rowbox_select(rowbox, ctp_get_sxy());
            if (sel_idx >= 0)
            {
                uteModuleGuiCommonSetCurrSreenSaverIndex(sel_idx);
                uteModuleGuiCommonGoBackLastScreen();
            }
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            mcb->flag_drag = true;
            mcb->flag_move_auto = false;
            break;

        case MSG_QDEC_FORWARD:
            // 向前滚动
            if (!mcb->flag_move_auto)
            {
                mcb->flag_move_auto = true;
                mcb->moveto_idx = rowbox->focus_icon_idx;
            }
            if (mcb->moveto_idx <= rowbox->img_cnt - 1 || rowbox->flag_cycle)
            {
                mcb->moveto_idx++;
                mcb->moveto_x = compo_rowbox_getx_byidx(rowbox, mcb->moveto_idx);
            }
            break;

        case MSG_QDEC_BACKWARD:
            // 向后滚动
            if (!mcb->flag_move_auto)
            {
                mcb->flag_move_auto = true;
                mcb->moveto_idx = rowbox->focus_icon_idx;
            }
            if (mcb->moveto_idx >= 0 || rowbox->flag_cycle)
            {
                mcb->moveto_idx--;
                mcb->moveto_x = compo_rowbox_getx_byidx(rowbox, mcb->moveto_idx);
            }
            break;

        case KU_BACK:
        case KU_DELAY_BACK:
            uteModuleGuiCommonSetCurrSreenSaverIndex(rowbox->img_idx[IMG_MAX / 2]);
            uteModuleGuiCommonGoBackLastScreen();
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

// 进入表盘预览功能
static void func_clock_saver_preview_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_preview_t));
    func_cb.frm_main = func_clock_saver_preview_form_create();

    compo_rowbox_t *rowbox = compo_getobj_byid(COMPO_ID_ROWBOX);
    rowbox->mcb = func_zalloc(sizeof(compo_rowbox_move_cb_t));
    compo_rowbox_move_init(rowbox);
}

// 退出表盘预览功能
static void func_clock_saver_preview_exit(void)
{
    compo_rowbox_t *rowbox = compo_getobj_byid(COMPO_ID_ROWBOX);
    func_free(rowbox->mcb);

    func_cb.last = FUNC_CLOCK_SAVER_PREVIEW;
}

// 表盘预览功能
void func_clock_saver_preview(void)
{
    printf("%s\n", __func__);
    func_clock_saver_preview_enter();
    while (func_cb.sta == FUNC_CLOCK_SAVER_PREVIEW)
    {
        func_clock_saver_preview_process();
        func_clock_saver_preview_message(msg_dequeue());
    }
    func_clock_saver_preview_exit();
}

#elif (CUR_PREVIEW_SAVER_STYLE == PREVIEW_STACKLIST_STYLE)

// 组件ID
enum
{
    // 列表(横向)
    COMPO_ID_STACKLIST = 1,

};

typedef struct f_clock_preview_t_
{

} f_clock_preview_t;

// 创建表盘预览窗体
compo_form_t *func_clock_saver_preview_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    // 创建背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(24, 24, 32));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    // 设置标题栏
    //  compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE);
    //  compo_form_set_title(frm, i18n[STR_DIAL_SWICTH]);
    //  compo_form_set_title_center(frm, true);

    // 创建预览图
    compo_stacklist_t *stacklist = compo_stacklist_create(frm, 3, STACKLIST_SYMM_STYLE, STACKLIST_HOR);
    compo_setid(stacklist, COMPO_ID_STACKLIST);
    stacklist->cur_index = sys_cb.dialplate_index;

    // 设置堆叠图片间中心位置的间隔和缩放比例
    compo_stacklist_set_user_para(stacklist, 60, 25);

    // 设置图片
    u8 dial_count = func_clock_get_max_dialplate_num();
    u32 prev_dial_pic[STACKLIST_PIC_COUNT_MAX] = {0};
    for (u8 i = 0; i < dial_count; i++)
    {
        prev_dial_pic[i] = func_clock_saver_preview_get_addr(dialplate_info[i]);
    }
    compo_stacklist_set_pic(stacklist, prev_dial_pic, dial_count);

    return frm;
}

// 表盘预览功能事件处理
static void func_clock_saver_preview_process(void)
{
    compo_stacklist_t *stacklist = compo_getobj_byid(COMPO_ID_STACKLIST);
    compo_stacklist_move(stacklist);
    func_process();
}

// 表盘预览功能消息处理
static void func_clock_saver_preview_message(size_msg_t msg)
{
    compo_stacklist_t *stacklist = compo_getobj_byid(COMPO_ID_STACKLIST);

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_stacklist_set_drag(stacklist, true);
            break;

        case MSG_CTP_CLICK:
        case KU_BACK:
        case KU_DELAY_BACK:
            // sys_cb.dialplate_index = stacklist->cur_index;
            uteModuleGuiCommonSetCurrWatchIndex(stacklist->cur_index);
            func_switch_to(FUNC_CLOCK, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO); // 切换回表盘界面
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            break;

        case MSG_QDEC_FORWARD:
            compo_stacklist_symm_pic_move(stacklist, 20);
            break;

        case MSG_QDEC_BACKWARD:
            compo_stacklist_symm_pic_move(stacklist, -20);
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

// 进入表盘预览功能
static void func_clock_saver_preview_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_preview_t));
    func_cb.frm_main = func_clock_saver_preview_form_create();
}

// 退出表盘预览功能
static void func_clock_saver_preview_exit(void)
{
    func_cb.last = FUNC_CLOCK_SAVER_PREVIEW;
}

// 表盘预览功能
void func_clock_saver_preview(void)
{
    printf("%s\n", __func__);
    func_clock_saver_preview_enter();
    while (func_cb.sta == FUNC_CLOCK_SAVER_PREVIEW)
    {
        func_clock_saver_preview_process();
        func_clock_saver_preview_message(msg_dequeue());
    }
    func_clock_saver_preview_exit();
}

#endif


