#include "include.h"
#include "func.h"

#define  TRACE_EN 1
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if 1//ECIG_POWER_CONTROL
//组件ID
enum
{
    COMPO_ID_NUM_ECIG_SET_POWER_UU = 1,
    COMPO_ID_NUM_ECIG_SET_POWER_UP,
    COMPO_ID_NUM_ECIG_SET_POWER,
    COMPO_ID_NUM_ECIG_SET_POWER_DOWN,
    COMPO_ID_NUM_ECIG_SET_POWER_DD,

    COMPO_ID_BTN_ECIG_SET_POWER,
    COMPO_ID_BTN_ECIG_SET_S_D,//单双发切换
    COMPO_ID_BTN_ECIG_BG_S_D,
};

typedef struct ecig_set_power_num_item_t_
{
    int num_cnt;
    u16 num_id;
    s16 load_id;
    int val;
    s16 x;
    s16 y;
    bool visible_en;
    u8 alpha;
} ecig_set_power_num_item_t;

typedef struct f_ecig_set_power_t_
{
    ecig_set_power_num_item_t set_num_item[11];
    area_t size;
    u16 num_height;
    s16 pcnt;
} f_ecig_set_power_t;


#define ECIG_SET_POWER_NUM_ITEM_CNT             ((int)(sizeof(tbl_ecig_set_power_num_item) / sizeof(tbl_ecig_set_power_num_item[0])))

#define ECIG_SET_NUM_PIC_CENTER_X              100     //时间框的中心坐标
#define ECIG_SET_NUM_PIC_CENTER_Y              164
#define ECIG_SET_TXT_ITEM_H_X                  100
#define ECIG_SET_TXT_ITEM_Y                    65
#define ECIG_SET_TXT_ITEM_Y_OFFSET             45

#define ECIG_SET_CLOCK_SET_NUM_POS_Y_START                 ECIG_SET_TXT_ITEM_Y
#define ECIG_SET_CLOCK_SET_NUM_POS_Y(y_start,space,cnt)    (y_start)+(space)*(cnt)


//搞个数字item，创建时遍历一下
static const ecig_set_power_num_item_t tbl_ecig_set_power_num_item[] =
{
    /*   num_cnt,              num_id,                           load_num_id           val,   x,                    y,                                             visible_en      alpha*/
    {2,        COMPO_ID_NUM_ECIG_SET_POWER_UU,      COMPO_ID_NUM_ECIG_SET_POWER_UU,    22,   ECIG_SET_TXT_ITEM_H_X,   ECIG_SET_TXT_ITEM_Y,                              true,           100},
    {2,        COMPO_ID_NUM_ECIG_SET_POWER_UP,      COMPO_ID_NUM_ECIG_SET_POWER_UP,    23,   ECIG_SET_TXT_ITEM_H_X,   ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET,      true,           100},
    {2,        COMPO_ID_NUM_ECIG_SET_POWER,         COMPO_ID_NUM_ECIG_SET_POWER,       0,    ECIG_SET_TXT_ITEM_H_X,   ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {2,        COMPO_ID_NUM_ECIG_SET_POWER_DOWN,    COMPO_ID_NUM_ECIG_SET_POWER_DOWN,  1,    ECIG_SET_TXT_ITEM_H_X,   ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {2,        COMPO_ID_NUM_ECIG_SET_POWER_DD,      COMPO_ID_NUM_ECIG_SET_POWER_DD,    2,    ECIG_SET_TXT_ITEM_H_X,   ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET*4,    true,           100},
};


//获取功率 上一个和下一个的数字
static u8 func_ecig_set_power_get_cal(s8 num, bool hour_en, u8 mode)     //mode 0:返回当前，1：返回上一个，2：返回下一个
{
    printf("%s,num=%d,mode=%d\n",__func__,num,mode);
    if (num < 1)
    {
        if (get_gear_func()==0)
        {
            num += 18;
        }
        else
        {
            num += 30;
        }
    }
    if (mode == 1)
    {
        if (get_gear_func()==0)
        {
            if (num <= 10 || num > 18)
            {
                return 18;
            }
        }
        else
        {
            if (num <= 20 || num > 30)
            {
                return 30;
            }
        }
        return num - 1;
    }
    else if (mode == 2)
    {
        if (get_gear_func()==0)
        {
            if (num >= 18)
            {
                return 10;
            }
        }
        else
        {
            if (num >= 30)
            {
                return 20;
            }
        }

        return num + 1;
    }
    else
    {
        if (get_gear_func()==0)
        {
            if (num > 18)
            {
                return 18;
            }
        }
        else
        {
            if (num > 30)
            {
                return 30;
            }
            if (num < 20)
            {
                return 20;
            }
        }

        return num;
    }
}




//创建功率窗体
compo_form_t *func_ecig_set_power_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ECIG_SET_POWER]);

    compo_picturebox_t *pic;
    compo_textbox_t *txt_title;
    // compo_textbox_t *txt_title = compo_textbox_create(frm, 10);
    // compo_textbox_set_align_center(txt_title, false);
    // compo_textbox_set_pos(txt_title, 30, 20);
    // compo_textbox_set(txt_title, i18n[STR_ECIG_SET_POWER]);



    //创建数字
    compo_textbox_t *txt_num;
    u8 power = ecig_get_power();
    char buf[4];
    for (u8 idx = 0; idx < ECIG_SET_POWER_NUM_ITEM_CNT; idx++)
    {
        txt_num = compo_textbox_create(frm, tbl_ecig_set_power_num_item[idx].num_cnt);
        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_48_BIN);
        compo_setid(txt_num, tbl_ecig_set_power_num_item[idx].num_id);
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, tbl_ecig_set_power_num_item[idx].x, tbl_ecig_set_power_num_item[idx].y);
        compo_textbox_set_visible(txt_num, tbl_ecig_set_power_num_item[idx].visible_en);
        compo_textbox_set_alpha(txt_num, tbl_ecig_set_power_num_item[idx].alpha);

        memset(buf, 0, sizeof(buf));
        if (tbl_ecig_set_power_num_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UU)        //上上次的时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(func_ecig_set_power_get_cal(power, 1, 1), 1, 1));
        }
        else if (tbl_ecig_set_power_num_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UP)     //上次的时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(power, 1, 1));
        }
        else if (tbl_ecig_set_power_num_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER)      //当前时间
        {
            snprintf(buf, sizeof(buf), "%02d", power);
        }
        else if (tbl_ecig_set_power_num_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DOWN)     //下次时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(power, 1, 2));
        }
        else if (tbl_ecig_set_power_num_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DD)       //下下次时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(func_ecig_set_power_get_cal(power, 1, 2), 1, 2));
        }
        compo_textbox_set(txt_num, buf);
    }



    compo_button_t * btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_ECIG_SET_POWER);
    compo_button_set_location(btn, 100, ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET*5/2, 100, ECIG_SET_TXT_ITEM_Y_OFFSET*5);

    compo_shape_t *shape_up;
    // shape_up = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    // compo_shape_set_color(shape_up, COLOR_RED);
    // compo_shape_set_location(shape_up, 160, ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET*5/2 - 20, 100, ECIG_SET_TXT_ITEM_Y_OFFSET*5);
    // compo_shape_set_alpha(shape_up, 180);

    shape_up = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(shape_up, COLOR_BLACK);
    compo_shape_set_location(shape_up, 120, ECIG_SET_TXT_ITEM_Y_OFFSET-5, 100, ECIG_SET_TXT_ITEM_Y_OFFSET*2 );
    compo_shape_set_alpha(shape_up, 255);

    shape_up = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(shape_up, COLOR_BLACK);
    compo_shape_set_location(shape_up, 120, ECIG_SET_TXT_ITEM_Y+ECIG_SET_TXT_ITEM_Y_OFFSET*4+ECIG_SET_TXT_ITEM_Y_OFFSET/2 + 6, 100, ECIG_SET_TXT_ITEM_Y_OFFSET*2 + 5);
    compo_shape_set_alpha(shape_up,255);


    compo_button_t * btn1 = compo_button_create_by_image(frm,UI_BUF_I330001_ECIG_KEY_S_BIN);
    compo_setid(btn1, COMPO_ID_BTN_ECIG_SET_S_D);
    compo_button_set_pos(btn1, 120, 60);
    if (get_gear_func()==0)
    {
        compo_button_set_bgimg(btn1,UI_BUF_I330001_ECIG_KEY_S_BIN);
    }
    else
    {
        compo_button_set_bgimg(btn1,UI_BUF_I330001_ECIG_KEY_D_BIN);
    }

    pic = compo_picturebox_create(frm, UI_BUF_I330001_ECIG_W_BIN);
    compo_picturebox_set_pos(pic, 155, 158);

    ///设置图片
    compo_button_t * btn_bg = compo_button_create_by_image(frm,UI_BUF_I330001_ECIG_BG_S_BIN);
    compo_setid(btn_bg, COMPO_ID_BTN_ECIG_BG_S_D);
    compo_button_set_pos(btn_bg, 120,255);
    if (get_gear_func()==0)
        compo_button_set_bgimg(btn_bg,UI_BUF_I330001_ECIG_BG_S_BIN);
    else
        compo_button_set_bgimg(btn_bg,UI_BUF_I330001_ECIG_BG_D_BIN);




    //电阻显示
    // txt_title = compo_textbox_create(frm, 3);
    // compo_textbox_set_font(txt_title, UI_BUF_0FONT_FONT_NUM_24_BIN);
    // compo_textbox_set_align_center(txt_title, false);
    // compo_textbox_set_pos(txt_title, 120, 180);
    // //snprintf(buf, sizeof(buf), "%02d", ecig_get_res());
    // memset(buf,0x00,4);
    // buf[0] = ecig_get_res() / 10 + 0x30;
    // buf[1] = '.';
    // buf[2] = ecig_get_res() % 10 + 0x30;
    // compo_textbox_set(txt_title, buf);
    // pic = compo_picturebox_create(frm, UI_BUF_ECIG_RES_BIN);
    // compo_picturebox_set_pos(pic, 200, 200);



    return frm;
}

//功率功能事件处理
static void func_ecig_set_power_process(void)
{
    func_process();
}
static void ecig_set_power_num_pos_cal(s32 dy, u8 id, bool press)
{
    f_ecig_set_power_t *f_ecig_set_power = (f_ecig_set_power_t*)func_cb.f_cb;

    area_t size = f_ecig_set_power->size;
    u16 num_height = f_ecig_set_power->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    ecig_set_power_num_item_t *time_item;
    u8 ofs = 0;

    if (id == COMPO_ID_BTN_ECIG_SET_POWER)
    {
        ofs = 0;
        time_item = &f_ecig_set_power->set_num_item[0];
    }
    if (press)
    {
        distance = dy - last_dy;
    }
    else
    {
        dy = 0;
        last_dy = 0;
        distance = 0;
    }

    time_item[0].y += distance;
    time_item[1].y += distance;
    time_item[2].y += distance;
    time_item[3].y += distance;
    time_item[4].y += distance;

    for (int i=0; i<5; i++)
    {
        if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UP+ofs)
        {
            //printf("0000>>>>> y %d %d %d \n",time_item[i].y,(ECIG_SET_NUM_PIC_CENTER_Y - size.hei/2),num_height);
            if ((time_item[i].y - (ECIG_SET_NUM_PIC_CENTER_Y - size.hei/2)) <= num_height/2)
            {
                dir = -1;
                //printf("1111>>>>> y %d %d %d \n",time_item[i].y,(ECIG_SET_NUM_PIC_CENTER_Y - size.hei/2),num_height);
                break;
            }
            else if (press == false && ((time_item[i].y - (ECIG_SET_NUM_PIC_CENTER_Y - size.hei/2)) <= num_height/2))
            {
                dir = -1;
                break;
            }
        }
        else if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DOWN+ofs)
        {
            //printf("2222>>>>> y %d %d %d \n",time_item[i].y,(ECIG_SET_NUM_PIC_CENTER_Y + size.hei/2),num_height);
            if (time_item[i].y >= (ECIG_SET_NUM_PIC_CENTER_Y + size.hei/2 - num_height/2))
            {
                dir = 1;
                //printf("3333>>>>> y %d %d %d \n",time_item[i].y,(ECIG_SET_NUM_PIC_CENTER_Y + size.hei/2),num_height/2);
                break;
            }
            else if (press == false && (time_item[i].y >= (ECIG_SET_NUM_PIC_CENTER_Y + size.hei/2 - num_height/2)))
            {
                dir = 1;
                break;
            }
        }
    }

    if (dir)
    {
        for(int i=0; i<5; i++)
        {
            time_item[i].load_id += dir;
            if (time_item[i].load_id < COMPO_ID_NUM_ECIG_SET_POWER_UU+ofs)
            {
                time_item[i].load_id = COMPO_ID_NUM_ECIG_SET_POWER_DD+ofs;
            }
            else if (time_item[i].load_id > COMPO_ID_NUM_ECIG_SET_POWER_DD+ofs)
            {
                time_item[i].load_id = COMPO_ID_NUM_ECIG_SET_POWER_UU+ofs;
            }
        }

        if (id == COMPO_ID_BTN_ECIG_SET_POWER)
        {
            f_ecig_set_power->pcnt += dir;
        }
    }

    if (press == false || dir)
    {
        for(int i=0; i<5; i++)
        {
            if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UU+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ECIG_SET_CLOCK_SET_NUM_POS_Y(ECIG_SET_CLOCK_SET_NUM_POS_Y_START,ECIG_SET_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ECIG_SET_POWER_UU-COMPO_ID_NUM_ECIG_SET_POWER_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DD+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ECIG_SET_CLOCK_SET_NUM_POS_Y(ECIG_SET_CLOCK_SET_NUM_POS_Y_START,ECIG_SET_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ECIG_SET_POWER_DD-COMPO_ID_NUM_ECIG_SET_POWER_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UP+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ECIG_SET_CLOCK_SET_NUM_POS_Y(ECIG_SET_CLOCK_SET_NUM_POS_Y_START,ECIG_SET_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ECIG_SET_POWER_UP-COMPO_ID_NUM_ECIG_SET_POWER_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ECIG_SET_CLOCK_SET_NUM_POS_Y(ECIG_SET_CLOCK_SET_NUM_POS_Y_START,ECIG_SET_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ECIG_SET_POWER-COMPO_ID_NUM_ECIG_SET_POWER_UU);
                time_item[i].alpha = 255;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DOWN+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ECIG_SET_CLOCK_SET_NUM_POS_Y(ECIG_SET_CLOCK_SET_NUM_POS_Y_START,ECIG_SET_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ECIG_SET_POWER_DOWN-COMPO_ID_NUM_ECIG_SET_POWER_UU);
                time_item[i].alpha = 100;
            }
        }
    }

    for (int i=0; i<5; i++)
    {
        compo_textbox_t *txt_num = compo_getobj_byid(time_item[i].num_id);
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, time_item[i].x, time_item[i].y);
        compo_textbox_set_visible(txt_num, time_item[i].visible_en);
        compo_textbox_set_alpha(txt_num, time_item[i].alpha);
    }

    last_dy = dy;

    //printf("===============================\n");
    //for(int i=0; i<5; i++) {
    //    printf("[%d] id:[%d], vis:[%d], xy:[%d,%d], ret:[%d], dir[%d], press:[%d], hcnt:[%d], mcnt[%d]\n", time_item[i].num_id, time_item[i].load_id,
    //            time_item[i].visible_en, time_item[i].x, time_item[i].y, ret, dir, press, _ecig_set_power->hcnt, _ecig_set_power->mcnt);
    //}
    //printf("===============================\n");
}


static void func_ecig_set_power_move_handle(u8 id)
{
    f_ecig_set_power_t *f_ecig_set_power = (f_ecig_set_power_t*)func_cb.f_cb;
    ecig_set_power_num_item_t *time_item;
    bool flag_press = 0;
    u32 tick = tick_get();
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[4];

    if (id == COMPO_ID_BTN_ECIG_SET_POWER)
    {
        time_item = &f_ecig_set_power->set_num_item[0];
    }
    u8 power = ecig_get_power();

    s8 power_disp = power;
    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        // printf("x:%d, y:%d\n",dx, dy);

        ecig_set_power_num_pos_cal(dy, id, flag_press);
        power_disp = ((power - f_ecig_set_power->pcnt));
        printf("power_disp=%d\n",power_disp);
        if (get_gear_func()==0)
        {
            if (power_disp>18)
            {
                power_disp = 10;
            }

            if (power_disp<10)
            {
                power_disp = 18;
            }
        }
        else
        {
            if (power_disp>30)
            {
                power_disp = 20;
            }

            if (power_disp<20)
            {
                power_disp = 30;
            }
        }



        // if (power_disp<8&&power_disp>0)
        // {
        //     power_disp = power_disp+10;
        // }


        if (tick_check_expire(tick, 15))        //15ms刷新一下数字框
        {
            tick = tick_get();
            if (id == COMPO_ID_BTN_ECIG_SET_POWER)
            {
                for (int idx = 0; idx < 5; idx++)
                {
                    memset(buf, 0, sizeof(buf));
                    compo_textbox_t *txt_num = compo_getobj_byid(time_item[idx].num_id);
                    if (time_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UU)        //上上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(func_ecig_set_power_get_cal(power_disp, 1, 1), 1, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_UP)     //上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(power_disp, 1, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER)      //当前时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(power_disp, 1, 0));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DOWN)     //下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(power_disp, 1, 2));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ECIG_SET_POWER_DD)       //下下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_ecig_set_power_get_cal(func_ecig_set_power_get_cal(power_disp, 1, 2), 1, 2));
                    }
                    compo_textbox_set(txt_num, buf);
                }
            }
        }

        if (!flag_press)
        {
            f_ecig_set_power->pcnt = 0;
            power = func_ecig_set_power_get_cal(power_disp, 1, 0);
            if (get_gear_func()==1)
            {
                //power = power/2;
                power = power;

            }
            ecig_set_power(power);

            printf("ecig_get_power() %d \n", ecig_get_power());
            break;
        }

        //evt_message(msg_dequeue());
        func_message(msg_dequeue());
        func_process();
    }

}


//触摸按钮效果处理
static void func_ecig_set_power_button_touch_handle(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ECIG_SET_POWER:
            func_ecig_set_power_move_handle(id);
            break;
        case COMPO_ID_BTN_ECIG_SET_S_D :
            //   u8 power  = ecig_get_power();

            if (get_gear_func()==0)
            {
                test_2st_gear_func();
                compo_button_t *btn1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);
                compo_button_t *btn2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D);
                compo_button_set_bgimg(btn2, UI_BUF_I330001_ECIG_KEY_D_BIN);
                compo_button_set_bgimg(btn1, UI_BUF_I330001_ECIG_BG_D_BIN);



            }
            else
            {

                test_1st_gear_func();
                compo_button_t *btn1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);
                compo_button_t *btn2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D);
                compo_button_set_bgimg(btn2, UI_BUF_I330001_ECIG_KEY_S_BIN);
                compo_button_set_bgimg(btn1, UI_BUF_I330001_ECIG_BG_S_BIN);
            }
            func_ecig_set_power_move_handle(COMPO_ID_BTN_ECIG_SET_POWER);


            break;

        default:
            break;
    }
}


//功率功能消息处理
static void func_ecig_set_power_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_ecig_set_power_button_touch_handle();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入功率功能
static void func_ecig_set_power_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_ecig_set_power_t));
    func_cb.frm_main = func_ecig_set_power_form_create();

    f_ecig_set_power_t *f_ecig_set_power = (f_ecig_set_power_t*) func_cb.f_cb;
    memcpy(f_ecig_set_power->set_num_item, tbl_ecig_set_power_num_item, sizeof(tbl_ecig_set_power_num_item));
    //f_ecig_set_power->size = gui_image_get_size(UI_BUF_ALARM_CLOCK_NUM_BIN);
    f_ecig_set_power->size.wid = 100;
    f_ecig_set_power->size.hei = ECIG_SET_TXT_ITEM_Y_OFFSET*5;
    f_ecig_set_power->num_height = ECIG_SET_TXT_ITEM_Y_OFFSET;
}

//退出功率功能
static void func_ecig_set_power_exit(void)
{
    func_cb.last = FUNC_ECIG_SET_POWER;
}

//功率功能
void func_ecig_set_power(void)
{
    printf("%s\n", __func__);
    func_ecig_set_power_enter();
    while (func_cb.sta == FUNC_ECIG_SET_POWER)
    {
        func_ecig_set_power_process();
        func_ecig_set_power_message(msg_dequeue());
    }
    func_ecig_set_power_exit();
}
#else
//组件ID
enum
{
    COMPO_ID_NUM_ECIG_SET_POWER_UU = 1,
    COMPO_ID_NUM_ECIG_SET_POWER_UP,
    COMPO_ID_NUM_ECIG_SET_POWER,
    COMPO_ID_NUM_ECIG_SET_POWER_DOWN,
    COMPO_ID_NUM_ECIG_SET_POWER_DD,

    COMPO_ID_BTN_ECIG_SET_POWER,
    COMPO_ID_BTN_ECIG_SET_S_D,//单双发切换
    COMPO_ID_BTN_ECIG_BG_S_D,
    COMPO_ID_BTN_ECIG_BG_NUM,
    COMPO_ID_BTN_ECIG_SET_ADD,//单双发切换
    COMPO_ID_BTN_ECIG_SET_DEL,
};



//创建功率窗体
compo_form_t *func_ecig_set_power_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ECIG_SET_POWER]);

    ///设置图片
    compo_button_t * btn_bg = compo_button_create_by_image(frm,UI_BUF_I330001_ECIG_BG_S_BIN);
    compo_setid(btn_bg, COMPO_ID_BTN_ECIG_BG_S_D);
    compo_button_set_pos(btn_bg, 120,255);
    if (get_gear_func()==0)
        compo_button_set_bgimg(btn_bg,UI_BUF_I330001_ECIG_BG_S_BIN);
    else
        compo_button_set_bgimg(btn_bg,UI_BUF_I330001_ECIG_BG_D_BIN);




    compo_button_t * btn1 = compo_button_create_by_image(frm,UI_BUF_I330001_ECIG_KEY_S_BIN);
    compo_setid(btn1, COMPO_ID_BTN_ECIG_SET_S_D);

    compo_button_set_pos(btn1, 120, 60);
    if (get_gear_func()==0)
    {
        compo_button_set_bgimg(btn1,UI_BUF_I330001_ECIG_KEY_S_BIN);
    }
    else
    {
        compo_button_set_bgimg(btn1,UI_BUF_I330001_ECIG_KEY_D_BIN);
    }





    // compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_ECIG_POWER_PIC_BIN);
    // compo_picturebox_cut(picbox, 0, 11);
    // compo_picturebox_set_pos(picbox, 120,180);
    // compo_setid(picbox,COMPO_ID_BTN_ECIG_BG_NUM);


    compo_button_t * btn2 = compo_button_create_by_image(frm,UI_BUF_I330001_ECIG_ADD_BIN);//＋
    compo_setid(btn2, COMPO_ID_BTN_ECIG_SET_ADD);
    // compo_button_set_pos(btn2, 90, 200);
    compo_button_set_location(btn2,90,210,40,30);


    compo_button_t * btn3 = compo_button_create_by_image(frm,UI_BUF_I330001_ECIG_REDUCTION_BIN);//减
    compo_setid(btn3, COMPO_ID_BTN_ECIG_SET_DEL);
    // compo_button_set_pos(btn3, 149, 200);
    compo_button_set_location(btn3,149,210,40,30);

    //   compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_ECIG_ARC_BG_BIN);
    //   compo_picturebox_set_pos(picbox,120,176);

    //   compo_picturebox_t *picbox2 = compo_picturebox_create(frm, UI_BUF_I330001_ECIG_ARC_SLIDER_BIN);
    //   compo_picturebox_set_pos(picbox2,120,176);
    //   compo_picturebox_set_rotation(picbox2,600);
    //   compo_picturebox_set_rotation_center(picbox2,100,99);





    return frm;
}

static void func_ecig_set_power_move_handle(u8 id)
{
    // f_ecig_set_power_t *f_ecig_set_power = (f_ecig_set_power_t*)func_cb.f_cb;
    // ecig_set_power_num_item_t *time_item;
    bool flag_press = 0;
    u32 tick = tick_get();
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[4];

    if (id == COMPO_ID_BTN_ECIG_SET_POWER)
    {
        // time_item = &f_ecig_set_power->set_num_item[0];
    }
    // u8 power = ecig_get_power();

    // s8 power_disp = power;
    flag_press = ctp_get_dxy(&dx, &dy);
    printf("x:%d, y:%d\n",dx, dy);
    // for (;;)
    // {
    //     flag_press = ctp_get_dxy(&dx, &dy);
    //     printf("x:%d, y:%d\n",dx, dy);

    //     // ecig_set_power_num_pos_cal(dy, id, flag_press);
    //     // power_disp = (power - f_ecig_set_power->pcnt) % 30;

    //     if (tick_check_expire(tick, 15))        //15ms刷新一下数字框
    //     {
    //         tick = tick_get();
    //         if (id == COMPO_ID_BTN_ECIG_SET_POWER)
    //         {

    //         }
    //     }

    //     if (!flag_press)
    //     {
    //         // f_ecig_set_power->pcnt = 0;
    //         // power = func_ecig_set_power_get_cal(power_disp, 1, 0);
    //         // ecig_set_power(power);
    //         // printf("ecig_get_power() %d \n", ecig_get_power());
    //         break;
    //     }

    //     //evt_message(msg_dequeue());
    //     func_message(msg_dequeue());
    //     func_process();
    // }

}
//触摸按钮效果处理
static void func_ecig_set_power_button_touch_handle(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ECIG_SET_POWER:
            break;
            func_ecig_set_power_move_handle(id);
        case COMPO_ID_BTN_ECIG_SET_S_D :

            if (get_gear_func()==0)
            {
                test_2st_gear_func();
                compo_button_t *btn1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);
                compo_button_t *btn2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D);
                compo_button_set_bgimg(btn2, UI_BUF_I330001_ECIG_KEY_D_BIN);
                compo_button_set_bgimg(btn1, UI_BUF_I330001_ECIG_BG_D_BIN);



            }
            else
            {
                test_1st_gear_func();
                compo_button_t *btn1 = compo_getobj_byid(COMPO_ID_BTN_ECIG_BG_S_D);
                compo_button_t *btn2 = compo_getobj_byid(COMPO_ID_BTN_ECIG_SET_S_D);
                compo_button_set_bgimg(btn2, UI_BUF_I330001_ECIG_KEY_S_BIN);
                compo_button_set_bgimg(btn1, UI_BUF_I330001_ECIG_BG_S_BIN);
            }



            break;
        case COMPO_ID_BTN_ECIG_SET_ADD :
            printf("%s,COMPO_ID_BTN_ECIG_SET_ADD\n",__func__);
            break;

        case COMPO_ID_BTN_ECIG_SET_DEL :
            printf("%s,COMPO_ID_BTN_ECIG_SET_DEL\n",__func__);

            break;


        default:
            break;
    }
}




//功率功能消息处理
static void func_ecig_set_power_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_ecig_set_power_button_touch_handle();
            break;
        // case MSG_CTP_SHORT_LEFT:
        // case MSG_CTP_SHORT_UP:
        // case MSG_CTP_SHORT_RIGHT:
        // case MSG_CTP_SHORT_DOWN:
        //     // func_menu_sub_drag_move();
        //     break;



        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//功率功能事件处理
static void func_ecig_set_power_process(void)
{
    func_process();
}

//进入功率功能
static void func_ecig_set_power_enter(void)
{
    func_cb.frm_main = func_ecig_set_power_form_create();

}

//退出功率功能
static void func_ecig_set_power_exit(void)
{
    func_cb.last = FUNC_ECIG_SET_POWER;
}

//功率功能
void func_ecig_set_power(void)
{
    printf("%s\n", __func__);
    func_ecig_set_power_enter();
    while (func_cb.sta == FUNC_ECIG_SET_POWER)
    {
        func_ecig_set_power_process();
        func_ecig_set_power_message(msg_dequeue());
    }
    func_ecig_set_power_exit();
}
#endif