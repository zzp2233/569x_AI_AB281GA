#include "include.h"
#include "func_cover.h"
#include "ute_project_config.h"
#include "ute_module_localRingtone.h"
#include "ute_drv_battery_common.h"
#include "ute_drv_motor.h"
#include "ute_module_message.h"
#include "ute_module_charencode.h"
#include "ute_module_systemtime.h"
#include "ute_module_sport.h"
#include "ute_module_menstrualcycle.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define ANIMATION_TICK_EXPIRE           18                          //对话框动画切换单位时间(ms)
#define ANIMATION_STEP                  (GUI_SCREEN_WIDTH / 8)      //步进
#define MSGBOX_MAX_TXT_LEN              100
#define MSGBOX_EXIT_TICK_EXPIRE         5000                        //定时退出单位时间(ms)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_OK = 1,
    COMPO_ID_BTN_CANCEL,
    COMPO_ID_BTN_DELETE,
    COMPO_ID_BTN_REMIND_LATER,
    COMPO_ID_BTN_ALARM_DELETE,
    COMPO_ID_BTN_ALARM_LATER,
};

//enum {
//    MSG_TYPE_WECHAT = 1,   //微信消息
//    MSG_TYPE_COVER,        //弹窗消息
//};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,
                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH-10, 128-20);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,
                                      func_cover_get_time_txt_y(msg_type));              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I330001_FINGWATCH_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_04_BIN);
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.7);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,GUI_SCREEN_WIDTH/1.2,30);//调整文本位置
                compo_textbox_set(txt_msg, i18n[STR_LOW_BATTERY]);
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/4.5,60,30);//调整文本位置
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_32_BIN);
                char level[4];
                memset(level,0,sizeof(level));
                sprintf(level,"%d%%",uteDrvBatteryCommonGetLvl());
                compo_textbox_set(txt_title, level);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE01_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,246 );
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 128);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {

                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           func_cover_get_title_txt_y(msg_type)-(widget_text_get_area(txt_title->txt).hei-widget_text_get_height()),
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK00_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK00_BIN).hei/2 - 10);
            break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK00_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_OK01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);  //需更替为删除图标
                compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20,
                                           gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).wid - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei, gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei);
                compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_SOON_BIN);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_CLOSE00_BIN).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_CLOSE00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_SOON_BIN).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,28+46/2);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,86+11);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,130+96/2,300,96);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,86+11);
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, 80, 240, 80);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),97,80);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg,130,50,170,36);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, 130,88);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I332001_FINGWATCH_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                // printf("555555555555555555555555\n");
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I332001_OTA_LOW_ELECTRICITY_BIN);
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.7);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 284+32/2,200,30);//调整文本位置
                compo_textbox_set(txt_msg, i18n[STR_LOW_BATTERY]);
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, 238+26/2,GUI_SCREEN_WIDTH,30);//调整文本位置
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_32_BIN);
                char level[4];
                memset(level,0,sizeof(level));
                sprintf(level,"%d%%",uteDrvBatteryCommonGetLvl());
                compo_textbox_set(txt_title, level);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,234+40);
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,28/2+140 );
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_64_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,75+18);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,150);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_t *txt_alarm_clock = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_alarm_clock, GUI_SCREEN_CENTER_X,72+34/2);
                compo_textbox_set(txt_alarm_clock, i18n[STR_ALARM_CLOCK]);
                compo_textbox_set_forecolor(txt_alarm_clock,make_color(0xff,0x83,0x00));

                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,160,70,32); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           220, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       258, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            printf("data:%s\n",msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>136)
            {
                txt_leng.hei = 136;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,258,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 136/2+72,258,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 68/2+100,
                                       275, 70);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set_multiline_drag(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 234+40);
            break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, 68+40,234+40);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, 212+40,234+40);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn,80/2+68,80/2+234);

            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_OK_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,80/2+212,80/2+234);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);  //需更替为删除图标
                compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20,
                                           gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).wid - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei, gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei);
                compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_SOON_BIN);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, 274);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4,274);
            break;
        case MSGBOX_MODE_BTN_FACTORR:///工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS"));///PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);///透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8, GUI_SCREEN_WIDTH / 2, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL"));///FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);///透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8, GUI_SCREEN_WIDTH / 2, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 40,
                                 40);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           130,
                                           GUI_SCREEN_WIDTH-10, 135);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_location(txt_time, GUI_SCREEN_CENTER_X, 40,GUI_SCREEN_WIDTH-20,widget_text_get_height());              //调整文本位置
                compo_textbox_set_forecolor(txt_time, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
                compo_textbox_set_right_align(txt_time, true);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                extern ute_drv_motor_t uteDrvMotorData;
                uteDrvMotorData.durationTimeMsec = UTE_MOTOR_DURATION_TIME;
                uteDrvMotorData.intervalTimeMsec = UTE_MOTOR_DURATION_TIME;

                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_REMIND_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
#define BAT_PERCENT_VALUE       uteDrvBatteryCommonGetLvl()  //电量百分比数值
                char txt_buf[30];
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_CHARGE_ICON_GIF_168X231_X36_Y53_LOW_BATTERY_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+20);

                memset(txt_buf,0,sizeof(txt_buf));
                snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
                compo_textbox_t *textbox = compo_textbox_create(frm, 5);
                compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-20,150,50);
                compo_textbox_set(textbox,txt_buf);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+20,120,30);
                compo_textbox_set_forecolor(textbox,COLOR_RED);
                compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I335001_27_MORE_3_TIMER_OUTPUT_4_END_BUTTON_ICON_PIC68X68_X32_140_Y194_NO_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,246 );
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 128);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_2_HONEYCOMB_ALARM_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-40);
                compo_picturebox_set_size(picbox,85,85);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
#if GUI_SCREEN_SIZE_240X284RGB_I335003_SUPPORT
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-110);
#else
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+60);
#endif
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
#if GUI_SCREEN_SIZE_240X284RGB_I335003_SUPPORT
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           GUI_SCREEN_CENTER_Y-118,
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
#else
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           GUI_SCREEN_CENTER_Y+54,
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
#endif
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
        {
            uint32_t res_ok=0;
            switch(func_cb.sta)
            {
                case FUNC_ALARM_CLOCK:
                    res_ok = UI_BUF_I335001_20_ALARM_CLOCK_6_QUANTITY_LIMIT_REMINDER_ICON_YES_208X52_X16_Y222_BIN;
                    break;
                case FUNC_HEARTRATE:
                case FUNC_BLOOD_OXYGEN:
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT
                case FUNC_PRESSURE:
#endif
                    res_ok = UI_BUF_I335001_7_SPO2_2_ICON_BUTTON_208X52_X16_Y222_00_BIN;
                    break;
                default:
                    res_ok = UI_BUF_I335001_20_ALARM_CLOCK_6_QUANTITY_LIMIT_REMINDER_ICON_YES_208X52_X16_Y222_BIN;
                    break;
            }
            btn = compo_button_create_by_image(frm, res_ok);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(res_ok).hei/2 - 10);
        }
        break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_00_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_3_EXERCISE_6_PAUSE_BUTTON_ICON_PIC102X52_X16_X122_Y222_01_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I335001_2_MESSAGE_LIST_ICON_DELETE_208X52_X16_Y521_BIN);  //需更替为删除图标
                // compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                // compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20,
                //                            gui_image_get_size(0).wid - gui_image_get_size(0).hei, gui_image_get_size(0).hei);
                // compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, 0);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335001_2_MESSAGE_LIST_ICON_DELETE_208X52_X16_Y521_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
#if GUI_SCREEN_SIZE_240X284RGB_I335003_SUPPORT
            btn = compo_button_create_by_image(frm, UI_BUF_I335003_ALARM_CLOCK_LATER_SLEEP_DELAY_BIN);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335003_ALARM_CLOCK_LATER_SLEEP_DELAY_BIN).hei/2 - 20);
#else
            btn = compo_button_create_by_image(frm, 0);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
#endif
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);

#if GUI_SCREEN_SIZE_240X284RGB_I335003_SUPPORT
            btn = compo_button_create_by_image(frm, UI_BUF_I335003_ALARM_CLOCK_LATER_CLOSE_BIN);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I335003_ALARM_CLOCK_LATER_CLOSE_BIN).hei/2 - 20);
#else
            btn = compo_button_create_by_image(frm, 0);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
#endif
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, 0);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
static uint16_t label[5]=
{
    STR_ALARM_CLOCK,    //闹钟
    STR_GETUP,    //起床
    STR_METTING,    //会议
    STR_APPOINTMENT,    //约会
    STR_DINNER_PARTY,    //聚餐
};
static compo_form_t* alarm_remdind_later(u8 index)
{
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 40);
    compo_textbox_set(textbox, (index == MSGBOX_RES_REMIND_LATER) ? i18n[STR_RING] : i18n[STR_CLOSED]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    return frm;
}

typedef struct
{
    u8 hour;
    u8 am_pm;
} msgbox_12hour_convert_t;

msgbox_12hour_convert_t msgbox_convert_to_12hour(u8 time)
{
    u8 am_pm = (time >= 12) ? 2 : 1;    //2 PM, 1 AM
    msgbox_12hour_convert_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (time == 0)
        {
            hour12.hour = 12;
        }
        else if (time > 12)
        {
            hour12.hour = time - 12;
        }
        else
        {
            hour12.hour = time;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = time;
    hour12.am_pm = 0;
    return hour12;
}

//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 44,
                                 44);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_location(txt_title,
                                           72, 26, 120, 48);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+4);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type)+5,
                                           GUI_SCREEN_WIDTH-10, 213);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
                compo_textbox_set_autoroll_mode(txt_msg, TEXT_AUTOROLL_MODE_NULL);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_right_align(txt_time, true);
                //compo_textbox_set_font(txt_time, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_location(txt_time, 200,
                                           28, 150, 48);              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
                if (widget_text_get_area(txt_time->txt).wid > 148)
                {
                    compo_textbox_set_right_align(txt_time, false);
                    compo_textbox_set(txt_time, time);
                }
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5+13, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5+13);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_location(txt_title, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 38,
                                           GUI_SCREEN_CENTER_Y/3.5+13, 144, 48);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 62,
                                           GUI_SCREEN_CENTER_Y/3.5+13-widget_text_get_height()-2,
                                           245,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_location(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 62,GUI_SCREEN_CENTER_Y/3.5+3+13, 230, 48);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_27_MORE_GIF_BIN);
                compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);  //需要更替为弹窗图标
                compo_animation_set_radix(animation, 12);
                compo_animation_set_interval(animation, 10);
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_ICON_WATCH_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

                compo_textbox_t *titleUser = compo_textbox_create(frm, strlen(i18n[STR_FIND_DEVICE]));
                compo_textbox_set_align_center(titleUser, false);
                compo_textbox_set_location(titleUser, FORM_TITLE_LEFT, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 4 - FORM_TITLE_LEFT, 50);
                compo_textbox_set(titleUser, i18n[STR_FIND_DEVICE]);
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);  //获取系统时间
                char time_buff[20] = {0};
                msgbox_12hour_convert_t timeHour = msgbox_convert_to_12hour(time.hour);
                if (timeHour.am_pm > 0)
                {
                    snprintf(time_buff, sizeof(time_buff), "%s%02d:%02d", timeHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM], timeHour.hour, time.min);
                }
                else
                {
                    snprintf(time_buff, sizeof(time_buff), "%02d:%02d", timeHour.hour, time.min);
                }
                titleUser = compo_textbox_create(frm, 20);
                compo_textbox_set_location(titleUser, FORM_TITLE_RIGHT + 20, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12, FORM_TITLE_HEIGHT);
                compo_textbox_set_multiline(titleUser, false);
                compo_textbox_set_align_center(titleUser, false);
                compo_textbox_set(titleUser, time_buff);
                //msg1
//                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
//                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
//                                           func_cover_get_txt_y(msg_type),
//                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
//                compo_textbox_set_multiline(txt_msg, true);
//                compo_textbox_set(txt_msg, msg);
//
//                //title
//                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
//                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
//                                      func_cover_get_title_txt_y(msg_type));
//                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
//                compo_textbox_set(txt_title, title);
            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                char *str_buf = ab_zalloc(strlen(i18n[STR_LOW_BATTERY_MODE])+10);
                char level[4];
                memset(level, 0, sizeof(level));
                snprintf(level, sizeof(level), "%d", uteDrvBatteryCommonGetLvl());
                uteModuleCharencodeReplaceSubString(i18n[STR_LOW_BATTERY_MODE], str_buf, "##", level);

                compo_picturebox_t *picbox;
                if (uteDrvBatteryCommonGetLvl() <= 10)
                {
                    picbox = compo_picturebox_create(frm, UI_BUF_I341001_CHARGE_LOW_BATTERY10_BIN);
                }
                else
                {
                    picbox = compo_picturebox_create(frm, UI_BUF_I341001_CHARGE_LOW_BATTERY20_BIN);
                }
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 97 + 184/2);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, strlen(str_buf));
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 355, 328, 200);
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, str_buf);
                compo_textbox_set_forecolor(txt_msg, COLOR_RED);
                //widget_text_set_ellipsis(txt_msg->txt, false);
                area_t txt_leng = widget_text_get_area(txt_msg->txt);
                if(txt_leng.hei > 120)
                {
                    txt_leng.hei = 120;
                }
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 360, 328, txt_leng.hei);  //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, str_buf);
                ab_free(str_buf);
#endif
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I341001_27_MORE_TIMER_CANCEL2_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X/2, 361);
                btn = compo_button_create_by_image(frm, UI_BUF_I341001_27_MORE_TIMER_AGAIN_BIN);  //again
                compo_setid(btn, COMPO_ID_BTN_OK);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X/2, 361);
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_total_sec);
                min = SEC_TO_MIN(sys_cb.timer_total_sec);
                sec = SEC_TO_SEC(sys_cb.timer_total_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 188 + 20);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_64_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_20_ALARM_CLOCK_CANCEL_BIN);///确定按钮
                compo_button_set_pos(btn_ok,49 + 110 / 2,294 + 110 / 2 + 10);
                compo_setid(btn_ok,COMPO_ID_BTN_ALARM_DELETE);

                btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_20_ALARM_CLOCK_SNOOZE_BIN);///确定按钮
                compo_button_set_pos(btn_ok,209 + 110 / 2,294 + 110 / 2 + 10);
                compo_setid(btn_ok,COMPO_ID_BTN_ALARM_LATER);

                u8 ringIndex = uteModuleSystemtimeGetAlarmRingIndex();//标签接口
                u8 data_len = 150;
                char str_buff[151] = {0};
                memset(str_buff,0,sizeof(str_buff));
                uteModuleSystemtimeGetAlarmLableData(str_buff,&data_len,ringIndex);

                compo_textbox_t *txt_msg = NULL;
                //msg1
                if (data_len)
                {
                    txt_msg = compo_textbox_create(frm, data_len);
                    compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                               184 + 120 / 2,
                                               GUI_SCREEN_WIDTH, 120); // 调整文本位置
                    compo_textbox_set_multiline(txt_msg, true);
                    compo_textbox_set_forecolor(txt_msg, make_color(0x66, 0x66, 0x66));
                    widget_text_set_ellipsis(txt_msg->txt, false);
                    compo_textbox_set(txt_msg, str_buff);
                    compo_textbox_set_align_center(txt_msg,true);
//                    txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
//                    compo_textbox_set_location(txt_msg, 274,
//                                               114 + 48 / 2,
//                                               120, 50); // 调整文本位置
//                    compo_textbox_set_forecolor(txt_msg, make_color(0x66, 0x66, 0x66));
//                    compo_textbox_set_align_center(txt_msg, false);
                }

                //if(txt_msg != NULL)
                {
                    u8 index = ute_moduleSystemtimeAlarmGetLabel(uteModuleSystemtimeGetAlarmRingIndex());//标签接口
                    txt_msg = compo_textbox_create(frm, strlen(i18n[label[index]]));
                    compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                               64 + 48 / 2,
                                               GUI_SCREEN_WIDTH, 50);              //调整文本位置
                    compo_textbox_set(txt_msg, i18n[label[index]]);//闹钟文字//
                }

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, 10);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,(100 + 64 / 2 + 20));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set(txt_title, title);
                //printf("alarm time = %s\n", title);

                if (msg != NULL)
                {
                    txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                    if(data_len)
                    {
                        compo_textbox_set_location(txt_msg, 8 + GUI_SCREEN_CENTER_X + widget_text_get_area(txt_title->txt).wid/2,
                                                   (94 + 64 / 2 + 20),
                                                   100, 50);              //调整文本位置
                        compo_textbox_set_align_center(txt_msg, false);
                    }
                    else
                    {
                        compo_textbox_set_location(txt_msg,GUI_SCREEN_CENTER_X,
                                                   (162 + 64 / 2 + 20),
                                                   100, 50);              //调整文本位置
                    }
                    compo_textbox_set_forecolor(txt_msg, make_color(0xff,0xff,0xff));
                    compo_textbox_set(txt_msg, msg);//上下午
                }
            }
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
            else if (sys_cb.cover_index == REMIND_COVER_PRAY)
            {
                uint16_t str_pray_buff[5] = {STR_FAJR, STR_DHUHR, STR_ASR, STR_MAGHRIB, STR_ISHA};

                char str_buff[20] = {0};
                ute_module_systemtime_custom_remind_t RemindData;
                uteModuleSystemtimeGetRemindData(&RemindData);
                extern uint8_t uteModuleScreenGetRemindSettingIndex(void);
                uint8_t RemindIndex = uteModuleScreenGetRemindSettingIndex();
                uint8_t hour_num = 0, min_num = 0, type = 0;
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);

                u16 width = 0;

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_pos(txt_msg, GUI_SCREEN_CENTER_X, 303 + 20);  //调整文本位置
                compo_textbox_set_font(txt_msg, UI_BUF_0FONT_FONT_NUM_64_BIN);

                for (uint8_t i = 0; i < APP_CUSTOM_REMIND_SETTING_DAY_CNT; i++)
                {
                    if ((RemindData.remindParam[RemindIndex].oneDayRemind[i].hour == time.hour) \
                        && (RemindData.remindParam[RemindIndex].oneDayRemind[i].min == time.min))
                    {
                        hour_num = RemindData.remindParam[RemindIndex].oneDayRemind[i].hour;
                        min_num = RemindData.remindParam[RemindIndex].oneDayRemind[i].min;
                        type = RemindData.remindParam[RemindIndex].oneDayRemind[i].type;
                        break;
                    }
                }

                if (uteModuleSystemtime12HOn())
                {
                    compo_textbox_t *txt_beind = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                    compo_textbox_set_pos(txt_beind, GUI_SCREEN_CENTER_X,
                                          303+18);
                    compo_textbox_set_align_center(txt_beind, false);
                    if(hour_num < 12)
                    {
                        snprintf(str_buff, 20, "%02d:%02d", hour_num, min_num);//, i18n[STR_AM]);
                        compo_textbox_set(txt_beind, i18n[STR_AM]);
                    }
                    else
                    {
                        hour_num %= 12;
                        if(hour_num == 0)
                        {
                            hour_num = 12;
                        }
                        snprintf(str_buff, 20, "%02d:%02d", hour_num, min_num);//, i18n[STR_PM]);
                        compo_textbox_set(txt_beind, i18n[STR_PM]);
                    }
                    compo_textbox_set_align_center(txt_msg, false);
                    compo_textbox_set(txt_msg, str_buff);
                    width = widget_text_get_area(txt_beind->txt).wid + widget_text_get_area(txt_msg->txt).wid;
                    compo_textbox_set_pos(txt_msg, GUI_SCREEN_CENTER_X - width/2 - 6,
                                          303-32);
                    compo_textbox_set_pos(txt_beind, GUI_SCREEN_CENTER_X - width/2 + widget_text_get_area(txt_msg->txt).wid + 6,
                                          303 + 8);
                }
                else
                {
                    snprintf(str_buff, 20, "%02d:%02d", hour_num, min_num);
                    compo_textbox_set(txt_msg, str_buff);
                }

                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-6);  //需要更替为弹窗图标
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, 390, 348, 50);  //调整文本位置
                compo_textbox_set(txt_title, i18n[str_pray_buff[type]]);

                uteDrvMotorStart(500, 500, 2);
            }
#endif
#if UTE_MODULE_SCREENS_MENSTRUAL_NOTIFY_SUPPORT
            else if (sys_cb.cover_index == REMIND_COVER_MENSTRUAL)
            {
                char buf[30] = {0};
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
                u8 type = uteModuleMenstrualCycleGetRemindStatus();
                if (type == 1 || (type == 2))
                {
                    compo_form_add_image(frm,  UI_BUF_I341001_26_WOMEN_HEALTH_00_BIN,
                                         GUI_SCREEN_CENTER_X,
                                         func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

                    //msg1
                    compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                    compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                               func_cover_get_txt_y(msg_type),
                                               GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height()); //调整文本位置
                    snprintf(buf, 30, "%s", (type == 1) ? i18n[STR_PERIOD_START] : i18n[STR_PERIOD_END]);
                    compo_textbox_set(txt_msg, buf);
                }
                else if (type == 3 || (type == 4))
                {
                    compo_form_add_image(frm,  UI_BUF_I341001_26_WOMEN_HEALTH_02_BIN,
                                         GUI_SCREEN_CENTER_X,
                                         func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

                    //msg1
                    compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                    compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                               func_cover_get_txt_y(msg_type),
                                               GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());  //调整文本位置
                    snprintf(buf, 30, "%s", (type == 3) ? i18n[STR_PERTILE_WINDOW_START] : i18n[STR_PERTILE_WINDOW_END]);
                    compo_textbox_set(txt_msg, buf);
                }
            }
#endif
#if APP_STAND_SPORT_STEP_KCAL_DISTANCE_NOTIFY_SUPPORT
            else if (sys_cb.cover_index == REMIND_COVER_GOAL)
            {
                u16 totalWid = 0;
                ute_module_target_notify_data_t todayTarget;
                uteModuleSportGetTodayTargetNotifyData(&todayTarget);

                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

                compo_textbox_t *txt_beind = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_pos(txt_beind, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_align_center(txt_beind, false);
                if (todayTarget.todayTargetNotifyType.isTodayDistanceTargetNotify)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_DIS_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KM]);
                }
                else if (todayTarget.todayTargetNotifyType.isTodayExerciseTimeTargetNotify)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_TIMES_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, i18n[STR_MINUTE]);
                }
                else if (todayTarget.todayTargetNotifyType.isTodayKcalTargetNotify)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_KCAL_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, i18n[STR_KCAL]);
                }
                else
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_STEPS_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, i18n[STR_STEP]);
                }

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set_forecolor(txt_msg, COLOR_GRAY);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type) + 20);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set(txt_title, title);

                totalWid = widget_text_get_area(txt_beind->txt).wid + widget_text_get_area(txt_title->txt).wid;
                if (totalWid > 360)
                {
                    compo_textbox_set_pos(txt_title, 15,
                                          func_cover_get_title_txt_y(msg_type) - 46 + 20);
                    compo_textbox_set_location(txt_beind, GUI_SCREEN_CENTER_X - totalWid/2 + widget_text_get_area(txt_title->txt).wid + 4,
                                               func_cover_get_title_txt_y(msg_type) - 5, 330 - widget_text_get_area(txt_title->txt).wid, widget_text_get_max_height());
                }
                else
                {
                    compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X - totalWid/2 - 4,
                                          func_cover_get_title_txt_y(msg_type) - 46 + 20);
                    compo_textbox_set_pos(txt_beind, GUI_SCREEN_CENTER_X - totalWid/2 + widget_text_get_area(txt_title->txt).wid + 4,
                                          func_cover_get_title_txt_y(msg_type) - 5);
                }
            }
#endif
            else if((sys_cb.cover_index >= REMIND_SPORT_LAST_SPEED) && (sys_cb.cover_index <= REMIND_SPORT_DANST_TARGET))
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);

                u16 totalWid = 0;
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

                compo_textbox_t *txt_beind = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_pos(txt_beind, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_align_center(txt_beind, false);
                if (sys_cb.cover_index == REMIND_SPORT_DANST_TARGET)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_DIS_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KM]);
                }
                else if (sys_cb.cover_index == REMIND_SPORT_TIME_TARGET)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_TIMES_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, i18n[STR_MINUTE]);
                }
                else if (sys_cb.cover_index == REMIND_SPORT_KCAL_TARGET)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_KCAL_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, i18n[STR_KCAL]);
                }
                else if (sys_cb.cover_index == REMIND_SPORT_LAST_SPEED)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_DIS_BIN, GUI_SCREEN_CENTER_X, 146);
                    char text_bux[20] = {0};
                    snprintf(text_bux,sizeof(text_bux),"%c%s",'/',uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KM]);
                    compo_textbox_set(txt_beind, text_bux);
                }
                else if (sys_cb.cover_index == REMIND_SPORT_LAST_KCAL)
                {
                    compo_form_add_image(frm, UI_BUF_I341001_3_EXERCISE_REMINDER_DURING_EXERCISE_KCAL_BIN, GUI_SCREEN_CENTER_X, 146);
                    compo_textbox_set(txt_beind, i18n[STR_KCAL]);
                }
                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set_forecolor(txt_msg, COLOR_GRAY);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type) + 20);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set(txt_title, title);

                totalWid = widget_text_get_area(txt_beind->txt).wid + widget_text_get_area(txt_title->txt).wid;
                if (totalWid > 360)
                {
                    compo_textbox_set_pos(txt_title, 15,
                                          func_cover_get_title_txt_y(msg_type) - 46 + 20);
                    compo_textbox_set_location(txt_beind, GUI_SCREEN_CENTER_X - totalWid/2 + widget_text_get_area(txt_title->txt).wid + 4,
                                               func_cover_get_title_txt_y(msg_type) - 5, 330 - widget_text_get_area(txt_title->txt).wid, widget_text_get_max_height());
                }
                else
                {
                    compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X - totalWid/2 - 4,
                                          func_cover_get_title_txt_y(msg_type) - 46 + 20);
                    compo_textbox_set_pos(txt_beind, GUI_SCREEN_CENTER_X - totalWid/2 + widget_text_get_area(txt_title->txt).wid + 4,
                                          func_cover_get_title_txt_y(msg_type) - 5);
                }
            }
            else if(sys_cb.cover_index == REMIND_HEART_WARING)
            {
                reset_sleep_delay_all();
                char txt_buf[20] = {0};
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_6_HEART_HEART_GIF_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,22 + 260 / 2);  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                compo_textbox_t *textbox = compo_textbox_create(frm, 3);
                compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set_align_center(textbox,false);
                compo_textbox_set_location(textbox,100,196 + 76 / 2 + 20, 180, 90);
                compo_textbox_set(textbox,title);
                compo_textbox_set_align_center(textbox, false);
                area_t leng_size = widget_text_get_area(textbox->txt);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox,false);
                compo_textbox_set_location(textbox,123 +leng_size.wid,238 + 76 / 2, 130, 40);
                compo_textbox_set_forecolor(textbox,make_color(0x66,0x66,0x66));
                compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

                textbox = compo_textbox_create(frm, 20);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,357 + 48 / 2, 340, 48);
                compo_textbox_set_forecolor(textbox,make_color(255,69,46));
                if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
                {
                    memset(txt_buf,0,sizeof(txt_buf));
                    snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);

                }
                else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
                {
                    memset(txt_buf,0,sizeof(txt_buf));
                    snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
                }
                compo_textbox_set(textbox,txt_buf);

            }
            else
            {
                if (sys_cb.cover_index == REMIND_COVER_HEALTH_SEDENTARY)
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                }
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BINDING:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, 80);
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
        }
        break;

        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN*3);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
        {
            uint32_t res_ok=0;
            switch(func_cb.sta)
            {
                case FUNC_ALARM_CLOCK:
                    res_ok = UI_BUF_I341001_20_ALARM_CLOCK_CONFIRM_BIN;
                    break;
                case FUNC_HEARTRATE:
                    res_ok = UI_BUF_I341001_6_HEART_REFRESH_BIN;
                    break;
                default:
                    res_ok = UI_BUF_I341001_6_HEART_REFRESH_BIN;
                    break;
            }
            btn = compo_button_create_by_image(frm, res_ok);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(res_ok).hei/2 - 20);
        }
        break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
        {

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN).hei/2 - 20);
            u32 index = 0;
            if(func_cb.sta == FUNC_ALARM_CLOCK_SUB_EDIT)
            {
                index = UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN;
            }
            else
            {
                index = UI_BUF_I341001_28_SET_CONFIRM_2_BIN;
            }
            btn = compo_button_create_by_image(frm, index);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_2_BIN).hei/2 - 20);
        }
        break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I341001_14_INFORMATION_DEL_BIN);  //需更替为删除图标
                // compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                // compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20,
                //                            gui_image_get_size(0).wid - gui_image_get_size(0).hei, gui_image_get_size(0).hei);
                // compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I341001_14_INFORMATION_DELETE_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_14_INFORMATION_DELETE_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, 0);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 44,
                                 44);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH-10, 128-20);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_font(txt_time, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_pos(txt_time, 295,
                                      40);              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                //compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_27_MORE_RING_BIN);
                //compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                //compo_animation_set_radix(animation,3);
                //compo_animation_set_interval(animation,30);
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_RING_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-15);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                char *str_buf = ab_zalloc(strlen(i18n[STR_LOW_BATTERY_MODE])+10);
                char level[4];
                memset(level,0,sizeof(level));
                snprintf(level,sizeof(level),"%d",uteDrvBatteryCommonGetLvl());
                uteModuleCharencodeReplaceSubString(i18n[STR_LOW_BATTERY_MODE], str_buf,"##",level);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, strlen(str_buf));
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20,GUI_SCREEN_WIDTH/1.2,60);//调整文本位置
                compo_textbox_set(txt_msg, str_buf);
                compo_textbox_set_multiline_drag(txt_msg,true);
                ab_free(str_buf);

                btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
                compo_setid(btn, COMPO_ID_BTN_CANCEL);
                compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                     GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN).hei/2 - 20);

                btn = compo_button_create_by_image(frm, UI_BUF_I341001_28_SET_CONFIRM_2_BIN);
                compo_setid(btn, COMPO_ID_BTN_OK);
                compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                     GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_2_BIN).hei/2 - 20);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I341001_27_MORE_TIMER_CANCEL2_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 361);
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 188);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_64_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_20_ALARM_CLOCK_CLOCK_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-40);
                compo_picturebox_set_size(picbox,85,85);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+60);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           GUI_SCREEN_CENTER_Y+51,
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
        {
            uint32_t res_ok=0;
            switch(func_cb.sta)
            {
                case FUNC_ALARM_CLOCK:
                    res_ok = UI_BUF_I341001_20_ALARM_CLOCK_CONFIRM_BIN;
                    break;
                case FUNC_HEARTRATE:
                    res_ok = UI_BUF_I341001_6_HEART_REFRESH_BIN;
                    break;
                default:
                    res_ok = UI_BUF_I341001_20_ALARM_CLOCK_CONFIRM_BIN;
                    break;
            }
            btn = compo_button_create_by_image(frm, res_ok);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(res_ok).hei/2 - 20);
        }
        break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CONFRIM2_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I341001_14_INFORMATION_DELETE_BIN);  //需更替为删除图标
                // compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                // compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20,
                //                            gui_image_get_size(0).wid - gui_image_get_size(0).hei, gui_image_get_size(0).hei);
                // compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, 0);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I341001_14_INFORMATION_DELETE_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, 0);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 44,
                                 44);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH-10, 128-20);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_font(txt_time, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_pos(txt_time, 295,
                                      40);              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                //compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_27_MORE_RING_BIN);
                //compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                //compo_animation_set_radix(animation,3);
                //compo_animation_set_interval(animation,30);
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I343001_27_MORE_RING_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-15);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                char *str_buf = ab_zalloc(strlen(i18n[STR_LOW_BATTERY_MODE])+10);
                char level[4];
                memset(level,0,sizeof(level));
                snprintf(level,sizeof(level),"%d",uteDrvBatteryCommonGetLvl());
                uteModuleCharencodeReplaceSubString(i18n[STR_LOW_BATTERY_MODE], str_buf,"##",level);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, strlen(str_buf));
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20,GUI_SCREEN_WIDTH/1.2,60);//调整文本位置
                compo_textbox_set(txt_msg, str_buf);
                compo_textbox_set_multiline_drag(txt_msg,true);
                ab_free(str_buf);

                btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CANCEL_BIN);
                compo_setid(btn, COMPO_ID_BTN_CANCEL);
                compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                     GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_COMM_CANCEL_BIN).hei/2 - 20);

                btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CONFIRM_BIN);
                compo_setid(btn, COMPO_ID_BTN_OK);
                compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                     GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_COMM_CONFIRM_BIN).hei/2 - 20);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CANCEL_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 361);
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 188);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I343001_20_ALARM_CLOCK_CLOCK_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-40);
                compo_picturebox_set_size(picbox,85,85);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+60);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           GUI_SCREEN_CENTER_Y+51,
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.1, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
        {
            uint32_t res_ok=0;
            switch(func_cb.sta)
            {
                case FUNC_ALARM_CLOCK:
                    res_ok = UI_BUF_I343001_COMM_OK_BIN;
                    break;
                case FUNC_HEARTRATE:
                    res_ok = UI_BUF_I343001_COMM_REFRESH_BIN;
                    break;
                default:
                    res_ok = UI_BUF_I343001_COMM_OK_BIN;
                    break;
            }
            btn = compo_button_create_by_image(frm, res_ok);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(res_ok).hei/2 - 20);
        }
        break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CANCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_COMM_CANCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CONFIRM_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_COMM_CONFIRM_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CANCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_COMM_CANCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I343001_COMM_CONFIRM_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_COMM_CONFIRM_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I343001_14_INFORMATION_DELETE_BIN);  //需更替为删除图标
                // compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                // compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20,
                //                            gui_image_get_size(0).wid - gui_image_get_size(0).hei, gui_image_get_size(0).hei);
                // compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, 0);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I343001_14_INFORMATION_DELETE_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, 0);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,
                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,80/2+244);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type)+15,
                                           270, 160);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,
                                      func_cover_get_time_txt_y(msg_type));              //调整文本位置
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, 80, 240, 80);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),97,80);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg,130,50,170,36);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, 130,88);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338001_REMIND_WATCH_RING_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,1);
                // compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
#define BAT_PERCENT_VALUE       uteDrvBatteryCommonGetLvl()  //电量百分比数值
                char txt_buf[30];
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_CHARGE_BATT_BG_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-296/2);

                memset(txt_buf,0,sizeof(txt_buf));
                snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
                compo_textbox_t *textbox = compo_textbox_create(frm, 5);
                compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-30,150,50);
                compo_textbox_set(textbox,txt_buf);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+25,120,30);
                compo_textbox_set_forecolor(textbox,COLOR_RED);
                compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, 0);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,246 );
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 128);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 274+34/2,250,40);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_20_ALARM_CLOCK_CLOCK_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,124/2+81);

                char str[100];
                memset(str,0,sizeof(str));
                snprintf(str,sizeof(str),"%s %s",title,msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X-30,250);
                compo_textbox_set_align_center(txt_title,false);
                compo_textbox_set(txt_title, str);

            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           220, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       311, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_HEIGHT/2.5)
            {
                txt_leng.hei = GUI_SCREEN_HEIGHT/2.5;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-widget_text_get_height(),txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       240, 165);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            widget_text_set_ellipsis(txt_msg->txt, false);      //避免既有滚动又有省略号的情况
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
        {
            uint32_t res_ok=0;
            switch(func_cb.sta)
            {
                case FUNC_ALARM_CLOCK:
                    res_ok = UI_BUF_I338001_20_ALARM_CLOCK_COMFIRM_BIN;
                    break;
                default:
                    res_ok = UI_BUF_I338001_20_ALARM_CLOCK_COMFIRM_BIN;
                    break;
            }
            btn = compo_button_create_by_image(frm, res_ok);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(res_ok).hei/2 - 20);
        }
        break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_COMFIRM_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, 80/2+209,80/2+224);

            btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_CANCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, 80/2+71,80/2+224);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_COMFIRM_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, 80/2+209,80/2+224);

            btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_CANCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, 80/2+71,80/2+224);
            break;
        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I338001_14_INFORMATION_DELETE_BIN);  //需更替为删除图标
            }
            else
            {
                btn = compo_button_create_by_image(frm, 0);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I338001_14_INFORMATION_DELETE_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); // FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_HEIGHT * 5 / 6);
            compo_textbox_set_forecolor(textbox, COLOR_RED);
            rect_t location_data = widget_get_location(textbox->txt);
            area_t txt_data = widget_text_get_area(textbox->txt);

            compo_button_t *btn = compo_button_create(frm); // 按钮FALL
            compo_button_set_location(btn,location_data.x, location_data.y,txt_data.wid,txt_data.hei*2.5);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);

            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); // PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_HEIGHT * 5 / 6);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);
            location_data = widget_get_location(textbox->txt);
            txt_data = widget_text_get_area(textbox->txt);

            btn = compo_button_create(frm); // 按钮PASS
            compo_button_set_location(btn,location_data.x, location_data.y,txt_data.wid,txt_data.hei*2.5);
            compo_setid(btn, COMPO_ID_BTN_OK);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, 0);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,28+46/2);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,86+11);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,130+96/2,300,96);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_pos(txt_time, GUI_SCREEN_CENTER_X,86+11);
                widget_text_set_color(txt_time->txt, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, 80, 240, 80);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),97,80);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg,130,50,170,36);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, 130,88);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I340001_FINGWATCH_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                // printf("555555555555555555555555\n");
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I340001_OTA_LOW_ELECTRICITY_BIN);
                compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.7);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 284+32/2,200,30);//调整文本位置
                compo_textbox_set(txt_msg, i18n[STR_LOW_BATTERY]);
                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, 238+26/2,GUI_SCREEN_WIDTH,30);//调整文本位置
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_32_BIN);
                char level[4];
                memset(level,0,sizeof(level));
                sprintf(level,"%d%%",uteDrvBatteryCommonGetLvl());
                compo_textbox_set(txt_title, level);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_CLOSE_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,234+90/2);
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,28/2+140 );
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_64_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,75+18);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,150);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_64_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_t *txt_alarm_clock = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_alarm_clock, GUI_SCREEN_CENTER_X,72+34/2);
                compo_textbox_set(txt_alarm_clock, i18n[STR_ALARM_CLOCK]);
                compo_textbox_set_forecolor(txt_alarm_clock,make_color(0xff,0x83,0x00));

                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,160,70,32); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           220, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       258, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            printf("data:%s\n",msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>136)
            {
                txt_leng.hei = 136;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,258,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, 136/2+72,258,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set_multiline_drag(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 234+40);
            break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, 68+40,234+40);

            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_OK01_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, 212+40,234+40);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn,90/2+62,90/2+234);

            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_OK_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn,90/2+206,90/2+234);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN);  //需更替为删除图标
                compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20,
                                           gui_image_get_size(UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN).wid - gui_image_get_size(UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN).hei, gui_image_get_size(UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN).hei);
                compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_CLOSE_BIN);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_SOON_BIN);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, 207+45, 234+45);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_CLOSE_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, 63+45,234+45);
            break;
        case MSGBOX_MODE_BTN_FACTORR:///工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS"));///PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);///透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8, GUI_SCREEN_WIDTH / 2, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL"));///FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);///透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*8, GUI_SCREEN_WIDTH / 2, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_button_t *btn;
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建遮罩层2
    compo_shape_t *masklayer2 = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //消息推送弹框使用，根据消息类型创建对应消息图标
    switch(msg_type)
    {
        case MSGBOX_MSG_TYPE_DETAIL:                  //详细消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_DETAIL\n");
            //图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 108+22/2,
                                 12+22/2);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, UTE_NOTIFY_MSG_CONTENT_MAX_SIZE+3);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           186, 128-20);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
//                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
//                compo_textbox_set_autoroll_mode(txt_msg, 2);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, true);
                compo_textbox_set_location(txt_time, GUI_SCREEN_CENTER_X, 70,GUI_SCREEN_WIDTH-20,widget_text_get_height());              //调整文本位置
                compo_textbox_set_forecolor(txt_time, make_color(128,128,128));
                compo_textbox_set(txt_time, time);
//                compo_textbox_set_right_align(txt_time, true);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_BRIEF:                   //简略消息弹窗
        {
            printf("MSGBOX_MSG_TYPE_BRIEF\n");

            //设置遮罩底层
            compo_shape_set_radius(masklayer, 20);
            compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            compo_shape_set_color(masklayer, make_color(0,0,0));
            compo_shape_set_alpha(masklayer, 0);

            //设置遮罩2
            compo_shape_set_radius(masklayer2, 15);
            compo_shape_set_location(masklayer2, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3.5, GUI_SCREEN_WIDTH/1.15, GUI_SCREEN_HEIGHT/5);
            compo_shape_set_color(masklayer2, make_color(44,44,44));
            compo_shape_set_alpha(masklayer2, 255);

            //图标
//            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
//                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 10,
//                                 func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 28,
                                 GUI_SCREEN_CENTER_Y/3.5);  //需要更替为弹窗图标

            //title
            if (title != NULL)
            {
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_align_center(txt_title, false);
                compo_textbox_set_pos(txt_title, 10,
                                      func_cover_get_title_txt_y(msg_type));
                //compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }

            //msg
            if (msg != NULL)
            {
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,
                                           GUI_SCREEN_CENTER_Y/3.5-widget_text_get_height()-2,
                                           145,widget_text_get_height()+3);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5+3);//调整文本位置
                compo_textbox_set(txt_time, time);
                compo_textbox_set_forecolor(txt_time, COLOR_GRAY);
            }
        }
        break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
        {
            //图标
            if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I342001_REMIND_WATCH_BIN);
                compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X,func_cover_get_pic_y(msg_type));  //需要更替为弹窗图标
                compo_animation_set_radix(animation,3);
                compo_animation_set_interval(animation,30);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);


            }
            else if (sys_cb.cover_index == REMIND_COVER_LOW_BATTERY)  //低电提醒
            {
#if UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
                char *str_buf = ab_zalloc(strlen(i18n[STR_LOW_BATTERY_MODE])+10);
                char level[4];
                memset(level,0,sizeof(level));
                snprintf(level,sizeof(level),"%d",uteDrvBatteryCommonGetLvl());
                uteModuleCharencodeReplaceSubString(i18n[STR_LOW_BATTERY_MODE], str_buf,"##",level);

                compo_textbox_t *txt_msg = compo_textbox_create(frm, strlen(str_buf));
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20,GUI_SCREEN_WIDTH/1.2,60);//调整文本位置
                compo_textbox_set(txt_msg, str_buf);
                compo_textbox_set_multiline_drag(txt_msg,true);
                ab_free(str_buf);

                btn = compo_button_create_by_image(frm, UI_BUF_I342001_3_EXERCISE_BTN_PAUSE_BIN);
                compo_setid(btn, COMPO_ID_BTN_CANCEL);
                compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                     GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_3_EXERCISE_BTN_PAUSE_BIN).hei/2 - 20);

                btn = compo_button_create_by_image(frm, UI_BUF_I342001_3_EXERCISE_BTN_PAUSE_BIN);
                compo_setid(btn, COMPO_ID_BTN_OK);
                compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                     GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_3_EXERCISE_BTN_PAUSE_BIN).hei/2 - 20);
#endif // UTE_MODULE_SCREENS_LOW_BATTERY_NOTIFY_SUPPORT
            }
            else if(sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)//计时器结束
            {
#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）
                u8 hour, min, sec;
                compo_button_t *btn;
                compo_textbox_t *txt;
                char str_buff[24];
                btn = compo_button_create_by_image(frm, UI_BUF_I342001_27_MORE_TIMEPIECE_CANCEL_BIN);  //close
                compo_setid(btn, COMPO_ID_BTN_DELETE);
                compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,246 );
                //新建数字
                hour = SEC_TO_HOUR(sys_cb.timer_custom_sec);
                min = SEC_TO_MIN(sys_cb.timer_custom_sec);
                sec = SEC_TO_SEC(sys_cb.timer_custom_sec);
                txt = compo_textbox_create(frm, 12);
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 128);
                compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d:%02d", hour, min, sec);
                compo_textbox_set(txt, str_buff);

                txt = compo_textbox_create(frm, strlen(i18n[STR_TIMER_FINIFH]));
                compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_X/1.5);
                compo_textbox_set(txt, i18n[STR_TIMER_FINIFH]);
            }
            else if (sys_cb.cover_index == REMIND_COVER_ALARM)//12小时制度闹钟特殊处理
            {
                compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_2_HONEYCOMB_CIRCLE_ALARM_BIN);
                compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-40);
                compo_picturebox_set_size(picbox,85,85);

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH, 50);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+60);
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set(txt_title, title);

                compo_textbox_set_multiline(txt_msg, false);
                compo_textbox_set_align_center(txt_msg, false);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X+widget_text_get_area(txt_title->txt).wid/2+8,
                                           GUI_SCREEN_CENTER_Y+54,
                                           widget_text_get_area(txt_msg->txt).wid,
                                           widget_text_get_height()); //调整文本位置
            }
            else
            {
                compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                     GUI_SCREEN_CENTER_X,
                                     func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标

                //msg1
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH/1.3, widget_text_get_max_height());              //调整文本位置
                compo_textbox_set(txt_msg, msg);

                //title
                compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
                compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X,
                                      func_cover_get_title_txt_y(msg_type));
                compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
                compo_textbox_set(txt_title, title);
            }
        }
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       200, GUI_SCREEN_HEIGHT);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);
            area_t txt_leng = widget_text_get_area(txt_msg->txt);
            if(txt_leng.hei>GUI_SCREEN_CENTER_Y)
            {
                txt_leng.hei = GUI_SCREEN_CENTER_Y;
            }
            if(msg == i18n[STR_ADDRESS_BOOK_SYNC])
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,txt_leng.wid,txt_leng.hei);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }
            else
            {
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, txt_leng.hei>widget_text_get_height()*3 ? 100 : GUI_SCREEN_CENTER_Y-txt_leng.hei/2,200,GUI_SCREEN_CENTER_Y);              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set_multiline_drag(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);

        }
        break;
        case MSGBOX_MSG_TYPE_SPORT:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.2,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y);              //调整文本位置
            compo_textbox_set_multiline(txt_msg, true);
            compo_textbox_set(txt_msg, msg);

            //title
            compo_textbox_t *txt_title = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);   //创建文本
            compo_textbox_set_pos(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
            compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_textbox_set(txt_title, title);
        }
        break;

        default:
            break;
    }

    //创建按钮
    switch (mode)
    {
        case MSGBOX_MODE_BTN_OK:            //确定按钮
        {
            uint32_t res_ok=0;
            switch(func_cb.sta)
            {
                case FUNC_ALARM_CLOCK:
                    res_ok = UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN;
                    break;
                case FUNC_HEARTRATE:
                    res_ok = UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN;
                    break;
                default:
                    res_ok = UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN;
                    break;
            }
            btn = compo_button_create_by_image(frm, res_ok);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(res_ok).hei/2 - 10);
        }
        break;

        case MSGBOX_MODE_BTN_OKCANCEL:      //确定与取消按钮1
            btn = compo_button_create_by_image(frm, UI_BUF_I342001_20_ALARM_CLOCK_CANCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_20_ALARM_CLOCK_CANCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_YESNO:         //确定与取消按钮2
            btn = compo_button_create_by_image(frm, UI_BUF_I342001_20_ALARM_CLOCK_CANCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4+5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_20_ALARM_CLOCK_CANCEL_BIN).hei/2 - 20);

            btn = compo_button_create_by_image(frm, UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN);
            compo_setid(btn, COMPO_ID_BTN_OK);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4-5,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN).hei/2 - 20);
            break;

        case MSGBOX_MODE_BTN_DELETE:        //删除按钮
            if (msg_type == MSGBOX_MSG_TYPE_DETAIL)
            {
                btn = compo_button_create_by_image(frm, UI_BUF_I342001_14_INFORMATION_DELETE_BIN);  //需更替为删除图标
                // compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CLEAR]));
                // compo_textbox_set_location(text, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20,
                //                            gui_image_get_size(0).wid - gui_image_get_size(0).hei, gui_image_get_size(0).hei);
                // compo_textbox_set(text, i18n[STR_CLEAR]);
            }
            else
            {
                btn = compo_button_create_by_image(frm, 0);  //需更替为删除图标

            }
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,
                                 GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I342001_14_INFORMATION_DELETE_BIN).hei/2 - 20);
            compo_setid(btn, COMPO_ID_BTN_DELETE);
            break;

        case MSGBOX_MODE_BTN_NONE:
            break;

        case MSGBOX_MODE_BTN_REMIND_LATER_CLOSE:        //稍后提醒与关闭按钮
            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_REMIND_LATER_BIN);
//            btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_OK01_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_REMIND_LATER);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);


            //btn = compo_button_create_by_image(frm, UI_BUF_POP_UP_CLOSE_BIN);
            btn = compo_button_create_by_image(frm, 0);
            compo_setid(btn, COMPO_ID_BTN_CANCEL);
            compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(0).hei/2 - 20);
            break;
        case MSGBOX_MODE_BTN_FACTORR://工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); //PASS
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);//透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); //FALL
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);//透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_CANCEL);

        }
        break;
        case MSGBOX_MODE_BTN_SURE:
        {
            btn = compo_button_create_by_image(frm, 0);///确定
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_setid(btn,COMPO_ID_BTN_OK);

            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]) );
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
            compo_textbox_set(textbox,i18n[STR_OK]);
        }
        break;
        default:
            halt(HALT_MSGBOX_MODE);
            break;
    }
    return frm;
}


#else
//创建对话框窗体
static compo_form_t *msgbox_frm_create(char *msg, char *title, char* time, int mode, char msg_type)
{
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
#endif //

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//单击按钮
static void msgbox_exit(void);
static void msgbox_check_force_exit(void);
static void msgbox_button_click(void)
{
    int id = compo_get_button_id();
    msg_cb_t *msg_cb = func_cb.msg_cb;
    switch (id)
    {
        case COMPO_ID_BTN_OK:
            msg_cb->res = MSGBOX_RES_OK;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_CANCEL:
            msg_cb->res = MSGBOX_RES_CANCEL;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;
        case COMPO_ID_BTN_DELETE:
            msg_cb->res = MSGBOX_RES_DELETE;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_REMIND_LATER:
            msg_cb->res = MSGBOX_RES_REMIND_LATER;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_ALARM_DELETE:
            msg_cb->res = MSGBOX_RES_DELETE;
            compo_form_destroy(msg_cb->frm);
            msg_cb->frm = alarm_remdind_later(msg_cb->res);
            msg_cb->alarm_ticks = tick_get();
#if UTE_MODULE_LOCAL_RINGTONE_ALARM_SUPPORT
            uteModuleLocalRingtoneStopRing();
            uteDrvMotorStop();
#endif
            break;
        case COMPO_ID_BTN_ALARM_LATER:
            if(uteModuleSystemtimeAlarmGetRepeatRemindTimes(uteModuleSystemtimeGetAlarmRingIndex()) == 1)
            {
                msg_cb->res = MSGBOX_RES_DELETE;
            }
            else
            {
                msg_cb->res = MSGBOX_RES_REMIND_LATER;
            }
            compo_form_destroy(msg_cb->frm);
            msg_cb->frm = alarm_remdind_later(msg_cb->res);
            msg_cb->alarm_ticks = tick_get();
#if UTE_MODULE_LOCAL_RINGTONE_ALARM_SUPPORT
            uteModuleLocalRingtoneStopRing();
            uteDrvMotorStop();
#endif
            break;

        default:
            break;
    }
}
#else
//单击按钮
static void msgbox_button_click(void)
{
    int id = compo_get_button_id();
    msg_cb_t *msg_cb = func_cb.msg_cb;
    switch (id)
    {
        case COMPO_ID_BTN_OK:
            msg_cb->res = MSGBOX_RES_OK;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_CANCEL:
            msg_cb->res = MSGBOX_RES_CANCEL;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_DELETE:
            msg_cb->res = MSGBOX_RES_DELETE;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case COMPO_ID_BTN_REMIND_LATER:
            msg_cb->res = MSGBOX_RES_REMIND_LATER;
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        default:
            break;
    }
}
#endif
//对话框消息处理
static void msgbox_message(size_msg_t msg)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
//            printf("MSG_CTP_CLICK\n");
            if (!msg_cb->flag_animation)
            {
                if (msg_cb->msg_type == MSGBOX_MSG_TYPE_BRIEF)
                {
                    point_t pt = ctp_get_sxy();
                    if (abs_s(pt.y - GUI_SCREEN_CENTER_Y/3.5) * 2 <= GUI_SCREEN_HEIGHT/5) //简略消息位置判断
                    {
                        msg_cb->res = MSGBOX_RES_ENTER_DETAIL_MSG;
                    }
                    else
                    {

                    }
                    msg_cb->flag_animation = true;
                    msg_cb->flag_entering = false;
                }
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
                else if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH && msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER)
                {
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_STOP, 1);
#if UTE_MODULE_LOCAL_RINGTONE_FIND_SUPPORT
                    uteModuleLocalRingtoneStopRing();
#endif
                    msg_cb->flag_animation = true;
                    msg_cb->flag_entering = false;
                    msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
                }
#endif
                else
                {
                    msgbox_button_click();                         //单击按钮
                }
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
        case KU_BACK:
        case KU_DELAY_BACK:
            if (msg_cb->msg_type != MSGBOX_MSG_TYPE_BINDING)
            {
                if(sys_cb.cover_index == REMIND_COVER_ALARM && msg_cb->res == MSGBOX_RES_NONE)
                {
                    if(uteModuleSystemtimeAlarmGetRepeatRemindTimes(uteModuleSystemtimeGetAlarmRingIndex()) == 1)
                    {
                        msg_cb->res = MSGBOX_RES_DELETE;
                    }
                    else
                    {
                        msg_cb->res = MSGBOX_RES_REMIND_LATER;
                    }
                    compo_form_destroy(msg_cb->frm);
                    msg_cb->frm = alarm_remdind_later(msg_cb->res);
                    msg_cb->alarm_ticks = tick_get();
#if UTE_MODULE_LOCAL_RINGTONE_ALARM_SUPPORT
                    uteModuleLocalRingtoneStopRing();
                    uteDrvMotorStop();
#endif
                    return;
                }
                if (tick_check_expire(msg_cb->tick, 200))  //1s钟后才能手动退出弹窗
                {
                    msg_cb->hand_exit_flag =true;
                }
                if(msg_cb->hand_exit_flag == false)
                {
                    return;
                }
                printf("KU_BACK\n");
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)
                {
                    msg_cb->res = MSGBOX_RES_DELETE;
                }
            }
            break;

        case EVT_MSGBOX_EXIT:
            printf("EVT_MSGBOX_EXIT\n");
            msg_cb->res = MSGBOX_RES_EXIT;
            msg_cb->show = false;
            break;
        case MSG_CHECK_LANGUAGE:
            printf("EVT_MSGBOX_EXIT\n");
            msg_cb->res = MSGBOX_RES_EXIT;
            msg_cb->show = false;
            msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            break;
        default:
            evt_message(msg);
    }
}


//对话框事件处理
static void msgbox_process(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    u32 wid, hei;
    reset_sleep_delay_all();
    if (msg_cb->flag_animation)
    {
        if (tick_check_expire(msg_cb->tick, ANIMATION_TICK_EXPIRE))
        {
            msg_cb->tick = tick_get();

            if (msg_cb->flag_entering)
            {
                if(sys_cb.cover_animat_dis)
                {
                    msg_cb->animation_dx = GUI_SCREEN_WIDTH;
                }
                else
                {
                    msg_cb->animation_dx += ANIMATION_STEP;
                }

            }
            else
            {
                if(sys_cb.cover_animat_dis)
                {
                    msg_cb->animation_dx = 0;
                }
                else
                {
                    msg_cb->animation_dx -= ANIMATION_STEP;
                }
            }
            if (msg_cb->animation_dx <= 0)
            {
                msg_cb->flag_animation = false;
                msg_cb->show = false;
                wid = 1;
                hei = 1;
            }
            else if (msg_cb->animation_dx >= GUI_SCREEN_WIDTH)
            {
                msg_cb->flag_animation = false;
                wid = GUI_SCREEN_WIDTH;
                hei = GUI_SCREEN_HEIGHT;
            }
            else
            {
                wid = msg_cb->animation_dx;
                hei = muls_shift16(GUI_SCREEN_HEIGHT, (wid << 16) / GUI_SCREEN_WIDTH);
            }
            compo_form_scale_to(msg_cb->frm, wid, hei);
        }
    }
    else
    {

        uint32_t msgbox_exit_time = MSGBOX_EXIT_TICK_EXPIRE;
        if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_COVER_ALARM)          //闹钟覆盖界面
        {
            reset_sleep_delay_all();
            msgbox_exit_time = UTE_LOCAL_ALARM_DEFAULT_RING_TIMES * 1000;
            if(msg_cb->res == MSGBOX_RES_REMIND_LATER || msg_cb->res == MSGBOX_RES_DELETE)
            {
                if (tick_check_expire(msg_cb->alarm_ticks, 5000))
                {
                    msg_cb->alarm_ticks = tick_get();
                    msg_cb->flag_animation = true;
                    msg_cb->flag_entering = false;
                }
                goto __exit;
            }
        }
        else if (msg_cb->msg_type == MSGBOX_MSG_TYPE_BINDING)
        {
            reset_sleep_delay_all();
            if (tick_check_expire(msg_cb->exit_tick, 30*1000))
            {
                msg_cb->exit_tick = tick_get();
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
            }
            goto __exit;
        }
        else if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_COVER_TIMER_FINISH)
        {
            reset_sleep_delay_all();
            if (tick_check_expire(msg_cb->exit_tick, 60*1000))
            {
                msg_cb->exit_tick = tick_get();
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
                msg_cb->timer_flag = false;
            }
            if (msg_cb->timer_flag && tick_check_expire(msg_cb->timer_tick, 3000))
            {
                msg_cb->timer_tick = tick_get();
                uteDrvMotorStart(250, 750, 2);
            }
            goto __exit;
        }
        else if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_GCOVER_BT_NOT_CONNECT)  //蓝牙未连接弹窗
        {
            if (bt_is_connected())              //蓝牙连接后立刻退出这个弹窗
            {
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;

                goto __exit;
            }
        }
        else if (msg_cb->msg_type == MSGBOX_MSG_TYPE_DETAIL)
        {
            goto __exit;
        }
        else if(sys_cb.cover_index == REMIND_COVER_FIND_WATCH && msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER)
        {
            reset_sleep_delay_all();
            if(uteDrvMotorGetRunningStatus() == false)
            {
                // uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 1);
            }
            if (tick_check_expire(msg_cb->exit_tick, 150*1000))    //查找手表
            {
                msg_cb->exit_tick = tick_get();
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
            }
            goto __exit;
        }
        else if(func_cb.sta == FUNC_FACTORY_TESTING || func_cb.sta == FUNC_TEST_MODE)
        {
            reset_sleep_delay_all();
            goto __exit;
        }

        if (tick_check_expire(msg_cb->exit_tick, msgbox_exit_time))   //定时退出
        {
            if (msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER && sys_cb.cover_index == REMIND_COVER_ALARM)          //闹钟覆盖界面
            {
                if(uteModuleSystemtimeAlarmGetRepeatRemindTimes(uteModuleSystemtimeGetAlarmRingIndex()) == 1)
                {
                    msg_cb->res = MSGBOX_RES_DELETE;
                }
                else
                {
                    msg_cb->res = MSGBOX_RES_REMIND_LATER;
                }
                compo_form_destroy(msg_cb->frm);
                msg_cb->frm = alarm_remdind_later(msg_cb->res);
                msg_cb->alarm_ticks = tick_get();
#if UTE_MODULE_LOCAL_RINGTONE_ALARM_SUPPORT
                uteModuleLocalRingtoneStopRing();
                uteDrvMotorStop();
#endif
            }
            else
            {
                msg_cb->exit_tick = tick_get();
                msg_cb->flag_animation = true;
                msg_cb->flag_entering = false;
                msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
            }
        }
    }

__exit:
    func_process();
}

//进入对话框
static void msgbox_enter(compo_form_t *frm)
{
    msg_cb_t *msg_cb = func_zalloc(sizeof(msg_cb_t));
    func_cb.msg_cb = msg_cb;
    msg_cb->frm = frm;
    msg_cb->show = true;
    msg_cb->flag_animation = true;
    msg_cb->flag_entering = true;
    msg_cb->tick = tick_get();
    msg_cb->exit_tick = tick_get();
    msg_cb->timer_tick = tick_get();
    msg_cb->timer_flag = true;
    compo_form_scale_to(msg_cb->frm, 1, 1);
}

//退出对话框
static void msgbox_exit(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    compo_form_destroy(msg_cb->frm);
    func_free(msg_cb);
    func_cb.msg_cb = NULL;
    uteDrvMotorStop();
#if UTE_MODULE_LOCAL_RINGTONE_FIND_SUPPORT
    uteModuleLocalRingtoneStopRing();
#endif
}


//界面sta切换到下列ID强制退出弹窗
static void msgbox_check_force_exit(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    if (func_cb.msgbox_enter_sta != func_cb.sta)
    {
        printf("%s -> EVT_MSGBOX_EXIT\n", __func__);
        msg_cb->res = MSGBOX_RES_EXIT;
        msg_cb->show = false;
    }
}

//对话框
int msgbox(char *msg, char *title, char* time, int mode, char msg_type)
{
    if (func_cb.sta == FUNC_CLOCK)
    {
        f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
        if (f_clk->sta != FUNC_CLOCK_MAIN)
        {
            printf("%s ---->EVT_MSGBOX_EXIT\n", __func__);
            return NULL;
        }
    }

    func_cb.msgbox_enter_sta = func_cb.sta;

    msg_cb_t *msg_cb;
    msgbox_enter(msgbox_frm_create(msg, title, time, mode, msg_type));
    msg_cb = func_cb.msg_cb;
    msg_cb->msg_type = msg_type;
    msg_cb->enter_msg_sta = func_cb.sta;
    while (msg_cb->show)
    {
        msgbox_message(msg_dequeue());
        msgbox_process();
        msgbox_check_force_exit();
    }
    int res = msg_cb->res;
    msgbox_exit();                      //这里释放到msg_cb结构体
    func_cb.msgbox_enter_sta = 0;
    sys_cb.cover_animat_dis = false;
    return res;
}


bool msgbox_frist_set_check(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    if (msg_cb != NULL)
    {
        if (msg_cb->enter_msg_sta != func_cb.sta)
        {
            return true;
        }
    }
    return false;
}

