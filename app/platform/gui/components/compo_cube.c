#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define CUBE_ROLL360_MODE                   1                                   //是否采用任意角度滚动模式

#define CUBE_HALF_CIRCUM(x)                 ((int)(M_PI * x))         //圆周一半
#define CUBE_MIN_POLAR                      0                                   //立方体的最大最小极角
#define CUBE_MAX_POLAR                      3600
#define CUBE_ITEM_ANGLE                     360

//移动相关控制
#define ANIMATION_TICK_EXPIRE               18                                  //动画单位时间Tick(ms)
#define ANIMATION_CNT_ENTERING              15                                  //入场动画拍数
#define ANIMATION_CNT_EXITING               15                                  //出场动画拍数
#define FOCUS_AUTO_STEP                     5                                   //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV                 16
#define DRAG_AUTO_SPEED                     (CUBE_ITEM_ANGLE * 80)              //拖动松手后的速度

#ifndef CUBE_TOUCH_STOP_WAIT_TIME
#define CUBE_TOUCH_STOP_WAIT_TIME           1000  // 触摸后停止自转等待时间（毫秒,0为不停止）
#endif

#ifndef CUBE_AUTO_SPIN_SPEED
#define CUBE_AUTO_SPIN_SPEED                7 // 自动旋转速度（0为不自转）
#endif

//    {UI_BUF_ICON_CUBE_CALL_BIN,             FUNC_CALL},
//    {UI_BUF_ICON_CUBE_HEART_RATE_BIN,       FUNC_HEARTRATE},
//    {UI_BUF_ICON_CUBE_MUSIC_BIN,            FUNC_BT},
////    {UI_BUF_ICON_CUBE_SLEEP_BIN,            FUNC_SLEEP},
////    {UI_BUF_ICON_CUBE_SPORT_BIN,            FUNC_SPORT},
////    {UI_BUF_ICON_CUBE_STEP_BIN,             FUNC_SLEEP},

//极角
static const u16 tbl_cube_polar[] =
{
//    372, 372, 372, 372, 372, 792, 792, 792, 792, 792,
//    1008, 1008, 1008, 1008, 1008, 1428, 1428, 1428, 1428, 1428,
    0, 900, 900, 900, 900, 1799
};

//方位角
static const u16 tbl_cube_azimuth[] =
{
//    0, 720, 1440, 2160, 2880, 0, 720, 1440, 2160, 2880,
//    360, 1080, 1800, 2520, 3240, 360, 1080, 1800, 2520, 3240,
    1800, 900, 1800, 2700, 0, 0
};

/**
 * @brief 创建一个立方体菜单组件
 * @param[in] frm : 窗体指针
 * @param[in] item : 立方体菜单项列表
 * @param[in] item_cnt : 立方体菜单项列表数量，必须是20项
 * @return 返回立方体菜单组件指针
 **/
compo_cube_t *compo_cube_create(compo_form_t *frm, s16 radius, compo_cube_item_t const *item, int item_cnt)
{
    if (item_cnt != CUBE_ITEM_CNT)
    {
        halt(HALT_GUI_COMPO_CUBE_CREATE);
    }
    compo_cube_t *cube = compo_create(frm, COMPO_TYPE_CUBE);
    widget_page_t *page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    int i;
    widget_axis3d_t *axis = widget_axis3d_create(page);
    cube->page = page;
    cube->item = item;
    cube->flag_need_update = true;
    cube->axis = axis;
    cube->radius = radius;
    for (i=0; i<CUBE_ITEM_CNT; i++)
    {
        widget_image3d_t *img = widget_image3d_create(page, item[i].res_addr);
        cube->item_img[i] = img;

        widget_image3d_set_axis(img, axis);
        widget_set_pos(img, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
        widget_image3d_set_r(img, radius);
        widget_image3d_set_polar(img, tbl_cube_polar[i]);
        widget_image3d_set_azimuth(img, tbl_cube_azimuth[i]);
    }
    cube->img_area = gui_image_get_size(item[0].res_addr);

    //初始角度
    cube->sph.rotation = 0;//2250;
    cube->sph.polar = 550;
    cube->sph.azimuth = 1400;//920;
#if CUBE_TOUCH_STOP_WAIT_TIME
    cube->move_cb.flag_stop_wait = true;
    cube->move_cb.flag_auto_spin = false;
    cube->move_cb.stop_wait_tick = tick_get();
#else
    cube->move_cb.flag_auto_spin = true;
    cube->move_cb.flag_stop_wait = false;
#endif
    compo_cube_update(cube);
    return cube;
}

/**
 * @brief 更新立方体菜单Widget
 * @param[in] cube : 立方体菜单指针
 **/
void compo_cube_update(compo_cube_t *cube)
{
    int i;
    if (!cube->flag_need_update)
    {
        return;
    }
    widget_axis3d_t *axis = cube->axis;
    cube->flag_need_update = false;
//    printf("cube: %d %d %d\n", cube->sph.polar, cube->sph.azimuth, cube->sph.rotation);
    widget_axis3d_set_sph(axis, cube->sph);
    for (i=0; i<CUBE_ITEM_CNT; i++)
    {
        widget_image3d_t *img = cube->item_img[i];
        widget_set_visible(img, widget_image3d_get_front(img) >= 0);
    }
}

/**
 * @brief 设置立方体菜单的旋转角度
 * @param[in] cube : 立方体菜单指针
 * @param[in] angle : 旋转角度
 **/
s32 compo_cube_set_rotation(compo_cube_t *cube, s32 angle)
{
    if (angle < 0)
    {
        angle = 3600 - (-angle % 3600);
    }
    if (angle >= 3600)
    {
        angle = angle % 3600;
    }
    if (cube->sph.rotation != angle)
    {
        cube->sph.rotation = angle;
        cube->flag_need_update = true;
    }
    return angle;
}

/**
 * @brief 立方体菜单滚动
 * @param[in] cube : 立方体菜单指针
 * @param[in] angle : 旋转角度
 **/
void compo_cube_roll(compo_cube_t *cube, s16 roll_polar)
{
    compo_cube_move_cb_t *mcb = &cube->move_cb;
    cube->sph = widget_axis3d_roll(cube->axis, roll_polar, mcb->roll_azimuth);
    cube->flag_need_update = true;
}

/**
 * @brief 立方体菜单滚动
 * @param[in] cube : 立方体菜单指针
 * @param[in] angle : 旋转角度
 **/
void compo_cube_roll_from(compo_cube_t *cube, s16 roll_polar, s16 roll_azimuth)
{
    compo_cube_move_cb_t *mcb = &cube->move_cb;
    cube->sph = widget_axis3d_roll_from(cube->axis, mcb->focus_sph, roll_polar, roll_azimuth);
    cube->flag_need_update = true;
}

/**
 * @brief 设置立方体菜单的极角
 * @param[in] cube : 立方体菜单指针
 * @param[in] angle : 极角
 **/
s32 compo_cube_set_polar(compo_cube_t *cube, s32 angle)
{
    if (angle < CUBE_MIN_POLAR)
    {
        angle = CUBE_MIN_POLAR;
    }
    else if (angle > CUBE_MAX_POLAR)
    {
        angle = CUBE_MAX_POLAR;
    }
    if (cube->sph.polar != angle)
    {
        cube->sph.polar = angle;
        cube->flag_need_update = true;
    }
    return angle;
}

/**
 * @brief 立方体菜单拖动与移动初始化
 * @param[in] cube : 立方体菜单指针
 * @return 返回COMPO_CUBE_STA状态枚举
 **/
u8 compo_cube_get_sta(compo_cube_t *cube)
{
    compo_cube_move_cb_t *mcb = &cube->move_cb;
    if (mcb == NULL)
    {
        return COMPO_CUBE_STA_IDLE;
    }
    if (mcb->flag_drag)
    {
        return COMPO_CUBE_STA_DARG;
    }
    else if (mcb->flag_move_auto)
    {
        return COMPO_CUBE_STA_MOVE;
    }
    else
    {
        return COMPO_CUBE_STA_IDLE;
    }
}

/**
 * @brief 获取立方体菜单项编号
 * @param[in] cube : 立方体菜单指针
 * @param[in] x:屏幕坐标x
 * @param[in] y:屏幕坐标y
 * @return 返回菜单编号
 **/
int compo_cube_get_idx(compo_cube_t *cube, s16 x, s16 y)
{
    int i;
    for (i=0; i<CUBE_ITEM_CNT; i++)
    {
        widget_image3d_t *img = cube->item_img[i];
        if (widget_image3d_get_front(img) >= 0 && widget_image3d_contains(img, x, y))
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 立方体菜单拖动与移动处理
 * @param[in] cube : 立方体菜单指针
 **/
void compo_cube_move(compo_cube_t *cube)
{
    compo_cube_move_cb_t *mcb = &cube->move_cb;
    if (mcb == NULL)
    {
        return;
    }

    if (mcb->flag_drag)
    {
        // 拖动逻辑保持不变
        s32 dx, dy, ax, ay;
        mcb->flag_drag = ctp_get_dxy(&dx, &dy);
        if (mcb->flag_drag)
        {
            // 拖动菜单图标
            ax = dx * 1800 / CUBE_HALF_CIRCUM(cube->radius);
            ay = dy * 1800 / CUBE_HALF_CIRCUM(cube->radius);
#if CUBE_ROLL360_MODE
            int rp = sqrt64(ax * ax + ay * ay);
            int ra = ARCTAN2(-ay, ax);
            compo_cube_roll_from(cube, rp, ra);
#else
            compo_cube_set_polar(cube, mcb->focus_sph.polar - ay);
            compo_cube_set_rotation(cube, mcb->focus_sph.rotation - ax);
#endif
            compo_cube_update(cube);
        }
        else
        {
            // 抬手后开始自动移动
            point_t last_dxy = ctp_get_last_dxy();
            int da;
#if CUBE_ROLL360_MODE
            mcb->roll_azimuth = ARCTAN2(-last_dxy.y, last_dxy.x);
            da = sqrt64(last_dxy.x * last_dxy.x + last_dxy.y * last_dxy.y);
#else
            da = last_dxy.x;
#endif
            mcb->moveto_a = da * DRAG_AUTO_SPEED / CUBE_HALF_CIRCUM(cube->radius);
            mcb->flag_move_auto = true;
            mcb->start_a = 0;
            mcb->tick = tick_get();
        }
    }

    if (mcb->flag_move_auto)
    {
        // 自动移动逻辑
        if (mcb->start_a == mcb->moveto_a)
        {
            mcb->flag_move_auto = false;              //移动完成
            compo_cube_update(cube);
            // 惯性减速完成，切换到停止等待状态
#if CUBE_TOUCH_STOP_WAIT_TIME
            mcb->flag_auto_spin = false;
            mcb->flag_stop_wait = true;
#else
            mcb->flag_stop_wait = false;
            mcb->flag_auto_spin = true;
#endif
            mcb->stop_wait_tick = tick_get();
        }
        else if (tick_check_expire(mcb->tick, ANIMATION_TICK_EXPIRE))
        {
            s32 da;
            mcb->tick = tick_get();
            da = mcb->moveto_a - mcb->start_a;
            if (da > 0)
            {
                if (da > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    da = da / FOCUS_AUTO_STEP_DIV;
                }
                else if (da > FOCUS_AUTO_STEP)
                {
                    da = FOCUS_AUTO_STEP;
                }
            }
            else
            {
                if (da < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    da = da / FOCUS_AUTO_STEP_DIV;
                }
                else if (da < -FOCUS_AUTO_STEP)
                {
                    da = -FOCUS_AUTO_STEP;
                }
            }
            mcb->start_a += da;
#if CUBE_ROLL360_MODE
            compo_cube_roll(cube, da);
#else
            compo_cube_set_rotation(cube, cube->sph.rotation - da);
#endif
            compo_cube_update(cube);
        }
    }

#if CUBE_AUTO_SPIN_SPEED
#if CUBE_TOUCH_STOP_WAIT_TIME
    if (mcb->flag_stop_wait)
    {
        // 停止等待逻辑
        if (tick_check_expire(mcb->stop_wait_tick, CUBE_TOUCH_STOP_WAIT_TIME))
        {
            mcb->flag_stop_wait = false;
            mcb->flag_auto_spin = true;
            mcb->tick = tick_get();
#if CUBE_ROLL360_MODE
            if(mcb->roll_azimuth == 0)
            {
                mcb->roll_azimuth = 450;
            }
#endif
        }
    }
#endif
    if (mcb->flag_auto_spin)
    {
        // 匀速自转逻辑
        if (tick_check_expire(mcb->tick, ANIMATION_TICK_EXPIRE))
        {
            mcb->tick = tick_get();
#if CUBE_ROLL360_MODE
            compo_cube_roll(cube, CUBE_AUTO_SPIN_SPEED);
#else
            compo_cube_set_rotation(cube, cube->sph.rotation - CUBE_AUTO_SPIN_SPEED);
#endif
            compo_cube_update(cube);
        }
    }
#endif
}

/**
 * @brief 立方体菜单拖动与移动控制
 * @param[in] cube : 立方体菜单指针
 * @param[in] cmd : 控制命令
 **/
void compo_cube_move_control(compo_cube_t *cube, int cmd)
{
    compo_cube_move_cb_t *mcb = &cube->move_cb;
    if (mcb == NULL)
    {
        return;
    }

    switch (cmd)
    {
        case COMPO_CUBE_MOVE_CMD_DRAG:
            // 开始拖动
            mcb->flag_drag = true;
            mcb->flag_move_auto = false;
            mcb->flag_auto_spin = false;
            mcb->flag_stop_wait = false; // 退出所有自动状态
            mcb->focus_sph = cube->sph;
            break;

        case COMPO_CUBE_MOVE_CMD_FORWARD:
            // 向前滚动
            if (!mcb->flag_move_auto)
            {
                mcb->flag_move_auto = true;
                mcb->start_a = 0;
                mcb->moveto_a = 0;
                mcb->roll_azimuth = 0;
            }
            mcb->moveto_a += CUBE_ITEM_ANGLE;
            break;

        case COMPO_CUBE_MOVE_CMD_BACKWARD:
            // 向后滚动
            if (!mcb->flag_move_auto)
            {
                mcb->flag_move_auto = true;
                mcb->start_a = 0;
                mcb->moveto_a = 0;
                mcb->roll_azimuth = 0;
            }
            mcb->moveto_a -= CUBE_ITEM_ANGLE;
            break;

        default:
            halt(HALT_GUI_COMPO_CUBE_MOVE_CMD);
            break;
    }
}

void compo_cube_set_pos(compo_cube_t *cube, s16 x, s16 y)
{
    widget_set_pos(cube->page, x, y);
}


