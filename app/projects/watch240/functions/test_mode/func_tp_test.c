#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_TP_TEST

typedef struct f_tp_test_t_
{
    u16 tp_id;
    int x;
    int y;
    u32 tick;
} f_tp_test_t;

enum
{
    COMPO_SHAPE_TP = 1,
    COMPO_SHAPE_TP20 = 20,
};

compo_form_t *func_tp_test_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_shape_t * shape = NULL;

    for(uint8_t i = 0; i< 2; i++)
    {
        shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
        ute_compo_shape_set(shape,GUI_SCREEN_WIDTH / 3 + i * GUI_SCREEN_WIDTH / 3,GUI_SCREEN_CENTER_Y,3,GUI_SCREEN_HEIGHT,0,0xff,COLOR_RED);

        shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
        ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X,UTE_DRV_SCREEN_HEIGHT / 3 + i * UTE_DRV_SCREEN_HEIGHT / 3,GUI_SCREEN_WIDTH,3,0,0xff,COLOR_RED);
    }

    for(uint8_t i = 0; i<20; i++)
    {
        shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
        compo_setid(shape, COMPO_SHAPE_TP + i);
    }

    return frm;
}

//时钟表盘右滑菜单功能消息处理
static void func_tp_test_message(size_msg_t msg)
{
    f_tp_test_t *f_tp_test= (f_tp_test_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_LEFT:
            break;
        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
extern struct ctp_cb;
static void func_tp_test_process(void)
{
    f_tp_test_t *f_tp_test = (f_tp_test_t *)func_cb.f_cb;
    point_t pt = {0};
    bool press = false;
    if(tick_check_expire(f_tp_test->tick, 30))
    {
        f_tp_test->tick = tick_get();
#if (CTP_SELECT == CTP_CST8X)
        press = ctp_cst8x_get_point(&pt.x, &pt.y);
#elif (CTP_SELECT == CTP_CHSC6X)
        press = ctp_chsc6x_get_point(&pt.x, &pt.y);
#elif (CTP_SELECT == CTP_AXS5106)
        press = AXS5106_get_touch_point(&pt.x, &pt.y);
#elif (CTP_SELECT == CTP_AXS152x)
        press = AXS152x_get_touch_point(&pt.x, &ctp_cb.y);
        if(press == 0x5A)
        {
            return;
        }
#elif (CTP_SELECT == CTP_SPT5113C)
        press = spt5113c_get_point(&pt.x, &pt.y);
#endif
        if(press&(abs(pt.x - f_tp_test->x) > 2 || abs(pt.x - f_tp_test->y) > 2))
        {
            f_tp_test->x = pt.x;
            f_tp_test->y = pt.y;
            f_tp_test->tp_id = (f_tp_test->tp_id + 1) % 20 + 1;
            u16 index_id = f_tp_test->tp_id;
            compo_shape_t* shape = compo_getobj_byid(index_id);
            ute_compo_shape_set(shape,pt.x,pt.y,6,6,3,0xff,COLOR_GREEN);
        }
    }
    func_process();
}

//创建界面
static void func_tp_test_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_tp_test_t));
    func_cb.frm_main = func_tp_test_form_create();
}
//退出任务
static void func_tp_test_exit(void)
{
    f_tp_test_t *f_tp_test = (f_tp_test_t *)func_cb.f_cb;
    func_cb.last = FUNC_TP_TEST;
}

void func_tp_test(void)
{
    printf("%s\n", __func__);
    func_tp_test_enter();
    while (func_cb.sta == FUNC_TP_TEST)
    {
        func_tp_test_process();
        func_tp_test_message(msg_dequeue());
    }
    func_tp_test_exit();
}

#endif
