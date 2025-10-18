#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_BIRD_SUPPORT

typedef struct f_bird_t_
{
    uint32_t tick;
    int16_t  speed;
    s16      bird_y_pos;
    s16      pipe_x_pos;
    s16      pipe_y_pos;
    uint8_t  pipe_index;
    uint8_t  pipe_cut;
    uint8_t  status;          //游戏状态
    uint8_t  bird_anim_cnt;
    int score;
    s16      pipe_x1_pos;
    s16      pipe_y1_pos;
    u8 move_speed;
} f_bird_t;

enum
{
    COMPO_ID_BIRD_STOP_PIC = 1,
    COMPO_ID_BIRD_BIRD_PIC,
    COMPO_ID_BIRD_PIPE_PIC_START,
    COMPO_ID_BIRD_PIPE_PIC_END = 6,
    COMPO_ID_BIRD_FAIL_PIC,
    COMPO_ID_BIRD_FINISH_PIC,
    COMPO_ID_BIRD_STOP_BTN,
    COMPO_ID_BIRD_START_BTN,
    COMPO_ID_TXT_SCORE,

    COMPO_ID_BIRD_BG2_PIC,
    COMPO_ID_COUNT_SCORE,

    COMPO_ID_BTN_NO,
    COMPO_ID_BTN_YES,
    COMPO_ID_PIC_CLOUD1,
    COMPO_ID_PIC_CLOUD2,
    COMPO_ID_PIC_CLOUD3,
};

enum
{
    BIRD_STATUS_STOP,
    BIRD_STATUS_WAIT_RUN,
    BIRD_STATUS_RUNING,
    BIRD_STATUS_BIRDOVER,
    BIRD_STATUS_FINISH,
    BIRD_STATUS_RESULT_WAIT,
    BIRD_STATUS_RUNNING_STOP,
};


#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define BIRD_DOWN_ACC_V                         1                           //向下加速度
#define BIRD_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              44
#define BIRD_HEIGHT                             (96 / 3)
#define PIPE_SPEED                              3                           //管道移动速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              100                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          50                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         168                         //管道图片高度
#define BIRD_VALID_HEIGHT                       300                         //有效界面高度

//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (BIRD_VALID_HEIGHT - (PIPE_PIC_HEIGHT / (pipe_down_cut)) / 2)

//创建游戏界面
compo_form_t *func_bird_form_create(void)
{
    compo_form_t *frm;
    component_t *compo;
    uint8_t i;
    uint32_t pipe_bin_addr[] = {UI_BUF_I330001_GAME_ZHUZI1_BIN, UI_BUF_I330001_GAME_ZHUZI2_BIN};

    //新建窗体和背景
    frm = compo_form_create(true);

    //背景图
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I330001_GAME_BG_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //stop pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I330001_GAME_PLAY_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //bird pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I330001_GAME_BIRD2_BIN);
    compo_setid(compo, COMPO_ID_BIRD_BIRD_PIC);
    compo_picturebox_cut((compo_picturebox_t *)compo, 0, 3);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, BIRD_X_POS, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //pipe pic
    for(i = 0; i < 4; i++)
    {
        compo = (component_t *)compo_picturebox_create(frm, pipe_bin_addr[i % 2]);
        compo_setid(compo, COMPO_ID_BIRD_PIPE_PIC_START + i);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
    }

    //fail pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I330001_GAME_WINDOW1_BIN);
    compo_setid(compo, COMPO_ID_BIRD_FAIL_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //finish pic
//    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I330001_GAME_WINDOW2_BIN);
//    compo_setid(compo, COMPO_ID_BIRD_FINISH_PIC);
//    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
//    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //stop btn
    compo = (component_t *)compo_button_create(frm);
    compo_setid(compo, COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X+50, GUI_SCREEN_CENTER_Y+35, 60, 60);
//    compo_button_set_visible((compo_button_t *)compo, false);

    //start btn
    compo = (component_t *)compo_button_create(frm);
    compo_setid(compo, COMPO_ID_BIRD_START_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X-50, GUI_SCREEN_CENTER_Y+35, 60, 60);
//    compo_button_set_visible((compo_button_t *)compo, false);

    return frm;
}


//游戏初始化
static void func_bird_init(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y);
    }

    bird->pipe_cut = 1;
    for(i = 0; i < 4; i++)
    {
        if(!(i % 2))
        {
            compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, bird->pipe_cut + i/2);
            }
        }
    }
    bird->bird_anim_cnt = 0;
    bird->speed = 0;
    bird->tick = tick_get();
    bird->bird_y_pos = GUI_SCREEN_CENTER_Y;
    bird->pipe_index = 0;
    bird->pipe_x_pos = GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X;
    bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
    bird->status = BIRD_STATUS_WAIT_RUN;
}


//游戏中
static void func_bird_runing(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;
    uint16_t id;
    s16  x_pos, y_pos;
    uint8_t pipe_cut;
    s16 diff;

    if (tick_check_expire(bird->tick, 1000 / 24)) //24hz
    {
        //bird pic refresh
        bird->tick = tick_get();
        bird->bird_y_pos += bird->speed;
        bird->speed += BIRD_DOWN_ACC_V;
        if(bird->bird_y_pos >= GUI_SCREEN_HEIGHT)
        {
            bird->status = BIRD_STATUS_BIRDOVER;
        }
        compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
        if(compo)
        {
            compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, bird->bird_y_pos);
            compo_picturebox_cut((compo_picturebox_t *)compo, bird->bird_anim_cnt, 3);
            bird->bird_anim_cnt ++;
            bird->bird_anim_cnt %= 3;
        }

        //pipe pic refresh
        x_pos = bird->pipe_x_pos;
        y_pos = bird->pipe_y_pos;
        pipe_cut = bird->pipe_cut;

        for (i = 0; i < 4; i++)
        {
            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + i) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                if(i % 2)
                {
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                }
                if(i == 2)
                {
                    x_pos += PIPE_X_GAP;
                    pipe_cut = 1 + (pipe_cut + 1) % 3;
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                }
                compo_picturebox_set_pos((compo_picturebox_t *)compo, x_pos, y_pos);
            }
        }
        if(bird->pipe_x_pos > (-PIPE_PIC_WIDTH / 2))
        {

            //碰撞检测
            if ((bird->pipe_x_pos + PIPE_PIC_WIDTH / 2) >= (BIRD_X_POS - BIRD_WIDTH / 2))
            {
                diff = (bird->pipe_x_pos - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if(diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / bird->pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, bird->pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }
            else    //下一管道
            {
                diff = ((bird->pipe_x_pos + PIPE_X_GAP) - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if (diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }

            if(diff > 0 && diff < PIPE_SPEED)
            {
                bird->pipe_x_pos -= diff;
            }
            else
            {
                bird->pipe_x_pos -= PIPE_SPEED;
            }

        }
        else
        {
            //前管道视野消失，位置切换至下一个管道
            bird->pipe_x_pos = x_pos;
            bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
            bird->pipe_cut = pipe_cut;

            if (bird->pipe_index)
            {
                bird->pipe_index = 0;
            }
            else
            {
                bird->pipe_index = 2;
            }

            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + 2) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, 1 + (pipe_cut + 1) % 3);
            }
        }
    }
}


//游戏失败
static void func_bird_fail(void)
{
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 130);
    }

    for(i = 0; i < 4; i++)
    {
        compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
        if(compo)
        {
            compo_picturebox_set_visible(compo, false);
        }
    }
}
#elif  GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#define BIRD_DOWN_ACC_V                         1                           //向下加速度
#define BIRD_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              72
#define BIRD_HEIGHT                             (144 / 3)
#define PIPE_SPEED                              3                           //管道移动初始速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              140                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          80                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         168                         //管道图片高度
#define BIRD_VALID_HEIGHT                       412                         //有效界面高度
#define PIPE_PIC0_HEIGHT                        180                         //上管道图片高度
#define PIPE_PIC1_HEIGHT                        220                         //下管道图片高度
#define PIPE_BG_HEIGHT                          35                          //底图背景高度
#define CRC_POINT                               4                           //碰撞容错像素点
#define CLOUD_GAP                               (GUI_SCREEN_CENTER_X - 30) //背景云层间隔
//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC0_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (BIRD_VALID_HEIGHT - (PIPE_PIC1_HEIGHT / (pipe_down_cut)) / 2)
enum
{
    GAME_RUNNING_PAGE,
    GAME_COME_BACK_PAGE,
    GAME_SCORE_PAGE,
};
static const uint32_t cloud_css[3][3] =
{
    {   UI_BUF_I341001_17_GAME_BIRD_WEATHER00_BIN, 60,48},
    {   UI_BUF_I341001_17_GAME_BIRD_WEATHER01_BIN, 114,48},
    {   UI_BUF_I341001_17_GAME_BIRD_WEATHER02_BIN, 60,46},
};
typedef struct cloud_t_
{
    u32 add;
    int x;
    int y;
    int width;
    int hight;
} cloud_t;

static cloud_t cloud[3] = {0};
static u8 page_type = 0;
static compo_form_t* __this = NULL;
static u32 bridIndex = 0;
//创建游戏界面
compo_form_t *func_bird_form_create(void)
{
    compo_form_t *frm;
    component_t *compo;
    uint32_t pipe_bin_addr[] = {UI_BUF_I341001_17_GAME_BIRD_ZHUZI1_BIN, UI_BUF_I341001_17_GAME_BIRD_ZHUZI2_BIN};

    //新建窗体和背景
    frm = compo_form_create(true);

    //背景图
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I341001_17_GAME_BIRD_BG00_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);


    for(uint8_t i = 0; i < 3; i++)
    {
        compo = (component_t *)compo_picturebox_create(frm, cloud[i].add);
        compo_setid(compo, COMPO_ID_PIC_CLOUD1 + i);
    }
    //stop pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I341001_17_GAME_BIRD_PLAY_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //bird pic
    compo = (component_t *)compo_picturebox_create(frm, bridIndex);
    compo_setid(compo, COMPO_ID_BIRD_BIRD_PIC);
    compo_picturebox_cut((compo_picturebox_t *)compo, 0, 3);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, BIRD_X_POS, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //pipe pic
    for(uint8_t i = 0; i < 4; i++)
    {
        compo = (component_t *)compo_picturebox_create(frm, pipe_bin_addr[i % 2]);
        compo_setid(compo, COMPO_ID_BIRD_PIPE_PIC_START + i);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
    }
    char txt_buf[30]= {};
    memset(txt_buf,0,sizeof(txt_buf));
    compo_textbox_t* textbox = compo_textbox_create(frm, 10);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,40,200,46);
    snprintf(txt_buf,sizeof(txt_buf),"%d",0);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_visible(textbox, false);
    compo_setid(textbox,COMPO_ID_COUNT_SCORE);

    compo_picturebox_t * pic = compo_picturebox_create(frm,UI_BUF_I341001_17_GAME_BIRD_BG02_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - PIPE_BG_HEIGHT / 2);
    compo_setid(pic, COMPO_ID_BIRD_BG2_PIC);
    //fail pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I341001_17_GAME_BIRD_BG01_BIN);
    compo_setid(compo, COMPO_ID_BIRD_FAIL_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,228,200,46);
    compo_textbox_set_visible(textbox,false);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:%d%s",i18n[STR_SCORE],0,i18n[STR_POINT]);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x00,0x00));
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_SCORE);


    //start btn
    compo = (component_t *)compo_button_create_by_image(frm,UI_BUF_I341001_17_GAME_BIRD_PLAY01_BIN);
    compo_setid(compo, COMPO_ID_BIRD_START_BTN);
    compo_button_set_location((compo_button_t *)compo, 69 + 90 / 2, 271 + 90 / 2, 90, 90);
    compo_button_set_visible((compo_button_t *)compo, false);

    //stop btn
    compo = (component_t *)compo_button_create_by_image(frm,UI_BUF_I341001_17_GAME_BIRD_QUIT_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_location((compo_button_t *)compo, 209 + 90 / 2, 271 + 90 / 2, 90, 90);
    compo_button_set_visible((compo_button_t *)compo, false);
    page_type = GAME_RUNNING_PAGE;
    __this = frm;
    return frm;
}

static compo_form_t *func_tetris_come_back_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t * shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,GUI_SCREEN_WIDTH,GUI_SCREEN_HEIGHT,25,0x80,make_color(0x00,0x00,0x00));

    compo_textbox_t* textbox = compo_textbox_create(frm, strlen(i18n[STR_EIXT]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y - 30,300,46);
    compo_textbox_set(textbox,i18n[STR_EIXT]);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_17_GAME_TETRIS_FAILURE_BIN);
    compo_setid(btn, COMPO_ID_BTN_NO);
    compo_button_set_pos(btn, 24+156/2, 347+80/2);

    btn = compo_button_create_by_image(frm, UI_BUF_I341001_17_GAME_TETRIS_SUCCESSFUL_BIN);
    compo_setid(btn, COMPO_ID_BTN_YES);
    compo_button_set_pos(btn, 188 + 156 / 2, 347+80/2);

    page_type = GAME_COME_BACK_PAGE;
    return frm;
}

//游戏初始化
static void func_bird_init(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
    }
    char txt_buf[30] = {};
    char txt_num_buf[30] = {};
    memset(txt_buf,0,sizeof(txt_buf));
    compo_textbox_t* textbox = compo_getobj_byid(COMPO_ID_TXT_SCORE);
    compo_textbox_set_visible(textbox,false);
    snprintf(txt_buf,sizeof(txt_buf),"%s:%d%s",i18n[STR_SCORE],0,i18n[STR_POINT]);
    compo_textbox_set(textbox,txt_buf);
    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        bridIndex = get_random(2) ? UI_BUF_I341001_17_GAME_BIRD_BIRD01_BIN : UI_BUF_I341001_17_GAME_BIRD_BIRD03_BIN;
        compo_picturebox_set(compo,bridIndex);
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_visible((compo_button_t *)compo, false);

    compo = compo_getobj_byid(COMPO_ID_BIRD_START_BTN);
    compo_button_set_visible((compo_button_t *)compo, false);

    textbox  = compo_getobj_byid(COMPO_ID_COUNT_SCORE);
    compo_textbox_set_visible(textbox,false);

    bird->bird_anim_cnt = 0;
    bird->speed = 0;
    bird->tick = tick_get();
    bird->bird_y_pos = GUI_SCREEN_CENTER_Y;
    bird->pipe_index = 0;
    bird->pipe_x_pos = GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X;
    bird->pipe_y_pos = GUI_SCREEN_HEIGHT - PIPE_BG_HEIGHT - get_random(PIPE_PIC1_HEIGHT - 2) / 2;
    bird->pipe_x1_pos = bird->pipe_x_pos;
    bird->pipe_y1_pos = GUI_SCREEN_HEIGHT - PIPE_BG_HEIGHT - get_random(PIPE_PIC1_HEIGHT - 2) / 2;
    bird->status = BIRD_STATUS_WAIT_RUN;
    bird->move_speed = PIPE_SPEED;
}


//游戏中
static void func_bird_runing(void)
{
    char txt_buf[10] = {};
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;
    uint16_t id;
    s16  x_pos, y_pos,x1_pos, y1_pos;
    uint8_t pipe_cut;
    s16 diff;

    if (tick_check_expire(bird->tick, 1000 / 24)) //24hz
    {
        //bird pic refresh
        bird->tick = tick_get();
        bird->bird_y_pos += bird->speed;
        bird->speed += BIRD_DOWN_ACC_V;
        if(bird->bird_y_pos >= GUI_SCREEN_HEIGHT)
        {
            bird->status = BIRD_STATUS_BIRDOVER;
        }
        compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
        if(compo)
        {
            compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, bird->bird_y_pos);
            compo_picturebox_cut((compo_picturebox_t *)compo, bird->bird_anim_cnt, 3);
            bird->bird_anim_cnt ++;
            bird->bird_anim_cnt %= 3;
        }

        //pipe pic refresh
        x_pos = bird->pipe_x_pos;
        y_pos = bird->pipe_y_pos;
        id = COMPO_ID_BIRD_PIPE_PIC_START;
        compo = compo_getobj_byid(id);
        compo_picturebox_set_location((compo_picturebox_t *)compo, bird->pipe_x_pos, bird->pipe_y_pos,PIPE_PIC_WIDTH,PIPE_PIC1_HEIGHT);
        compo = compo_getobj_byid(id + 1);
        compo_picturebox_set_location((compo_picturebox_t *)compo, bird->pipe_x_pos, bird->pipe_y_pos - PIPE_PIC1_HEIGHT / 2 - PIPE_Y_GAP - PIPE_PIC0_HEIGHT / 2,PIPE_PIC_WIDTH,PIPE_PIC0_HEIGHT);

        x1_pos = x_pos + PIPE_X_GAP;
        bird->pipe_x1_pos = x1_pos;
        y1_pos = bird->pipe_y1_pos;
        compo = compo_getobj_byid(id + 2);
        compo_picturebox_set_location((compo_picturebox_t *)compo, x1_pos, y1_pos,PIPE_PIC_WIDTH,PIPE_PIC1_HEIGHT);
        compo = compo_getobj_byid(id + 3);
        compo_picturebox_set_location((compo_picturebox_t *)compo, x1_pos, y1_pos - PIPE_PIC1_HEIGHT / 2 - PIPE_Y_GAP - PIPE_PIC0_HEIGHT / 2,PIPE_PIC_WIDTH,PIPE_PIC0_HEIGHT);

        cloud[0].x = cloud[0].x - bird->move_speed / 2;
        cloud[1].x = cloud[0].x + CLOUD_GAP;
        cloud[2].x = cloud[0].x + CLOUD_GAP * 2;
        for(uint8_t i = 0; i < 3; i++)
        {
            compo = compo_getobj_byid(COMPO_ID_PIC_CLOUD1 + i);
            compo_picturebox_set_location((compo_picturebox_t *)compo,cloud[i].x,cloud[i].y,cloud[i].width,cloud[i].hight);
        }

        if((BIRD_X_POS - BIRD_WIDTH / 2) - (bird->pipe_x_pos + PIPE_PIC_WIDTH / 2) > 0 &&\
           (BIRD_X_POS - BIRD_WIDTH / 2) - (bird->pipe_x_pos + PIPE_PIC_WIDTH / 2) <= bird->move_speed)
        {
            bird->score++;
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t* textbox  = compo_getobj_byid(COMPO_ID_COUNT_SCORE);
            compo_textbox_set_visible(textbox,bird->score ? true :false);
            snprintf(txt_buf,sizeof(txt_buf),"%d",bird->score);
            compo_textbox_set(textbox,txt_buf);
        }
        if(bird->pipe_x_pos > (-PIPE_PIC_WIDTH / 2))
        {
            if((bird->bird_y_pos + BIRD_HEIGHT / 2 - CRC_POINT) > (bird->pipe_y_pos - PIPE_PIC1_HEIGHT / 2) || (bird->bird_y_pos - BIRD_HEIGHT / 2 + CRC_POINT) < (bird->pipe_y_pos - PIPE_PIC1_HEIGHT / 2 - PIPE_Y_GAP)) // -+--|
            {
                //碰撞检测
                if ((bird->pipe_x_pos - PIPE_PIC_WIDTH / 2  - CRC_POINT) >= (BIRD_X_POS - BIRD_WIDTH / 2) && (bird->pipe_x_pos - PIPE_PIC_WIDTH / 2 + CRC_POINT) <= (BIRD_X_POS + BIRD_WIDTH / 2))
                {
                    bird->status = BIRD_STATUS_BIRDOVER;
                }
                else if((bird->pipe_x_pos - PIPE_PIC_WIDTH / 2 + CRC_POINT) <= (BIRD_X_POS - BIRD_WIDTH / 2) && (bird->pipe_x_pos + PIPE_PIC_WIDTH / 2 - CRC_POINT) >= (BIRD_X_POS - BIRD_WIDTH / 2))
                {
                    bird->status = BIRD_STATUS_BIRDOVER;
                }
                else if((bird->pipe_x_pos + PIPE_PIC_WIDTH / 2 - CRC_POINT) >= (BIRD_X_POS - BIRD_WIDTH / 2) && (bird->pipe_x_pos + PIPE_PIC_WIDTH / 2 + CRC_POINT) <= (BIRD_X_POS + BIRD_WIDTH / 2))
                {
                    bird->status = BIRD_STATUS_BIRDOVER;
                }
            }
            if(bird->bird_y_pos + BIRD_HEIGHT / 2 - CRC_POINT >= GUI_SCREEN_HEIGHT - PIPE_BG_HEIGHT)
            {
                bird->status = BIRD_STATUS_BIRDOVER;
            }
        }
        else
        {
            static u8 temp = 0;
            if(temp != bird->score / 10)
            {
                temp = bird->score / 10;
                bird->move_speed = bird->move_speed > 5 ? 6 : bird->move_speed + 1;
            }
            //前管道视野消失，位置切换至下一个管道
            bird->pipe_x_pos =  bird->pipe_x1_pos;
            bird->pipe_y_pos =  bird->pipe_y1_pos;
            bird->pipe_y1_pos = GUI_SCREEN_HEIGHT - PIPE_BG_HEIGHT - get_random(PIPE_PIC1_HEIGHT - 2) / 2;
        }
        if(cloud[0].x < -cloud[0].width / 2)
        {
            memcpy(&cloud[0],&cloud[1],sizeof(cloud_t));
            memcpy(&cloud[1],&cloud[2],sizeof(cloud_t));
            cloud[2].x = cloud[0].x + CLOUD_GAP * 2;
            cloud[2].y = get_random(100) + 80;
            u8 index_add = get_random(3);
            cloud[2].add = cloud_css[index_add][0];
            cloud[2].width = get_random(20) + cloud_css[index_add][1];
            cloud[2].hight = get_random(20) + cloud_css[index_add][2] - 10;
            compo = compo_getobj_byid(COMPO_ID_PIC_CLOUD1);
            compo_picturebox_set((compo_picturebox_t *)compo,cloud[0].add);
            compo_picturebox_set_location((compo_picturebox_t *)compo, cloud[0].x, cloud[0].y,cloud[0].width,cloud[0].hight);
            compo = compo_getobj_byid(COMPO_ID_PIC_CLOUD2);
            compo_picturebox_set((compo_picturebox_t *)compo,cloud[1].add);
            compo_picturebox_set_location((compo_picturebox_t *)compo, cloud[1].x, cloud[1].y,cloud[1].width,cloud[1].hight);
            compo = compo_getobj_byid(COMPO_ID_PIC_CLOUD3);
            compo_picturebox_set((compo_picturebox_t *)compo,cloud[2].add);
            compo_picturebox_set_location((compo_picturebox_t *)compo, cloud[2].x, cloud[2].y,cloud[2].width,cloud[2].hight);
        }
        bird->pipe_x_pos -= bird->move_speed;
    }
}


//游戏失败
static void func_bird_fail(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    char txt_buf[30] = {};
    memset(txt_buf,0,sizeof(txt_buf));
    compo_textbox_t* textbox = compo_getobj_byid(COMPO_ID_TXT_SCORE);
    compo_textbox_set_visible(textbox,true);
    snprintf(txt_buf,sizeof(txt_buf),"%s:%d%s",i18n[STR_SCORE],bird->score,i18n[STR_POINT]);
    compo_textbox_set(textbox,txt_buf);
    bird->score = 0;

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_visible((compo_button_t *)compo, true);

    compo = compo_getobj_byid(COMPO_ID_BIRD_START_BTN);
    compo_button_set_visible((compo_button_t *)compo, true);

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 130);
    }
    memset(txt_buf,0,sizeof(txt_buf));
    textbox  = compo_getobj_byid(COMPO_ID_COUNT_SCORE);
    compo_textbox_set_visible(textbox,false);
    snprintf(txt_buf,sizeof(txt_buf),"%d",0);
    compo_textbox_set(textbox,txt_buf);

    for(i = 0; i < 4; i++)
    {
        compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
        if(compo)
        {
            compo_picturebox_set_visible(compo, false);
        }
    }
}
#elif  GUI_SCREEN_SIZE_240X284RGB_I335003_SUPPORT
#define BIRD_DOWN_ACC_V                         1                           //向下加速度
#define BIRD_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              44
#define BIRD_HEIGHT                             (96 / 3)
#define PIPE_SPEED                              3                           //管道移动速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              100                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          50                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         168                         //管道图片高度
#define BIRD_VALID_HEIGHT                       300                         //有效界面高度

//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (BIRD_VALID_HEIGHT - (PIPE_PIC_HEIGHT / (pipe_down_cut)) / 2)

//创建游戏界面
compo_form_t *func_bird_form_create(void)
{
    compo_form_t *frm;
    component_t *compo;
    uint8_t i;
    uint32_t pipe_bin_addr[] = {UI_BUF_I335003_GAME_ZHUZI1_BIN, UI_BUF_I335003_GAME_ZHUZI2_BIN};

    //新建窗体和背景
    frm = compo_form_create(true);

    //背景图
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I335003_GAME_BG_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //stop pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I335003_GAME_PLAY_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //bird pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I335003_GAME_BIRD2_BIN);
    compo_setid(compo, COMPO_ID_BIRD_BIRD_PIC);
    compo_picturebox_cut((compo_picturebox_t *)compo, 0, 3);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, BIRD_X_POS, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //pipe pic
    for(i = 0; i < 4; i++)
    {
        compo = (component_t *)compo_picturebox_create(frm, pipe_bin_addr[i % 2]);
        compo_setid(compo, COMPO_ID_BIRD_PIPE_PIC_START + i);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
    }

    //fail pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I335003_GAME_WINDOW1_BIN);
    compo_setid(compo, COMPO_ID_BIRD_FAIL_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //finish pic
//    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I335003_GAME_WINDOW2_BIN);
//    compo_setid(compo, COMPO_ID_BIRD_FINISH_PIC);
//    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
//    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //stop btn
    compo = (component_t *)compo_button_create(frm);
    compo_setid(compo, COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X+50, GUI_SCREEN_CENTER_Y+35, 60, 60);
//    compo_button_set_visible((compo_button_t *)compo, false);

    //start btn
    compo = (component_t *)compo_button_create(frm);
    compo_setid(compo, COMPO_ID_BIRD_START_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X-50, GUI_SCREEN_CENTER_Y+35, 60, 60);
//    compo_button_set_visible((compo_button_t *)compo, false);

    return frm;
}


//游戏初始化
static void func_bird_init(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y);
    }

    bird->pipe_cut = 1;
    for(i = 0; i < 4; i++)
    {
        if(!(i % 2))
        {
            compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, bird->pipe_cut + i/2);
            }
        }
    }
    bird->bird_anim_cnt = 0;
    bird->speed = 0;
    bird->tick = tick_get();
    bird->bird_y_pos = GUI_SCREEN_CENTER_Y;
    bird->pipe_index = 0;
    bird->pipe_x_pos = GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X;
    bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
    bird->status = BIRD_STATUS_WAIT_RUN;
}


//游戏中
static void func_bird_runing(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;
    uint16_t id;
    s16  x_pos, y_pos;
    uint8_t pipe_cut;
    s16 diff;

    if (tick_check_expire(bird->tick, 1000 / 24)) //24hz
    {
        //bird pic refresh
        bird->tick = tick_get();
        bird->bird_y_pos += bird->speed;
        bird->speed += BIRD_DOWN_ACC_V;
        if(bird->bird_y_pos >= GUI_SCREEN_HEIGHT)
        {
            bird->status = BIRD_STATUS_BIRDOVER;
        }
        compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
        if(compo)
        {
            compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, bird->bird_y_pos);
            compo_picturebox_cut((compo_picturebox_t *)compo, bird->bird_anim_cnt, 3);
            bird->bird_anim_cnt ++;
            bird->bird_anim_cnt %= 3;
        }

        //pipe pic refresh
        x_pos = bird->pipe_x_pos;
        y_pos = bird->pipe_y_pos;
        pipe_cut = bird->pipe_cut;

        for (i = 0; i < 4; i++)
        {
            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + i) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                if(i % 2)
                {
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                }
                if(i == 2)
                {
                    x_pos += PIPE_X_GAP;
                    pipe_cut = 1 + (pipe_cut + 1) % 3;
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                }
                compo_picturebox_set_pos((compo_picturebox_t *)compo, x_pos, y_pos);
            }
        }
        if(bird->pipe_x_pos > (-PIPE_PIC_WIDTH / 2))
        {

            //碰撞检测
            if ((bird->pipe_x_pos + PIPE_PIC_WIDTH / 2) >= (BIRD_X_POS - BIRD_WIDTH / 2))
            {
                diff = (bird->pipe_x_pos - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if(diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / bird->pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, bird->pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }
            else    //下一管道
            {
                diff = ((bird->pipe_x_pos + PIPE_X_GAP) - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if (diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }

            if(diff > 0 && diff < PIPE_SPEED)
            {
                bird->pipe_x_pos -= diff;
            }
            else
            {
                bird->pipe_x_pos -= PIPE_SPEED;
            }

        }
        else
        {
            //前管道视野消失，位置切换至下一个管道
            bird->pipe_x_pos = x_pos;
            bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
            bird->pipe_cut = pipe_cut;

            if (bird->pipe_index)
            {
                bird->pipe_index = 0;
            }
            else
            {
                bird->pipe_index = 2;
            }

            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + 2) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, 1 + (pipe_cut + 1) % 3);
            }
        }
    }
}


//游戏失败
static void func_bird_fail(void)
{
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 130);
    }

    for(i = 0; i < 4; i++)
    {
        compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
        if(compo)
        {
            compo_picturebox_set_visible(compo, false);
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define BIRD_DOWN_ACC_V                         1                           //向下加速度
#define BIRD_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              56
#define BIRD_HEIGHT                             (132 / 3)
#define PIPE_SPEED                              3                           //管道移动速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              130                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          57                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         213                         //管道图片高度
#define BIRD_VALID_HEIGHT                       380                         //有效界面高度

//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (BIRD_VALID_HEIGHT - (PIPE_PIC_HEIGHT / (pipe_down_cut)) / 2)

//创建游戏界面
compo_form_t *func_bird_form_create(void)
{
    compo_form_t *frm;
    component_t *compo;
    uint8_t i;
    uint32_t pipe_bin_addr[] = {UI_BUF_I332001_GAME_ZHUZI1_BIN, UI_BUF_I332001_GAME_ZHUZI2_BIN};

    //新建窗体和背景
    frm = compo_form_create(true);

    //背景图
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I332001_GAME_BG_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //stop pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I332001_GAME_PLAY_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //bird pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I332001_GAME_BIRD2_BIN);
    compo_setid(compo, COMPO_ID_BIRD_BIRD_PIC);
    compo_picturebox_cut((compo_picturebox_t *)compo, 0, 3);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, BIRD_X_POS, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //pipe pic
    for(i = 0; i < 4; i++)
    {
        compo = (component_t *)compo_picturebox_create(frm, pipe_bin_addr[i % 2]);
        compo_setid(compo, COMPO_ID_BIRD_PIPE_PIC_START + i);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
    }

    //fail pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I332001_GAME_WINDOW1_BIN);
    compo_setid(compo, COMPO_ID_BIRD_FAIL_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);


    //stop btn
    compo = (component_t *)compo_button_create(frm);
//    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_I332001_SLEEP_BG_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X+70, GUI_SCREEN_CENTER_Y+18, 80, 80);

    //start btn
    compo = (component_t *)compo_button_create(frm);
//    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_I332001_SLEEP_BG_BIN);
    compo_setid(compo, COMPO_ID_BIRD_START_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X-70, GUI_SCREEN_CENTER_Y+18, 80, 80);

    return frm;
}
//游戏初始化
static void func_bird_init(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y);
    }

    bird->pipe_cut = 1;
    for(i = 0; i < 4; i++)
    {
        if(!(i % 2))
        {
            compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, bird->pipe_cut + i/2);
            }
        }
    }
    bird->bird_anim_cnt = 0;
    bird->speed = 0;
    bird->tick = tick_get();
    bird->bird_y_pos = GUI_SCREEN_CENTER_Y;
    bird->pipe_index = 0;
    bird->pipe_x_pos = GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X;
    bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
    bird->status = BIRD_STATUS_WAIT_RUN;
}


//游戏中
static void func_bird_runing(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;
    uint16_t id;
    s16  x_pos, y_pos;
    uint8_t pipe_cut;
    s16 diff;

    if (tick_check_expire(bird->tick, 1000 / 24)) //24hz
    {
        //bird pic refresh
        bird->tick = tick_get();
        bird->bird_y_pos += bird->speed;
        bird->speed += BIRD_DOWN_ACC_V;
        if(bird->bird_y_pos >= GUI_SCREEN_HEIGHT)
        {
            bird->status = BIRD_STATUS_BIRDOVER;
        }
        compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
        if(compo)
        {
            compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, bird->bird_y_pos);
            compo_picturebox_cut((compo_picturebox_t *)compo, bird->bird_anim_cnt, 3);
            bird->bird_anim_cnt ++;
            bird->bird_anim_cnt %= 3;
        }

        //pipe pic refresh
        x_pos = bird->pipe_x_pos;
        y_pos = bird->pipe_y_pos;
        pipe_cut = bird->pipe_cut;

        for (i = 0; i < 4; i++)
        {
            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + i) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                if(i % 2)
                {
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                }
                if(i == 2)
                {
                    x_pos += PIPE_X_GAP;
                    pipe_cut = 1 + (pipe_cut + 1) % 3;
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                }
                compo_picturebox_set_pos((compo_picturebox_t *)compo, x_pos, y_pos);
            }
        }
        if(bird->pipe_x_pos > (-PIPE_PIC_WIDTH / 2))
        {

            //碰撞检测
            if ((bird->pipe_x_pos + PIPE_PIC_WIDTH / 2) >= (BIRD_X_POS - BIRD_WIDTH / 2))
            {
                diff = (bird->pipe_x_pos - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if(diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / bird->pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, bird->pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }
            else    //下一管道
            {
                diff = ((bird->pipe_x_pos + PIPE_X_GAP) - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if (diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }

            if(diff > 0 && diff < PIPE_SPEED)
            {
                bird->pipe_x_pos -= diff;
            }
            else
            {
                bird->pipe_x_pos -= PIPE_SPEED;
            }

        }
        else
        {
            //前管道视野消失，位置切换至下一个管道
            bird->pipe_x_pos = x_pos;
            bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
            bird->pipe_cut = pipe_cut;

            if (bird->pipe_index)
            {
                bird->pipe_index = 0;
            }
            else
            {
                bird->pipe_index = 2;
            }

            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + 2) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, 1 + (pipe_cut + 1) % 3);
            }
        }
    }
}


//游戏失败
static void func_bird_fail(void)
{
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 130);
    }

    for(i = 0; i < 4; i++)
    {
        compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
        if(compo)
        {
            compo_picturebox_set_visible(compo, false);
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define BIRD_DOWN_ACC_V                         1                           //向下加速度
#define BIRD_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              56
#define BIRD_HEIGHT                             (132 / 3)
#define PIPE_SPEED                              3                           //管道移动速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              130                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          57                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         213                         //管道图片高度
#define BIRD_VALID_HEIGHT                       380                         //有效界面高度

//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (BIRD_VALID_HEIGHT - (PIPE_PIC_HEIGHT / (pipe_down_cut)) / 2)

//创建游戏界面
compo_form_t *func_bird_form_create(void)
{
    compo_form_t *frm;
    component_t *compo;
    uint8_t i;
    uint32_t pipe_bin_addr[] = {UI_BUF_I340001_GAME_ZHUZI1_BIN, UI_BUF_I340001_GAME_ZHUZI2_BIN};

    //新建窗体和背景
    frm = compo_form_create(true);

    //背景图
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I340001_GAME_BG_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //stop pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I340001_GAME_PLAY_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //bird pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I340001_GAME_BIRD2_BIN);
    compo_setid(compo, COMPO_ID_BIRD_BIRD_PIC);
    compo_picturebox_cut((compo_picturebox_t *)compo, 0, 3);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, BIRD_X_POS, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //pipe pic
    for(i = 0; i < 4; i++)
    {
        compo = (component_t *)compo_picturebox_create(frm, pipe_bin_addr[i % 2]);
        compo_setid(compo, COMPO_ID_BIRD_PIPE_PIC_START + i);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
    }

    //fail pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_I340001_GAME_WINDOW1_BIN);
    compo_setid(compo, COMPO_ID_BIRD_FAIL_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);


    //stop btn
    compo = (component_t *)compo_button_create(frm);
//    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_I340001_SLEEP_BG_BIN);
    compo_setid(compo, COMPO_ID_BIRD_STOP_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X+70, GUI_SCREEN_CENTER_Y+18, 80, 80);

    //start btn
    compo = (component_t *)compo_button_create(frm);
//    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_I340001_SLEEP_BG_BIN);
    compo_setid(compo, COMPO_ID_BIRD_START_BTN);
    compo_button_set_location((compo_button_t *)compo, GUI_SCREEN_CENTER_X-70, GUI_SCREEN_CENTER_Y+18, 80, 80);

    return frm;
}
//游戏初始化
static void func_bird_init(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y);
    }

    bird->pipe_cut = 1;
    for(i = 0; i < 4; i++)
    {
        if(!(i % 2))
        {
            compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, bird->pipe_cut + i/2);
            }
        }
    }
    bird->bird_anim_cnt = 0;
    bird->speed = 0;
    bird->tick = tick_get();
    bird->bird_y_pos = GUI_SCREEN_CENTER_Y;
    bird->pipe_index = 0;
    bird->pipe_x_pos = GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X;
    bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
    bird->status = BIRD_STATUS_WAIT_RUN;
}


//游戏中
static void func_bird_runing(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;
    uint16_t id;
    s16  x_pos, y_pos;
    uint8_t pipe_cut;
    s16 diff;

    if (tick_check_expire(bird->tick, 1000 / 24)) //24hz
    {
        //bird pic refresh
        bird->tick = tick_get();
        bird->bird_y_pos += bird->speed;
        bird->speed += BIRD_DOWN_ACC_V;
        if(bird->bird_y_pos >= GUI_SCREEN_HEIGHT)
        {
            bird->status = BIRD_STATUS_BIRDOVER;
        }
        compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
        if(compo)
        {
            compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, bird->bird_y_pos);
            compo_picturebox_cut((compo_picturebox_t *)compo, bird->bird_anim_cnt, 3);
            bird->bird_anim_cnt ++;
            bird->bird_anim_cnt %= 3;
        }

        //pipe pic refresh
        x_pos = bird->pipe_x_pos;
        y_pos = bird->pipe_y_pos;
        pipe_cut = bird->pipe_cut;

        for (i = 0; i < 4; i++)
        {
            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + i) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                if(i % 2)
                {
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                }
                if(i == 2)
                {
                    x_pos += PIPE_X_GAP;
                    pipe_cut = 1 + (pipe_cut + 1) % 3;
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                }
                compo_picturebox_set_pos((compo_picturebox_t *)compo, x_pos, y_pos);
            }
        }
        if(bird->pipe_x_pos > (-PIPE_PIC_WIDTH / 2))
        {

            //碰撞检测
            if ((bird->pipe_x_pos + PIPE_PIC_WIDTH / 2) >= (BIRD_X_POS - BIRD_WIDTH / 2))
            {
                diff = (bird->pipe_x_pos - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if(diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(bird->pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / bird->pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, bird->pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }
            else    //下一管道
            {
                diff = ((bird->pipe_x_pos + PIPE_X_GAP) - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if (diff <= 0)
                {
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / pipe_cut / 2) <= (bird->bird_y_pos + BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (bird->bird_y_pos - BIRD_HEIGHT / 2))
                    {
                        bird->status = BIRD_STATUS_BIRDOVER;
                    }
                }
            }

            if(diff > 0 && diff < PIPE_SPEED)
            {
                bird->pipe_x_pos -= diff;
            }
            else
            {
                bird->pipe_x_pos -= PIPE_SPEED;
            }

        }
        else
        {
            //前管道视野消失，位置切换至下一个管道
            bird->pipe_x_pos = x_pos;
            bird->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
            bird->pipe_cut = pipe_cut;

            if (bird->pipe_index)
            {
                bird->pipe_index = 0;
            }
            else
            {
                bird->pipe_index = 2;
            }

            id = COMPO_ID_BIRD_PIPE_PIC_START + (bird->pipe_index + 2) % 4;
            compo = compo_getobj_byid(id);
            if(compo)
            {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, 1 + (pipe_cut + 1) % 3);
            }
        }
    }
}


//游戏失败
static void func_bird_fail(void)
{
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_FAIL_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 130);
    }

    for(i = 0; i < 4; i++)
    {
        compo = compo_getobj_byid(COMPO_ID_BIRD_PIPE_PIC_START + i);
        if(compo)
        {
            compo_picturebox_set_visible(compo, false);
        }
    }
}
#else
#define BIRD_DOWN_ACC_V                         1                           //向下加速度
#define BIRD_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              44
#define BIRD_HEIGHT                             (96 / 3)
#define PIPE_SPEED                              3                           //管道移动速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              100                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          50                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         168                         //管道图片高度
#define BIRD_VALID_HEIGHT                       300                         //有效界面高度

//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (BIRD_VALID_HEIGHT - (PIPE_PIC_HEIGHT / (pipe_down_cut)) / 2)

//创建游戏界面
compo_form_t *func_bird_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
//游戏初始化
static void func_bird_init(void)
{
}
//游戏中
static void func_bird_runing(void)
{
}
//游戏失败
static void func_bird_fail(void)
{

}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

static void func_bird_start(void)
{
    int btn_id = compo_get_button_id();
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, false);
    }
    compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
    if(compo)
    {
        compo_picturebox_set_visible(compo, true);
    }

    for(i = 0; i < 4; i++)
    {
        compo = compo_getobj_byid(i + COMPO_ID_BIRD_PIPE_PIC_START);
        if(compo)
        {
            compo_picturebox_set_visible(compo, true);
        }
    }
}

//游戏过程状态机
static void func_bird_process(void)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;

    switch (bird->status)
    {
        case BIRD_STATUS_WAIT_RUN:
        case BIRD_STATUS_RESULT_WAIT:
            break;

        case BIRD_STATUS_STOP:
            func_bird_init();
            bird->status = BIRD_STATUS_WAIT_RUN;
            break;

        case BIRD_STATUS_RUNING:
            func_bird_runing();
            break;

        case BIRD_STATUS_BIRDOVER:
        case BIRD_STATUS_FINISH:
            func_bird_fail();
            bird->status = BIRD_STATUS_RESULT_WAIT;
            break;

        default:
            break;
    }
}

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//单击处理
static void func_bird_click(void)
{
    int btn_id = compo_get_button_id();
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    if (BIRD_STATUS_WAIT_RUN == bird->status)
    {
        func_bird_start();
        if(page_type == GAME_RUNNING_PAGE)
        {
            bird->status = BIRD_STATUS_RUNING;
        }
    }
    else if (BIRD_STATUS_RUNING == bird->status)
    {
        if(bird->bird_y_pos <= -BIRD_HEIGHT)
        {
            bird->speed = 0;
        }
        else
        {
            if(bird->speed > 0)
            {
                bird->speed = -BIRD_UP_ACC_V;
            }
            else
            {
                bird->speed -= BIRD_UP_ACC_V;
            }
        }
    }
    else if (BIRD_STATUS_RESULT_WAIT == bird->status)
    {
        if (COMPO_ID_BIRD_STOP_BTN == btn_id)
        {
            bird->status = BIRD_STATUS_STOP;
            func_back_to();
        }
        else if (COMPO_ID_BIRD_START_BTN == btn_id)
        {
            func_bird_init();

            func_bird_start();
            delay_ms(40);
            bird->status = BIRD_STATUS_RUNING;
        }
    }
    else if (BIRD_STATUS_RUNNING_STOP == bird->status)
    {
        if(page_type == GAME_COME_BACK_PAGE)
        {
            if (COMPO_ID_BTN_NO == btn_id)
            {
                compo_form_destroy(func_cb.frm_main);
                page_type = GAME_RUNNING_PAGE;
                func_cb.frm_main = __this;
                bird->status = BIRD_STATUS_RUNING;
                return;
            }
            else if (COMPO_ID_BTN_YES == btn_id)
            {
                compo_form_destroy(func_cb.frm_main);
                func_cb.frm_main = __this;
                func_back_to();
                return;
            }
        }

    }
}
#else
//单击处理
static void func_bird_click(void)
{
    int btn_id = compo_get_button_id();
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    if (BIRD_STATUS_WAIT_RUN == bird->status)
    {
        compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
        if(compo)
        {
            compo_picturebox_set_visible(compo, false);
        }
        compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
        if(compo)
        {
            compo_picturebox_set_visible(compo, true);
        }

        for(i = 0; i < 4; i++)
        {
            compo = compo_getobj_byid(i + COMPO_ID_BIRD_PIPE_PIC_START);
            if(compo)
            {
                compo_picturebox_set_visible(compo, true);
            }
        }

        bird->status = BIRD_STATUS_RUNING;

    }
    else if (BIRD_STATUS_RUNING == bird->status)
    {
        if(bird->bird_y_pos <= -BIRD_HEIGHT)
        {
            bird->speed = 0;
        }
        else
        {
            if(bird->speed > 0)
            {
                bird->speed = -BIRD_UP_ACC_V;
            }
            else
            {
                bird->speed -= BIRD_UP_ACC_V;
            }
        }
    }
    else if (BIRD_STATUS_RESULT_WAIT == bird->status)
    {
        if (COMPO_ID_BIRD_STOP_BTN == btn_id)
        {
            bird->status = BIRD_STATUS_STOP;
        }
        else if (COMPO_ID_BIRD_START_BTN == btn_id)
        {
            func_bird_init();

            compo = compo_getobj_byid(COMPO_ID_BIRD_STOP_PIC);
            if(compo)
            {
                compo_picturebox_set_visible(compo, false);
            }
            compo = compo_getobj_byid(COMPO_ID_BIRD_BIRD_PIC);
            if(compo)
            {
                compo_picturebox_set_visible(compo, true);
            }

            for(i = 0; i < 4; i++)
            {
                compo = compo_getobj_byid(i + COMPO_ID_BIRD_PIPE_PIC_START);
                if(compo)
                {
                    compo_picturebox_set_visible(compo, true);
                }
            }
            delay_ms(40);
            bird->status = BIRD_STATUS_RUNING;
        }
    }
}
#endif


//消息处理
static void func_bird_message(size_msg_t msg)
{
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_bird_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_SHORT_LEFT:
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            if(bird->status == BIRD_STATUS_RUNING)
            {
                return;
            }
            else
            {
                func_message(msg);
            }
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CHECK_LANGUAGE:
            if(bird->status == BIRD_STATUS_RUNING)
            {
                bird->status = BIRD_STATUS_RUNNING_STOP;
                func_cb.frm_main = func_tetris_come_back_form_create();
                return;
            }
            else if (bird->status == BIRD_STATUS_RUNNING_STOP)
            {
                compo_form_destroy(func_cb.frm_main);
                page_type = GAME_RUNNING_PAGE;
                func_cb.frm_main = __this;
                bird->status = BIRD_STATUS_RUNING;
                break;
            }
            else
            {
                func_message(msg);
            }
            break;
        case MSG_CTP_LONG:
            break;

        default:
            if(page_type == GAME_COME_BACK_PAGE)
            {
                return;
            }
            func_message(msg);
            break;
    }
}


//进入游戏功能
static void func_bird_enter(void)
{
    if(func_cb.f_cb == NULL)
    {
        func_cb.f_cb = func_zalloc(sizeof(f_bird_t));
    }
    func_cb.frm_main = func_bird_form_create();
    f_bird_t *bird = (f_bird_t *)func_cb.f_cb;
    if(func_cb.last != FUNC_CLOCK_SAVER || bird->status != BIRD_STATUS_RUNNING_STOP)
    {
        bird->status = BIRD_STATUS_STOP;
        func_bird_init();
    }
    else
    {
        func_bird_start();
        func_bird_runing();
        func_cb.frm_main = func_tetris_come_back_form_create();
    }
}


//退出游戏功能
static void func_bird_exit(void)
{
    if(page_type == GAME_COME_BACK_PAGE)
    {
        compo_form_destroy(func_cb.frm_main);
        page_type = GAME_RUNNING_PAGE;
        func_cb.frm_main = __this;
    }
    func_cb.last = FUNC_BIRD;
}


//游戏功能
void func_bird(void)
{
    printf("%s\n", __func__);
    func_bird_enter();
    while (func_cb.sta == FUNC_BIRD)
    {
        func_bird_process();
        func_process();
        func_bird_message(msg_dequeue());
    }
    func_bird_exit();
}

#endif