#ifndef _COMPO_CUBE_H
#define _COMPO_CUBE_H

#define CUBE_ITEM_CNT               6               //立方体菜单项列表数量，必须是6项

//立方体菜单移动控制命令
enum COMPO_CUBE_MOVE_CMD
{
    COMPO_CUBE_MOVE_CMD_NONE,
    COMPO_CUBE_MOVE_CMD_DRAG,                       //开始拖动
    COMPO_CUBE_MOVE_CMD_FORWARD,                    //向前滚动
    COMPO_CUBE_MOVE_CMD_BACKWARD,                   //向后滚动
};

//立方体菜单当前状态
enum COMPO_CUBE_STA
{
    COMPO_CUBE_STA_IDLE,                        //空闲状态
    COMPO_CUBE_STA_DARG,                        //拖动中
    COMPO_CUBE_STA_MOVE,                        //移动中
};


//立方体菜单项定义
typedef struct compo_cube_item_t_
{
    u32 res_addr;               //图标
    u8 func_sta;                //功能ID
} compo_cube_item_t;

//立方体菜单移动控制
typedef struct compo_cube_move_cb_t_
{
    u32 tick;
    sph_t focus_sph;                    //当前球坐标
    s32 start_a;                        //开始角度
    s32 moveto_a;                       //设定自动移到的角度
    s16 roll_azimuth;
    bool flag_drag;                     //开始拖动
    bool flag_move_auto;                //自动移到坐标
    u8 sta;
    u8 animation_cnt;
    bool flag_auto_spin;                //匀速自转标志
    bool flag_stop_wait;                //停止等待标志
    u32 stop_wait_tick;                 //停止等待定时器
} compo_cube_move_cb_t;

typedef struct compo_cube_t_
{
    COMPO_STRUCT_COMMON;
    compo_cube_move_cb_t move_cb;   //移动和拖动处理
    widget_page_t *page;

    sph_t sph;                          //球坐标
    area_t img_area;                    //立方体上图片大小

    bool flag_need_update;              //是否需要更新

    widget_axis3d_t *axis;              //轴
    compo_cube_item_t const *item;
    s16 radius;
    widget_image3d_t *item_img[CUBE_ITEM_CNT];
} compo_cube_t;

/**
 * @brief 创建一个立方体菜单组件
 * @param[in] frm : 窗体指针
 * @param[in] res_addr : 图像资源地址
 * @return 返回图像指针
 **/
compo_cube_t *compo_cube_create(compo_form_t *frm, s16 radius, compo_cube_item_t const *item, int item_cnt);

/**
 * @brief 更新立方体菜单Widget
 * @param[in] cube : 立方体菜单指针
 **/
void compo_cube_update(compo_cube_t *cube);

/**
 * @brief 设置立方体菜单的自旋角
 * @param[in] cube : 立方体菜单指针
 * @param[in] angle : 自旋角
 **/
s32 compo_cube_set_rotation(compo_cube_t *cube, s32 angle);

/**
 * @brief 设置立方体菜单的极角
 * @param[in] cube : 立方体菜单指针
 * @param[in] angle : 极角
 **/
s32 compo_cube_set_polar(compo_cube_t *cube, s32 angle);

/**
 * @brief 立方体菜单拖动与移动初始化
 * @param[in] cube : 立方体菜单指针
 * @return 返回COMPO_FOOTBALL_STA状态枚举
 **/
u8 compo_cube_get_sta(compo_cube_t *cube);

/**
 * @brief 获取立方体菜单项编号
 * @param[in] cube : 立方体菜单指针
 * @param[in] x:屏幕坐标x
 * @param[in] y:屏幕坐标y
 * @return 返回菜单编号
 **/
int compo_cube_get_idx(compo_cube_t *cube, s16 x, s16 y);

/**
 * @brief 立方体菜单拖动与移动处理
 * @param[in] cube : 立方体菜单指针
 **/
void compo_cube_move(compo_cube_t *cube);

/**
 * @brief 立方体菜单拖动与移动控制
 * @param[in] cube : 立方体菜单指针
 * @param[in] cmd : 控制命令
 **/
void compo_cube_move_control(compo_cube_t *cube, int cmd);

void compo_cube_set_pos(compo_cube_t *cube, s16 x, s16 y);

#endif
