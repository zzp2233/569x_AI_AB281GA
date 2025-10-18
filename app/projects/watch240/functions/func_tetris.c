#include "include.h"
#include "func.h"
#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
#define BULLET_NUM  10
#define ENEMY_NUM  5

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

static co_timer_t timer;

#define BLOCK_BEGIN_X                   (18 + TETRIS_BLOCK_SIZE * 4 + TETRIS_BLOCK_SIZE / 2)
#define BLOCK_BEGIN_Y                   (0)
#define LOWER_BEGIN_Y                   (BLOCK_BEGIN_Y + TETRIS_BLOCK_SIZE)
#define HIGHER_BEGIN_Y                  (BLOCK_BEGIN_Y - TETRIS_BLOCK_SIZE)

#define NEXT_BLOCK_BEGIN_X              (285 + TETRIS_BLOCK_SIZE / 2)
#define NEXT_BLOCK_BEGIN_Y              (165 + TETRIS_BLOCK_SIZE / 2)

#define TETRIS_BLOCK_SIZE               23
#define TETRIS_NEXTBLOCK_SIZE           22

//边界
#define BOTTOM_Y                        (368 - TETRIS_BLOCK_SIZE)
#define LEFT_BORDER                     (18 + TETRIS_BLOCK_SIZE)
#define RIGHT_BORDER                    (238 - TETRIS_BLOCK_SIZE)
#define RIGHT_BORDER_ROTATE             (RIGHT_BORDER + TETRIS_BLOCK_SIZE)

//开始按钮的坐标信息
#define START_X                         (GUI_SCREEN_CENTER_X)
#define START_Y                         (GUI_SCREEN_HEIGHT - 70)
#define START_WID                       200
#define START_HEI                       50

#define CLOSE_X                         (GUI_SCREEN_CENTER_X + 115)
#define CLOSE_Y                         (GUI_SCREEN_CENTER_Y - 150)
#define CLOSE_WID                       65
#define CLOSE_HEI                       52

#define LEFT_X                           (20 + LEFT_WID / 2)
#define LEFT_Y                           (381 + LEFT_HEI / 2)
#define LEFT_WID                         80
#define LEFT_HEI                         80

#define DOWN_X                          (104 + DOWN_WID / 2)
#define DOWN_Y                          (381 + DOWN_HEI / 2)
#define DOWN_WID                        80
#define DOWN_HEI                        80

#define ROTATE_X                        (188 + ROTATE_WID / 2)
#define ROTATE_Y                        (381 + ROTATE_HEI / 2)
#define ROTATE_WID                      80
#define ROTATE_HEI                      80

#define RIGHT_X                         (272 + RIGHT_WID / 2)
#define RIGHT_Y                         (381 + RIGHT_HEI / 2)
#define RIGHT_WID                       80
#define RIGHT_HEI                       80

#define RESTART_X                       GUI_SCREEN_CENTER_X
#define RESTART_Y                       (230 + 68 / 2)
#define RESTART_WID                     170
#define RESTART_HEI                     50

#define RETURN_X                        GUI_SCREEN_CENTER_X
#define RETURN_Y                        (304 + 68 / 2)
#define RETURN_WID                      170
#define RETURN_HEI                      50

#define EXIT_CANCEL_X                   (GUI_SCREEN_CENTER_X - 70)
#define EXIT_CANCEL_Y                   (GUI_SCREEN_CENTER_Y + 142)
#define EXIT_CANCEL_WID                 100
#define EXIT_CANCEL_HEI                 60

#define EXIT_RIGHT_X                    (GUI_SCREEN_CENTER_X + 80)
#define EXIT_RIGHT_Y                    (GUI_SCREEN_CENTER_Y + 142)
#define EXIT_RIGHT_WID                  100
#define EXIT_RIGHT_HEI                  60

enum
{
    COMPO_ID_GAME_FRM_PIC = 1,
    COMPO_ID_GAME_BACKGROUND_PIC,
    COMPO_ID_GAME_BACKGROUND_RUN_PIC,
    COMPO_ID_GAME_SUSPEND_BUTTON,
    COMPO_ID_GAME_START_BUTTON,
    COMPO_ID_CARDBOX_TOTAL_SCORE,
    COMPO_ID_CARDBOX_LINE_SCORE,
    COMPO_ID_CARDBOX_MAXSCORE_SCORE,
    COMPO_ID_CARDBOX_GAMEOVER_SCORE,

    COMPO_ID_BTN_NO,
    COMPO_ID_BTN_YES,
    COMPO_ID_BTN_OVER_NO,
    COMPO_ID_BTN_OVER_YES,

    COMPO_ID_BTN_STAR,
    COMP_ID_BLOCK0,
    COMP_ID_BLOCK1 = COMP_ID_BLOCK0 + 3,
};
enum
{
    GAME_STATUS_STOP,
    GAME_STATUS_WAIT_RUN,
    GAME_STATUS_RUNING,
    GAME_STATUS_GAMEOVER,
    GAME_STATUS_FINISH,
    GAME_STATUS_EXIT,
    GAME_STATUS_RESULT_WAIT,
};
//方块颜色
enum
{
    TETRIS_COLOR_YELLOW,
    TETRIS_COLOR_ORANGE,
    TETRIS_COLOR_RED,
    TETRIS_COLOR_PURPLE,
    TETRIS_COLOR_GREEN,
    TETRIS_COLOR_BLUE,
};
//方块形状
enum
{
    TETRIS_SHAPE_Z,
    TETRIS_SHAPE_S,
    TETRIS_SHAPE_L,
    TETRIS_SHAPE_J,
    TETRIS_SHAPE_I,
    TETRIS_SHAPE_T,
    TETRIS_SHAPE_O,
};



typedef struct game_object_t
{
    s16                 x;
    s16                 y;
} oneblock_t;


typedef struct f_tetris_t_
{
    widget_image_t*      gameover_img;
    widget_image_t*      gameexit_img;
    u8                   random_color;
    u8                   random_shape;
    u8                   random_status;
    u8                   cur_random_shape;
    u8                   cur_random_status;
    widget_image_t*      cur_image_block[4];            //下落的方块
    u32                  block_picx;
    widget_image_t*      next_image_block[4];           //显示的下一方块
    //widget_image_t*      recovery_block[300];           //方块回收数组
    oneblock_t           block[4];
    oneblock_t           nextblock[4];
    u32                  nextblock_resaddr;
    u16                  pos_x;
    u16                  pos_y;
    u16                  rotate_x;
    u16                  rotate_y;
    u32                  downtime;                      //方块下落时间tick
    u16                  downtime_interval;             //方块下落速度
    u8                   status;                        //游戏状态
    bool                 isbottom;                      //方块是否到达底部或者是否与其他方块重叠标志位
    bool                 isstart;                       //游戏进行标志位
    int                  map[15][10];                   //地图数组
    widget_image_t*      line_block_address[15][10];    //存放已存在方块的数组
    u32                  line_block_pic[15][10];        //存放数组UI
    int                  next_free_index;               //方块回收index，表示回收数组里存放了几个方块待使用
    bool                 canMove;                       //移动标志位
    u16                  total_score;                   //总分
    u8                   clear_line_count_once;         //一次性消了多少行，不同行分数不同
    u16                  clear_line_score_once;         //每次下落方块或者消行得分
    bool                 isinto_timer;                  //是否进入定时器标志位
    bool                 isfail_clear;                  //失败清除标志位
    bool                 isexit;                        //是否处于退出界面
    bool                 issuspend;                     //是否按下暂停游戏键
    s16 reverse_x[4];                                   //方块下落y方向改变，实际对应到地图坐标轴是x在改变
    s16 reverse_y[4];
    int map_x;                                          //方块x改变对应到map地图
    int map_y;
} f_tetris_t;

enum
{
    GAME_RUNNING_PAGE,
    GAME_COME_BACK_PAGE,
    GAME_SCORE_PAGE,
    GAME_COME_STOP_PAGE,
};
static u8 page_type = 0;
//一维是方块种类，二维是方块旋转角度，三维是一个方块有四个小块 ，四维是每个方块的x，y坐标, 第四维中的第三个成员只是为了让方块产生位置不要超出地图第一列，需要具体调试,第四、五个成员是为了让nextblock的x、y保持在方框内
static int tetris_offsets[7][4][4][5] =
{
    // TETRIS_SHAPE_Z
    {
        {{0, 0, -1, 1, -1}, {1, 0, -1, 1, -1}, {1, 1, -1, 1, -1}, {2, 1, -1, 1, -1}},
        {{1, 0, -2, 7, -9}, {0, 1, -2, 7, -9}, {1, 1, -2, 7, -9}, {0, 2, -2, 7, -9}},
        {{0, 0, -1, 1, -1}, {1, 0, -1, 1, -1}, {1, 1, -1, 1, -1}, {2, 1, -1, 1, -1}},
        {{1, 0, -2, 7, -9}, {0, 1, -2, 7, -9}, {1, 1, -2, 7, -9}, {0, 2, -2, 7, -9}},
    },
    // TETRIS_SHAPE_S
    {
        {{1, 0, -1, 0, -2}, {2, 0, -1, 0, -2}, {0, 1, -1, 0, -2}, {1, 1, -1, 0, -2}},
        {{0, 0, -2, 9, -10}, {0, 1, -2, 9, -10}, {1, 1, -2, 9, -10}, {1, 2, -2, 9, -10}},
        {{1, 0, -1, 0, -2}, {2, 0, -1, 0, -2}, {0, 1, -1, 0, -2}, {1, 1, -1, 0, -2}},
        {{0, 0, -2, 7, -8}, {0, 1, -2, 7, -8}, {1, 1, -2, 7, -8}, {1, 2, -2, 7, -8}},
    },
    // TETRIS_SHAPE_L
    {
        {{0, 0, -2, 9, -10}, {0, 1, -2, 9, -10}, {0, 2, -2, 9, -10}, {1, 2, -2, 9, -10}},
        {{0, 0, -1, 0, 0}, {1, 0, -1, 0, 0}, {2, 0, -1, 0, 0}, {0, 1, -1, 0, 0}},
        {{0, 0, -2, 5, -9}, {1, 0, -2, 5, -9}, {1, 1, -2, 5, -9}, {1, 2, -2, 5, -9}},
        {{2, 0, -1, 0, -5}, {0, 1, -1, 0, -5}, {1, 1, -1, 0, -5}, {2, 1, -1, 0, -5}}
    },
    // TETRIS_SHAPE_J
    {
        {{1, 0, -2, 5, -10}, {1, 1, -2, 5, -10}, {0, 2, -2, 5, -10}, {1, 2, -2, 5, -10}},
        {{0, 0, -1, 2, -2}, {0, 1, -1, 2, -2}, {1, 1, -1, 2, -2}, {2, 1, -1, 2, -2}},
        {{0, 0, -2, 9, -8}, {1, 0, -2, 9, -8}, {0, 1, -2, 9, -8}, {0, 2, -2, 9, -8}},
        {{0, 0, -1, 0, 0}, {1, 0, -1, 0, 0}, {2, 0, -1, 0, 0}, {2, 1, -1, 0, 0}}
    },
    // TETRIS_SHAPE_I
    {
        {{0, 1, -1, -4, -9}, {1, 1, -1, -4, -9}, {2, 1, -1, -4, -9}, {3, 1, -1, -8, -9}},
        {{0, 0, -3, 15, -19}, {0, 1, -3, 15, -19}, {0, 2, -3, 15, -19}, {0, 3, -3, 15, -19}},
        {{0, 1, -1, -4, -9}, {1, 1, -1, -4, -9}, {2, 1, -1, -4, -9}, {3, 1, -1, -8, -9}},
        {{0, 0, -3, 15, -19}, {0, 1, -3, 15, -19}, {0, 2, -3, 15, -19}, {0, 3, -3, 15, -19}},
    },
    // TETRIS_SHAPE_T
    {
        {{0, 0, -1, 0, 0}, {1, 0, -1, 0, 0}, {2, 0, -1, 0, 0}, {1, 1, -1, 0, 0}},
        {{1, 0, -2, 6, -10}, {0, 1, -2, 6, -10}, {1, 1, -2, 6, -10}, {1, 2, -2, 6, -10}},
        {{1, 0, -1, 0, -4}, {0, 1, -1, 0, -4}, {1, 1, -1, 0, -4}, {2, 1, -1, 0, -4}},
        {{0, 0, -2, 11, -10}, {0, 1, -2, 11, -10}, {1, 1, -2, 11, -10}, {0, 2, -2, 11, -10}}
    },
    // TETRIS_SHAPE_O
    {
        {{0, 0, -1, 8, -2}, {1, 0, -1, 8, -2}, {0, 1, -1, 8, -2}, {1, 1, -1, 8, -2}},
        {{0, 0, -1, 8, -2}, {1, 0, -1, 8, -2}, {0, 1, -1, 8, -2}, {1, 1, -1, 8, -2}},
        {{0, 0, -1, 8, -2}, {1, 0, -1, 8, -2}, {0, 1, -1, 8, -2}, {1, 1, -1, 8, -2}},
        {{0, 0, -1, 8, -2}, {1, 0, -1, 8, -2}, {0, 1, -1, 8, -2}, {1, 1, -1, 8, -2}},
    },

};

static int score_max = 0;
int tetris_get_score_max()
{
    return score_max;
}

void  tetris_set_score_max(int score)
{
    score_max = (score_max < score) ? score : score_max;
    uteModuleGuiCommonSaverTetrisMaxScore(score_max);
}

void block_pos_init(f_tetris_t *f_tetris)
{
    u8 shape_index = f_tetris->random_shape;
    u8 status_index = f_tetris->random_status;

    for (int i = 0; i < 4; i++)
    {
        f_tetris->block[i].x = BLOCK_BEGIN_X + tetris_offsets[shape_index][status_index][i][0] * TETRIS_BLOCK_SIZE;
        f_tetris->block[i].y = BLOCK_BEGIN_Y + tetris_offsets[shape_index][status_index][i][1] * TETRIS_BLOCK_SIZE + tetris_offsets[shape_index][status_index][i][2] * TETRIS_BLOCK_SIZE ;
    }//

    for (int i = 0; i < 4; ++i)
    {
        widget_set_location(f_tetris->cur_image_block[i], f_tetris->block[i].x, f_tetris->block[i].y, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE);
    }
}

void block_pos_rotate_update(f_tetris_t *f_tetris)
{
    f_tetris_t f_tetris_temp = {0};
    u8 shape_index = f_tetris->cur_random_shape;
    u8 status_index = f_tetris->cur_random_status;

    // 计算当前状态中最大x值
    int max_x_offset = 0;
    for (int i = 0; i < 4; i++)
    {
        if (tetris_offsets[shape_index][status_index][i][0] > max_x_offset)
        {
            max_x_offset = tetris_offsets[shape_index][status_index][i][0];
        }
    }

    // 检查是否可以旋转
    bool is_rotate_allow = true;
    int required_space = TETRIS_BLOCK_SIZE * max_x_offset;
    if (f_tetris->rotate_x + required_space > RIGHT_BORDER_ROTATE)
    {
        is_rotate_allow = false;
    }

    if (is_rotate_allow)
    {
        for (int i = 0; i < 4; i++)
        {
            f_tetris_temp.block[i].x = f_tetris->rotate_x + tetris_offsets[shape_index][status_index][i][0] * TETRIS_BLOCK_SIZE;
            f_tetris_temp.block[i].y = f_tetris->rotate_y + tetris_offsets[shape_index][status_index][i][1] * TETRIS_BLOCK_SIZE;
            if(f_tetris_temp.block[i].y > BOTTOM_Y)
            {
                is_rotate_allow = false;
                return;
            }
            if (f_tetris->map[(f_tetris_temp.block[i].y/ TETRIS_BLOCK_SIZE)][(f_tetris_temp.block[i].x / TETRIS_BLOCK_SIZE - 1)] == 1)
            {
                TRACE("error !!!\n");
                is_rotate_allow = false;
                return;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        f_tetris->block[i].x = f_tetris_temp.block[i].x;
        f_tetris->block[i].y = f_tetris_temp.block[i].y;
        widget_set_location(f_tetris->cur_image_block[i], f_tetris->block[i].x, f_tetris->block[i].y, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE);
    }
}

void next_block_pos_init(f_tetris_t *f_tetris)
{
    int shape_index = f_tetris->random_shape;
    int status_index = f_tetris->random_status;
    for (int i = 0; i < 4; i++)
    {
        f_tetris->nextblock[i].x = NEXT_BLOCK_BEGIN_X + tetris_offsets[shape_index][status_index][i][0] * (TETRIS_NEXTBLOCK_SIZE - 5) + tetris_offsets[shape_index][status_index][i][3];
        f_tetris->nextblock[i].y = NEXT_BLOCK_BEGIN_Y + tetris_offsets[shape_index][status_index][i][1] * (TETRIS_NEXTBLOCK_SIZE - 5) + tetris_offsets[shape_index][status_index][i][4];
    }

    for (int i = 0; i < 4; ++i)
    {
        widget_set_location(f_tetris->next_image_block[i], f_tetris->nextblock[i].x, f_tetris->nextblock[i].y, (TETRIS_NEXTBLOCK_SIZE - 5), (TETRIS_NEXTBLOCK_SIZE - 5));
    }
}


void block_pos_update(f_tetris_t *f_tetris)
{
    for(int i=0; i<4; i++)
    {
        f_tetris->block[i].x += f_tetris->pos_x;
        f_tetris->block[i].y += f_tetris->pos_y;
        f_tetris->reverse_x[i] = f_tetris->block[i].y;
        f_tetris->reverse_y[i] = f_tetris->block[i].x;

    }
    f_tetris->pos_x = 0;
    f_tetris->pos_y = 0;
}

void block_update(f_tetris_t *f_tetris)
{
    widget_set_location(f_tetris->cur_image_block[0], f_tetris->block[0].x, f_tetris->block[0].y, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE);
    widget_set_location(f_tetris->cur_image_block[1], f_tetris->block[1].x, f_tetris->block[1].y, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE);
    widget_set_location(f_tetris->cur_image_block[2], f_tetris->block[2].x, f_tetris->block[2].y, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE);
    widget_set_location(f_tetris->cur_image_block[3], f_tetris->block[3].x, f_tetris->block[3].y, TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE);
}

int remove_first_four_elements(f_tetris_t *f_tetris)
{
    if (f_tetris->next_free_index < 4)
    {
        return -1;
    }

    for (int i = 0; i < f_tetris->next_free_index - 4; i++)
    {
        //f_tetris->recovery_block[i] = f_tetris->recovery_block[i + 4];
    }

    for (int i = f_tetris->next_free_index - 4; i < f_tetris->next_free_index; i++)
    {
        //f_tetris->recovery_block[i] = NULL;
    }
    f_tetris->next_free_index -= 4;
    return 0;
}
void display_nextblock(f_tetris_t *f_tetris)
{
    f_tetris->random_color = get_random(6) ;
    f_tetris->random_shape = get_random(7) ;
    f_tetris->random_status = get_random(4) ;

    //产生特定的方块，用于调试
    // f_tetris->random_shape = TETRIS_SHAPE_J;
    // f_tetris->random_status = 3;


    if (f_tetris->random_color == TETRIS_COLOR_YELLOW)
    {
        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS00_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_ORANGE)
    {
        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS00_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_RED)
    {
        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS01_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_PURPLE)
    {
        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS02_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_GREEN)
    {
        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS04_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_BLUE)
    {
        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS03_BIN;
    }

    for (int i = 0; i < 4; i++)
    {
        widget_image_set(f_tetris->next_image_block[i], f_tetris->nextblock_resaddr);
    }


    next_block_pos_init(f_tetris);
}

void create_start_block(compo_form_t *frm, f_tetris_t *f_tetris,bool is_creat)
{
    f_tetris->cur_random_shape = f_tetris->random_shape;
    f_tetris->cur_random_status = f_tetris->random_status;
    u32 res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS00_BIN;
    if (f_tetris->random_color == TETRIS_COLOR_YELLOW)
    {
        res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS00_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_ORANGE)
    {
        res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS00_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_RED)
    {
        res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS01_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_PURPLE)
    {
        res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS02_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_GREEN)
    {
        res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS04_BIN;
    }
    else if (f_tetris->random_color == TETRIS_COLOR_BLUE)
    {
        res_addr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS03_BIN;
    }
    f_tetris->block_picx = res_addr;
    if(f_tetris->next_free_index <= 4)
    {
        for(int i=0; i<4; i++)
        {
            compo_picturebox_t *pic = NULL;
            if(is_creat)
            {
                pic = compo_picturebox_create(frm, res_addr);
                compo_setid(pic, COMP_ID_BLOCK0 + i);
            }
            else
            {
                pic = compo_getobj_byid(COMP_ID_BLOCK0 + i);
                widget_image_set(pic->img,res_addr);
            }
            widget_set_visible(pic->img, true);
            f_tetris->cur_image_block[i] = pic->img;
            widget_set_visible(f_tetris->cur_image_block[i], true);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            // widget_image_set(f_tetris->recovery_block[i], res_addr);
            // f_tetris->cur_image_block[i] = f_tetris->recovery_block[i];
        }
        //remove_first_four_elements(f_tetris);
    }


    f_tetris->downtime_interval = 1000;
    f_tetris->rotate_x = BLOCK_BEGIN_X;
    f_tetris->rotate_y = BLOCK_BEGIN_Y;

    f_tetris->isstart = 1;

    block_pos_init(f_tetris);
}
static compo_form_t* __this = NULL;
static widget_page_t* widget_page = NULL;
compo_form_t *func_tetris_form_create(void)
{
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_setid(frm, COMPO_ID_GAME_FRM_PIC);
    component_t *compo;
    compo_cardbox_t *cardbox;
    char str_buff[25];


    //背景图
    widget_image_t* bg = widget_image_create(frm->page_body, UI_BUF_I341001_17_GAME_TETRIS_BG_BIN);
    widget_set_location(bg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 368, 448);


    f_tetris->random_color = get_random(6) ;   //0 - 5
    f_tetris->random_shape = get_random(7) ;   //0 - 6
    f_tetris->random_status = get_random(4) ;   //0 - 3
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int x = 18 + TETRIS_NEXTBLOCK_SIZE / 2 + TETRIS_BLOCK_SIZE * j;
            int y = BLOCK_BEGIN_Y + TETRIS_BLOCK_SIZE + TETRIS_BLOCK_SIZE * i;
            f_tetris->line_block_address[i][j] = widget_image_create(frm->page_body,NULL);
            widget_set_pos(f_tetris->line_block_address[i][j],x,y);
        }
    }
    create_start_block(frm, f_tetris,true);

    widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    //开始背景图
    bg = widget_image_create(widget_page, UI_BUF_I341001_17_GAME_TETRIS_BG_TOP_BIN);
    widget_set_location(bg, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 368, 448);

    //suspend button
    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_I341001_17_GAME_TETRIS_SUSPEND_BIN);
    compo_setid(compo, COMPO_ID_GAME_SUSPEND_BUTTON);
    compo_button_set_pos((compo_button_t *)compo, 263 + 80 / 2, 78 + 60 / 2);
    compo_button_set_visible((compo_button_t *)compo, false);

    //START button
    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_I341001_17_GAME_TETRIS_START_BIN);
    compo_setid(compo, COMPO_ID_GAME_START_BUTTON);
    compo_button_set_pos((compo_button_t *)compo, 263 + 80 / 2, 78 + 60 / 2);
    compo_button_set_visible((compo_button_t *)compo, false);

    for(int i=0; i<4; i++)
    {
        widget_set_visible(f_tetris->cur_image_block[i], false);

        f_tetris->nextblock_resaddr = UI_BUF_I341001_17_GAME_TETRIS_BLOCKS00_BIN;
        widget_image_t *img = widget_image_create(frm->page_body, f_tetris->nextblock_resaddr);
        widget_set_visible(img, false);
        f_tetris->next_image_block[i] = img;
    }

    display_nextblock(f_tetris);

    //总分数
    cardbox = compo_cardbox_create(frm, 0, 2, 4, 140, 50);
    compo_setid(cardbox, COMPO_ID_CARDBOX_TOTAL_SCORE);
    memset(str_buff,0,sizeof(str_buff));
    snprintf(str_buff,sizeof(str_buff),"%d",f_tetris->total_score);
    compo_cardbox_text_set(cardbox, 0, str_buff);
    compo_cardbox_text_set_location(cardbox, 0, 0, -7, 140, 50);
    compo_cardbox_set_pos(cardbox, 262 + 100 / 2, 235 + 66 / 2);
    compo_cardbox_set_visible(cardbox,1);

    //消行加分
    f_tetris->clear_line_score_once = 0;
    cardbox = compo_cardbox_create(frm, 0, 2, 4, 130, 50);
    compo_setid(cardbox, COMPO_ID_CARDBOX_LINE_SCORE);
    memset(str_buff,0,sizeof(str_buff));
    snprintf(str_buff,sizeof(str_buff),"+%d",f_tetris->clear_line_score_once);
    compo_cardbox_text_set(cardbox, 0, str_buff);
    compo_cardbox_text_set_location(cardbox, 0, 0, -10, 80, 46);
    compo_cardbox_set_pos(cardbox, 262 + 100 / 2, 309 + 66 / 2);
    compo_cardbox_set_visible(cardbox,1);

    //创建游戏结束背景
    f_tetris->gameover_img = widget_image_create(frm->page_body, UI_BUF_I341001_17_GAME_TETRIS_BG1_BIN);
    widget_set_pos(f_tetris->gameover_img, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    widget_set_visible(f_tetris->gameover_img, false);

    compo_picturebox_t* pic = compo_picturebox_create(frm,UI_BUF_I341001_17_GAME_TETRIS_PAUSE_BIN);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,230 + 68 / 2);
    compo_picturebox_set_visible(pic,false);
    compo_setid(pic,COMPO_ID_BTN_OVER_NO);

    pic = compo_picturebox_create(frm,UI_BUF_I341001_17_GAME_TETRIS_EXIT_BIN);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,304 + 68 / 2);
    compo_picturebox_set_visible(pic,false);
    compo_setid(pic,COMPO_ID_BTN_OVER_YES);


    //结束界面当前得分
    cardbox = compo_cardbox_create(frm, 0, 0, 2, 250, 100);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 30,200, 80);
    compo_setid(cardbox, COMPO_ID_CARDBOX_GAMEOVER_SCORE);
    memset(str_buff,0,sizeof(str_buff));
    snprintf(str_buff,sizeof(str_buff),"%d%s",f_tetris->total_score,i18n[STR_POINT]);
    compo_cardbox_text_set(cardbox, 0, str_buff);
    compo_cardbox_text_set_location(cardbox, 0, 0, 15, 250, 60);
    widget_text_set_color(cardbox->text[0], COLOR_RED);

    compo_cardbox_text_set(cardbox, 1, i18n[STR_SCORE]);
    compo_cardbox_text_set_location(cardbox, 1, 0, - 20, 250, 60);
    widget_text_set_color(cardbox->text[1], make_color(0x2d,0x9b,0xf6));

    compo_cardbox_set_visible(cardbox,0);
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

static compo_form_t *func_tetris_stop_form_create(void)
{
    char str_buff[25];
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t * shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,GUI_SCREEN_WIDTH,GUI_SCREEN_HEIGHT,25,0x80,make_color(0x00,0x00,0x00));

    memset(str_buff,0,sizeof(str_buff));
    snprintf(str_buff,sizeof(str_buff),"%s %d %s",i18n[STR_PERSONAL_BEST],tetris_get_score_max(),i18n[STR_POINT]);

    compo_textbox_t* textbox = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,330 - 30,GUI_SCREEN_WIDTH - 20,46);
    compo_textbox_set(textbox,str_buff);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_17_GAME_TETRIS_PAUSE_BIN);
    compo_setid(btn, COMPO_ID_BTN_STAR);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 347+80/2);

    page_type = GAME_COME_STOP_PAGE;
    return frm;
}

static void func_tetris_init(void)
{
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    compo_button_t *compo_suspend_button = compo_getobj_byid(COMPO_ID_GAME_SUSPEND_BUTTON);
    compo_button_t *compo_start_button = compo_getobj_byid(COMPO_ID_GAME_START_BUTTON);
    compo_cardbox_t *compo_total_score = compo_getobj_byid(COMPO_ID_CARDBOX_TOTAL_SCORE);
    compo_cardbox_t *compo_line_score = compo_getobj_byid(COMPO_ID_CARDBOX_LINE_SCORE);
    compo_cardbox_t *compo_gameover_score = compo_getobj_byid(COMPO_ID_CARDBOX_GAMEOVER_SCORE);
    compo_picturebox_t* icon_cancle = compo_getobj_byid(COMPO_ID_BTN_OVER_NO);
    compo_picturebox_t* icon_exit = compo_getobj_byid(COMPO_ID_BTN_OVER_YES);
    char str_buff[25];
    switch(f_tetris->status)
    {
        case GAME_STATUS_RESULT_WAIT:
        {
            widget_set_visible(f_tetris->gameover_img, true);
            compo_picturebox_set_visible(icon_cancle, true);
            compo_picturebox_set_visible(icon_exit, true);
            //游戏结束，页面切换，方块回收
            memset(str_buff,0,sizeof(str_buff));
            snprintf(str_buff,sizeof(str_buff),"%d%s",f_tetris->total_score,i18n[STR_POINT]);
            compo_cardbox_text_set(compo_gameover_score, 0, str_buff);
            compo_cardbox_set_visible(compo_gameover_score, 1);
            if(f_tetris->isfail_clear == 0)
            {
                f_tetris->isfail_clear = 1;
                for(int i=0; i<15; i++)
                {
                    for(int j=0; j<10; j++)
                    {
                        f_tetris->line_block_pic[i][j] = NULL;
                        widget_image_set(f_tetris->line_block_address[i][j],f_tetris->line_block_pic[i][j]);

                    }
                }
                for(int i=0; i<4; i++)
                {
                    widget_set_visible(f_tetris->cur_image_block[i], FALSE);
                }
            }

            if(tetris_get_score_max() < f_tetris->total_score)
            {
                tetris_set_score_max(f_tetris->total_score);
            }
            sprintf(str_buff,"%s %d",i18n[STR_HIGHT_RECORD],tetris_get_score_max());

        }
        break;
    }
}

static void score_calculate_process(co_timer_t *timer, void *param)
{
    compo_cardbox_t *compo_total_score = compo_getobj_byid(COMPO_ID_CARDBOX_TOTAL_SCORE);
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    char str_buff[16];
    f_tetris->clear_line_count_once = 0;
    co_timer_enable(timer, 0);
    f_tetris->isinto_timer = 0;
    f_tetris->total_score += f_tetris->clear_line_score_once;
    sprintf(str_buff,"%d",f_tetris->total_score);
    compo_cardbox_text_set(compo_total_score, 0, str_buff);


}

static void func_tetris_process(void)
{
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    compo_form_t *frm = compo_getobj_byid(COMPO_ID_GAME_FRM_PIC);
    compo_button_t *compo_suspend_button = compo_getobj_byid(COMPO_ID_GAME_SUSPEND_BUTTON);
    compo_button_t *compo_start_button = compo_getobj_byid(COMPO_ID_GAME_START_BUTTON);
    compo_cardbox_t *compo_total_score = compo_getobj_byid(COMPO_ID_CARDBOX_TOTAL_SCORE);
    compo_cardbox_t *compo_line_score = compo_getobj_byid(COMPO_ID_CARDBOX_LINE_SCORE);
    compo_cardbox_t *compo_gameover_score = compo_getobj_byid(COMPO_ID_CARDBOX_GAMEOVER_SCORE);
    compo_picturebox_t* icon_cancle = compo_getobj_byid(COMPO_ID_BTN_OVER_NO);
    compo_picturebox_t* icon_exit = compo_getobj_byid(COMPO_ID_BTN_OVER_YES);
    char str_buff[25];


    if(f_tetris->issuspend == 0)
    {
        compo_button_set_visible(compo_suspend_button, true);
    }
    else
    {
        compo_button_set_visible(compo_start_button, true);
    }

    if(f_tetris->isstart && f_tetris->issuspend == 0)
    {
        if (f_tetris->isbottom == 0)
        {
            if (tick_check_expire(f_tetris->downtime, f_tetris->downtime_interval))
            {
                f_tetris->downtime = tick_get();

                f_tetris->canMove = 1;

                for (int i = 0; i < 4; i++)
                {
                    f_tetris->reverse_x[i] = f_tetris->block[i].y;
                    f_tetris->reverse_y[i] = f_tetris->block[i].x;
                }
                for (int i = 0; i < 4; i++)
                {
                    //这里|| 后面的不是固定的，随着屏幕大小不同而不同，需要实际调试
                    char temp_x = ((f_tetris->reverse_x[i])/ TETRIS_BLOCK_SIZE);
                    temp_x = (temp_x > 0) ? temp_x : 0;
                    char temp_y = ((f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE) / TETRIS_BLOCK_SIZE);
                    temp_y = (temp_y > 0) ? temp_y : 0;
                    if (f_tetris->block[i].y  >= BOTTOM_Y || f_tetris->map[temp_x][temp_y] == 1)
                    {
                        f_tetris->canMove = 0;
                        break;
                    }
                }

                if (f_tetris->canMove)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        f_tetris->block[i].y += TETRIS_BLOCK_SIZE;
                    }
                    f_tetris->rotate_y += TETRIS_BLOCK_SIZE;
                    block_update(f_tetris);
                }
                else
                {
                    f_tetris->isbottom = 1;
                    for (int i = 0; i < 4; i++)
                    {
                        f_tetris->map_x = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE > 14 ? 14 : (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;
                        f_tetris->map_y = f_tetris->reverse_y[i] / TETRIS_BLOCK_SIZE - 1;    //后面这个-1不是固定的，随着不同屏幕尺寸大小图像大小不一样会导致不同，需要实际调试
                        if(f_tetris->map_x  >= 0 && f_tetris->map_y >= 0)
                        {
                            f_tetris->map[f_tetris->map_x][f_tetris->map_y] = 1;
                            f_tetris->line_block_pic[f_tetris->map_x][f_tetris->map_y] = f_tetris->block_picx;
                            widget_image_set(f_tetris->line_block_address[f_tetris->map_x ][f_tetris->map_y],f_tetris->line_block_pic[f_tetris->map_x][f_tetris->map_y]);
                            widget_set_visible(f_tetris->line_block_address[f_tetris->map_x ][f_tetris->map_y], true);
                        }
                    }
                }

                // 遍历二维数组
                for (int i = 0; i < 15; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        TRACE("%d\t", f_tetris->map[i][j]);
                    }
                    TRACE("\n");
                }
                TRACE("\n");
                sprintf(str_buff,"+0");
                compo_cardbox_text_set(compo_line_score, 0, str_buff);
            }

        }
        else
        {
            for (int col = 0; col < 10; col++)
            {
                if (f_tetris->map[0][col] == 1)
                {
                    TRACE("game over!\n");
                    widget_set_visible(f_tetris->gameover_img, true);
                    compo_picturebox_set_visible(icon_cancle, true);
                    compo_picturebox_set_visible(icon_exit, true);
                    //游戏结束，页面切换，方块回收
                    memset(str_buff,0,sizeof(str_buff));
                    snprintf(str_buff,sizeof(str_buff),"%d%s",f_tetris->total_score,i18n[STR_POINT]);
                    compo_cardbox_text_set(compo_gameover_score, 0, str_buff);
                    compo_cardbox_set_visible(compo_gameover_score, 1);
                    if(f_tetris->isfail_clear == 0)
                    {
                        f_tetris->isfail_clear = 1;
                        for(int i=0; i<15; i++)
                        {
                            for(int j=0; j<10; j++)
                            {
                                f_tetris->line_block_pic[i][j] = NULL;
                                widget_image_set(f_tetris->line_block_address[i][j],f_tetris->line_block_pic[i][j]);

                            }
                        }
                        for(int i=0; i<4; i++)
                        {
                            widget_set_visible(f_tetris->cur_image_block[i], FALSE);
                        }
                    }

                    if(tetris_get_score_max()< f_tetris->total_score)
                    {
                        tetris_set_score_max(f_tetris->total_score);
                    }
                    sprintf(str_buff,"%s %d",i18n[STR_HIGHT_RECORD],tetris_get_score_max());

                    f_tetris->status = GAME_STATUS_RESULT_WAIT;

                    break;  //只要有一列里面的第0行有方块就直接结束
                }
            }
            if(f_tetris->status == GAME_STATUS_RUNING)
            {
                f_tetris->clear_line_score_once = 10;
                sprintf(str_buff,"+%d",f_tetris->clear_line_score_once);
                compo_cardbox_text_set(compo_line_score, 0, str_buff);
                f_tetris->total_score = f_tetris->total_score + f_tetris->clear_line_score_once;
                sprintf(str_buff,"%d",f_tetris->total_score);
                compo_cardbox_text_set(compo_total_score, 0, str_buff);
                // 到达底部产生新的block
                create_start_block(frm, f_tetris,false);
                f_tetris->isbottom = 0;
                display_nextblock(f_tetris);
            }
        }

//消行检测之判断哪一行满了
        for (int i = 14; i >= 0; i--)
        {
            int allOnes = 1;
            for (int j = 0; j < 10; j++)
            {
                if (f_tetris->map[i][j] != 1)
                {
                    allOnes = 0;
                    break;
                }

            }
            if (allOnes)
            {
                f_tetris->clear_line_count_once++;
                for(int j=0; j<10; j++)
                {
                    f_tetris->line_block_pic[i][j] = NULL;
                    f_tetris->map[i][j] = 0;
                    widget_image_set(f_tetris->line_block_address[i][j],f_tetris->line_block_pic[i][j]);
                }
                for(int t=i-1; t>=0; t--)
                {
                    for(int y=0; y<10; y++)
                    {
                        f_tetris->line_block_pic[t + 1][y] = NULL;
                        f_tetris->map[t + 1][y] = 0;
                        f_tetris->line_block_pic[t + 1][y] = f_tetris->line_block_pic[t][y];
                        if(f_tetris->line_block_pic[t + 1][y] != NULL)
                        {
                            f_tetris->map[t + 1][y] = 1;
                        }
                        widget_image_set(f_tetris->line_block_address[t + 1][y],f_tetris->line_block_pic[t][y]);

                    }
                }
            }
            //消行加分
            if(i == 0 && f_tetris->clear_line_count_once)
            {
                if(f_tetris->clear_line_count_once == 1)
                    f_tetris->clear_line_score_once = 100;
                else if(f_tetris->clear_line_count_once == 2)
                    f_tetris->clear_line_score_once = 200;
                else if(f_tetris->clear_line_count_once == 3)
                    f_tetris->clear_line_score_once = 300;
                else if(f_tetris->clear_line_count_once == 4)
                    f_tetris->clear_line_score_once = 400;
                sprintf(str_buff,"+%d",f_tetris->clear_line_score_once);
                compo_cardbox_text_set(compo_line_score, 0, str_buff);

                if(f_tetris->isinto_timer == 0)
                {
                    f_tetris->isinto_timer = 1;
                    co_timer_enable(&timer, 1);
                }

            }
        }


    }


}

//游戏过程状态机
static void func_tetris_game_process(void)
{
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
    if(GAME_STATUS_RUNING == f_tetris->status)
    {
        reset_sleep_delay_all();
        func_tetris_process();
    }

}

static void func_tetris_click(void)
{
    int btn_id = compo_get_button_id();
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    compo_button_t *compo_suspend_button = compo_getobj_byid(COMPO_ID_GAME_SUSPEND_BUTTON);
    compo_button_t *compo_start_button = compo_getobj_byid(COMPO_ID_GAME_START_BUTTON);
    compo_cardbox_t *compo_total_score = compo_getobj_byid(COMPO_ID_CARDBOX_TOTAL_SCORE);
    compo_cardbox_t *compo_line_score = compo_getobj_byid(COMPO_ID_CARDBOX_LINE_SCORE);
    compo_cardbox_t *compo_gameover_score = compo_getobj_byid(COMPO_ID_CARDBOX_GAMEOVER_SCORE);
    compo_picturebox_t* icon_cancle = compo_getobj_byid(COMPO_ID_BTN_OVER_NO);
    compo_picturebox_t* icon_exit = compo_getobj_byid(COMPO_ID_BTN_OVER_YES);
    char str_buff[25];

    point_t pt;

    int x_left[4],y_left[4];
    int x_right[4],y_right[4];
    int x_rotate[4],y_rotate[4];
    int x_down[4],y_down[4];
    for (int i = 0; i < 4; i++)
    {
        f_tetris->reverse_x[i] = f_tetris->block[i].y;
        f_tetris->reverse_y[i] = f_tetris->block[i].x;

        x_left[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;
        y_left[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE - 1 ;

        x_right[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;
        y_right[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE + 1;

        x_rotate[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE - 1;
        y_rotate[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;

        x_down[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE + 1;
        y_down[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;

        if(x_left[i] < 0) x_left[i] = 0;
        if(y_left[i] < 0) y_left[i] = 0;
        if(x_right[i] < 0) x_right[i] = 0;
        if(y_right[i] < 0) y_right[i] = 0;
        if(x_rotate[i] < 0) x_rotate[i] = 0;
        if(y_rotate[i] < 0) y_rotate[i] = 0;
        if(x_down[i] < 0) x_down[i] = 0;
        if(y_down[i] < 0) y_down[i] = 0;

    }

    pt = ctp_get_sxy();

    if (GAME_STATUS_WAIT_RUN == f_tetris->status)
    {
        if (abs_s(pt.x - START_X) * 2 <= START_WID && abs_s(pt.y - START_Y) * 2 <= START_HEI)
        {
            compo_cardbox_set_visible(compo_gameover_score, 0);

            f_tetris->status = GAME_STATUS_RUNING;
            f_tetris->isstart = 1;
            for(int i=0; i<4; i++)
            {
                widget_set_visible(f_tetris->cur_image_block[i], true);
                widget_set_visible(f_tetris->next_image_block[i], true);
            }
            compo_cardbox_set_visible(compo_total_score, 1);
            compo_cardbox_set_visible(compo_line_score, 1);

            f_tetris->downtime_interval = 1000;
        }
    }
    else if (GAME_STATUS_FINISH == f_tetris->status)
    {
        if (abs_s(pt.x - START_X) * 2 <= START_WID && abs_s(pt.y - START_Y) * 2 <= START_HEI)
        {
            compo_cardbox_set_visible(compo_gameover_score, 0);

            f_tetris->status = GAME_STATUS_RUNING;
            f_tetris->isstart = 1;


            for(int i=0; i<4; i++)
            {
                widget_set_visible(f_tetris->next_image_block[i], true);
            }
            compo_cardbox_set_visible(compo_total_score, 1);
            compo_cardbox_set_visible(compo_line_score, 1);

        }
    }
    else if(GAME_STATUS_RESULT_WAIT == f_tetris->status)//
    {
        if (abs_s(pt.x - RESTART_X) * 2 <= RESTART_WID && abs_s(pt.y - RESTART_Y) * 2 <= RESTART_HEI)
        {
            f_tetris->isfail_clear = 0;
            widget_set_visible(f_tetris->gameover_img, 0);
            compo_picturebox_set_visible(icon_cancle, false);
            compo_picturebox_set_visible(icon_exit, false);
            compo_cardbox_set_visible(compo_gameover_score, 0);
            f_tetris->status = GAME_STATUS_RUNING;
            for(int i=0; i<15; i++)
            {
                for(int j=0; j<10; j++)
                {
                    f_tetris->line_block_pic[i][j] = NULL;
                    widget_image_set(f_tetris->line_block_address[i][j],f_tetris->line_block_pic[i][j]);
                    f_tetris->map[i][j] = 0;
                }
            }
            f_tetris->total_score = -10;
            compo_cardbox_text_set(compo_total_score, 0, str_buff);

        }
        else if (abs_s(pt.x - RETURN_X) * 2 <= RETURN_WID && abs_s(pt.y - RETURN_Y) * 2 <= RETURN_HEI)
        {
            func_back_to();
        }

    }
    else if(GAME_STATUS_RUNING == f_tetris->status)
    {
        if(COMPO_ID_GAME_SUSPEND_BUTTON == btn_id && f_tetris->isexit == 0)
        {
            f_tetris->isstart = 0;
            f_tetris->issuspend = 1;
            compo_button_set_visible(compo_suspend_button, false);
            compo_button_set_visible(compo_start_button, true);
            func_cb.frm_main = func_tetris_stop_form_create();
        }
        else if(COMPO_ID_GAME_START_BUTTON == btn_id && f_tetris->isexit == 0)
        {
            f_tetris->isstart = 1;
            f_tetris->issuspend = 0;
            compo_button_set_visible(compo_suspend_button, true);
            compo_button_set_visible(compo_start_button, false);
        }
        else if (abs_s(pt.x - (16 + 238 / 2)) * 2 <= 238 && abs_s(pt.y - (8 + 364 / 2)) * 2 <= 364)
        {
            if(f_tetris->isstart)
            {
                bool canMove = true;


                for (int i = 0; i < 4; i++)
                {
                    if ((f_tetris->map[x_rotate[i]][y_rotate[i]] == 1) || (f_tetris->rotate_x + TETRIS_BLOCK_SIZE < LEFT_BORDER) || (f_tetris->rotate_x + TETRIS_BLOCK_SIZE > RIGHT_BORDER))
                    {
                        canMove = false;
                        break;
                    }
                }

                if (canMove)
                {
                    f_tetris->cur_random_status += 1;
                    if(f_tetris->cur_random_status > 3)
                        f_tetris->cur_random_status = 0;
                    block_pos_rotate_update(f_tetris);
                }
            }
        }
        else if (abs_s(pt.x - CLOSE_X) * 2 <= CLOSE_WID * 2 && abs_s(pt.y - CLOSE_Y) * 2 <= CLOSE_HEI * 2)
        {
            f_tetris->isstart = 0;
            f_tetris->issuspend = 1;
            compo_button_set_visible(compo_suspend_button, false);
            compo_button_set_visible(compo_start_button, true);
            func_cb.frm_main = func_tetris_come_back_form_create();
            return;

        }
        else if (abs_s(pt.x - EXIT_CANCEL_X) * 2 <= EXIT_CANCEL_WID && abs_s(pt.y - EXIT_CANCEL_Y) * 2 <= EXIT_CANCEL_HEI && f_tetris->isexit == 1)
        {
            f_tetris->isstart = 1;
            f_tetris->isexit = 0;
            widget_set_visible(f_tetris->gameexit_img, 0);
            widget_set_visible(f_tetris->gameover_img, 0);
            compo_picturebox_set_visible(icon_cancle, false);
            compo_picturebox_set_visible(icon_exit, false);
            for(int i=0; i<15; i++)
            {
                for(int j=0; j<10; j++)
                {
                    if(f_tetris->map[i][j] != 0)
                    {
                    }

                }
            }
            for(int i=0; i<4; i++)
            {
                widget_set_visible(f_tetris->cur_image_block[i], true);
            }

        }
        else if (abs_s(pt.x - EXIT_RIGHT_X) * 2 <= EXIT_RIGHT_WID && abs_s(pt.y - EXIT_RIGHT_Y) * 2 <= EXIT_RIGHT_HEI && f_tetris->isexit == 1)
        {
            f_tetris->isexit = 0;
            f_tetris->status = GAME_STATUS_WAIT_RUN;
            f_tetris->issuspend = 0;
            f_tetris->total_score = 0;
            sprintf(str_buff,"%d",f_tetris->total_score);
            compo_cardbox_text_set(compo_total_score, 0, str_buff);

            widget_set_visible(f_tetris->gameexit_img, 0);
            compo_button_set_visible(compo_suspend_button, false);
            compo_button_set_visible(compo_start_button, false);
            compo_cardbox_set_visible(compo_total_score, false);
            compo_cardbox_set_visible(compo_line_score, false);
            if(tetris_get_score_max()  < f_tetris->total_score)
            {
                tetris_set_score_max(f_tetris->total_score);
            }

            for(int i=0; i<15; i++)
            {
                for(int j=0; j<10; j++)
                {
                    if(f_tetris->map[i][j] != 0)
                    {
                        //if (f_tetris->recovery_block[f_tetris->next_free_index] == NULL)
                        //{
                        //    f_tetris->recovery_block[f_tetris->next_free_index] = f_tetris->line_block_address[i][j];
                        //     f_tetris->next_free_index++;
                        // }
                        f_tetris->line_block_pic[i][j] = NULL;
                        widget_image_set(f_tetris->line_block_address[i][j],f_tetris->line_block_pic[i][j]);
                        f_tetris->map[i][j] = 0;
                    }

                }
            }
            for(int i=0; i<4; i++)
            {
                widget_set_visible(f_tetris->next_image_block[i], false);
            }
            for(int i=0; i<4; i++)
            {
                block_pos_init(f_tetris);
                f_tetris->rotate_x = BLOCK_BEGIN_X;
                f_tetris->rotate_y = BLOCK_BEGIN_Y;
                block_pos_rotate_update(f_tetris);
            }

        }
        else if (abs_s(pt.x - LEFT_X) * 2 <= LEFT_WID && abs_s(pt.y - LEFT_Y) * 2 <= LEFT_HEI && f_tetris->isexit == 0 && f_tetris->issuspend == 0)
        {
            if (f_tetris->isstart)
            {
                bool canMove = true;

                for (int i = 0; i < 4; i++)
                {
                    if ((f_tetris->block[i].y > BOTTOM_Y + TETRIS_BLOCK_SIZE || f_tetris->block[i].x < LEFT_BORDER || f_tetris->map[x_left[i]][y_left[i]] == 1))
                    {
                        canMove = false;
                        break;
                    }
                }

                if (canMove)
                {
                    f_tetris->pos_x -= TETRIS_BLOCK_SIZE;
                    f_tetris->rotate_x -= TETRIS_BLOCK_SIZE;
                    block_pos_update(f_tetris);
                    block_update(f_tetris);
                }
            }
        }
        else if (abs_s(pt.x - DOWN_X) * 2 <= DOWN_WID && abs_s(pt.y - DOWN_Y) * 2 <= DOWN_HEI && f_tetris->isexit == 0 && f_tetris->issuspend == 0)
        {
            if(f_tetris->isstart)
            {
                bool canMove = true;


                for (int i = 0; i < 4; i++)
                {
                    if ((f_tetris->map[x_down[i]][y_down[i]] == 1))
                    {
                        canMove = false;
                        break;
                    }
                }

                if (canMove)
                {
                    f_tetris->downtime_interval = 100;
                }
            }

        }
        else if (abs_s(pt.x - ROTATE_X) * 2 <= ROTATE_WID && abs_s(pt.y - ROTATE_Y) * 2 <= ROTATE_HEI && f_tetris->isexit == 0 && f_tetris->issuspend == 0)
        {
            if(f_tetris->isstart)
            {
                bool canMove = true;


                for (int i = 0; i < 4; i++)
                {
                    if ((f_tetris->map[x_rotate[i]][y_rotate[i]] == 1) || (f_tetris->rotate_x + TETRIS_BLOCK_SIZE < LEFT_BORDER) || (f_tetris->rotate_x + TETRIS_BLOCK_SIZE > RIGHT_BORDER))
                    {
                        canMove = false;
                        break;
                    }
                }

                if (canMove)
                {
                    f_tetris->cur_random_status += 1;
                    if(f_tetris->cur_random_status > 3)
                        f_tetris->cur_random_status = 0;
                    block_pos_rotate_update(f_tetris);
                }
            }
        }
        else if (abs_s(pt.x - RIGHT_X) * 2 <= RIGHT_WID && abs_s(pt.y - RIGHT_Y) * 2 <= RIGHT_HEI && f_tetris->isexit == 0 && f_tetris->issuspend == 0)
        {
            if(f_tetris->isstart)
            {
                bool canMove = true;

                for (int i = 0; i < 4; i++)
                {
                    if ((f_tetris->block[i].y > BOTTOM_Y + TETRIS_BLOCK_SIZE || f_tetris->block[i].x > RIGHT_BORDER || f_tetris->map[x_right[i]][y_right[i]] == 1))
                    {
                        canMove = false;
                        break;
                    }
                }

                if (canMove)
                {
                    f_tetris->pos_x += TETRIS_BLOCK_SIZE;
                    f_tetris->rotate_x += TETRIS_BLOCK_SIZE;
                    block_pos_update(f_tetris);
                    block_update(f_tetris);
                }

            }

        }

    }
}
static void func_tetris_come_back_click(void)
{
    compo_button_t *compo_suspend_button = compo_getobj_byid(COMPO_ID_GAME_SUSPEND_BUTTON);
    compo_button_t *compo_start_button = compo_getobj_byid(COMPO_ID_GAME_START_BUTTON);
    int btn_id = compo_get_button_id();
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    switch(btn_id)
    {
        case COMPO_ID_BTN_NO:
            compo_form_destroy(func_cb.frm_main);
            page_type = GAME_RUNNING_PAGE;
            func_cb.frm_main = __this;
            f_tetris->isstart = 1;
            f_tetris->issuspend = 0;
            compo_button_set_visible(compo_suspend_button, true);
            compo_button_set_visible(compo_start_button, false);
            break;
        case COMPO_ID_BTN_YES:
            compo_form_destroy(func_cb.frm_main);
            func_cb.frm_main = __this;
            func_back_to();
            break;
    }
}

static void func_tetris_message(size_msg_t msg)
{
    compo_button_t *compo_suspend_button = compo_getobj_byid(COMPO_ID_GAME_SUSPEND_BUTTON);
    compo_button_t *compo_start_button = compo_getobj_byid(COMPO_ID_GAME_START_BUTTON);
    int btn_id = compo_get_button_id();
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    int x_left[4],y_left[4];
    int x_right[4],y_right[4];
    int x_rotate[4],y_rotate[4];
    int x_down[4],y_down[4];
    for (int i = 0; i < 4; i++)
    {
        f_tetris->reverse_x[i] = f_tetris->block[i].y;
        f_tetris->reverse_y[i] = f_tetris->block[i].x;

        x_left[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;
        y_left[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE - 1 ;

        x_right[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;
        y_right[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE + 1;

        x_rotate[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE - 1;
        y_rotate[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;

        x_down[i] = (f_tetris->reverse_x[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE + 1;
        y_down[i] = (f_tetris->reverse_y[i] - TETRIS_BLOCK_SIZE)  / TETRIS_BLOCK_SIZE;

        if(x_left[i] < 0) x_left[i] = 0;
        if(y_left[i] < 0) y_left[i] = 0;
        if(x_right[i] < 0) x_right[i] = 0;
        if(y_right[i] < 0) y_right[i] = 0;
        if(x_rotate[i] < 0) x_rotate[i] = 0;
        if(y_rotate[i] < 0) y_rotate[i] = 0;
        if(x_down[i] < 0) x_down[i] = 0;
        if(y_down[i] < 0) y_down[i] = 0;

    }
    switch (msg)
    {

        case MSG_CTP_TOUCH:
            break;

        case MSG_CTP_CLICK:
            switch(page_type)
            {
                case GAME_RUNNING_PAGE:
                    func_tetris_click();
                    break;
                case GAME_COME_BACK_PAGE:
                    func_tetris_come_back_click();
                    break;
                case GAME_COME_STOP_PAGE:
                    if(COMPO_ID_BTN_STAR == btn_id)
                    {
                        compo_form_destroy(func_cb.frm_main);
                        page_type = GAME_RUNNING_PAGE;
                        func_cb.frm_main = __this;
                        f_tetris->isstart = 1;
                        f_tetris->issuspend = 0;
                        compo_button_set_visible(compo_suspend_button, true);
                        compo_button_set_visible(compo_start_button, false);
                    }
                    return;
            }

            break;

        //case MSG_CTP_SHORT_UP:
        //case MSG_CTP_SHORT_DOWN:
        //    if(f_tetris->status == GAME_STATUS_RUNING)
        //    {
        //        if(f_tetris->isstart)
        //        {
        //            bool canMove = true;
//
//
        //            for (int i = 0; i < 4; i++)
        //            {
        //                if ((f_tetris->map[x_down[i]][y_down[i]] == 1))
        //                {
        //                    canMove = false;
        //                    break;
        //                }
        //            }
//
        //            if (canMove)
        //            {
        //                f_tetris->downtime_interval = 100;
        //            }
        //        }
        //    }
        //    break;
//
        case MSG_CTP_SHORT_LEFT:
            if(f_tetris->status == GAME_STATUS_RUNING)
            {
                if (f_tetris->isstart)
                {
                    bool canMove = true;

                    for (int i = 0; i < 4; i++)
                    {
                        if ((f_tetris->block[i].y > BOTTOM_Y + TETRIS_BLOCK_SIZE || f_tetris->block[i].x < LEFT_BORDER || f_tetris->map[x_left[i]][y_left[i]] == 1))
                        {
                            canMove = false;
                            break;
                        }
                    }

                    if (canMove)
                    {
                        f_tetris->pos_x -= TETRIS_BLOCK_SIZE;
                        f_tetris->rotate_x -= TETRIS_BLOCK_SIZE;
                        block_pos_update(f_tetris);
                        block_update(f_tetris);
                    }
                }
            }
            break;
        case MSG_CTP_SHORT_RIGHT:
            if(f_tetris->status == GAME_STATUS_RUNING)
            {
                if(f_tetris->isstart)
                {
                    bool canMove = true;

                    for (int i = 0; i < 4; i++)
                    {
                        if ((f_tetris->block[i].y > BOTTOM_Y + TETRIS_BLOCK_SIZE || f_tetris->block[i].x > RIGHT_BORDER || f_tetris->map[x_right[i]][y_right[i]] == 1))
                        {
                            canMove = false;
                            break;
                        }
                    }

                    if (canMove)
                    {
                        f_tetris->pos_x += TETRIS_BLOCK_SIZE;
                        f_tetris->rotate_x += TETRIS_BLOCK_SIZE;
                        block_pos_update(f_tetris);
                        block_update(f_tetris);
                    }

                }
            }
            break;

        case MSG_CTP_LONG:
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CHECK_LANGUAGE:
            if(page_type == GAME_COME_BACK_PAGE || page_type == GAME_COME_STOP_PAGE)
            {
                compo_form_destroy(func_cb.frm_main);
                page_type = GAME_RUNNING_PAGE;
                func_cb.frm_main = __this;
                f_tetris->isstart = 1;
                f_tetris->issuspend = 0;
                compo_button_set_visible(compo_suspend_button, true);
                compo_button_set_visible(compo_start_button, false);
                return;
            }
            else if(page_type == GAME_RUNNING_PAGE && GAME_STATUS_RUNING == f_tetris->status)
            {
                f_tetris->isstart = 0;
                f_tetris->issuspend = 1;
                compo_button_set_visible(compo_suspend_button, false);
                compo_button_set_visible(compo_start_button, true);
                func_cb.frm_main = func_tetris_come_back_form_create();
            }
            else
            {
                func_message(msg);
            }

            break;

        default:
            if(page_type == GAME_COME_BACK_PAGE || page_type == GAME_COME_STOP_PAGE)
            {
                return;
            }
            func_message(msg);
            break;
    }
}



static void func_tetris_enter(void)
{
    if(func_cb.f_cb == NULL)
    {
        func_cb.f_cb = func_zalloc(sizeof(f_tetris_t));
    }
    score_max = uteModuleGuiCommonReadTetrisMaxScore();
    func_cb.frm_main = func_tetris_form_create();
    f_tetris_t *f_tetris = (f_tetris_t *)func_cb.f_cb;
    compo_cardbox_t *compo_total_score = compo_getobj_byid(COMPO_ID_CARDBOX_TOTAL_SCORE);
    compo_cardbox_t *compo_line_score = compo_getobj_byid(COMPO_ID_CARDBOX_LINE_SCORE);
    if(func_cb.last != FUNC_CLOCK_SAVER)
    {
        f_tetris->status = GAME_STATUS_RUNING;  //进入该界面游戏直接就运行了的
        f_tetris->isstart = 1;
    }
    else
    {
        func_tetris_init();
    }
    for(int i=0; i<4; i++)
    {
        widget_set_visible(f_tetris->cur_image_block[i], true);
        widget_set_visible(f_tetris->next_image_block[i], true);
    }
    compo_cardbox_set_visible(compo_total_score, 1);
    compo_cardbox_set_visible(compo_line_score, 1);

    co_timer_set(&timer, 50, TIMER_REPEAT, LEVEL_LOW_PRI, score_calculate_process, NULL);
    co_timer_enable(&timer, 0);
}


static void func_tetris_exit(void)
{
    if(page_type == GAME_COME_BACK_PAGE || page_type == GAME_COME_STOP_PAGE)
    {
        compo_form_destroy(func_cb.frm_main);
        page_type = GAME_RUNNING_PAGE;
        func_cb.frm_main = __this;
    }
    func_cb.last = FUNC_TETRIS;
}


void func_tetris(void)
{
    printf("%s\n", __func__);
    func_tetris_enter();
    while (func_cb.sta == FUNC_TETRIS)
    {
        func_tetris_game_process();
        func_process();
        func_tetris_message(msg_dequeue());
    }
    func_tetris_exit();
}

#endif //UTE_MODULE_SCREENS_TETRIS_SUPPORT