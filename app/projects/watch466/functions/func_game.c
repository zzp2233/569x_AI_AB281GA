#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_game_t_ {
    uint32_t tick;
    int16_t  speed;
    s16      bird_y_pos;
    s16      pipe_x_pos;
    s16      pipe_y_pos;
    uint8_t  pipe_index;
    uint8_t  pipe_cut;
    uint8_t  status;          //游戏状态
    uint8_t  bird_anim_cnt;
} f_game_t;

enum
{
    COMPO_ID_GAME_STOP_PIC = 1,
    COMPO_ID_GAME_BIRD_PIC,
    COMPO_ID_GAME_PIPE_PIC_START,
    COMPO_ID_GAME_PIPE_PIC_END = 6,
    COMPO_ID_GAME_FAIL_PIC,
    COMPO_ID_GAME_FINISH_PIC,
    COMPO_ID_GAME_STOP_BTN,
    COMPO_ID_GAME_START_BTN,
};

enum
{
    GAME_STATUS_STOP,
    GAME_STATUS_WAIT_RUN,
    GAME_STATUS_RUNING,
    GAME_STATUS_GAMEOVER,
    GAME_STATUS_FINISH,
    GAME_STATUS_RESULT_WAIT,
};

#define GAME_DOWN_ACC_V                         1                           //向下加速度
#define GAME_UP_ACC_V                           5                           //向上加速度
#define BIRD_X_POS                              (GUI_SCREEN_CENTER_X / 2)   //小鸟图X坐标
#define BIRD_WIDTH                              44
#define BIRD_HEIGHT                             (96 / 3)
#define PIPE_SPEED                              3                           //管道移动速度
#define PIPE_X_GAP                              (GUI_SCREEN_CENTER_X + 80)  //管道水平间隔
#define PIPE_Y_GAP                              100                         //管道纵向间隔
#define PIPE_PIC_WIDTH                          50                          //管道图片宽度
#define PIPE_PIC_HEIGHT                         168                         //管道图片高度
#define GAME_VALID_HEIGHT                       300                         //有效界面高度

//计算上管道y坐标(中心参考点)
#define CAL_PIPE_UP_Y_POS(pipe_down_y_pos, pipe_down_cut)    ((pipe_down_y_pos) - (PIPE_PIC_HEIGHT / (pipe_down_cut) / 2) - (PIPE_PIC_HEIGHT / 2) - PIPE_Y_GAP)
//计算下管道y坐标(中心参考点)
#define CAL_PIPE_DOWM_Y_POS(pipe_down_cut)                   (GAME_VALID_HEIGHT - (PIPE_PIC_HEIGHT / (pipe_down_cut)) / 2)


//创建游戏界面
compo_form_t *func_game_form_create(void)
{
    compo_form_t *frm;
    component_t *compo;
    uint8_t i;
    uint32_t pipe_bin_addr[] = {UI_BUF_GAME_GAME_PIPE_UP_BIN, UI_BUF_GAME_GAME_PIPE_DOWN_BIN};

    //新建窗体和背景
    frm = compo_form_create(true);

    //背景图
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_GAME_GAME_BG_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //stop pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_GAME_GAME_START_BIN);
    compo_setid(compo, COMPO_ID_GAME_STOP_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //bird pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_GAME_GAME_BIRD_BIN);
    compo_setid(compo, COMPO_ID_GAME_BIRD_PIC);
    compo_picturebox_cut((compo_picturebox_t *)compo, 0, 3);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, BIRD_X_POS, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //pipe pic
    for(i = 0; i < 4; i++) {
        compo = (component_t *)compo_picturebox_create(frm, pipe_bin_addr[i % 2]);
        compo_setid(compo, COMPO_ID_GAME_PIPE_PIC_START + i);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
    }

    //fail pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_GAME_GAMEOVER_BIN);
    compo_setid(compo, COMPO_ID_GAME_FAIL_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //finish pic
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_GAME_GAME_FINISH_BIN);
    compo_setid(compo, COMPO_ID_GAME_FINISH_PIC);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //stop btn
    compo = (component_t *)compo_button_create(frm);
    compo_setid(compo, COMPO_ID_GAME_STOP_BTN);
    compo_button_set_location((compo_button_t *)compo, 112, 249, 80, 80);
    compo_button_set_visible((compo_button_t *)compo, false);

    //start btn
    compo = (component_t *)compo_button_create(frm);
    compo_setid(compo, COMPO_ID_GAME_START_BTN);
    compo_button_set_location((compo_button_t *)compo, 208, 249, 80, 80);
    compo_button_set_visible((compo_button_t *)compo, false);

    return frm;
}


//游戏初始化
static void func_game_init(void)
{
    f_game_t *game = (f_game_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_GAME_FAIL_PIC);
    if(compo) {
        compo_picturebox_set_visible(compo, false);
    }

    compo = compo_getobj_byid(COMPO_ID_GAME_FINISH_PIC);
    if(compo) {
        compo_picturebox_set_visible(compo, false);
    }

    compo = compo_getobj_byid(COMPO_ID_GAME_STOP_PIC);
    if(compo) {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_GAME_BIRD_PIC);
    if(compo) {
        compo_picturebox_set_visible(compo, false);
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y);
    }

    game->pipe_cut = 1;
    for(i = 0; i < 4; i++) {
        if(!(i % 2)) {
            compo = compo_getobj_byid(COMPO_ID_GAME_PIPE_PIC_START + i);
            if(compo) {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, game->pipe_cut + i/2);
            }
        }
    }
    game->bird_anim_cnt = 0;
    game->speed = 0;
    game->tick = tick_get();
    game->bird_y_pos = GUI_SCREEN_CENTER_Y;
    game->pipe_index = 0;
    game->pipe_x_pos = GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X;
    game->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(game->pipe_cut);
    game->status = GAME_STATUS_WAIT_RUN;
}


//游戏中
static void func_game_runing(void)
{
    f_game_t *game = (f_game_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;
    uint16_t id;
    s16  x_pos, y_pos;
    uint8_t pipe_cut;
    s16 diff;

    if (tick_check_expire(game->tick, 1000 / 24)) //24hz
    {
        //bird pic refresh
        game->tick = tick_get();
        game->bird_y_pos += game->speed;
        game->speed += GAME_DOWN_ACC_V;
        if(game->bird_y_pos >= GUI_SCREEN_HEIGHT) {
            game->status = GAME_STATUS_GAMEOVER;
        }
        compo = compo_getobj_byid(COMPO_ID_GAME_BIRD_PIC);
        if(compo) {
            compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X / 2, game->bird_y_pos);
            compo_picturebox_cut((compo_picturebox_t *)compo, game->bird_anim_cnt, 3);
            game->bird_anim_cnt ++;
            game->bird_anim_cnt %= 3;
        }

        //pipe pic refresh
        x_pos = game->pipe_x_pos;
        y_pos = game->pipe_y_pos;
        pipe_cut = game->pipe_cut;

        for (i = 0; i < 4; i++) {
            id = COMPO_ID_GAME_PIPE_PIC_START + (game->pipe_index + i) % 4;
            compo = compo_getobj_byid(id);
            if(compo) {
                if(i % 2) {
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                }
                if(i == 2) {
                    x_pos += PIPE_X_GAP;
                    pipe_cut = 1 + (pipe_cut + 1) % 3;
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                }
                compo_picturebox_set_pos((compo_picturebox_t *)compo, x_pos, y_pos);
            }
        }
        if(game->pipe_x_pos > (-PIPE_PIC_WIDTH / 2)) {

            //碰撞检测
            if ((game->pipe_x_pos + PIPE_PIC_WIDTH / 2) >= (BIRD_X_POS - BIRD_WIDTH / 2)) {
                diff = (game->pipe_x_pos - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if(diff <= 0) {
                    y_pos = CAL_PIPE_DOWM_Y_POS(game->pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / game->pipe_cut / 2) <= (game->bird_y_pos + BIRD_HEIGHT / 2)) {
                        game->status = GAME_STATUS_GAMEOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, game->pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (game->bird_y_pos - BIRD_HEIGHT / 2)) {
                        game->status = GAME_STATUS_GAMEOVER;
                    }
                }
            }
            else {  //下一管道
                diff = ((game->pipe_x_pos + PIPE_X_GAP) - PIPE_PIC_WIDTH / 2) - (BIRD_X_POS + BIRD_WIDTH / 2);
                if (diff <= 0) {
                    y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
                    if((y_pos - PIPE_PIC_HEIGHT / pipe_cut / 2) <= (game->bird_y_pos + BIRD_HEIGHT / 2)) {
                        game->status = GAME_STATUS_GAMEOVER;
                    }
                    y_pos = CAL_PIPE_UP_Y_POS(y_pos, pipe_cut);
                    if((y_pos + PIPE_PIC_HEIGHT / 2) >= (game->bird_y_pos - BIRD_HEIGHT / 2)) {
                        game->status = GAME_STATUS_GAMEOVER;
                    }
                }
            }

            if(diff > 0 && diff < PIPE_SPEED) {
                game->pipe_x_pos -= diff;
            } else {
                game->pipe_x_pos -= PIPE_SPEED;
            }

        } else {
            //前管道视野消失，位置切换至下一个管道
            game->pipe_x_pos = x_pos;
            game->pipe_y_pos = CAL_PIPE_DOWM_Y_POS(pipe_cut);
            game->pipe_cut = pipe_cut;

            if (game->pipe_index) {
                game->pipe_index = 0;
            } else {
                game->pipe_index = 2;
            }

            id = COMPO_ID_GAME_PIPE_PIC_START + (game->pipe_index + 2) % 4;
            compo = compo_getobj_byid(id);
            if(compo) {
                compo_picturebox_cut((compo_picturebox_t *)compo, 0, 1 + (pipe_cut + 1) % 3);
            }
        }
    }
}


//游戏失败
static void func_game_fail(void)
{
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_GAME_FAIL_PIC);
    if(compo) {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_GAME_BIRD_PIC);
    if(compo) {
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 130);
    }

    for(i = 0; i < 4; i++) {
        compo = compo_getobj_byid(COMPO_ID_GAME_PIPE_PIC_START + i);
        if(compo) {
            compo_picturebox_set_visible(compo, false);
        }
    }
}


//游戏成功
static void func_game_finish(void)
{
    compo_picturebox_t *compo;
    uint8_t i;

    compo = compo_getobj_byid(COMPO_ID_GAME_FINISH_PIC);
    if(compo) {
        compo_picturebox_set_visible(compo, true);
    }

    compo = compo_getobj_byid(COMPO_ID_GAME_BIRD_PIC);
    if(compo) {
        compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 100);
    }

    for(i = 0; i < 4; i++) {
        compo = compo_getobj_byid(COMPO_ID_GAME_PIPE_PIC_START + i);
        if(compo) {
            compo_picturebox_set_visible(compo, false);
        }
    }
}


//游戏过程状态机
static void func_game_process(void)
{
    f_game_t *game = (f_game_t *)func_cb.f_cb;

    switch (game->status)
    {
        case GAME_STATUS_WAIT_RUN:
        case GAME_STATUS_RESULT_WAIT:
            break;

        case GAME_STATUS_STOP:
            func_game_init();
            game->status = GAME_STATUS_WAIT_RUN;
            break;

        case GAME_STATUS_RUNING:
            func_game_runing();
            break;

        case GAME_STATUS_GAMEOVER:
            func_game_fail();
            game->status = GAME_STATUS_RESULT_WAIT;
            break;

        case GAME_STATUS_FINISH:
            func_game_finish();
            game->status = GAME_STATUS_RESULT_WAIT;
            break;

        default:
            break;
    }
}


//单击处理
static void func_game_click(void)
{
    int btn_id = compo_get_button_id();
    f_game_t *game = (f_game_t *)func_cb.f_cb;
    compo_picturebox_t *compo;
    uint8_t i;

    if (GAME_STATUS_WAIT_RUN == game->status) {
        compo = compo_getobj_byid(COMPO_ID_GAME_STOP_PIC);
        if(compo) {
            compo_picturebox_set_visible(compo, false);
        }
        compo = compo_getobj_byid(COMPO_ID_GAME_BIRD_PIC);
        if(compo) {
            compo_picturebox_set_visible(compo, true);
        }

        for(i = 0; i < 4; i++) {
            compo = compo_getobj_byid(i + COMPO_ID_GAME_PIPE_PIC_START);
            if(compo) {
                compo_picturebox_set_visible(compo, true);
            }
        }

        game->status = GAME_STATUS_RUNING;

    } else if (GAME_STATUS_RUNING == game->status) {
        if(game->bird_y_pos <= -BIRD_HEIGHT) {
            game->speed = 0;
        } else {
            if(game->speed > 0) {
                game->speed = -GAME_UP_ACC_V;
            } else {
                game->speed -= GAME_UP_ACC_V;
            }
        }
    } else if (GAME_STATUS_RESULT_WAIT == game->status) {
        if (COMPO_ID_GAME_STOP_BTN == btn_id) {
            game->status = GAME_STATUS_STOP;
        } else if (COMPO_ID_GAME_START_BTN == btn_id) {
            func_game_init();

            compo = compo_getobj_byid(COMPO_ID_GAME_STOP_PIC);
            if(compo) {
                compo_picturebox_set_visible(compo, false);
            }
            compo = compo_getobj_byid(COMPO_ID_GAME_BIRD_PIC);
            if(compo) {
                compo_picturebox_set_visible(compo, true);
            }

            for(i = 0; i < 4; i++) {
                compo = compo_getobj_byid(i + COMPO_ID_GAME_PIPE_PIC_START);
                if(compo) {
                    compo_picturebox_set_visible(compo, true);
                }
            }
			delay_ms(40);
            game->status = GAME_STATUS_RUNING;
        }
    }
}


//消息处理
static void func_game_message(size_msg_t msg)
{
    switch (msg) {
    case MSG_CTP_CLICK:
        func_game_click();
        break;

    case MSG_CTP_SHORT_UP:
    case MSG_CTP_SHORT_DOWN:
        break;

    case MSG_CTP_SHORT_LEFT:
        if (func_cb.flag_sort) {
            func_message(msg);
        }
        break;


    case MSG_CTP_LONG:
        break;

    default:
        func_message(msg);
        break;
    }
}


//进入游戏功能
static void func_game_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_game_t));
    func_cb.frm_main = func_game_form_create();
    f_game_t *game = (f_game_t *)func_cb.f_cb;
    game->status = GAME_STATUS_STOP;
}


//退出游戏功能
static void func_game_exit(void)
{
    func_cb.last = FUNC_GAME;
}


//游戏功能
void func_game(void)
{
    printf("%s\n", __func__);
    func_game_enter();
    while (func_cb.sta == FUNC_GAME) {
        func_game_process();
        func_process();
        func_game_message(msg_dequeue());
    }
    func_game_exit();
}
