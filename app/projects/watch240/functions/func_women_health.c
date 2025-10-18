#include "include.h"
#include "func.h"
#include "ute_module_menstrualcycle.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
static int total_month = 0;
typedef struct f_women_health_t_
{
    s16 pic_x;
    s16 pic_y;
    s16 txt_x;
    s16 txt_y;
    s16 txt_w;
    s16 txt_h;
    u16 str_id;
    u32 res_addr;
    u8  state;
    bool open_flag;
    //u32 tick;
} f_women_health_t;

enum
{
    NO_DATA=0,      //请前往APP同步数据
    MENSTRUAL_CYCLE,//月经期
    PREGNANCY,      //易孕期
    SAFE_PERIOD,    //安全期
};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=48/2+96,  .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+175, .txt_w=224, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I330001_PERIOD_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=110/2+75, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+218, .txt_w=224, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I330001_PERIOD_PERIOD_MP_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=110/2+75, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+218, .txt_w=224, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I330001_PERIOD_PERIOD_FP_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=110/2+75, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+218, .txt_w=224, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I330001_PERIOD_SAFE_MP_BIN},
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=92/2+64,  .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+188, .txt_w=224, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I335001_26_WOMEN_1_SYNCHRONOUS_DATA_ICON_DATA_56X92_X92_Y64_BIN},//前往app同步
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+50, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+178, .txt_w=224, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I335001_26_WOMEN_7_ESTIMATED_MENSTRUAL_PERIOD_ICON_96X96_X72_Y50_02_BIN},//月经
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+50, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+178, .txt_w=224, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I335001_26_WOMEN_7_ESTIMATED_MENSTRUAL_PERIOD_ICON_96X96_X72_Y50_01_BIN},//易孕
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+50, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+178, .txt_w=224, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I335001_26_WOMEN_7_ESTIMATED_MENSTRUAL_PERIOD_ICON_96X96_X72_Y50_00_BIN},//安全
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define WOMEN_HEALTH_X_START_GAP            31                                                                                      //x方向边界间隙
#define WOMEN_HEALTH_X_GAP                  16                                                                                       //间隔间隙
#define WOMEN_HEALTH_PIC_WIDTH              30 //    ((GUI_SCREEN_WIDTH - 6*WOMEN_HEALTH_X_GAP - 2*WOMEN_HEALTH_X_START_GAP) / 7)        //图片宽度

#define WOMEN_HEALTH_Y_START_GAP            32         //y方向上边界间隙
#define WOMEN_HEALTH_Y_GAP                  10         //间隔间隙
#define WOMEN_HEALTH_PIC_HEIGHT             30  //WOMEN_HEALTH_PIC_WIDTH          //图片高度

#define WOMEN_HEALTH_PIC_MAX                35

//judge leap year
#define IS_LEAP_YEAR(year)                          (!((year) % 400) || (((year) % 100) && !((year) % 4)))

//基姆拉尔森计算公式，求某天的星期(当月份为1月或2月时，当作上一年的13月和14月)
#define CAL_DAY_OF_WEEK(year, month, day)     (uint16_t)(((day) + 1 + 2 * (month) + 3 * (month + 1) / 5 + \
                                                                (year) + (year) / 4 - (year) / 100 + (year) / 400) % 7)

static uint16_t current_checked_btn_id = 0;     //选中的按钮id

#define UI_NO_UPDATE_RES                UI_BUF_I341001_26_WOMEN_HEALTH_ROUND_00_BIN          //无信息UI资源
#define UI_MENSTRUAL_PERIOD_RES         UI_BUF_I341001_26_WOMEN_HEALTH_ROUND_01_BIN          //月经期UI资源
#define UI_FERTILE_PERIOD_RES           UI_BUF_I341001_26_WOMEN_HEALTH_ROUND_02_BIN          //易孕期UI资源
#define UI_SELECT_BTN_RES               UI_BUF_I341001_26_WOMEN_HEALTH_ROUND_SELECT_BIN      // 定义选择按钮的资源标识符
#define UI_LAST_BTN_RES                 UI_BUF_I341001_26_WOMEN_HEALTH_VECTOR_1_BIN          // 定义最后一个按钮的资源标识符
#define UI_NEXT_BTN_RES                 UI_BUF_I341001_26_WOMEN_HEALTH_VECTOR_BIN            // 定义下一个按钮的资源标识符

const char week_txt[7]= {STR_SUNDAY, STR_MONDAY,STR_TUESDAY,STR_WEDNESDAY,STR_THURSDAY,STR_FRIDAY,STR_SATURDAY,};

enum
{
    // 定义组件标识符的枚举列表
    // 用于标识不同的组件或界面元素
    COMPO_ID_MON_TEXT = 1, // 月文本组件标识符
    COMPO_ID_NO_DATA_TEXT, // 无数据文本组件标识符
    COMPO_ID_SYNC_DATA_TEXT, // 同步数据文本组件标识符
    COMPO_ID_PERIOD_TEXT, // 周期文本组件标识符
    COMPO_ID_PERIOD_DATE_TEXT, // 周期日期文本组件标识符
    COMPO_ID_PERIOD_DATE_NEXT_TEXT, // 下一个周期日期文本组件标识符

    COMPO_ID_BTN_DATE, // 日期按钮组件标识符
    COMPO_ID_BTN_DATE_END = 44, // 日期结束按钮组件标识符

    COMPO_ID_BTN_LAST, // 上一个按钮组件标识符
    COMPO_ID_BTN_NEXT, // 下一个按钮组件标识符

    COMPO_ID_PIC_SELECT, // 图片选择组件标识符

};

static point_t func_women_health_get_btn_pos(uint16_t id)
{
    int display_year = (total_month - 1) / 12;
    int display_month = (total_month - 1) % 12 + 1;
    uint8_t week_day = uteModuleMenstrualCycleWeekDay(display_year, display_month);
    week_day = (week_day == 7) ? 0 : week_day;

    point_t pos = {0,0};
    if (id >= COMPO_ID_BTN_DATE && id <= COMPO_ID_BTN_DATE + uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month))
    {
    }
    else
    {
        return pos;
    }

    u8 left = WOMEN_HEALTH_X_START_GAP + WOMEN_HEALTH_PIC_WIDTH / 2;                   //开始的x位置
    uint8_t mon_day_count = uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month);                               //get current month day count
    printf("get id = %d,week_day %d\n",id,week_day);

    u8 index = id - COMPO_ID_BTN_DATE + week_day;
    pos.y = WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT / 2 + (index / 7) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP);
    pos.x = left + (WOMEN_HEALTH_PIC_WIDTH + WOMEN_HEALTH_X_GAP) *(index % 7);
    printf("%s->mon_day_count[%d,%d]=>[%d], btn_xy[%d,%d]\n", __func__, display_year, display_month, mon_day_count, pos.x, pos.y);
    return pos;
}

void func_women_health_update(void)
{
    if (uteModuleMenstrualCycleIsOpen())
    {
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        ute_menstrual_cycle_param_t periodParam;
        uteModuleMenstrualCycleGetParam(&periodParam);
        //test start
        int display_year = (total_month - 1) / 12;
        int display_month = (total_month - 1) % 12 + 1;
        int year_high = periodParam.lastTimestamp[0] << 8;
        int menstual_year = year_high + periodParam.lastTimestamp[1];
        uint16_t mon_day_count = uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month);
        uint8_t week_day = uteModuleMenstrualCycleWeekDay(display_year, display_month);
        int day = current_checked_btn_id + 1 - COMPO_ID_BTN_DATE;
        int diffDays = uteModuleMenstrualCycleGetDiffDay(menstual_year, periodParam.lastTimestamp[2], periodParam.lastTimestamp[3],display_year,display_month,day);
        if (diffDays >= - week_day - 1)
        {
            diffDays = diffDays % periodParam.cycleDays + 1;
        }
        else
        {
            diffDays = (periodParam.cycleDays - (abs(diffDays) % periodParam.cycleDays)) % periodParam.cycleDays + 1;
        }
        uint16_t mon_day_count_btn_id = mon_day_count - 1 + COMPO_ID_BTN_DATE;

        compo_textbox_t* period_txt = compo_getobj_byid(COMPO_ID_PERIOD_TEXT);          //易孕期、月经期文本
        compo_textbox_t* date_txt = compo_getobj_byid(COMPO_ID_PERIOD_DATE_TEXT);       //第X天 文本
        compo_textbox_t* date_next_txt = compo_getobj_byid(COMPO_ID_PERIOD_DATE_NEXT_TEXT); //距离XX期X天 文本

        compo_textbox_t* no_data_text = compo_getobj_byid(COMPO_ID_NO_DATA_TEXT);
        compo_textbox_t* sync_data_text = compo_getobj_byid(COMPO_ID_SYNC_DATA_TEXT);
        if (func_cb.sta != FUNC_WOMEN_HEALTH)
        {
            //printf("[error] func_cb.sta != FUNC_WOMEN_HEALTH\n");
            return;
        }

        if (current_checked_btn_id >= COMPO_ID_BTN_DATE && current_checked_btn_id < COMPO_ID_BTN_DATE + mon_day_count)
        {
            ;
        }
        else
        {
            //printf("[error] current_checked_btn_id err:%d",current_checked_btn_id);
            return;
        }

        //文本描述更新
        if (periodParam.keepDays == 0 || periodParam.lastTimestamp[3] == 0 || !uteModuleMenstrualCycleIsOpen())
        {
            compo_textbox_set_visible(no_data_text, true);
            compo_textbox_set_visible(sync_data_text, true);
        }
        else
        {
            for(u8 i = 0; i < mon_day_count; i++)
            {
                u8 state = uteModuleMenstrualCycleGetStatusByTime(display_year,display_month,i+1);
                compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_DATE + i);
                //         1,月经期; 2,排卵期; 3,排卵日; 4,安全期1; 5,安全期2;
                switch(state)
                {
                    case 1:
                        compo_button_set_bgimg(btn, UI_MENSTRUAL_PERIOD_RES);
                        break;
                    case 2:
                    case 3:
                        compo_button_set_bgimg(btn, UI_FERTILE_PERIOD_RES);
                        break;
                    case 4:
                    case 5:
                        break;
                }
            }

            char tmp_buf[128] = {0};
            char tmp_buf_number[10] = {0};
            compo_textbox_set_forecolor(period_txt, COLOR_WHITE);
            compo_textbox_set_forecolor(date_txt, COLOR_WHITE);
            compo_textbox_set_forecolor(date_next_txt, COLOR_WHITE);

            if((diffDays > periodParam.keepDays) && (diffDays <= (periodParam.cycleDays - 19)))
            {
                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",(periodParam.cycleDays - 19) - diffDays + 1);
                uteModuleCharencodeReplaceSubString( i18n[STR_DAYS_TO_FERTILE_WINDOWS],tmp_buf,"##",tmp_buf_number);
                compo_textbox_set(date_txt, tmp_buf);
                compo_textbox_set_forecolor(date_txt, make_color(145,108,252));
                compo_textbox_set_visible(date_txt, true);
                compo_textbox_set(period_txt, " ");
                compo_textbox_set_visible(period_txt, false);
                compo_textbox_set(date_next_txt, " ");
                compo_textbox_set_visible(date_next_txt, false);
            }
            else if (diffDays <= periodParam.keepDays && diffDays> 0)      //当选择按钮子在 月经期区域时
            {
                compo_textbox_set(period_txt, i18n[STR_MENSTRUAL_CYCLE]);
                compo_textbox_set_forecolor(period_txt, make_color(253,94,181));
                compo_textbox_set_visible(period_txt, true);

                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",diffDays);
                uteModuleCharencodeReplaceSubString( i18n[STR_DAY],tmp_buf,"##",tmp_buf_number);
                compo_textbox_set(date_txt, tmp_buf);
                compo_textbox_set_visible(date_txt, true);

                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",(periodParam.cycleDays - 19) - diffDays + 1);
                uteModuleCharencodeReplaceSubString( i18n[STR_DAYS_TO_FERTILE_WINDOWS],tmp_buf,"##",tmp_buf_number);
                compo_textbox_set(date_next_txt, tmp_buf);
                compo_textbox_set_visible(date_next_txt, true);
                compo_textbox_set_location(date_next_txt, GUI_SCREEN_CENTER_X,
                                           40*4 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                                           318, 48);

            }
            else if ((diffDays > (periodParam.cycleDays - 19)) && (diffDays <= (periodParam.cycleDays - 9)))
            {
                compo_textbox_set(period_txt, i18n[STR_PREGNANCY]);
                compo_textbox_set_forecolor(period_txt, make_color(145,108,252));
                compo_textbox_set_visible(period_txt, true);

                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",diffDays - (periodParam.cycleDays - 19));
                uteModuleCharencodeReplaceSubString( i18n[STR_DAY],tmp_buf,"##",tmp_buf_number);

                compo_textbox_set(date_txt, tmp_buf);
                compo_textbox_set_visible(date_txt, true);

                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",periodParam.cycleDays - (diffDays) + 1);
                uteModuleCharencodeReplaceSubString( i18n[STR_DAYS_TO_NEXT_PERIOD],tmp_buf,"##",tmp_buf_number);

                compo_textbox_set(date_next_txt, tmp_buf);
                compo_textbox_set_visible(date_next_txt, true);
                compo_textbox_set_location(date_next_txt, GUI_SCREEN_CENTER_X,
                                           40*4 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                                           318, 48);

            }
            else if(diffDays > (periodParam.cycleDays - 9) && (diffDays <= periodParam.cycleDays))
            {
                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",periodParam.cycleDays - (diffDays) + 1);
                uteModuleCharencodeReplaceSubString( i18n[STR_DAYS_TO_NEXT_PERIOD],tmp_buf,"##",tmp_buf_number);

                compo_textbox_set_visible(date_next_txt, true);
                compo_textbox_set_forecolor(date_next_txt, make_color(255, 87, 165));
                compo_textbox_set_location(date_next_txt, GUI_SCREEN_CENTER_X,
                                           40*3 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                                           328, 48);
                compo_textbox_set(date_next_txt, tmp_buf);
                compo_textbox_set_visible(date_txt, false);
                compo_textbox_set_visible(period_txt, false);
            }
            else
            {
                memset(tmp_buf, 0, sizeof(tmp_buf));
                memset(tmp_buf_number,0,sizeof(tmp_buf_number));
                snprintf(tmp_buf_number,sizeof(tmp_buf_number),"%d",1 - diffDays);
                uteModuleCharencodeReplaceSubString( i18n[STR_DAYS_TO_PERIOD],tmp_buf,"##",tmp_buf_number);
                compo_textbox_set(date_txt, tmp_buf);
                compo_textbox_set_forecolor(date_txt, make_color(253,94,181));
                compo_textbox_set_visible(date_txt, true);
                compo_textbox_set(period_txt, " ");
                compo_textbox_set_visible(period_txt, false);
                compo_textbox_set(date_next_txt, " ");
                compo_textbox_set_visible(date_next_txt, false);
            }
        }

        //日期文本更新
        if (day > 0 && day <= 31)
        {
            ;
        }
        else
        {
            printf("day error\n");
            day = 1;
        }
        char date[128];
        uint16_t current_day_week = CAL_DAY_OF_WEEK(display_year, display_month, day);
        snprintf(date, sizeof(date), "  %02d/%02d  %s  ",display_month, day, i18n[week_txt[current_day_week]]);
        compo_textbox_t* mon_text = compo_getobj_byid(COMPO_ID_MON_TEXT);
        compo_textbox_set(mon_text, date);
    }
    else
    {
        compo_textbox_t *no_data_text = compo_getobj_byid(COMPO_ID_NO_DATA_TEXT);
        compo_textbox_t *sync_data_text = compo_getobj_byid(COMPO_ID_SYNC_DATA_TEXT);
        compo_picturebox_t* select = compo_getobj_byid(COMPO_ID_PIC_SELECT);

        compo_textbox_set_visible(no_data_text, true);
        compo_textbox_set_visible(sync_data_text, true);
        compo_picturebox_set_visible(select, false);
    }
}

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    // create base form
    compo_form_t* frm = compo_form_create(true);
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE);
    // compo_form_set_title(frm, "women health");

    int16_t x_pos = WOMEN_HEALTH_X_START_GAP + WOMEN_HEALTH_PIC_WIDTH / 2;                  //开始的x位置
    int16_t y_pos = WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT / 2;                 //开始的y位置

    int display_year = (total_month - 1) / 12;
    int display_month = (total_month - 1) % 12 + 1;
    uint8_t mon_day_count = uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month);                               //get current month day count
    uint8_t week_day = uteModuleMenstrualCycleWeekDay(display_year, display_month);
    week_day = (week_day == 7) ? 0 : week_day;
    printf("%s->[%d,%d] -> mon_day_count[%d]", __func__, display_year, display_month, mon_day_count);
    for (int i = week_day; i < mon_day_count + week_day; i++)
    {
        u8 left = WOMEN_HEALTH_X_START_GAP + WOMEN_HEALTH_PIC_WIDTH / 2;
        if (!(i%7))             //计算y轴坐标
        {
            y_pos = WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT / 2 + (i / 7) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP);
        }
        x_pos = left + (WOMEN_HEALTH_PIC_WIDTH + WOMEN_HEALTH_X_GAP) * (i % 7);
        compo_button_t* btn = compo_button_create_by_image(frm, UI_NO_UPDATE_RES);        // create women health content
        compo_button_set_location(btn, x_pos, y_pos, WOMEN_HEALTH_PIC_WIDTH, WOMEN_HEALTH_PIC_HEIGHT);
        compo_setid(btn, COMPO_ID_BTN_DATE + i - week_day);
    }

    uint16_t year = display_year;
    uint8_t month = display_month;
    uint8_t day = compo_cb.tm.day;

    current_checked_btn_id = day - 1 + COMPO_ID_BTN_DATE;
    if (current_checked_btn_id >= COMPO_ID_BTN_DATE && current_checked_btn_id < COMPO_ID_BTN_DATE + uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month))
    {
        ;
    }
    else
    {
        current_checked_btn_id = COMPO_ID_BTN_DATE;
    }

    compo_picturebox_t* select = compo_picturebox_create(frm, UI_SELECT_BTN_RES);
    compo_picturebox_set_pos(select, func_women_health_get_btn_pos(current_checked_btn_id).x, func_women_health_get_btn_pos(current_checked_btn_id).y);
    compo_picturebox_set_visible(select, true);
    compo_setid(select, COMPO_ID_PIC_SELECT);


    compo_textbox_t* text;
    //日期
    char date[128];
    // year = 2025; month = 7; day = 7;        //test
    uint16_t current_day_week = CAL_DAY_OF_WEEK(year, month, day);
    snprintf(date, sizeof(date), "  %02d/%02d  %s  ", month, day, i18n[week_txt[current_day_week]]);
    text = compo_textbox_create(frm, 20);
    compo_textbox_set_pos(text, GUI_SCREEN_CENTER_X,
                          widget_text_get_height() + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP));
    compo_textbox_set(text, date);
    compo_setid(text, COMPO_ID_MON_TEXT);

    //切换按钮
    uint16_t month_txt_wid = widget_text_get_area(text->txt).wid;
    compo_picturebox_t* last_pic = compo_picturebox_create(frm, UI_LAST_BTN_RES);
    compo_picturebox_set_pos(last_pic, GUI_SCREEN_CENTER_X - month_txt_wid/2,
                             widget_text_get_height() + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP));

    compo_picturebox_t* next_pic = compo_picturebox_create(frm, UI_NEXT_BTN_RES);
    compo_picturebox_set_pos(next_pic, GUI_SCREEN_CENTER_X + month_txt_wid/2,
                             widget_text_get_height() + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP));

    // compo_button_t* last_btn = compo_button_create_by_image(frm, UI_BUF_UI2CUM_UTE_24_WOMEN_HEALTH_VECTOR_1_BIN);    //test
    compo_button_t* last_btn = compo_button_create(frm);
    compo_setid(last_btn, COMPO_ID_BTN_LAST);
    compo_button_set_location(last_btn, GUI_SCREEN_CENTER_X - month_txt_wid/2,
                              widget_text_get_height() + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                              month_txt_wid/4, widget_text_get_height()*2);

    // compo_button_t* nex_btn = compo_button_create_by_image(frm, UI_BUF_UI2CUM_UTE_24_WOMEN_HEALTH_VECTOR_BIN);       //test
    compo_button_t* nex_btn = compo_button_create(frm);
    compo_setid(nex_btn, COMPO_ID_BTN_NEXT);
    compo_button_set_location(nex_btn, GUI_SCREEN_CENTER_X + month_txt_wid/2,
                              widget_text_get_height() + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                              month_txt_wid/4, widget_text_get_height()*2);

    //没有更新文本
    text = compo_textbox_create(frm, strlen(i18n[STR_LONG_TIME_NO_UP]));
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,
                               widget_text_get_height()*2 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                               GUI_SCREEN_WIDTH-30, 48);
    compo_textbox_set(text, i18n[STR_LONG_TIME_NO_UP]);
    compo_textbox_set_visible(text, false);
    compo_setid(text, COMPO_ID_NO_DATA_TEXT);

    //同步数据文本
    text = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_APP_DATA]));
    compo_textbox_set_multiline(text, true);
    widget_text_set_ellipsis(text->txt, false);
    compo_textbox_set_align_center_top(text, true);
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,
                               30 + widget_text_get_height()*3/2 + widget_text_get_height()*2 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                               GUI_SCREEN_WIDTH - 30, 130);
    compo_textbox_set(text, i18n[STR_PLEASE_APP_DATA]);
    compo_textbox_set_visible(text, false);
    compo_setid(text, COMPO_ID_SYNC_DATA_TEXT);

    //月经期/易孕期 文本
    text = compo_textbox_create(frm, 100);
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,
                               40*2 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                               GUI_SCREEN_WIDTH-30, 48);
    compo_textbox_set_visible(text, false);
    // compo_textbox_set(text, "易孕期");      //test
    compo_setid(text, COMPO_ID_PERIOD_TEXT);


    //第x天 文本
    text = compo_textbox_create(frm, 20);
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,
                               40*3 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                               GUI_SCREEN_WIDTH-30, 48);
    // compo_textbox_set(text, "第x天");           //test
    compo_textbox_set_visible(text, false);
    compo_setid(text, COMPO_ID_PERIOD_DATE_TEXT);

    //距离XX期X天
    text = compo_textbox_create(frm, 100);
    compo_textbox_set_location(text, GUI_SCREEN_CENTER_X,
                               40*4 + WOMEN_HEALTH_Y_START_GAP + WOMEN_HEALTH_PIC_HEIGHT + ((mon_day_count-1) / 7 + 1) * (WOMEN_HEALTH_PIC_HEIGHT + WOMEN_HEALTH_Y_GAP),
                               318, 48);
    // compo_textbox_set(text, "距离xx期x天");     //test
    compo_textbox_set_visible(text, false);
    widget_text_set_ellipsis(text->txt, false);
    compo_setid(text, COMPO_ID_PERIOD_DATE_NEXT_TEXT);


    if (func_cb.sta == FUNC_WOMEN_HEALTH)
    {
        func_women_health_update();
    }

    return frm;
}
static void func_women_health_button_click(void)
{
    // printf("%s\n", __func__);
    // f_women_health_t* f_women_health = (f_women_health_t*)func_cb.f_cb;
    int display_year = total_month / 12;
    int display_month = total_month % 13;

    if (func_cb.sta != FUNC_WOMEN_HEALTH)
    {
        return;
    }

    int btn_id = compo_get_button_id();
    compo_picturebox_t* select = compo_getobj_byid(COMPO_ID_PIC_SELECT);
    if (btn_id >= COMPO_ID_BTN_DATE && btn_id < COMPO_ID_BTN_DATE + uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month))
    {
        printf("btn_id [%d,%d]=>[%d]\n", COMPO_ID_BTN_DATE, COMPO_ID_BTN_DATE - 1 + uteModuleMenstrualCycleJudgeMonthDay(display_year, display_month), btn_id);
        compo_picturebox_set_pos(select, func_women_health_get_btn_pos(btn_id).x, func_women_health_get_btn_pos(btn_id).y);
        current_checked_btn_id = btn_id;
    }
    else
    {
        switch (btn_id)
        {
            case COMPO_ID_BTN_NEXT:
                total_month++;
                compo_form_destroy(func_cb.frm_main);
                func_cb.frm_main = func_women_health_form_create();
                return;

            case COMPO_ID_BTN_LAST:
                total_month--;
                compo_form_destroy(func_cb.frm_main);
                func_cb.frm_main = func_women_health_form_create();
                return;
            default:
                break;
        }
    }

    func_women_health_update();
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
static const f_women_health_t f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=109+142/2, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+292, .txt_w=308, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I341001_23_SOS_PHONE_BIN},//前往app同步
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=112/2+118, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+317, .txt_w=308, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I341001_26_WOMEN_HEALTH_00_BIN},//月经
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=112/2+118, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+317, .txt_w=308, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I341001_26_WOMEN_HEALTH_02_BIN},//易孕
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=112/2+118, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+317, .txt_w=308, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I341001_26_WOMEN_HEALTH_01_BIN},//安全
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=91+123/2, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+262, .txt_w=308, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I343001_23_SOS_PHONE_BIN},//前往app同步
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=136/2+79, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+272, .txt_w=308, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I343001_26_WOMEN_HEALTH_00_BIN},//月经
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=136/2+79, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+272, .txt_w=308, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I343001_26_WOMEN_HEALTH_02_BIN},//易孕
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=136/2+79, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+272, .txt_w=308, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I343001_26_WOMEN_HEALTH_01_BIN},//安全
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=92/2+86,   .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+212, .txt_w=288, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I332001_PERIOD_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I332001_PERIOD_PERIOD_MP_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I332001_PERIOD_PERIOD_FP_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I332001_PERIOD_SAFE_MP_BIN},
};

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }

    if(state)
    {
        //设置标题栏
        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);
    }

    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=288, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_00_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_01_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_02_BIN},
};

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }


    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    u16 color = COLOR_WHITE;

    switch (state)
    {
        case MENSTRUAL_CYCLE:
            color = make_color(255,87,165);
            break;
        case PREGNANCY:
            color = make_color(161,72,255);
            break;
        case SAFE_PERIOD:
            color = make_color(72,144,255);
            break;
        default:
            break;
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);
    compo_textbox_set_forecolor(textbox,color);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=92/2+86,   .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+212, .txt_w=288, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I340001_PERIOD_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I340001_PERIOD_PERIOD_MP_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I340001_PERIOD_PERIOD_FP_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I340001_PERIOD_SAFE_MP_BIN},
};

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }

    if(state)
    {
        //设置标题栏
        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);
    }

    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+53,  .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+159, .txt_w=224, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I342001_26_WOMEN_HEALTH_PHONE_BIN},//前往app同步
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=82/2+53, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+159, .txt_w=224, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I342001_26_WOMEN_HEALTH_00_BIN},//月经
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=82/2+53, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+159, .txt_w=224, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I342001_26_WOMEN_HEALTH_02_BIN},//易孕
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=82/2+53, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+159, .txt_w=224, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I342001_26_WOMEN_HEALTH_01_BIN},//安全
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#else
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

#endif

//女性健康功能事件处理
static void func_women_health_process(void)
{
    f_women_health_t *f_women_health = (f_women_health_t*)func_cb.f_cb;
    static u8 cycle_temp[7] = {0};
    //if (tick_check_expire(f_women_health->tick, 100))
    {
        //f_women_health->tick = tick_get();
        ute_menstrual_cycle_param_t periodParam;
        uteModuleMenstrualCycleGetParam(&periodParam);
        u8 crc = memcmp(&periodParam,cycle_temp,sizeof(cycle_temp));
        if(crc)
        {
            memcpy(cycle_temp,&periodParam,sizeof(cycle_temp));
            compo_form_destroy(func_cb.frm_main);
            func_cb.frm_main = func_women_health_form_create();
            return;
        }
    }
    func_process();
}
//女性健康功能消息处理
static void func_women_health_message(size_msg_t msg)
{
    f_women_health_t *f_women_health = (f_women_health_t*)func_cb.f_cb;
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_women_health_button_click();
            break;

        case MSG_QDEC_FORWARD:
            // func_calendar_date_update(true);
            break;

        case MSG_QDEC_BACKWARD:
            // func_calendar_date_update(false);
            break;

        case MSG_SYS_500MS:
            break;

        default:
            func_message(msg);
            break;
    }

#else
    switch (msg)
    {
        case MSG_SYS_1S:
        {
            u8 state=NO_DATA;
            uteModuleMenstrualCycleGetStatus(&state);
            if(state!= f_women_health->state || f_women_health->open_flag !=uteModuleMenstrualCycleIsOpen())
            {
                f_women_health->state = state;
                f_women_health->open_flag = uteModuleMenstrualCycleIsOpen();
                msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            }
        }
        break;
        case MSG_CTP_CLICK:
            break;
        default:
            func_message(msg);
            break;
    }
#endif
}

//进入女性健康功能
static void func_women_health_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_women_health_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    total_month = time.year *12 + time.month;
    func_cb.frm_main = func_women_health_form_create();

//    f_women_health_t *f_women_health = (f_women_health_t*)func_cb.f_cb;
//    uteModuleMenstrualCycleGetStatus(&f_women_health->state);
//    f_women_health->open_flag = uteModuleMenstrualCycleIsOpen();

}

//退出女性健康功能
static void func_women_health_exit(void)
{
    func_cb.last = FUNC_WOMEN_HEALTH;
}

//女性健康功能
void func_women_health(void)
{
    printf("%s\n", __func__);
    func_women_health_enter();
    while (func_cb.sta == FUNC_WOMEN_HEALTH)
    {
        func_women_health_process();
        func_women_health_message(msg_dequeue());
    }
    func_women_health_exit();
}
#endif
