#include "include.h"
#include "func.h"
#include "ute_drv_motor.h"

#define TRACE_EN            1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_MOTOR_GRADE_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

enum
{
    COMPO_ID_BTN_SURE=1,
    COMPO_ID_TXT_1,
    COMPO_ID_TXT_2,
    COMPO_ID_TXT_3,
    COMPO_ID_TXT_4,
    COMPO_ID_TXT_5,
    COMPO_ID_BTN_MIN_BG,
};
typedef struct f_motor_grade_t_
{
    bool touch_flag;
    uint16_t breathe_mode[5];
    s32 move_dy;
    s32 move_dy_data;
    u8 mode;
    s8 offset;
    s8 offset_old;
} f_motor_grade_t;

#define TXT_SPACING    (105-62)
#define CENTER_TXT_Y   (130/2)

const static uint16_t MOTOR_GRADE_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴
const static uint16_t txt_mode[3] = {STR_MIDDLE, STR_LOW,STR_HIGH_1 };

static u8 func_motor_grade_get_mode(void)
{
    switch (uteDrvMotorGetVibrationLevel())
    {
        case UTE_DRV_MOTOR_PWM_VIBRATION_LOW:
            return 2;
            break;
        case UTE_DRV_MOTOR_PWM_VIBRATION_MEDIUM:
            return 1;
            break;
        case UTE_DRV_MOTOR_PWM_VIBRATION_HIGH:
            return 0;
            break;
        default:
            return 0;
            break;
    }
}
static u8 func_motor_grade_get_mode_motor(u8 grade)
{
    switch (grade)
    {
        case 2:
            return UTE_DRV_MOTOR_PWM_VIBRATION_LOW;
            break;
        case 1:
            return UTE_DRV_MOTOR_PWM_VIBRATION_MEDIUM;
            break;
        case 0:
            return UTE_DRV_MOTOR_PWM_VIBRATION_HIGH;
            break;
        default:
            return UTE_DRV_MOTOR_PWM_VIBRATION_HIGH;
            break;
    }
}

// 函数功能：获取设置时间（上下 & 上上下下 ）的数// 核心函数：偏移后timer值等于timer_data[2]
static void func_motor_grade_get_timer(uint8_t *timer, uint16_t *timer_data, int8_t num)
{
    // 1. 计算中心索引（0-2，支持正负偏移循环）
    int center_idx = (*timer - 1 + num);
    center_idx = (center_idx % 3 + 3) % 3;  // 转为0-2索引

    // 2. 计算左右相邻索引（循环取模）
    int left_idx = (center_idx - 1 + 3) % 3;   // 左侧相邻索引（如中心2→1，中心0→2）
    int right_idx = (center_idx + 1) % 3;       // 右侧相邻索引（如中心1→2，中心2→0）

    // 3. 对称填充5个元素（重点修正此处）
    timer_data[0] = txt_mode[right_idx];  // 右1（第一个元素）
    timer_data[1] = txt_mode[left_idx];   // 左1（第二个元素）
    timer_data[2] = txt_mode[center_idx]; // 中心（第三个元素）
    timer_data[3] = timer_data[0];        // 右2（对称复制右1，第四个元素）
    timer_data[4] = timer_data[1];        // 左2（对称复制左1，第五个元素）

    // 4. 更新timer为中心值（1-3，对应txt_mode索引+1）
    *timer = center_idx + 1;
}
//创建设置窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_motor_grade_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    uint16_t txt_data[5];
    uint8_t mode = func_motor_grade_get_mode();

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VBRATION]);

    func_motor_grade_get_timer(&mode,txt_data,0);///获取模式

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 62+130/2, GUI_SCREEN_WIDTH,130);
    for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
    {
        // printf("idx:%d\n",txt_data[idx-COMPO_ID_TXT_1]);
        compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,50);
        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,MOTOR_GRADE_TXT_Y[idx-COMPO_ID_TXT_1]);;
        compo_textbox_set(txt,i18n[txt_data[idx-COMPO_ID_TXT_1]]);
        compo_setid(txt,idx);
        compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
    }

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮
    compo_button_set_location(btn_min,GUI_SCREEN_CENTER_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I335001_19_BREATHING_TRAINING_2_1_SET_TIME_ICON_YES_208X52_X16_Y222_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I335001_19_BREATHING_TRAINING_2_1_SET_TIME_ICON_YES_208X52_X16_Y222_BIN).hei/2-10);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 105, 220, 1);
    compo_shape_set_color(shape,make_color(47,47,47));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 150, 220, 1);
    compo_shape_set_color(shape,make_color(47,47,47));

    if(func_cb.sta == FUNC_MOTOR_GRADE)
    {
        f_motor_grade_t*f_alarm_clock_sub_set = (f_motor_grade_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->breathe_mode[i]  = txt_data[i];
        }
        f_alarm_clock_sub_set->mode  = mode;
    }

    return frm;
}
/*函数功能：滑动处理*/
static void func_breathe_set_mode_sub_move(void)
{
    f_motor_grade_t *f_disturd_set = (f_motor_grade_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            f_disturd_set->offset = f_disturd_set->move_dy_data/TXT_SPACING;

            if(f_disturd_set->offset != f_disturd_set->offset_old )
            {
                if(f_disturd_set->offset != 0)
                {
                    func_motor_grade_get_timer(&f_disturd_set->mode,f_disturd_set->breathe_mode,-f_disturd_set->offset + f_disturd_set->offset_old);///获取时间
                }
                f_disturd_set->offset_old = f_disturd_set->offset;
                uteDrvMotorSetTempVibrationLevel(func_motor_grade_get_mode_motor(f_disturd_set->mode));
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
            }

            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,MOTOR_GRADE_TXT_Y[idx-COMPO_ID_TXT_1]+f_disturd_set->move_dy);
                compo_textbox_set(txt,i18n[f_disturd_set->breathe_mode[idx-COMPO_ID_TXT_1]]);
                compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
            }
        }
        else   //松手状态
        {
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,MOTOR_GRADE_TXT_Y[idx-COMPO_ID_TXT_1]);
            }
        }
    }
}
//单击按钮
static void func_motor_grade_button_click(void)
{
    f_motor_grade_t *f_disturd_set = (f_motor_grade_t*) func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
            uteDrvMotorSetVibrationLevel(func_motor_grade_get_mode_motor(f_disturd_set->mode));
            func_backing_to();
            break;
        default:
            break;
    }
}
static void func_motor_grade_button_touch_handle(void)
{
    f_motor_grade_t *f_motor_grade = (f_motor_grade_t*) func_cb.f_cb;
    if(f_motor_grade->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_MIN_BG:
                f_motor_grade->touch_flag = true;
                break;
        }
    }
}
#else
typedef struct f_motor_grade_t_
{

} f_motor_grade_t;
compo_form_t *func_motor_grade_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    return;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//设置功能事件处理
static void func_motor_grade_process(void)
{
#if (GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT)
    func_breathe_set_mode_sub_move();
#endif
    func_process();
}

//设置功能消息处理
static void func_motor_grade_message(size_msg_t msg)
{
#if (GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT)
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_motor_grade_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_motor_grade_button_click();
            break;
        default:
            func_message(msg);
            break;
    }
#else
    switch (msg)
    {
        default:
            func_message(msg);
            break;
    }
#endif
}

//进入设置功能
static void func_motor_grade_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_motor_grade_t));
    func_cb.frm_main = func_motor_grade_form_create();
}

//退出设置功能
static void func_motor_grade_exit(void)
{
    printf("%s\n",__func__);
    func_cb.last = FUNC_MOTOR_GRADE;
}

//呼吸设置功能
void func_motor_grade(void)
{
    printf("%s\n", __func__);
    func_motor_grade_enter();
    while (func_cb.sta == FUNC_MOTOR_GRADE)
    {
        func_motor_grade_process();
        func_motor_grade_message(msg_dequeue());
    }
    func_motor_grade_exit();
}

#endif








