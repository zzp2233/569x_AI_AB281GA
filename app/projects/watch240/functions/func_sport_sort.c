#include "include.h"
#include "func.h"

#include "ute_all_sports_int_algorithms.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define CARD_WIDTH         109 //卡片宽度
#define CARD_HEIGHT        102 //卡片高度
#define CARD_ROUND         20  //卡片圆角
#define CARD_COLOUR        make_color(41,41,41)//卡片背景色
#define CARD_PIC_WIDTH     44 //卡片图片宽度
#define CARD_PIC_HEIGHT    44 //卡片图片高度
#define CARD_PIC_X         (-24)
#define CARD_PIC_Y         (-24)
#define CARD_TXT_WIDTH     86 //卡片文本宽度
#define CARD_TXT_HEIGHT    30 //卡片文本高度
#define CARD_TXT_X         (-46)
#define CARD_TXT_Y         (12)

typedef struct f_sport_sort_t_
{
    s32 card_x;
    s32 card_y;
    s32 card_pic_res;
    s32 card_txt_str;
} f_sport_sort_t;
3
#define SOPRT_MENU_CNT                       ((int)(sizeof(f_sport_sort) / sizeof(f_sport_sort[0])))
static const f_sport_sort_t f_sport_sort[]=
{
    {.card_x = 62,  .card_y = 109},
    {.card_x = 177, .card_y = 109},
    {.card_x = 62,  .card_y = 217},
    {.card_x = 177, .card_y = 217},
};

extern bool func_sport_get_by_order(uint8_t sport_order, uint16_t *str_idx, uint32_t *icon_addr);
compo_form_t *func_sport_sort_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);

    u16 str_id = 0;
    u32 icon_addr = 0;
    for(int idx=1; idx<=SOPRT_MENU_CNT; idx++)
    {
        compo_cardbox_t * cardbox = compo_cardbox_create(frm, 1, 1, 1, CARD_WIDTH, CARD_HEIGHT);
        compo_cardbox_set_location(cardbox,f_sport_sort[idx-1].card_x,f_sport_sort[idx-1].card_y,CARD_WIDTH, CARD_HEIGHT);
        compo_setid(cardbox, idx);

        compo_cardbox_rect_set_location(cardbox,0,0,0,CARD_WIDTH, CARD_HEIGHT,CARD_ROUND);
        compo_cardbox_rect_set_color(cardbox,0, CARD_COLOUR);

        if(idx == SOPRT_MENU_CNT)
        {
            str_id = STR_MORE;
            icon_addr = UI_BUF_I330001_SPORT_ICON_25_CAIDAN_BIN;
        }
        else
        {
            func_sport_get_by_order(idx, &str_id, &icon_addr);
        }

        compo_cardbox_icon_set(cardbox,0,icon_addr);
        compo_cardbox_icon_set_location(cardbox,0,CARD_PIC_X,CARD_PIC_Y,CARD_PIC_WIDTH,CARD_PIC_HEIGHT);

        compo_cardbox_text_set_location(cardbox,0,CARD_TXT_X,CARD_TXT_Y,CARD_TXT_WIDTH,CARD_TXT_HEIGHT);
        compo_cardbox_text_set_align_center(cardbox,0,false);
        compo_cardbox_text_set(cardbox,0,i18n[str_id]);
        compo_cardbox_text_scroll_process(cardbox, true);
    }
    return frm;
}

//获取点击卡片组件的id
static u16 func_sport_sort_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=1; i<=SOPRT_MENU_CNT; i++)
    {
        id = i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//卡片点击处理
static void func_sport_sort_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_sport_sort_card_get_btn_id(pt);
    if (compo_id == 0 || compo_id > SOPRT_MENU_CNT)
    {
        return;
    }
    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == SOPRT_MENU_CNT)           //更多运动
        {
            sys_cb.sport_idx = 0;
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
            func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
        }
        else
        {
            u8 sportMode = uteModuleSportFindHundredSportIndex(compo_id - 1);
            printf("sportMode = %d\n", sportMode);
            uteTaskGuiStartScreen(FUNC_SPORT_SWITCH, 0, __func__);
            uteModuleSportStartMoreSports(sportMode, 1, 0);
        }
    }
}

//更新卡片内容状态
static void func_sport_sort_card_update(void)
{
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    if(uteModuleSportGetAppUpdateTrainingListFlag())
    {
        extern bool func_sport_get_by_order(uint8_t sport_order, uint16_t *str_idx, uint32_t *icon_addr) ;
        u16 str_id = 0;
        u32 icon_addr = 0;
        func_sport_get_by_order(1, &str_id, &icon_addr);
        msg_enqueue(MSG_CHECK_LANGUAGE);
        return;
    }
#endif
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define CARD_WIDTH         136 //卡片宽度
#define CARD_HEIGHT        136 //卡片高度
#define CARD_ROUND         (CARD_HEIGHT/2)  //卡片圆角
#define CARD_COLOUR        make_color(41,41,41)//卡片背景色
#define CARD_PIC_WIDTH     50 //卡片图片宽度
#define CARD_PIC_HEIGHT    50 //卡片图片高度
#define CARD_PIC_X         (0)
#define CARD_PIC_Y         (-25)
#define CARD_TXT_WIDTH     100 //卡片文本宽度
#define CARD_TXT_HEIGHT    30 //卡片文本高度
#define CARD_TXT_X         (0)
#define CARD_TXT_Y         (25)

typedef struct f_sport_sort_t_
{
    s32 card_x;
    s32 card_y;
    s32 card_pic_res;
    s32 card_txt_str;
} f_sport_sort_t;

#define SOPRT_MENU_CNT                       ((int)(sizeof(f_sport_sort) / sizeof(f_sport_sort[0])))
static const f_sport_sort_t f_sport_sort[]=
{
    {.card_x = 108,  .card_y = 108},
    {.card_x = 252,  .card_y = 108},
    {.card_x = 108,  .card_y = 252},
    {.card_x = 252,  .card_y = 252},
};

extern bool func_sport_get_by_order(uint8_t sport_order, uint16_t *str_idx, uint32_t *icon_addr);
compo_form_t *func_sport_sort_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    u16 str_id = 0;
    u32 icon_addr = 0;
    for(int idx=1; idx<=SOPRT_MENU_CNT; idx++)
    {
        compo_cardbox_t * cardbox = compo_cardbox_create(frm, 1, 1, 1, CARD_WIDTH, CARD_HEIGHT);
        compo_cardbox_set_location(cardbox,f_sport_sort[idx-1].card_x,f_sport_sort[idx-1].card_y,CARD_WIDTH, CARD_HEIGHT);
        compo_setid(cardbox, idx);

        compo_cardbox_rect_set_location(cardbox,0,0,0,CARD_WIDTH, CARD_HEIGHT,CARD_ROUND);
        compo_cardbox_rect_set_color(cardbox,0, CARD_COLOUR);

        if(idx == SOPRT_MENU_CNT)
        {
            str_id = STR_MORE;
            icon_addr = UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN;
        }
        else
        {
            func_sport_get_by_order(idx, &str_id, &icon_addr);
        }

        compo_cardbox_icon_set(cardbox,0,icon_addr);
        compo_cardbox_icon_set_location(cardbox,0,CARD_PIC_X,CARD_PIC_Y,CARD_PIC_WIDTH,CARD_PIC_HEIGHT);

        compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_24_BIN);
        compo_cardbox_text_set_location(cardbox,0,CARD_TXT_X,CARD_TXT_Y,CARD_TXT_WIDTH,CARD_TXT_HEIGHT);
        compo_cardbox_text_set(cardbox,0,i18n[str_id]);
        compo_cardbox_text_scroll_process(cardbox, true);
    }
    return frm;
}

//获取点击卡片组件的id
static u16 func_sport_sort_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=1; i<=SOPRT_MENU_CNT; i++)
    {
        id = i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//卡片点击处理
static void func_sport_sort_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_sport_sort_card_get_btn_id(pt);
    if (compo_id == 0 || compo_id > SOPRT_MENU_CNT)
    {
        return;
    }
    printf("click compo_id:%d\n", compo_id);
    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == SOPRT_MENU_CNT)           //更多运动
        {
            sys_cb.sport_idx = 0;
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
            func_cb.sta = FUNC_SPORT;
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
        }
        else
        {
            u8 sportMode = uteModuleSportFindHundredSportIndex(compo_id - 1);
            printf("sportMode = %d\n", sportMode);
            uteTaskGuiStartScreen(FUNC_SPORT_SWITCH, 0, __func__);
            uteModuleSportStartMoreSports(sportMode, 1, 0);
        }
    }
}

//更新卡片内容状态
static void func_sport_sort_card_update(void)
{
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    if(uteModuleSportGetAppUpdateTrainingListFlag())
    {
        extern bool func_sport_get_by_order(uint8_t sport_order, uint16_t *str_idx, uint32_t *icon_addr) ;
        u16 str_id = 0;
        u32 icon_addr = 0;
        func_sport_get_by_order(1, &str_id, &icon_addr);
        msg_enqueue(MSG_CHECK_LANGUAGE);
        return;
    }
#endif
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

static void func_sport_sort_process(void)
{
    func_sport_sort_card_update();
    func_process();
}

static void func_sport_sort_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_sort_card_click();
            break;
        case MSG_CTP_LONG:
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            break;

        default:
            func_message(msg);
            break;
    }
}

static void func_sport_sort_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sort_t));
//    f_sport_sort_t *f_sport_sort = (f_sport_sort_t*)func_cb.f_cb;
    func_cb.frm_main = func_sport_sort_form_create();
}

static void func_sport_sort_exit(void)
{
//    f_sport_sort_t *f_sport_sort = (f_sport_sort_t*)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_SORT;
}

void func_sport_sort(void)
{
    printf("%s\n", __func__);
    func_sport_sort_enter();
    while (func_cb.sta == FUNC_SPORT_SORT)
    {
        func_sport_sort_message(msg_dequeue());
        func_sport_sort_process();
    }
    func_sport_sort_exit();
}


