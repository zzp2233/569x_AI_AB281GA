#include "include.h"
#include "func.h"
#include "ute_module_charencode.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_breathe_finish_t_
{
    char str[30];
} f_breathe_finish_t;

enum
{
    COMPO_ID_BTN_OK=1,
};
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
compo_form_t *func_breathe_finish_form_create(void)
{
    char txt_buf[100];
    char time_buf[30];
    char time_num[10];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_BREATHE_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/5);

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3,GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/5);
    compo_textbox_set(textbox,txt_buf);

    // printf("breathe->time:%d\n",sys_cb.breathe_duration);
    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%ld",sys_cb.breathe_duration / 60000);

    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:%s",i18n[STR_BREATHE_TIME],time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+30, GUI_SCREEN_WIDTH, 30);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);///确定
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_setid(btn,COMPO_ID_BTN_OK);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set(textbox,i18n[STR_OK]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

compo_form_t *func_breathe_finish_form_create(void)
{
    char txt_buf[50];
    char time_buf[30];
    char time_num[10];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_BREATHE_HR_BIN);
    compo_picturebox_set_pos(picbox,116, 153);

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,108);
    compo_textbox_set(textbox,txt_buf);

    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%d",sys_cb.breathe_duration / 60000);

    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:%s",i18n[STR_BREATHE_TIME],time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 32/2+204, 200, 30);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);///确定
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,64/2+266);
    compo_setid(btn,COMPO_ID_BTN_OK);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,64/2+266);
    compo_textbox_set(textbox,i18n[STR_OK]);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_breathe_finish_form_create(void)
{

    char txt_buf[50];
    char time_buf[30];
    char time_num[10];

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_19_BREATHING_TRAINING_5_COMPLETE_THE_TRAINING_ICON_PIC40X40_X16_Y64_155_HEART_BIN);
    compo_picturebox_set_pos(picbox,45, 90);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 80, 52, UTE_DRV_SCREEN_WIDTH-80, 30);
    compo_textbox_set(textbox,i18n[STR_HEART_RATE]);
    compo_textbox_set_forecolor(textbox,make_color(150,150,150));
    compo_textbox_set_autoroll_mode(textbox,2);

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,80,82);
    compo_textbox_set(textbox,txt_buf);

    area_t txt_wid = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, txt_wid.wid+85, 94, 200, 30);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    //设置图片
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_19_BREATHING_TRAINING_5_COMPLETE_THE_TRAINING_ICON_PIC40X40_X16_Y64_155_TIME_BIN);
    compo_picturebox_set_pos(picbox,45, 90+95);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_TIME]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 80, 55+95, UTE_DRV_SCREEN_WIDTH-80, 30);
    compo_textbox_set(textbox,i18n[STR_BREATHE_TIME]);
    compo_textbox_set_forecolor(textbox,make_color(150,150,150));
    compo_textbox_set_autoroll_mode(textbox,2);

    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%ld",sys_cb.breathe_duration / 60000);
    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s",time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 85, 105+82, 200, 30);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I335001_19_BREATHING_TRAINING_2_1_SET_TIME_ICON_YES_208X52_X16_Y222_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I335001_19_BREATHING_TRAINING_2_1_SET_TIME_ICON_YES_208X52_X16_Y222_BIN).hei/2-10);
    compo_setid(btn_ok,COMPO_ID_BTN_OK);


    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-3, 220, 1);
    compo_shape_set_color(shape,make_color(47,47,47));

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
compo_form_t *func_breathe_finish_form_create(void)
{

    char txt_buf[50];
    char time_buf[30];
    char time_num[10];

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_19_BREATHING_TRAINING_TRAINING_TIME_BIN);
    compo_picturebox_set_pos(picbox,53, 132);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 110, 85, 200, 40);
    compo_textbox_set(textbox,i18n[STR_HEART_RATE]);
    compo_textbox_set_forecolor(textbox,make_color(150,150,150));

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,110,124+23);
    compo_textbox_set(textbox,txt_buf);

    area_t txt_wid = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, txt_wid.wid+118, 143, 200, 40);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    //设置图片
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_19_BREATHING_TRAINING_HEART_BIN);
    compo_picturebox_set_pos(picbox,53,268);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_TIME]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 110, 229, 200, 40);
    compo_textbox_set(textbox,i18n[STR_BREATHE_TIME]);
    compo_textbox_set_forecolor(textbox,make_color(150,150,150));

    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%ld",sys_cb.breathe_duration / 60000);
    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s",time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 110, 265+23, 200, 40);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_OK);


    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 200, 320, 1);
    compo_shape_set_color(shape,make_color(47,47,47));

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
compo_form_t *func_breathe_finish_form_create(void)
{

    char txt_buf[50];
    char time_buf[30];
    char time_num[10];

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338001_19_BREATHING_TRAINING_HEART_BIN);
    compo_picturebox_set_pos(picbox,52/2+20, 52/2+88);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 95, 80, 200, 35);
    compo_textbox_set(textbox,i18n[STR_HEART_RATE]);
    // compo_textbox_set_forecolor(textbox,make_color(150,150,150));

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,95,116);
    compo_textbox_set(textbox,txt_buf);

    area_t txt_wid = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, txt_wid.wid+105, 132, 200, 35);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    //设置图片
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_19_BREATHING_TRAINING_TRAINING_TIME_BIN);
    compo_picturebox_set_pos(picbox,52/2+20, 52/2+88+114);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_TIME]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 95, 80+114, 200, 35);
    compo_textbox_set(textbox,i18n[STR_BREATHE_TIME]);
    // compo_textbox_set_forecolor(textbox,make_color(150,150,150));

    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%ld",sys_cb.breathe_duration / 60000);
    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s",time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,95,116+114);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I338001_19_BREATHING_TRAINING_COMFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I338001_19_BREATHING_TRAINING_COMFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_OK);


    // compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    // compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-3, 220, 1);
    // compo_shape_set_color(shape,make_color(47,47,47));

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

compo_form_t *func_breathe_finish_form_create(void)
{
    char txt_buf[50];
    char time_buf[30];
    char time_num[10];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_BREATHE_HR_BIN);
    compo_picturebox_set_pos(picbox,116, 153);

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,108);
    compo_textbox_set(textbox,txt_buf);

    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%d",sys_cb.breathe_duration / 60000);

    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:%s",i18n[STR_BREATHE_TIME],time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 32/2+204, 200, 30);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);///确定
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,64/2+266);
    compo_setid(btn,COMPO_ID_BTN_OK);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,64/2+266);
    compo_textbox_set(textbox,i18n[STR_OK]);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

compo_form_t *func_breathe_finish_form_create(void)
{

    char txt_buf[50];
    char time_buf[30];
    char time_num[10];

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BREATHE_FINISH]);

    //设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I342001_19_BREATHING_TRAINING_HEART_BIN);
    compo_picturebox_set_pos(picbox,34/2+28, 34/2+59);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 78, 48, 200, 24);
    compo_textbox_set(textbox,i18n[STR_HEART_RATE]);
    // compo_textbox_set_forecolor(textbox,make_color(150,150,150));

    memset(txt_buf,0,sizeof(txt_buf));
    u8 heart_value = bsp_sensor_hrs_data_get();
    if(heart_value != 0 && heart_value!= 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,78,72);
    compo_textbox_set(textbox,txt_buf);

    area_t txt_wid = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, txt_wid.wid+83, 81, 200, 24);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    //设置图片
    picbox = compo_picturebox_create(frm, UI_BUF_I342001_19_BREATHING_TRAINING_TRAINING_TIME_BIN);
    compo_picturebox_set_pos(picbox,34/2+28, 34/2+120);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_TIME]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox, 78, 114, 200, 24);
    compo_textbox_set(textbox,i18n[STR_BREATHE_TIME]);
    // compo_textbox_set_forecolor(textbox,make_color(150,150,150));

    memset(time_num,0,sizeof(time_num));
    snprintf(time_num,sizeof(time_num),"%ld",sys_cb.breathe_duration / 60000);
    memset(time_buf,0,sizeof(time_buf));
    uteModuleCharencodeReplaceSubString(i18n[STR_MIN_JOINT], time_buf,"##",time_num);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s",time_buf);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_pos(textbox,78,136);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I342001_19_BREATHING_TRAINING_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I342001_19_BREATHING_TRAINING_CONFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_OK);


    // compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    // compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-3, 220, 1);
    // compo_shape_set_color(shape,make_color(47,47,47));

    return frm;
}

#else
compo_form_t *func_breathe_finish_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

static void func_breathe_finish_process(void)
{
    func_process();
}


static void func_breathe_finish_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:

            if(compo_get_button_id() == COMPO_ID_BTN_OK)
            {
                func_back_to();
            }
            break;
        default:
            func_message(msg);
            break;
    }
}


static void func_breathe_finish_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_breathe_finish_t));
    func_cb.frm_main = func_breathe_finish_form_create();
}


static void func_breathe_finish_exit(void)
{
    func_cb.last = FUNC_BREATHE_FINISH;
}


void func_breathe_finish(void)
{
    printf("%s\n", __func__);
    func_breathe_finish_enter();
    while (func_cb.sta == FUNC_BREATHE_FINISH)
    {
        func_breathe_finish_process();
        func_breathe_finish_message(msg_dequeue());
    }
    func_breathe_finish_exit();
}
