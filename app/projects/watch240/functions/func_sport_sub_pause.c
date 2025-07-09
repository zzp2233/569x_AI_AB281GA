#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_module_heart.h"
#include "func_cover.h"

#if UTE_MODULE_SCREENS_SPORT_PAUSE_SUPPORT

#define TRACE_EN        0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
enum
{
    COMPO_ID_BTN_SPORT_STOP,         //暂停
    COMPO_ID_BTN_SPORT_EXIT,         //退出

    COMPO_ID_TXT_BLE_OFF,               //蓝牙断开

    COMPO_ID_SPORT_PAUSE_TIME,            //运动时间/断链倒计时
    COMPO_ID_PIC_SPORT_PAUSE_TITLE,         //运动标题图片

};


typedef struct f_sport_sub_pause_t_
{
    u16         count_time;     //app发起运动断链后倒计时
    bool        ble_line_back_flag;//蓝牙回链状态,true:标识刚刚断开后连接上了
    bool        ble_line_flag;  //蓝牙连接状态
} f_sport_sub_pause_t;

static bool sport_start_flag = false;
extern u32 func_sport_get_disp_mode(void);//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_pause_form_create(void)
{
    char txt_buf[70];
    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t* pic = compo_picturebox_create(frm,func_sport_get_ui(sys_cb.sport_idx));///运动类型图片
    compo_picturebox_set_size(pic,70,70);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,70/2+20);
    compo_setid(pic,COMPO_ID_PIC_SPORT_PAUSE_TITLE);

    compo_textbox_t* txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 58/2+126, 240, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(u16)(data->totalSportTime / 3600),(u16)(((data->totalSportTime) % 3600) / 60),(u16)((data->totalSportTime) % 60));
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_SPORT_PAUSE_TIME);

    compo_button_t * btn = compo_button_create_by_image(frm,UI_BUF_I340001_SPORT_BTN_PLAY_BIN);
    compo_setid(btn,COMPO_ID_BTN_SPORT_STOP);
    compo_button_set_pos(btn,90/2+62,90/2+234);

    btn =compo_button_create_by_image(frm,UI_BUF_I340001_SPORT_BTN_CLOSE_BIN);
    compo_setid(btn,COMPO_ID_BTN_SPORT_EXIT);
    compo_button_set_pos(btn,90/2+206,90/2+234);

    txt = compo_textbox_create(frm, strlen(i18n[STR_BLE_OFF])+20);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-60, 200, 30);
    compo_setid(txt,COMPO_ID_TXT_BLE_OFF);
    compo_textbox_set(txt, i18n[STR_BLE_OFF]);
    // compo_textbox_set_autoroll_mode(txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set_visible(txt,false);

    ab_free(data);

    return frm;
}
//按键处理
static void func_sport_sub_pause_click_handler(void)
{
    f_sport_sub_pause_t *f_sport_pause = (f_sport_sub_pause_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SPORT_STOP:
            if(uteModuleSportMoreSportIsAppStart())//app发起的运动
            {
                if(ble_is_connect())//app连接时按键有效
                {
                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    func_cb.sta = FUNC_SPORT_SUB_RUN;
                    func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                }
            }
            else
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                func_cb.sta = FUNC_SPORT_SUB_RUN;
                func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
            }
            break;
        case COMPO_ID_BTN_SPORT_EXIT:
        {
            int res=0;
            bool sport_flag = uteModuleSportMoreSportsIsLessData();
            // sport_flag ^= 1;
            if (sport_flag)
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG2], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_SPORT);
            }
            else
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG1], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_NONE);
            }

            if (res == MSGBOX_RES_OK)
            {
                uteModuleSportStopMoreSports();                             //通知APP退出运动
                if (!sport_flag)
                {
                    func_cb.sta = FUNC_SPORT_FINISH;
                }
                sport_start_flag = false;
                if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
                {
                    task_stack_pop();
                }
            }
        }
        break;
        default:
            break;
    }
}
static void func_sport_sub_pause_updata(void)
{
    f_sport_sub_pause_t *f_sport_pause = (f_sport_sub_pause_t *)func_cb.f_cb;
    compo_textbox_t* txt_time_right = compo_getobj_byid(COMPO_ID_SPORT_PAUSE_TIME);
    compo_textbox_t* txt_ble_off    = compo_getobj_byid(COMPO_ID_TXT_BLE_OFF);

    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);
    char txt_buf[70];

    printf("SportIsAppStart:%d,ble_is_connec:%d,SportGetStatus():%d\n",uteModuleSportMoreSportIsAppStart(),ble_is_connect(),uteModuleSportMoreSportGetStatus());
    if(uteModuleSportMoreSportIsAppStart())//app发起的运动
    {
        if(ble_is_connect())//app连接
        {
            //判断蓝牙是否回链、蓝牙是否刚刚连接，避免回链后一直提示语导致滚动暂停
            if(f_sport_pause->ble_line_back_flag == true && (f_sport_pause->ble_line_flag != ble_is_connect()))
            {
                compo_textbox_set(txt_ble_off, i18n[STR_BLE_SUCCESSFUL]);
                compo_textbox_set_visible(txt_ble_off,true);
            }
            f_sport_pause->ble_line_flag = ble_is_connect();    //记录蓝牙链接状态

            if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_UPDATE_DATA || uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_CONTINUE)
            {
                f_sport_pause->ble_line_back_flag = false;//默认没有回链
                func_cb.sta = FUNC_SPORT_SUB_RUN;
                func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
            }
        }
        else
        {
            f_sport_pause->ble_line_flag = ble_is_connect();    //记录蓝牙链接状态
            f_sport_pause->ble_line_back_flag = true;//当断开时，设置为回链状态，在下次链接时使用
            f_sport_pause->count_time = 5*60-uteModuleSportMoreSportGetDisconnectTimeoutCnt();//开始倒计时5分钟，如5分钟未回链，运动结束
            compo_textbox_set_visible(txt_ble_off,true);
            if(txt_time_right != NULL)
            {
                snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d",f_sport_pause->count_time/60,f_sport_pause->count_time%60);
                compo_textbox_set(txt_time_right, txt_buf);
            }
            if(f_sport_pause->count_time == 0)
            {
                uteModuleSportStopMoreSports();                             //通知APP退出运动
                if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
                {
                    task_stack_pop();
                }
            }
        }
    }
    if(ble_is_connect())
    {
        if(txt_time_right != NULL)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
            compo_textbox_set(txt_time_right, txt_buf);
        }
    }
    ab_free(data);
}

#else


#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//功能事件处理
static void func_sport_sub_pause_process(void)
{

    func_process();
}

//室内跑步功能消息处理
static void func_sport_sub_pause_message(size_msg_t msg)
{
    f_sport_sub_pause_t *f_sport_pause = (f_sport_sub_pause_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_sub_pause_click_handler();
            break;
        case MSG_SYS_500MS:
            func_sport_sub_pause_updata();
            break;
        case KU_BACK:
            if(uteModuleSportMoreSportIsAppStart())//app发起的运动
            {
                if(ble_is_connect() && f_sport_pause->ble_line_back_flag == false)//app连接时按键有效
                {
                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    func_cb.sta = FUNC_SPORT_SUB_RUN;
                    func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                }
            }
            else
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                func_cb.sta = FUNC_SPORT_SUB_RUN;
                func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
            }
            printf("sadklhas 12");
            break;
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG_LEFT:
            if(uteModuleSportMoreSportIsAppStart())//app发起的运动
            {
                if(ble_is_connect() && f_sport_pause->ble_line_back_flag == false)//app连接时触摸有效
                {
                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    func_cb.sta = FUNC_SPORT_SUB_RUN;
                    func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                }
            }
            else
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                func_cb.sta = FUNC_SPORT_SUB_RUN;
                func_switch_to(FUNC_SPORT_SUB_RUN, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
            }
            printf("sadklhas 12");
            break;
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG_RIGHT:
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入室内跑步功能
static void func_sport_sub_pause_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sub_pause_t));

    func_cb.frm_main = func_sport_sub_pause_form_create();
}

//退出室内跑步功能
static void func_sport_sub_pause_exit(void)
{
    printf("%s\n", __func__);
    f_sport_sub_pause_t *f_sport_pause = (f_sport_sub_pause_t*)func_cb.f_cb;

    if(sys_cb.refresh_language_flag == false || sport_start_flag == true)//刷新语言时不清除数据
    {
        uteModuleGuiCommonDisplayOffAllowGoBack(true);
    }
}

//室内跑步功能
void func_sport_sub_pause(void)
{
    printf("%s\n", __func__);
    func_sport_sub_pause_enter();
    while (func_cb.sta == FUNC_SPORT_SUB_PAUSE)
    {
        func_sport_sub_pause_message(msg_dequeue());
        func_sport_sub_pause_process();
    }
    func_sport_sub_pause_exit();
}

#endif //UTE_MODULE_SCREENS_SPORT_PAUSE_SUPPORT
