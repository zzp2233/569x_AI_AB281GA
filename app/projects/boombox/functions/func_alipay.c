#include "include.h"
#include "func.h"
#include "func_alipay.h"
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
#include "alipay_ble_common.h"
#include "alipay_transit.h"
#endif

#if SECURITY_PAY_EN

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

enum
{
    COMPO_ID_RATE = 1,
    //按钮
    COMPO_ID_BTN_PAYCODE,       //付款码
    COMPO_ID_BTN_SETTING,       //设置
    COMPO_ID_BTN_HELP,          //帮助
    COMPO_ID_BTN_UNBIND,        //解除绑定
    COMPO_ID_BTN_UNBIND_YES,    //解除绑定-确认
    COMPO_ID_BTN_UNBIND_NO,     //解除绑定-取消
    COMPO_ID_BTN_CONFIRM,       //确认
    COMPO_ID_BTN_TRANSITCODE,   //乘车码
    COMPO_ID_BTN_SWITCH_TRANS_CODE, //切换乘车码
};

enum
{
    ALIPAY_PAGE_LOGO = 1,
    ALIPAY_PAGE_SCAN,
    ALIPAY_PAGE_BINDING,
    ALIPAY_PAGE_BIND_FAIL,
    ALIPAY_PAGE_OPTION_LIST,
    ALIPAY_PAGE_QRCODE,
    ALIPAY_PAGE_BARCODE,
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    ALIPAY_PAGE_LAST_TRANSITCODE,
    ALIPAY_PAGE_TRANSIT_LIST,
    ALIPAY_PAGE_TRANSITCODE_FAIL,
    ALIPAY_PAGE_TRANSITCODE,
#endif
    ALIPAY_PAGE_UNBIND,
    ALIPAY_PAGE_HELP,
    ALIPAY_PAGE_UNBIND_CONFIRM,
    ALIPAY_PAGE_UNBIND_FINISH,
};


typedef struct f_alipay_t_
{
    uint8_t page;
    uint8_t last_page;
    uint8_t logo_display_cnt;
    uint8_t binding_time_cnt;
    uint8_t bind_fail_time_cnt;
    uint8_t bind_success_time_cnt;
    uint8_t bind_code_refresh_time_cnt;
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    compo_listbox_t *listbox;
    uint8_t transitcode_sel;
    uint8_t transitcode[512];
    uint32_t len_transitcode;
    char error_str[256];
    compo_listbox_item_t *transit_list;
    char card_title[40];
#endif
} f_alipay_t;

extern struct Node* csi_head;
extern u32 __alipay_start, __alipay_size;

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
extern u32 __dram_ali_start, __dram_ali_size;
void gui_ali_init(void);
void get_customer_heap_info(void **buf, u32 *size);
int initializeFileSystem(void);
void get_customer_heap_info2(void **buf, u32 *size);
void customer_heap_init2(void *buf, u32 size);
//const char transit_url[256] = "alipays://platformapi/startapp?appId=20002047&scene=bus";
#endif

//创建支付宝logo窗体
compo_form_t *func_alipay_form_logo_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO2_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    return frm;
}

//创建支付宝窗体
compo_form_t *func_alipay_form_scan_create(void)
{


    int binding_str_len = 256;
    char binding_str[binding_str_len];
    memset(binding_str, 0, binding_str_len);
    u8 res = alipay_get_binding_code(binding_str, &binding_str_len);
    printf("res:%d, binding_str: %s\n", res, binding_str);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALIPAY]);

    compo_qrcodebox_t *qrcode = compo_qrcodebox_create(frm, 0, 100);
    compo_qrcodebox_set(qrcode, (char *)binding_str);

    func_free(binding_str);
    return frm;
}

//创建支付宝窗体
compo_form_t *func_alipay_form_binding_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALIPAY]);

    compo_picturebox_t *pic_bg1 = compo_picturebox_create(frm, UI_BUF_ALIPAY_BG1_BIN);
    compo_picturebox_set_pos(pic_bg1, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic_bg1, true);

    compo_arc_t *arc = compo_arc_create(frm);
    compo_setid(arc, COMPO_ID_RATE);
    compo_arc_set_alpha(arc, 0xff, 0);
    compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 180, 180);
    compo_arc_set_width(arc, 7);
    compo_arc_set_rotation(arc, 0);
    compo_arc_set_angles(arc, 0, 3600);
    compo_arc_set_color(arc, COLOR_BLUE, COLOR_WHITE);
    compo_arc_set_value(arc, 250);

    compo_picturebox_t *pic_logo = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO_BIN);
    compo_picturebox_set_pos(pic_logo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic_logo, true);

    return frm;
}

//创建支付宝绑定状态窗体
compo_form_t *func_alipay_form_bind_fail_create(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (!f_alipay)
    {
        return NULL;
    }

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_BIND_STA_BIN);
    compo_picturebox_cut(pic, 0, 2);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 30);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_BIND_FAILED]);

    return frm;
}

//创建支付宝选项列表窗体
compo_form_t *func_alipay_form_option_list_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//带乘车码
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    //新建按钮
    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_PAYCODE);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 100);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_TRANSITCODE);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 190);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_SETTING);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 280);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_HELP);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 370);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 0, 3);
    compo_picturebox_set_pos(pic, 40, 100);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 0, 3);
    compo_picturebox_set_pos(pic, 40, 190);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 1, 3);
    compo_picturebox_set_pos(pic, 40, 280);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 2, 3);
    compo_picturebox_set_pos(pic, 40, 370);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 100 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 190 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_TRANSITCODE]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 280- widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_SETTING]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 370 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_HELP]);
#else

    //新建按钮
    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_PAYCODE);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 100);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_SETTING);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 190);
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BG_BIN);
    compo_setid(btn, COMPO_ID_BTN_HELP);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 280);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 0, 3);
    compo_picturebox_set_pos(pic, 40, 100);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 1, 3);
    compo_picturebox_set_pos(pic, 40, 190);
    pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_ALI_OPTION_BIN);
    compo_picturebox_cut(pic, 2, 3);
    compo_picturebox_set_pos(pic, 40, 280);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 100 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 190 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_SETTING]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 70, 280 - widget_text_get_height() / 2, 220, 70);
    compo_textbox_set(txt, i18n[STR_HELP]);
#endif

    return frm;
}

//创建支付宝二维码窗体
compo_form_t *func_alipay_form_qrcode_create(void)
{

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //页码示意条
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_WHITE);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X - 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_GRAY);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X + 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);

    u32 paycode_len = 20;
    u8 *paycode = func_zalloc(paycode_len);
    memset(paycode, 0, paycode_len);
    u8 res = alipay_get_paycode(paycode, &paycode_len);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_paycode(paycode, &paycode_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_paycode err\n");
            break;
        }
    }
    printf("res:%d, paycode: %s\n", res, paycode);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, 64);
    compo_qrcodebox_set(qrbox, (char *)paycode);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 180);

    //新建图标
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_size(pic, 45, 45);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_SWITCH_BARCODE]);

    func_free(paycode);
    return frm;
}


#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
enum
{
    COMPO_ID_LISTBOX = 1,
};

AT(.alipay_data)
alipay_tansit_CardBaseVO_t card_list[11];


AT(.alipay_data)
compo_listbox_custom_item_t tbl_txt_list[11];

void transit_list_create(compo_form_t *frm, f_alipay_t *f_alipay, compo_listbox_custom_item_t *tbl_txt_list, int card_num)
{
    if(f_alipay->transit_list == NULL)
    {
        f_alipay->transit_list = func_zalloc(sizeof(compo_listbox_item_t)*card_num);
    }

    f_alipay->listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_NORMAL);
    compo_listbox_set(f_alipay->listbox, f_alipay->transit_list, card_num);
    compo_setid(f_alipay->listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_bgimg(f_alipay->listbox, UI_BUF_COMMON_BG_BIN);
    compo_listbox_set_text_modify(f_alipay->listbox, tbl_txt_list);
    compo_listbox_set_focus_byidx(f_alipay->listbox, 1);
    compo_listbox_update(f_alipay->listbox);


    compo_listbox_t *listbox = f_alipay->listbox;
    printf("listbox->mcb:%x\n", listbox->mcb);
    if (listbox->mcb == NULL)
    {
        listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    }
    compo_listbox_move_init_modify(listbox, 127, compo_listbox_gety_byidx(listbox, card_num - 2));
}

//创建支付宝二维码窗体
compo_form_t *func_alipay_form_transit_list_create(void)
{

    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    //新建窗体
    compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层

    int32_t res = RV_UNKNOWN;

    //未获取到上一次卡信息
    if (res != RV_OK)
    {

        uint32_t len_card_list = 2048;
        uint32_t card_num = 2048;
        res = alipay_transit_get_card_list_offline(card_list, &len_card_list, &card_num);
        if (res == RV_OK)
        {
            for(u8 i=0; i<card_num; i++)
            {
                printf("offline [%d]---> cardNo:%d, cardType:%d, title:%s\n", i, card_list[i].cardNo, card_list[i].cardType, card_list[i].title);
                memcpy(tbl_txt_list[i].str_txt, &card_list[i].title, 30 - 1);
            }
            transit_list_create(frm, f_alipay, tbl_txt_list, card_num);
        }
        printf("res2:%d, card_num:%d\n", res, (int *)card_num);

#define CARD_NUM 1

        if (res != RV_OK)
        {
            //读取失败, 本地不存在，通过联网接口获取
            if (res == 5)
            {
//                memset(card_list, 0, sizeof(alipay_tansit_CardBaseVO_t));
                res = alipay_transit_get_card_list_online(card_list, &len_card_list, &card_num);
                printf("res3:%d, %s\n", res, f_alipay->error_str);
                if (res == RV_OK)
                {

//                    const compo_listbox_item_t tbl_sport_list[card_num];
                    for(u8 i=0; i<card_num; i++)
                    {
                        printf("online [%d]---> cardNo:%d, cardType:%d, title:%s\n", i, card_list[i].cardNo, card_list[i].cardType, card_list[i].title);
                        memcpy(tbl_txt_list[i].str_txt, &card_list[i].title, 30 - 1);
                    }

                    transit_list_create(frm, f_alipay, tbl_txt_list, card_num);



                }
            }
        }

    }

    return frm;
}


//创建默认卡信息窗体
compo_form_t *func_alipay_form_last_transitcode_create(u8 *page_sta)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;

    //获取最近一次使用的乘车码码值，断电后会清空
    int32_t res = alipay_transit_get_the_last_transitCode(f_alipay->card_title, f_alipay->transitcode, &
                  f_alipay->len_transitcode, f_alipay->error_str, 256);

    printf("%s res:%d\n", __func__, res);

    if (res == RV_OK)       //成功，显示默认乘车码
    {
        //新建窗体
        compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层
        printf("Get transitcode success, len:%d\n", f_alipay->len_transitcode);
        print_r(f_alipay->transitcode, f_alipay->len_transitcode);
        func_alipay_form_create_by_page(ALIPAY_PAGE_TRANSITCODE);
        return frm;
    }
    else
    {
        *page_sta = ALIPAY_PAGE_TRANSIT_LIST;
        return func_alipay_form_transit_list_create();
    }

}

//创建乘车码二维码窗体
compo_form_t *func_alipay_form_transitcode_create(void)
{
//    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, f_alipay->len_transitcode+1);
    compo_qrcodebox_set_level(qrbox, QR_LEVEL_L);
    compo_qrcodebox_2d_set(qrbox, (const char *)f_alipay->transitcode, f_alipay->len_transitcode);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 180);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 30, 300, widget_text_get_height());
    compo_textbox_set(txt, f_alipay->card_title); //card_list[i].title
    txt = compo_textbox_create(frm, 50);
    compo_textbox_set_multiline(txt, true);
//    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 50, 300, widget_text_get_height());
//    char str_res[50];
//    sprintf(str_res, "transitcode res %d", res);
//    compo_textbox_set(txt, str_res);

    //新建文本
    compo_textbox_t *txt_switch = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt_switch, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 20, 200, 50);
    compo_textbox_set(txt_switch, "切换交通卡");

//    area_t text_area = widget_text_get_area(txt_switch->txt);

    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_SWITCH_TRANS_CODE);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 40, 200, 50);

    printf("Get transitcode len:%d\n", f_alipay->len_transitcode);
    return frm;

}

//创建支付宝绑定状态窗体
compo_form_t *func_alipay_form_transitcode_fail_create(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (!f_alipay)
    {
        return NULL;
    }

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_BIND_STA_BIN);
    compo_picturebox_cut(pic, 0, 2);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 80);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 64);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 300, 80);
    compo_textbox_set_multiline(txt, true);
    printf("AAA\n");
    compo_textbox_set(txt, f_alipay->error_str);
    printf("BBB\n");

    return frm;
}
#endif

//创建支付宝条形码窗体
compo_form_t *func_alipay_form_barcode_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    //页码示意条
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_GRAY);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X - 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, COLOR_WHITE);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X + 8, 15, 10, 10);
    widget_rect_set_radius(rect->rect, 5);

    u32 paycode_len = 20;
    u8 *paycode = func_zalloc(paycode_len);
    memset(paycode, 0, paycode_len);
    u8 res = alipay_get_paycode(paycode, &paycode_len);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_paycode(paycode, &paycode_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_paycode err\n");
            break;
        }
    }
    printf("res:%d, paycode: %s\n", res, paycode);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_HORIZONTAL, 64);
    compo_qrcodebox_set(qrbox, (char *)paycode);
    compo_qrcodebox_set_bitwid(qrbox, 10);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 200);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_PAYCODE]);
    txt = compo_textbox_create(frm, 32);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 80, 300, widget_text_get_height());
    compo_textbox_set(txt, (char *)paycode);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_SWITCH_QRCODE]);

    func_free(paycode);
    return frm;
}

//创建支付宝解绑（设置）窗体
compo_form_t *func_alipay_form_unbind_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);
    widget_rect_set_radius(rect->rect, 30);

    //新建按钮
    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_UNBIND);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);

    u8 res;
    u32 id_len = 128;
    u32 name_len = 128;
    u8 *id = func_zalloc(id_len);
    u8 *name = func_zalloc(name_len);
    memset(id, 0, id_len);
    memset(name, 0, name_len);
    res = alipay_get_logon_ID(id, &id_len);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_logon_ID(id, &id_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_logon_ID err\n");
            break;
        }
    }
    printf("alipay_get_logon_ID: %d: %s\n", res, id);
    res = alipay_get_nick_name(name, &name_len);
    retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_nick_name(name, &name_len);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_nick_name err\n");
            break;
        }
    }
    printf("alipay_get_logon_name: %d: %s\n", res, name);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_SETTING]);
    txt = compo_textbox_create(frm, 32);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 20, 300, widget_text_get_height());
    compo_textbox_set(txt, (char *)id);
    txt = compo_textbox_create(frm, 32);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20, 300, widget_text_get_height());
    compo_textbox_set_forecolor(txt, COLOR_GRAY);
    compo_textbox_set(txt, (char *)name);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 130, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_UNBIND]);

    func_free(id);
    func_free(name);

    return frm;
}

//创建支付宝二维码窗体
compo_form_t *func_alipay_form_help_create(void)
{

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_radius(rect, 80);

    u32 len_aid_code = 256;
    char *aid_code = func_zalloc(len_aid_code);
    memset(aid_code, 0, len_aid_code);
    u8 res = alipay_get_aid_code(aid_code, &len_aid_code, true);
    u8 retry_cnt = 0;
    while (res != ALIPAY_RV_OK)
    {
        res = alipay_get_aid_code(aid_code, &len_aid_code, true);
        retry_cnt++;
        if (retry_cnt > ALIPAY_API_RETRY_CNT)
        {
            retry_cnt = 0;
            printf("alipay_get_paycode err\n");
            break;
        }
    }
    printf("res:%d, aid_code: %s, len_aid_code: %d\n", res, aid_code, len_aid_code);

    //新建二维码
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, 200);
    compo_qrcodebox_set(qrbox, (char *)aid_code);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 200);

    //新建图标
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_ALIPAY_LOGO_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_size(pic, 45, 45);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 45, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_HELP_TILE]);

    func_free(aid_code);
    return frm;
}


//创建支付宝确认解绑窗体
compo_form_t *func_alipay_form_unbind_confirm_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); //白
    compo_shape_set_color(rect, COLOR_WHITE);
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X - 60, GUI_SCREEN_HEIGHT - 60, 100, 60);
    widget_rect_set_radius(rect->rect, 30);
    rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); //蓝
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X + 60, GUI_SCREEN_HEIGHT - 60, 100, 60);
    widget_rect_set_radius(rect->rect, 30);

    //新建按钮
    compo_button_t *btn = compo_button_create(frm);  //确认
    compo_setid(btn, COMPO_ID_BTN_UNBIND_YES);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X - 60, GUI_SCREEN_HEIGHT - 60, 100, 60);
    btn = compo_button_create(frm);  //取消
    compo_setid(btn, COMPO_ID_BTN_UNBIND_NO);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X + 60, GUI_SCREEN_HEIGHT - 60, 100, 60);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 50, 300, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_UNBIND]);
    txt = compo_textbox_create(frm, 64);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 300, 150);
    compo_textbox_set_multiline(txt, true);
    compo_textbox_set(txt, i18n[STR_ALIPAY_UNBIND_TIP]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X - 60, GUI_SCREEN_HEIGHT - 60, 80, widget_text_get_height());
    compo_textbox_set_forecolor(txt, COLOR_BLACK);
    compo_textbox_set(txt, i18n[STR_CONFIMR]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 60, GUI_SCREEN_HEIGHT - 60, 80, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_CANCEL]);

    return frm;
}

//创建支付宝解绑完成窗体
compo_form_t *func_alipay_form_unbind_finish_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮蓝色背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(51, 121, 246));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);
    widget_rect_set_radius(rect->rect, 30);

    //新建按钮
    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_CONFIRM);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 150, 60);

    //新建文本
    compo_textbox_t *txt = compo_textbox_create(frm, 64);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 300, 150);
    compo_textbox_set_multiline(txt, true);
    compo_textbox_set(txt, i18n[STR_ALIPAY_UNBIND_TIP2]);
    txt = compo_textbox_create(frm, 16);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 60, 130, widget_text_get_height());
    compo_textbox_set(txt, i18n[STR_CONFIMR]);

    return frm;
}

AT(.text.func.alipay)
void func_alipay_prepare(void)
{
    printf("__alipay_start: 0x%x, size: 0x%x\n", &__alipay_start, &__alipay_size);
    memset(&__alipay_start, 0, (u32)&__alipay_size);
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    memset(&__dram_ali_start, 0, (u32)&__dram_ali_size);
    printf("__dram_ali_start: 0x%x, size: 0x%x\n", &__dram_ali_start, &__dram_ali_size);
#endif

#if (SECURITY_PAY_VENDOR == SECURITY_VENDOR_HS)
    HS_IIC_Init();
#endif
#if (SECURITY_PAY_VENDOR == SECURITY_VENDOR_HED)
    uint8_t *alipay_com_outbuf = func_zalloc(200);
    uint32_t alipay_outlen =0;
    HED_IIC_Init();
    api_connect(alipay_com_outbuf, &alipay_outlen);
    printf("api_connect atr:\n");
    print_r(alipay_com_outbuf, alipay_outlen);
    func_free(alipay_com_outbuf);
#endif
    u8 sta = alipay_pre_init();
    printf("alipay_pre_init: %d\n", sta);
    if (sta != ALIPAY_RV_OK)
    {
        func_back_to();
    }

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    alipay_init_transit_module();
    initializeFileSystem();
#endif

    sta = alipay_get_binding_status();
    printf("alipay_get_binding_status: %d\n", sta);
    if (sta == true)
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
    }
    else
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_SCAN);
    }

    printf("func_alipay_prepare finish\n");
}

void func_alipay_init(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    f_alipay->page = ALIPAY_PAGE_LOGO;
}

//支付宝功能事件处理
static void func_alipay_process(void)
{
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (f_alipay->page == ALIPAY_PAGE_TRANSIT_LIST)
    {
        compo_listbox_move(f_alipay->listbox);
    }
#endif

    func_process();
}

//创建支付宝窗体（默认为logo页面）
compo_form_t *func_alipay_form_create(void)
{
    return func_alipay_form_logo_create();
}


//创建指定页面的窗体（销毁&创建）
void func_alipay_form_create_by_page(u8 page_sta)
{
    printf("%s: %d\n", __func__, page_sta);
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    u8 next_page_sta = page_sta;
    compo_form_t *frm = func_cb.frm_main;

    if (frm != NULL)
    {
        compo_form_destroy(frm);
        frm = NULL;
    }

    switch (next_page_sta)
    {
        case ALIPAY_PAGE_LOGO:
            frm = func_alipay_form_logo_create();
            break;

        case ALIPAY_PAGE_SCAN:
            frm = func_alipay_form_scan_create();
            break;

        case ALIPAY_PAGE_BINDING:
            frm = func_alipay_form_binding_create();
            break;

        case ALIPAY_PAGE_BIND_FAIL:
            frm = func_alipay_form_bind_fail_create();
            break;

        case ALIPAY_PAGE_OPTION_LIST:
            frm = func_alipay_form_option_list_create();
            break;

        case ALIPAY_PAGE_QRCODE:
            frm = func_alipay_form_qrcode_create();
            break;

        case ALIPAY_PAGE_BARCODE:
            frm = func_alipay_form_barcode_create();
            break;

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
        case ALIPAY_PAGE_LAST_TRANSITCODE:
            frm = func_alipay_form_last_transitcode_create(&next_page_sta);
            break;

        case ALIPAY_PAGE_TRANSIT_LIST:
            frm = func_alipay_form_transit_list_create();
            break;

        case ALIPAY_PAGE_TRANSITCODE_FAIL:
            frm = func_alipay_form_transitcode_fail_create();
            break;

        case ALIPAY_PAGE_TRANSITCODE:
            frm = func_alipay_form_transitcode_create();
            break;
#endif

        case ALIPAY_PAGE_UNBIND:
            frm = func_alipay_form_unbind_create();
            break;

        case ALIPAY_PAGE_HELP:
            frm = func_alipay_form_help_create();
            break;

        case ALIPAY_PAGE_UNBIND_CONFIRM:
            frm = func_alipay_form_unbind_confirm_create();
            break;

        case ALIPAY_PAGE_UNBIND_FINISH:
            frm = func_alipay_form_unbind_finish_create();
            break;
    }

    if (frm)
    {
        f_alipay->last_page = f_alipay->page;
        f_alipay->page = next_page_sta;

        printf(">>>next_page_sta[%d]\n", next_page_sta);
    }
}


//检测绑定状态改变之后跳转界面
void alipay_binding_detect(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (f_alipay->page == ALIPAY_PAGE_LOGO)
    {
        f_alipay->logo_display_cnt++;
        printf("logo_display_cnt: %d\n", f_alipay->logo_display_cnt);
        if (f_alipay->logo_display_cnt == ALIPAY_LOGO_TIMEOUT)
        {
            f_alipay->logo_display_cnt = 0;
            func_alipay_prepare();
        }

    }
    else if (f_alipay->page == ALIPAY_PAGE_SCAN || f_alipay->page == ALIPAY_PAGE_BINDING)
    {
        u32 ret;
        binding_status_e sta = ALIPAY_STATUS_UNKNOWN;
        ret = alipay_query_binding_result(&sta);
        printf("ret: %d, sta: %d, page: %d\n", ret, sta, f_alipay->page);

        if (sta == ALIPAY_STATUS_START_BINDING && f_alipay->page == ALIPAY_PAGE_SCAN)
        {
            func_alipay_form_create_by_page(ALIPAY_PAGE_BINDING);
        }
        compo_arc_t *arc = compo_getobj_byid(COMPO_ID_RATE);
        if (f_alipay->page == ALIPAY_PAGE_BINDING)
        {

            switch (sta)
            {
                case ALIPAY_STATUS_BINDING_OK:
                    compo_arc_set_value(arc, 1000);
                    break;
                case ALIPAY_STATUS_BINDING_FAIL:
                    break;
                default :
                    break;
            }
        }
        //binding success go to option
        if (sta == ALIPAY_STATUS_BINDING_OK && f_alipay->page == ALIPAY_PAGE_BINDING)
        {
            f_alipay->bind_success_time_cnt++;
            printf("bind_success_time_cnt: %d\n", f_alipay->bind_success_time_cnt);
            if (f_alipay->bind_success_time_cnt == ALIPAY_SUCCESS_TIMEOUT)
            {
                f_alipay->bind_success_time_cnt = 0;
                func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
            }
        }

        //binding timeout go to logo
        if (sta != ALIPAY_STATUS_BINDING_OK && f_alipay->page == ALIPAY_PAGE_BINDING)
        {
            f_alipay->binding_time_cnt++;
            if (f_alipay->binding_time_cnt == ALIPAY_BIND_TIMEOUT)
            {
                f_alipay->binding_time_cnt = 0;
                func_alipay_form_create_by_page(ALIPAY_PAGE_BIND_FAIL);
            }
        }

    }

    //bind failed go to logo
    else if (f_alipay->page == ALIPAY_PAGE_BIND_FAIL)
    {
        f_alipay->bind_fail_time_cnt++;
        printf("bind_fail_time_cnt: %d\n", f_alipay->bind_fail_time_cnt);
        if (f_alipay->bind_fail_time_cnt == ALIPAY_FAIL_TIMEOUT)
        {
            f_alipay->bind_fail_time_cnt = 0;
//                alipay_bind_env_destroy();
            printf("alipay_bind_env_destroy\n");
            printf("%s\n", __func__);
            freeList(&csi_head);
            func_alipay_prepare();
        }
    }

    //刷新支付码 ALIPAY_CODE_REFRESH_TIMEOUT 秒 刷新一次
    else if (f_alipay->page == ALIPAY_PAGE_QRCODE || f_alipay->page == ALIPAY_PAGE_BARCODE)
    {
        f_alipay->bind_code_refresh_time_cnt++;
        //printf("bind_code_refresh_time_cnt: %d\n", f_alipay->bind_code_refresh_time_cnt);

        if (f_alipay->bind_code_refresh_time_cnt >= ALIPAY_CODE_REFRESH_TIMEOUT)
        {
            printf("===>> qr code refresh\n");
            f_alipay->bind_code_refresh_time_cnt = 0;
            func_alipay_form_create_by_page(f_alipay->page);
        }
    }

}

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
void func_alipay_transitcode_list_click(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_alipay->listbox;

    f_alipay->transitcode_sel = compo_listbox_select(listbox, ctp_get_sxy());
    func_free(listbox->mcb);
    func_free(f_alipay->transit_list);
    f_alipay->transit_list = NULL;
    listbox->mcb = NULL;

//    if (f_alipay->transitcode_sel < 0 || f_alipay->transitcode_sel >= MENU_LIST_CNT) {
//        return;
//    }

//    char error_message[256];

    printf("select card %d\n", f_alipay->transitcode_sel);
    alipay_transit_card_status_t card_status;
    alipay_transit_check_card_status(card_list[f_alipay->transitcode_sel].cardNo, card_list[f_alipay->transitcode_sel].cardType, &card_status);
    printf("card 3 status--> is_exists:%d, expire_timestamp:%d, remain_use_count:%d\n", card_status.is_exists, card_status.expire_timestamp, card_status.remain_use_count);
    int32_t res = alipay_transit_update_card_data(card_list[f_alipay->transitcode_sel].cardNo, card_list[f_alipay->transitcode_sel].cardType, f_alipay->error_str, 512);
    if (res == RV_OK)
    {
        f_alipay->len_transitcode = 512;
        res = alipay_transit_get_TransitCode(card_list[f_alipay->transitcode_sel].cardNo, card_list[f_alipay->transitcode_sel].cardType, f_alipay->transitcode, &f_alipay->len_transitcode, f_alipay->error_str, 256);
        if (res == RV_OK)
        {
            printf("Get transitcode success, len:%d\n", f_alipay->len_transitcode);
            print_r(f_alipay->transitcode, f_alipay->len_transitcode);
            memcpy(f_alipay->card_title, card_list[f_alipay->transitcode_sel].title, 40);
            func_alipay_form_create_by_page(ALIPAY_PAGE_TRANSITCODE);
        }
        else
        {
            printf("res5:%d, %s\n", res, f_alipay->error_str);
            func_alipay_form_create_by_page(ALIPAY_PAGE_TRANSITCODE_FAIL);

        }
    }
    else
    {
        printf("res4:%d, %s\n", res, f_alipay->error_str);
        func_alipay_form_create_by_page(ALIPAY_PAGE_TRANSITCODE_FAIL);
    }
}
#endif

//支付宝按钮点击处理
static void func_alipay_click_handler(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    if (f_alipay->page == ALIPAY_PAGE_TRANSIT_LIST)
    {
        func_alipay_transitcode_list_click();
    }
    else
#endif

    {

        u8 id = compo_get_button_id();
        if (id)
        {
            printf(">>>button_id:%d\n", id);
            switch (f_alipay->page)
            {
                case ALIPAY_PAGE_OPTION_LIST:
                    if (id == COMPO_ID_BTN_PAYCODE)
                    {
                        func_alipay_form_create_by_page(ALIPAY_PAGE_QRCODE);
                    }
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
                    else if(id == COMPO_ID_BTN_TRANSITCODE)
                    {
                        func_alipay_form_create_by_page(ALIPAY_PAGE_LAST_TRANSITCODE);
                    }
#endif


                    else if(id == COMPO_ID_BTN_SETTING)
                    {
                        func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND);
                    }
                    else if(id == COMPO_ID_BTN_HELP)
                    {
                        func_alipay_form_create_by_page(ALIPAY_PAGE_HELP);
                    }
                    break;

                case ALIPAY_PAGE_UNBIND:
                    func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND_CONFIRM);
                    break;

                case ALIPAY_PAGE_UNBIND_CONFIRM:
                    if (id == COMPO_ID_BTN_UNBIND_YES)
                    {
                        if (alipay_unbinding() == ALIPAY_RV_OK)
                        {
                            freeList(&csi_head);
                            printf("alipay_unbinding success!\n");
                        }
                        else
                        {
                            printf("alipay_unbinding fail!\n");
                        }
                        func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND_FINISH);
                    }
                    else
                    {
                        func_alipay_form_create_by_page(ALIPAY_PAGE_UNBIND);
                    }
                    break;

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
                case ALIPAY_PAGE_TRANSITCODE:
                case ALIPAY_PAGE_LAST_TRANSITCODE:
                    if(id == COMPO_ID_BTN_SWITCH_TRANS_CODE)
                    {
                        func_alipay_form_create_by_page(ALIPAY_PAGE_TRANSIT_LIST);
                    }
                    break;
#endif

                case ALIPAY_PAGE_UNBIND_FINISH:
                    func_alipay_prepare();
                    break;
            }
        }
    }
}

//支付宝功能消息处理
static void func_alipay_touch_move(size_msg_t msg)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (msg == MSG_CTP_SHORT_RIGHT && f_alipay->page == ALIPAY_PAGE_BARCODE)
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_QRCODE);
    }

    if (msg == MSG_CTP_SHORT_LEFT && f_alipay->page == ALIPAY_PAGE_QRCODE)
    {
        func_alipay_form_create_by_page(ALIPAY_PAGE_BARCODE);
    }
    if (f_alipay->page == ALIPAY_PAGE_SCAN || f_alipay->page == ALIPAY_PAGE_OPTION_LIST)
    {
        func_message(msg);
    }
}

//支付宝功能消息处理
static void func_alipay_touch_key(void)
{
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    if (f_alipay->page == ALIPAY_PAGE_SCAN || f_alipay->page == ALIPAY_PAGE_OPTION_LIST)
    {
        func_directly_back_to();
        return;
    }


    switch (f_alipay->page)
    {
        case ALIPAY_PAGE_QRCODE:
        case ALIPAY_PAGE_BARCODE:
        case ALIPAY_PAGE_UNBIND:
        case ALIPAY_PAGE_UNBIND_CONFIRM:
        case ALIPAY_PAGE_HELP:
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
        case ALIPAY_PAGE_TRANSIT_LIST:
            if (f_alipay->page == ALIPAY_PAGE_TRANSIT_LIST)
            {
                compo_listbox_t *listbox = f_alipay->listbox;
                func_free(listbox->mcb);
                func_free(f_alipay->transit_list);
                f_alipay->transit_list = NULL;
                listbox->mcb = NULL;
                printf("free listbox\n");
            }
#endif
            func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
            break;

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
        case ALIPAY_PAGE_TRANSITCODE:
        case ALIPAY_PAGE_LAST_TRANSITCODE:
        case ALIPAY_PAGE_TRANSITCODE_FAIL:
            printf("last_page:%d\n", f_alipay->last_page);
            if (f_alipay->last_page == ALIPAY_PAGE_OPTION_LIST || f_alipay->page == ALIPAY_PAGE_LAST_TRANSITCODE)
            {
                func_alipay_form_create_by_page(ALIPAY_PAGE_OPTION_LIST);
            }
            else
            {
                func_alipay_form_create_by_page(ALIPAY_PAGE_TRANSIT_LIST);
            }
#endif
            break;
        default:
            break;
    }
}


//支付宝功能消息处理
static void func_alipay_message(size_msg_t msg)
{
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    f_alipay_t *f_alipay = (f_alipay_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_alipay->listbox;
    if (f_alipay->page == ALIPAY_PAGE_TRANSIT_LIST && compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
#endif

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_alipay_click_handler();

            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;
        case KU_BACK:
            func_alipay_touch_key();
            break;

        case MSG_SYS_1S:
            alipay_binding_detect();
            break;


        case MSG_CTP_SHORT_RIGHT:
            func_alipay_touch_move(MSG_CTP_SHORT_RIGHT);
            break;
        case MSG_CTP_SHORT_LEFT:
            func_alipay_touch_move(MSG_CTP_SHORT_LEFT);
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入支付宝功能
static void func_alipay_enter(void)
{
#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    os_gui_draw_w4_done();      //关tft前要等当前帧刷完
    gui_ali_init();
    void *heap;
    u32 heap_size;
    get_customer_heap_info2(&heap, &heap_size);
    printf("heap:%x, size:%x\n", heap, heap_size);
    customer_heap_init2(heap, heap_size);
#endif

    func_cb.f_cb = func_zalloc(sizeof(f_alipay_t));
    func_cb.frm_main = func_alipay_form_create();
    func_alipay_init();
}


//退出支付宝功能
static void func_alipay_exit(void)
{
    printf("%s\n", __func__);
    freeList(&csi_head);

#if SECURITY_PAY_EN && SECURITY_TRANSITCODE_EN
    void *heap;
    u32 heap_size;
    get_customer_heap_info(&heap, &heap_size);
    printf("heap:%x, size:%x\n", heap, heap_size);
    customer_heap_init2(heap, heap_size);
#endif

    func_cb.last = FUNC_ALIPAY;
}

//支付宝功能
void func_alipay(void)
{
    printf("%s\n", __func__);
    func_alipay_enter();
    while (func_cb.sta == FUNC_ALIPAY)
    {
        func_alipay_process();
        func_alipay_message(msg_dequeue());
    }
    func_alipay_exit();
}
#endif // SECURITY_PAY_EN

