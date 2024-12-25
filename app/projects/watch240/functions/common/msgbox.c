#include "include.h"
#include "func_cover.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define ANIMATION_TICK_EXPIRE           18                          //对话框动画切换单位时间(ms)
#define ANIMATION_STEP                  (GUI_SCREEN_WIDTH / 8)      //步进
#define MSGBOX_MAX_TXT_LEN              64
#define MSGBOX_EXIT_TICK_EXPIRE         5000                        //定时退出单位时间(ms)

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_OK = 1,
    COMPO_ID_BTN_CANCEL,
    COMPO_ID_BTN_DELETE,
    COMPO_ID_BTN_REMIND_LATER,
};

//enum {
//    MSG_TYPE_WECHAT = 1,   //微信消息
//    MSG_TYPE_COVER,        //弹窗消息
//};

typedef struct msg_cb_t_
{
    compo_form_t *frm;
    int res;
    u32 tick;
    s32 animation_dx;
    bool show;
    bool flag_animation;
    bool flag_entering;
    u32 exit_tick;

    char msg_type;
    u8 enter_msg_sta;
} msg_cb_t;

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
                compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
//                compo_textbox_set_align_center(txt_msg, true);
                compo_textbox_set_align_center_top(txt_msg, true);
                compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X,
                                           func_cover_get_txt_y(msg_type),
                                           GUI_SCREEN_WIDTH-10, 128);              //调整文本位置
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
                                           160,widget_text_get_height());              //调整文本位置
                compo_textbox_set_multiline(txt_msg, true);
                compo_textbox_set(txt_msg, msg);
            }

            //time
            if (time != NULL)
            {
                compo_textbox_t *txt_time = compo_textbox_create(frm, 20);
                compo_textbox_set_align_center(txt_time, false);
                compo_textbox_set_pos(txt_time, gui_image_get_size(func_cover_get_pic_res_addr(msg_type)).wid/2 + 52,GUI_SCREEN_CENTER_Y/3.5);//调整文本位置
                compo_textbox_set(txt_time, time);
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
            }
            else
            {
            compo_form_add_image(frm, func_cover_get_pic_res_addr(msg_type),
                                 GUI_SCREEN_CENTER_X,
                                 func_cover_get_pic_y(msg_type)-20);  //需要更替为弹窗图标
            }

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
        break;


        case MSGBOX_MSG_TYPE_NONE:
        {
            //msg
            compo_textbox_t *txt_msg = compo_textbox_create(frm, MSGBOX_MAX_TXT_LEN);
            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
                                       GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT/2.5);              //调整文本位置
//            compo_textbox_set_multiline(txt_msg, true);
//            compo_textbox_set(txt_msg, msg);
//            compo_textbox_set_location(txt_msg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y,
//                                       widget_text_get_area(txt_msg->txt).wid, widget_text_get_area(txt_msg->txt).hei);              //调整文本位置
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
        case MSGBOX_MODE_BTN_FACTORR:///工厂测试
        {
            compo_textbox_t *textbox;
            textbox = compo_textbox_create(frm, strlen(i18n[STR_PASS]));///PASS
            compo_textbox_set(textbox, i18n[STR_PASS]);
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);

            btn = compo_button_create(frm);///透明按钮PASS
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
            compo_setid(btn,COMPO_ID_BTN_OK);

            textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
            compo_textbox_set(textbox, i18n[STR_FALL]);
            compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9);
            compo_textbox_set_forecolor(textbox, COLOR_RED);

            btn = compo_button_create(frm);///透明按钮FALL
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  GUI_SCREEN_CENTER_Y/10*2+GUI_SCREEN_HEIGHT/11*9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
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
                    msg_cb->res = MSGBOX_RES_ENTER_DETAIL_MSG;
                    msg_cb->flag_animation = true;
                    msg_cb->flag_entering = false;
                }
                else
                {
                    msgbox_button_click();                         //单击按钮
                }
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
        case KU_BACK:
            printf("KU_BACK\n");
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            break;

        case EVT_MSGBOX_EXIT:
            printf("EVT_MSGBOX_EXIT\n");
            msg_cb->res = MSGBOX_RES_EXIT;
            msg_cb->show = false;
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

    if (msg_cb->flag_animation)
    {
        if (tick_check_expire(msg_cb->tick, ANIMATION_TICK_EXPIRE))
        {
            msg_cb->tick = tick_get();
            if (msg_cb->flag_entering)
            {
                msg_cb->animation_dx += ANIMATION_STEP;
            }
            else
            {
                msg_cb->animation_dx -= ANIMATION_STEP;
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
            msgbox_exit_time = UTE_LOCAL_ALARM_DEFAULT_RING_TIMES * 1000;
        }
        else if ((msg_cb->msg_type == MSGBOX_MSG_TYPE_DETAIL)   ||                                                      //详细消息界面弹窗
                 (sys_cb.cover_index == REMIND_COVER_FIND_WATCH && msg_cb->msg_type == MSGBOX_MSG_TYPE_REMIND_COVER) ||    //查找手表
                 (msg_cb->msg_type == MSGBOX_MSG_TYPE_BRIEF)
        )
        {
            goto __exit;
        }

        if (tick_check_expire(msg_cb->exit_tick, msgbox_exit_time))   //定时退出
        {
            msg_cb->exit_tick = tick_get();
            msg_cb->flag_animation = true;
            msg_cb->flag_entering = false;
            msg_cb->res = MSGBOX_RES_TIMEOUT_EXIT;
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
    compo_form_scale_to(msg_cb->frm, 1, 1);
}

//退出对话框
static void msgbox_exit(void)
{
    msg_cb_t *msg_cb = func_cb.msg_cb;
    compo_form_destroy(msg_cb->frm);
    func_free(msg_cb);
    func_cb.msg_cb = NULL;
}

//对话框
int msgbox(char *msg, char *title, char* time, int mode, char msg_type)
{
    msg_cb_t *msg_cb;
    msgbox_enter(msgbox_frm_create(msg, title, time, mode, msg_type));
    msg_cb = func_cb.msg_cb;
    msg_cb->msg_type = msg_type;
    msg_cb->enter_msg_sta = func_cb.sta;
    while (msg_cb->show)
    {
        msgbox_message(msg_dequeue());
        msgbox_process();
    }
    int res = msg_cb->res;
    msgbox_exit();                      //这里释放到msg_cb结构体
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

