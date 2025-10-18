#include "include.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define CARD_SETTING_ITEM_CNT           9

#define CARD_ADD_TEXT_POS_X             15
#define CARD_ADD_TEXT_POS_Y             85
#define CARD_ADD_TEXT_HEI               widget_text_get_height()
#define CARD_ADD_TEXT_WID               GUI_SCREEN_WIDTH - 20

#define CARD_ADD_TEXT                   i18n[STR_ADDED]
#define CARD_UNADD_TEXT                 i18n[STR_CAN_ADD]

#define CARD_HEIGHT                     90

#define CO_CARD_POS_MAP(x,y,w,h)        ((x)+((w)/2)), ((y)+((h)/2)), (w), (h)
#define CO_ELEM_POS_MAP(x,y,w,h)        ((x)-((w)/2)), ((y)-((h)/2))
#define CO_CARD_POS_MAP_Y(y,h)          ((y)+((h)/2))

#define UI_UP_EN_BIN                    UI_BUF_I341001_28_SET_CARD_SETTINGS_UP_BIN
#define UI_UP_DIS_BIN                   UI_BUF_I341001_28_SET_CARD_SETTINGS_UP_DIS_BIN

#define UI_ADD_EN_BIN                   UI_BUF_I341001_28_SET_CARD_SETTINGS_ADD_BIN
#define UI_ADD_DIS_BIN                  UI_BUF_I341001_28_SET_CARD_SETTINGS_ADD_BIN

#define UI_DEL_EN_BIN                   UI_BUF_I341001_28_SET_CARD_SETTINGS_REDUCE_BIN
//#define UI_DEL_DIS_BIN                  UI_BUF_I341001_28_SET_CARD_SETTINGS_REDUCE_BIN

#define TEXT_COLOR_EN                   COLOR_WHITE
#define TEXT_COLOR_DIS                  COLOR_WHITE//COLOR_GRAY

#define CLICK_ICON_AREA_SIZE            20


typedef struct
{
    u16 str_id;
    u16 func_sta;
} card_str_t;

typedef struct card_info_t_
{
    u8 added_cnt;
    u8 unadd_cnt;
    card_str_t card_str_sta[CARD_SETTING_ITEM_CNT];
} card_info_t;

static const card_info_t card_info_raw =
{
    .added_cnt = 6,
    .unadd_cnt = 3,
    .card_str_sta = {
        [0] = {STR_EVREY_DAY_ACTIVITY,  FUNC_ACTIVITY},
        [1] = {STR_HEART_RATE,          FUNC_HEARTRATE},
        [2] = {STR_BLOOD_OXYGEN,        FUNC_BLOOD_OXYGEN},
        [3] = {STR_STRESS,              FUNC_PRESSURE},
        [4] = {STR_SLEEP,               FUNC_SLEEP},
        [5] = {STR_WEATHER,             FUNC_WEATHER},
        [6] = {STR_MUSIC,               FUNC_BT},
        [7] = {STR_PHONE,               FUNC_CALL},
        [8] = {STR_SPORTS,              FUNC_SPORT},
    },
};

static card_info_t card_info;
static bool ui_init_flag = false;
static bool unadd_text_is_create = false;

enum
{
    COMPO_ID_TXT_ADDED = 1,
    COMPO_ID_TXT_UNADD,

    COMPO_ID_CARD_START,
    COMPO_ID_CARD_END = COMPO_ID_CARD_START+CARD_SETTING_ITEM_CNT,

    COMPO_ID_SCROLLBAR = 20,
};

typedef struct f_card_setting_t_
{
    page_tp_move_t *ptm;
    u32 tick1;
    bool touch_flag;
} f_card_setting_t;


static void func_card_setting_update_o_create(compo_form_t* frm, bool is_create)
{
    if (is_create == false)
    {
        if (func_cb.sta != FUNC_CARD_SETTING)
        {
            return;
        }
    }
    card_info_t card_info_tmp;
    if (ui_init_flag == false)
    {
        memcpy(&card_info_tmp, &card_info_raw, sizeof(card_info_tmp));
    }
    else
    {
        memcpy(&card_info_tmp, &card_info, sizeof(card_info_tmp));
    }

//    printf("added:%d, unadd:%d ", card_info_tmp.added_cnt, card_info_tmp.unadd_cnt);
//    for(int i=0; i<CARD_SETTING_ITEM_CNT; i++) {
//        printf("[%d,%d] ",card_info_tmp.card_str_sta[i].str_id, card_info_tmp.card_str_sta[i].func_sta);
//    }
//    printf("\n");

    //卡片
    for (int i=0; i<COMPO_ID_CARD_END-COMPO_ID_CARD_START; i++)
    {
        if (i < card_info_tmp.added_cnt)    //已添加
        {
            if (i == 0)
            {
                //卡片添加
                compo_textbox_t* text_added = (is_create == true)? compo_textbox_create(frm, 20) : compo_getobj_byid(COMPO_ID_TXT_ADDED);
                compo_textbox_set_align_center(text_added, false);
                compo_textbox_set_location(text_added, CARD_ADD_TEXT_POS_X, CARD_ADD_TEXT_POS_Y, CARD_ADD_TEXT_WID, CARD_ADD_TEXT_HEI);
                compo_textbox_set(text_added, CARD_ADD_TEXT);
                compo_textbox_set_forecolor(text_added, make_color(102, 102, 102));
                if (is_create)
                {
                    compo_setid(text_added, COMPO_ID_TXT_ADDED);
                }
            }
            compo_cardbox_t* card = (is_create == true)? compo_cardbox_create(frm, 1, 2, 1, GUI_SCREEN_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
//            printf("card[%d] = 0x%x\n", COMPO_ID_CARD_START+i, card);
            if (is_create)
            {
                compo_setid(card, COMPO_ID_CARD_START+i);
            }
            compo_cardbox_set_location(card, CO_CARD_POS_MAP(0, CARD_ADD_TEXT_POS_Y+CARD_ADD_TEXT_HEI+CARD_HEIGHT*i, GUI_SCREEN_WIDTH, CARD_HEIGHT));
            compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
            compo_cardbox_rect_set_location(card, 0, 10, CARD_HEIGHT/2 - 1, 348, 1, 10);
            compo_cardbox_icon_set_location(card, 0, CO_ELEM_POS_MAP(19+38/2, CARD_HEIGHT/2, GUI_SCREEN_WIDTH, CARD_HEIGHT), 38, 38);
            compo_cardbox_icon_set(card, 0, (i == 0)? UI_UP_DIS_BIN : UI_UP_EN_BIN);
            compo_cardbox_icon_set_visible(card, 0, true);
            compo_cardbox_icon_set_location(card, 1, CO_ELEM_POS_MAP(311+38/2, CARD_HEIGHT/2, GUI_SCREEN_WIDTH, CARD_HEIGHT), 38, 38);
            compo_cardbox_icon_set(card, 1, UI_DEL_EN_BIN);
            widget_set_alpha(card->icon[1], (card_info_tmp.added_cnt == 1) ? 120 : 0xff);
            compo_cardbox_text_set_align_center(card, 0, false);
            compo_cardbox_text_set_location(card, 0, CO_ELEM_POS_MAP(71, CARD_HEIGHT/2-46/2+2, GUI_SCREEN_WIDTH, CARD_HEIGHT), 180, 46);
            compo_cardbox_text_set_forecolor(card, 0, (i == 0)? TEXT_COLOR_DIS : TEXT_COLOR_EN);
            compo_cardbox_text_set(card, 0, i18n[card_info_tmp.card_str_sta[i].str_id]);
        }
        else                //未添加
        {
            if (i == card_info_tmp.added_cnt)
            {
                //卡片未添加
                compo_textbox_t* text_added = (is_create == true || unadd_text_is_create == false)? compo_textbox_create(frm, 20) : compo_getobj_byid(COMPO_ID_TXT_UNADD);
                compo_textbox_set_align_center(text_added, false);
                compo_textbox_set_location(text_added, CARD_ADD_TEXT_POS_X, 15+CARD_ADD_TEXT_POS_Y+CARD_ADD_TEXT_HEI+CARD_HEIGHT*i, CARD_ADD_TEXT_WID, CARD_ADD_TEXT_HEI);
                compo_textbox_set(text_added, CARD_UNADD_TEXT);
                compo_textbox_set_forecolor(text_added, make_color(102, 102, 102));
                compo_textbox_set_visible(text_added, (card_info_tmp.unadd_cnt > 0) ? true : false);
                if (is_create || unadd_text_is_create == false)
                {
                    compo_setid(text_added, COMPO_ID_TXT_UNADD);
                }
//                if (is_create == true) {
//                    printf("unadd_text_is_create\n");
                unadd_text_is_create = true;
//                }
            }
            compo_cardbox_t* card = (is_create == true)? compo_cardbox_create(frm, 1, 2, 1, GUI_SCREEN_WIDTH, CARD_HEIGHT) : compo_getobj_byid(COMPO_ID_CARD_START+i);
            if (is_create)
            {
                compo_setid(card, COMPO_ID_CARD_START+i);
            }
            compo_cardbox_set_location(card, CO_CARD_POS_MAP(0, 15+CARD_ADD_TEXT_POS_Y+CARD_ADD_TEXT_HEI+CARD_ADD_TEXT_HEI+CARD_HEIGHT*i, GUI_SCREEN_WIDTH, CARD_HEIGHT));
            compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
            compo_cardbox_rect_set_location(card, 0, 10, CARD_HEIGHT/2 - 1, 348, 1, 10);
            compo_cardbox_icon_set_location(card, 0, CO_ELEM_POS_MAP(19+38/2, CARD_HEIGHT/2, GUI_SCREEN_WIDTH, CARD_HEIGHT), 38, 38);
            compo_cardbox_icon_set(card, 0, (i == 0)? UI_UP_DIS_BIN : UI_UP_EN_BIN);
            compo_cardbox_icon_set_visible(card, 0, false);
            compo_cardbox_icon_set_location(card, 1, CO_ELEM_POS_MAP(311+38/2, CARD_HEIGHT/2, GUI_SCREEN_WIDTH, CARD_HEIGHT), 38, 38);
            compo_cardbox_icon_set(card, 1, (i == 0)? UI_ADD_DIS_BIN : UI_ADD_EN_BIN);
            compo_cardbox_text_set_align_center(card, 0, false);
            compo_cardbox_text_set_location(card, 0, CO_ELEM_POS_MAP(19, CARD_HEIGHT/2-46/2+2, GUI_SCREEN_WIDTH, CARD_HEIGHT), 180, 46);
            compo_cardbox_text_set_forecolor(card, 0, (i == 0)? TEXT_COLOR_DIS : TEXT_COLOR_EN);
            compo_cardbox_text_set(card, 0, i18n[card_info_tmp.card_str_sta[i].str_id]);
        }
    }

    if (is_create == false && unadd_text_is_create == true)
    {
        compo_textbox_t* text_added = compo_getobj_byid(COMPO_ID_TXT_UNADD);
        compo_textbox_set_visible(text_added, (card_info_tmp.unadd_cnt > 0) ? true : false);
    }

}

compo_form_t* func_card_setting_form_create(void)
{
    compo_form_t* frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_CARD]);
    func_card_setting_update_o_create(frm, true);

    compo_scroll_t* scroll = compo_scroll_create(frm, SCROLL_TYPE_VERTICAL);
    compo_setid(scroll, COMPO_ID_SCROLLBAR);
    compo_scroll_set_w_r(scroll, 10);
    compo_scroll_set_range(scroll, 96);
    compo_scroll_set_color(scroll, COLOR_WHITE, make_color(51, 51, 51));
    compo_scroll_set_pos(scroll, 356, 78);
    compo_scroll_set_value(scroll, 0);
    compo_scroll_set_edge_circle(scroll, true);

    return frm;
}

static void func_card_setting_process(void)
{
    f_card_setting_t *f_card_setting = (f_card_setting_t*)func_cb.f_cb;

    func_card_setting_update_o_create(func_cb.frm_main, false);

    for (int i=0; i<COMPO_ID_CARD_END-COMPO_ID_CARD_START; i++)
    {
        compo_cardbox_t *card = compo_getobj_byid(COMPO_ID_CARD_START + i);
        //        if (id - COMPO_ID_TEXT_QUICK_START > uteModuleQuickReplyGetTotalContentCnt() - 1)
        compo_cardbox_text_scroll_process(card, true);
    }

    if(f_card_setting->ptm)
    {
        compo_page_move_process(f_card_setting->ptm);
    }

    s32 dx, dy;
    s16 value = abs_s(f_card_setting->ptm->move_offset) * 1000 / abs_s(f_card_setting->ptm->down_spring_offset);
    compo_scroll_t* scroll = compo_getobj_byid(COMPO_ID_SCROLLBAR);
    compo_scroll_set_pos(scroll, 356, 78 - f_card_setting->ptm->move_offset);
    compo_scroll_set_value(scroll, value);

    if (f_card_setting->touch_flag)
    {
        f_card_setting->tick1 = tick_get();
        widget_set_visible(scroll->scroll_bg, true);
        widget_set_visible(scroll->scroll_fg, true);
        f_card_setting->touch_flag = false;
    }
    f_card_setting->touch_flag = ctp_get_dxy(&dx, &dy);
    if(tick_check_expire(f_card_setting->tick1, 3000))
    {
        widget_set_visible(scroll->scroll_bg, false);
        widget_set_visible(scroll->scroll_fg, false);
    }

    func_process();
}

static u16 func_card_setting_click_card_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<COMPO_ID_CARD_END - COMPO_ID_CARD_START; i++)
    {
        id = COMPO_ID_CARD_START + i;
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

static void func_card_setting_click(void)
{
    f_card_setting_t *f_card_setting = (f_card_setting_t*)func_cb.f_cb;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_card_setting_click_card_id(pt);
    TRACE("%s -> click id:%d\n", __func__, compo_id);
    compo_cardbox_t* card = compo_getobj_byid(compo_id);
    rect_t up_icon_pos = compo_cardbox_get_icon_absolute(card, 0);
    rect_t add_del_icon_pos = compo_cardbox_get_icon_absolute(card, 1);

    u16 index = compo_id - COMPO_ID_CARD_START;


    if (compo_cardbox_icon_get_visible(card, 0) && (abs_s(pt.x - up_icon_pos.x) * 2 <= (up_icon_pos.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - up_icon_pos.y) * 2 <= (up_icon_pos.hei + CLICK_ICON_AREA_SIZE)))
    {
        TRACE("click icon up\n");
        card_str_t tmp_card_str;
        //处理tbl 顺序
        if (index > 0 && index < card_info.added_cnt)
        {
            tmp_card_str.func_sta = card_info.card_str_sta[index-1].func_sta;
            tmp_card_str.str_id = card_info.card_str_sta[index-1].str_id;

            card_info.card_str_sta[index-1].func_sta = card_info.card_str_sta[index].func_sta;
            card_info.card_str_sta[index-1].str_id = card_info.card_str_sta[index].str_id;

            card_info.card_str_sta[index].func_sta = tmp_card_str.func_sta;
            card_info.card_str_sta[index].str_id = tmp_card_str.str_id;
        }
    }

    if (compo_cardbox_icon_get_visible(card, 1) &&(abs_s(pt.x - add_del_icon_pos.x) * 2 <= (add_del_icon_pos.wid + CLICK_ICON_AREA_SIZE) && abs_s(pt.y - add_del_icon_pos.y) * 2 <= (add_del_icon_pos.hei + CLICK_ICON_AREA_SIZE)))
    {
        TRACE("click icon down\n");
        card_str_t tmp_card_str_1;
        card_str_t tmp_card_str[CARD_SETTING_ITEM_CNT];
        u8 tmp_card_str_index = 0;
        //处理tbl 顺序
        if (index >= 0 && index < card_info.added_cnt && card_info.added_cnt > 1)                                  //删除
        {
            card_info.added_cnt--;
            card_info.unadd_cnt++;
            for (int i=0; i<CARD_SETTING_ITEM_CNT; i++)
            {
                if (card_info.card_str_sta[i].func_sta != card_info.card_str_sta[index].func_sta &&
                    card_info.card_str_sta[i].str_id != card_info.card_str_sta[index].str_id)
                {
                    tmp_card_str[tmp_card_str_index] = card_info.card_str_sta[i];
                    tmp_card_str_index++;
                }
            }
            tmp_card_str[tmp_card_str_index] = card_info.card_str_sta[index];

            for(int i=0; i<CARD_SETTING_ITEM_CNT; i++)
            {
                TRACE("[%d,%d] ",tmp_card_str[i].str_id, tmp_card_str[i].func_sta);
                card_info.card_str_sta[i].func_sta = tmp_card_str[i].func_sta;
                card_info.card_str_sta[i].str_id = tmp_card_str[i].str_id;
            }
            TRACE("\n");
        }
        else if (index >= card_info.added_cnt && index < CARD_SETTING_ITEM_CNT)          //可添加
        {

            for (int i=0; i<CARD_SETTING_ITEM_CNT; i++)
            {
                if (i < card_info.added_cnt)
                {
                    tmp_card_str[tmp_card_str_index] = card_info.card_str_sta[i];
                    tmp_card_str_index++;
                }
                else if (i >= card_info.added_cnt)
                {
                    if (i == card_info.added_cnt)
                    {
                        tmp_card_str_1 = card_info.card_str_sta[i];
                        tmp_card_str[tmp_card_str_index] = card_info.card_str_sta[index];
                    }
                    else if (i == index)
                    {
                        tmp_card_str[tmp_card_str_index] = tmp_card_str_1;
                    }
                    else
                    {
                        tmp_card_str[tmp_card_str_index] = card_info.card_str_sta[i];
                    }
                    tmp_card_str_index++;
                }
            }

            for(int i=0; i<CARD_SETTING_ITEM_CNT; i++)
            {
                TRACE("[%d,%d] ",tmp_card_str[i].str_id, tmp_card_str[i].func_sta);
                card_info.card_str_sta[i].func_sta = tmp_card_str[i].func_sta;
                card_info.card_str_sta[i].str_id = tmp_card_str[i].str_id;
            }
            TRACE("\n");

            card_info.added_cnt++;
            card_info.unadd_cnt--;
        }
    }

    //保存序号
    func_cb.sort_cnt = card_info.added_cnt+1;
    memset(func_cb.tbl_sort, 0, sizeof(func_cb.tbl_sort));
    func_cb.tbl_sort[0] = FUNC_CLOCK;
    TRACE("tbl_sort cnt:%d -> [%d ", func_cb.sort_cnt, func_cb.tbl_sort[0]);
    for (int i=0; i<card_info.added_cnt; i++)
    {
        func_cb.tbl_sort[i+1] = card_info.card_str_sta[i].func_sta;
        TRACE("%d ", func_cb.tbl_sort[i+1]);
    }
    TRACE("]\n");


    uteModuleGuiCommonSavescreenTblSort(func_cb.tbl_sort, func_cb.sort_cnt);


}

static void func_card_setting_message(size_msg_t msg)
{
    f_card_setting_t *f_card_setting = (f_card_setting_t*)func_cb.f_cb;


    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_card_setting->ptm)
            {
                compo_page_move_touch_handler(f_card_setting->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            func_card_setting_click();
            break;
        default:
            func_message(msg);
            break;
    }

}

// 检查 value 是否在数组 arr 中（线性查找）
static uint16_t is_in_array(card_str_t value, u8 *arr, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        if (arr[i] == value.func_sta)
        {
            return 1;  // 找到
        }
    }
    return 0;  // 未找到
}

// 自定义排序函数（不使用 malloc 和 qsort）
static void custom_sort(u8 *a, uint16_t a_size, card_str_t *b, uint16_t b_size, card_str_t *result)
{


    // 1. 先把 a 的元素按顺序复制到 result 中
//    memcpy(result, a, a_size * sizeof(uint16_t));

    u8 tmp[CARD_SETTING_ITEM_CNT] = {0};
    u8 tmp_size = 0;
    TRACE("tmp[");
    for (uint16_t i=0; i<a_size; i++)
    {
        for (uint16_t j=0; j<b_size; j++)
        {
            if (a[i] == b[j].func_sta)
            {
                tmp[tmp_size] = a[i];
                TRACE("%d ", tmp[tmp_size]);
                tmp_size++;
            }
        }
    }
    TRACE("] size=%d\n", tmp_size);

    card_info.added_cnt = tmp_size;
    card_info.unadd_cnt = CARD_SETTING_ITEM_CNT - card_info.added_cnt;

    for (uint16_t i=0; i<tmp_size; i++)
    {
        result[i].func_sta = tmp[i];
        for (uint16_t j=0; j<b_size; j++)
        {
            if (tmp[i] == b[j].func_sta)
            {
                result[i].str_id = b[j].str_id;
                break;
            }
        }
    }

    // 2. 遍历 b，把不在 a 中的元素按顺序追加到 result 后面
    uint16_t result_index = tmp_size;  // 当前写入位置
    for (uint16_t i = 0; i < b_size; i++)
    {
        if (!is_in_array(b[i], tmp, tmp_size))
        {
            result[result_index++] = b[i];
        }
    }
}

static void func_card_setting_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_card_setting_t));

    func_cb.frm_main = func_card_setting_form_create();

    f_card_setting_t *f_card_setting = (f_card_setting_t*)func_cb.f_cb;
    f_card_setting->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 1024-50,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_card_setting->ptm, func_cb.frm_main->page_body, &info);
    f_card_setting->touch_flag = true;

    card_str_t result[CARD_SETTING_ITEM_CNT];  // 栈分配，避免 malloc
    if (ui_init_flag == false)
    {
        memcpy(&card_info, &card_info_raw, sizeof(card_info));
        custom_sort(func_cb.tbl_sort, func_cb.sort_cnt, card_info.card_str_sta, CARD_SETTING_ITEM_CNT, result);
        ui_init_flag = true;
    }
    else
    {
        custom_sort(func_cb.tbl_sort, func_cb.sort_cnt, card_info.card_str_sta, CARD_SETTING_ITEM_CNT, result);
    }

//    memcpy(card_info.card_str_sta, result, CARD_SETTING_ITEM_CNT);

    for(int i=0; i<CARD_SETTING_ITEM_CNT; i++)
    {
        TRACE("[%d,%d] ",result[i].str_id, result[i].func_sta);
        card_info.card_str_sta[i].func_sta = result[i].func_sta;
        card_info.card_str_sta[i].str_id = result[i].str_id;
    }
    TRACE("\n");

    for(int i=0; i<CARD_SETTING_ITEM_CNT; i++)
    {
        TRACE("[%d,%d] ",card_info.card_str_sta[i].str_id, card_info.card_str_sta[i].func_sta);
    }
    TRACE("\n");
}

static void func_card_setting_exit(void)
{
    func_cb.last = FUNC_CARD_SETTING;
    f_card_setting_t *f_card_setting = (f_card_setting_t*)func_cb.f_cb;
    if (f_card_setting->ptm)
    {
        func_free(f_card_setting->ptm);
    }
    unadd_text_is_create = false;
}

void func_card_setting(void)
{
    printf("%s\n", __func__);
    func_card_setting_enter();
    while (func_cb.sta == FUNC_CARD_SETTING)
    {
        func_card_setting_process();
        func_card_setting_message(msg_dequeue());
    }
    func_card_setting_exit();
}

