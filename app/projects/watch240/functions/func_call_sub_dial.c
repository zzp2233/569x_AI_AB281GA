#include "include.h"
#include "func.h"

enum
{

    COMPO_ID_BTN_NUM0 = 1,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_BTN_NUM8,
    COMPO_ID_BTN_NUM9,
    COMPO_ID_BTN_CALL,
    COMPO_ID_BTN_DEL,
    COMPO_ID_NUM_DISP_ZERO,
    COMPO_ID_NUM_DISP_ONE,
    COMPO_ID_NUM_DISP_TWS,
    COMPO_ID_NUM_DISP_THR,
    COMPO_ID_NUM_DISP_FOUR,
    COMPO_ID_NUM_DISP_FIVE,
    COMPO_ID_NUM_DISP_SIX,
    COMPO_ID_NUM_DISP_SEV,
    COMPO_ID_NUM_DISP_EIG,
    COMPO_ID_NUM_DISP_NIN,
    COMPO_ID_NUM_DISP_TEN,
    COMPO_ID_NUM_DISP_ELE,
    COMPO_ID_BTN_INCOMING,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MSG,

    COMPO_ID_PIC_NUM,
};

#define CALL_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_call_disp_btn_item) / sizeof(tbl_call_disp_btn_item[0])))

typedef struct password_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
} call_disp_btn_item_t;

//按钮item，创建时遍历一下
static const call_disp_btn_item_t tbl_call_disp_btn_item[] =
{
    {UI_BUF_COMMON_1_CLICK_BIN,             COMPO_ID_BTN_NUM1,         45,     84},
    {UI_BUF_COMMON_2_CLICK_BIN,             COMPO_ID_BTN_NUM2,         120,    84},
    {UI_BUF_COMMON_3_CLICK_BIN,             COMPO_ID_BTN_NUM3,         195,    84},
    {UI_BUF_COMMON_4_CLICK_BIN,             COMPO_ID_BTN_NUM4,         45,     140},
    {UI_BUF_COMMON_5_CLICK_BIN,             COMPO_ID_BTN_NUM5,         120,    140},
    {UI_BUF_COMMON_6_CLICK_BIN,             COMPO_ID_BTN_NUM6,         195,    140},
    {UI_BUF_CALL_7_CLICK_BIN,               COMPO_ID_BTN_NUM7,         45,     198},
    {UI_BUF_COMMON_8_CLICK_BIN,             COMPO_ID_BTN_NUM8,         120,    198},
    {UI_BUF_CALL_9_CLICK_BIN,               COMPO_ID_BTN_NUM9,         195,    198},
    {UI_BUF_CALL_0_CLICK_BIN,               COMPO_ID_BTN_NUM0,         120,    252},
    {UI_BUF_SETTING_PASSWORD_DEL_CLICK_BIN, COMPO_ID_BTN_DEL,          195,    252},
    {UI_BUF_CALL_CALL_CLICK_BIN,            COMPO_ID_BTN_CALL,         45,    252},
};

typedef struct f_call_sub_dial_t_
{
    u8 phone_number_cnt;
    u8 phone_number[11];
} f_call_sub_dial_t;

//打电话页面
compo_form_t *func_call_sub_dial_form_create(void)
{

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALL_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_call_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn, tbl_call_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_call_disp_btn_item[idx_btn].x, tbl_call_disp_btn_item[idx_btn].y);
    }
    printf("W:%d H:%d\n",gui_image_get_size(UI_BUF_COMMON_1_CLICK_BIN).wid,gui_image_get_size(UI_BUF_COMMON_1_CLICK_BIN).hei);


    //创建文本
    compo_textbox_t *txt_title = compo_textbox_create(frm, strlen(i18n[STR_ENTER_NUMBER]));
    compo_setid(txt_title, COMPO_ID_PIC_NUM);
    //compo_textbox_set_align_center(txt_title, false);
    compo_textbox_set_pos(txt_title, 10, 18);
    compo_textbox_set_location(txt_title, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/6, GUI_SCREEN_WIDTH, widget_text_get_height());
    //compo_textbox_set_autoroll_mode(txt_title,1);
    compo_textbox_set(txt_title, i18n[STR_ENTER_NUMBER]);

    //创建数字
    compo_textbox_t *txt_num;
    txt_num = compo_textbox_create(frm, 11);
    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_setid(txt_num, COMPO_ID_NUM_DISP_ZERO);
    compo_textbox_set_align_center(txt_num, true);
    compo_textbox_set_pos(txt_num, 112, 30);
    compo_textbox_set(txt_num, "");

    return frm;
}

//创建来电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_call_answer_form_create(void)
{
    compo_button_t *btn;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //来电按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_ANSWER_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCOMING);
    compo_button_set_pos(btn, 54, 212);

    //挂电话按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_REJECT_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 188, 216);

    //信息按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_MES_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_MSG);
    compo_button_set_pos(btn, 120, 246);

    //创建消息文本
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_pos(txt, 120, 258);
    compo_textbox_set(txt,"信息");
    return frm;
}

//创建接通窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calling_form_create(void)
{
    compo_button_t *btn;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //新建按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_MUTE_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_CALL);
    compo_button_set_pos(btn, 36, 240);

    btn = compo_button_create_by_image(frm, UI_BUF_CALL_REJECT_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 120, 196);

    return frm;
}


//进入打电话界面
static void func_call_sub_dial_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_call_sub_dial_t));
    func_cb.frm_main = func_call_sub_dial_form_create();
}

//电话功能事件处理
static void func_call_sub_dial_process(void)
{
    func_process();                                  //刷新UI
}

//退出电话表盘功能
static void func_call_sub_dial_exit(void)
{
    func_cb.last = FUNC_CALL_SUB_DIAL;
}

//计算前八位的数字显示。
u32 func_calculate_number(u8 cnt, u8 *data)
{
    if(cnt == 8)
    {
        return data[0]*10000000+data[1]*1000000+data[2]*100000+data[3]*10000+data[4]*1000+data[5]*100+data[6]*10+data[7]*1;
    }
    else if(cnt == 7)
    {
        return data[0]*1000000+data[1]*100000+data[2]*10000+data[3]*1000+data[4]*100+data[5]*10+data[6]*1;
    }
    else if(cnt == 6)
    {
        return data[0]*100000+data[1]*10000+data[2]*1000+data[3]*100+data[4]*10+data[5]*1;
    }
    else if(cnt == 5)
    {
        return data[0]*10000+data[1]*1000+data[2]*100+data[3]*10+data[4]*1;
    }
    else if(cnt == 4)
    {
        return data[0]*1000+data[1]*100+data[2]*10+data[3]*1;
    }
    else if(cnt == 3)
    {
        return data[0]*100+data[1]*10+data[2]*1;
    }
    else if(cnt == 2)
    {
        return data[0]*10+data[1]*1;
    }
    else if(cnt == 1)
    {
        return data[0]*1;
    }
    return 0;
}

static void func_call_sub_dial_button_click(void)
{
    int id = compo_get_button_id();
    //获取文本组件的地址
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_PIC_NUM);

    //获取数字组件地址
    char buf[34];
    compo_textbox_t *txt_num = compo_getobj_byid(COMPO_ID_NUM_DISP_ZERO);

    f_call_sub_dial_t *call = (f_call_sub_dial_t *)func_cb.f_cb;

    switch (id)
    {
        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM9:
            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt < 11)
            {
                call->phone_number[call->phone_number_cnt++] = id - 1;
            }
            break;

        case COMPO_ID_BTN_CALL:
            memset(sys_cb.outgoing_number, 0, 16);
            for(int i=0; i<call->phone_number_cnt; i++)
            {
                sys_cb.outgoing_number[i] = call->phone_number[i] + 0x30;
            }
#if MODEM_CAT1_EN
            if (bsp_modem_get_init_flag())
            {
                modem_call_dial(sys_cb.outgoing_number);
            }
            else
#endif
            {
                bt_call_redial_number();
            }
            break;

        case COMPO_ID_BTN_DEL:
            compo_textbox_set_visible(txt, false);
            if(call->phone_number_cnt > 0)
            {
                call->phone_number_cnt--;
            }
            break;

        default:
            break;
    }

    memset(buf, 0, sizeof(buf));
    if(call->phone_number_cnt == 1)
    {
        snprintf(buf, sizeof(buf), "%d", call->phone_number[0]);
    }
    else if(call->phone_number_cnt == 2)
    {
        snprintf(buf, sizeof(buf), "%d%d", call->phone_number[0], call->phone_number[1]);
    }
    else if(call->phone_number_cnt == 3)
    {
        snprintf(buf, sizeof(buf), "%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2]);
    }
    else if(call->phone_number_cnt == 4)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3]);
    }
    else if(call->phone_number_cnt == 5)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3], call->phone_number[4]);
    }
    else if(call->phone_number_cnt == 6)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3], call->phone_number[4], call->phone_number[5]);
    }
    else if(call->phone_number_cnt == 7)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3],
                 call->phone_number[4], call->phone_number[5], call->phone_number[6]);
    }
    else if(call->phone_number_cnt == 8)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3],
                 call->phone_number[4], call->phone_number[5], call->phone_number[6], call->phone_number[7]);
    }
    else if(call->phone_number_cnt == 9)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3],
                 call->phone_number[4], call->phone_number[5], call->phone_number[6], call->phone_number[7], call->phone_number[8]);
    }
    else if(call->phone_number_cnt == 10)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3],
                 call->phone_number[4], call->phone_number[5], call->phone_number[6], call->phone_number[7], call->phone_number[8], call->phone_number[9]);
    }
    else if(call->phone_number_cnt == 11)
    {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d%d%d%d", call->phone_number[0], call->phone_number[1], call->phone_number[2], call->phone_number[3],
                 call->phone_number[4], call->phone_number[5], call->phone_number[6], call->phone_number[7], call->phone_number[8], call->phone_number[9], call->phone_number[10]);
    }
    else
    {
    }
    compo_textbox_set(txt_num, buf);
}

//电话消息处理
static void func_call_sub_dial_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_call_sub_dial_button_click();
            break;


        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//电话显示界面
void func_call_sub_dial(void)
{
    printf("%s\n", __func__);
    func_call_sub_dial_enter();
    while (func_cb.sta == FUNC_CALL_SUB_DIAL)
    {
        func_call_sub_dial_process();
        func_call_sub_dial_message(msg_dequeue());
    }
    func_call_sub_dial_exit();
}
