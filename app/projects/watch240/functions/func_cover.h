#ifndef _FUNC_COVER_H
#define _FUNC_COVER_H

//enum {
//    COVER_NULL,
//    //带时间提醒和按钮的
//    COVER_ALARM,
//    COVER_HEALTH_SEDENTARY,
//    COVER_HEALTH_DRINK,
//    //只有图标和文字提醒的
//    COVER_FIND_WATCH,
//    COVER_GOAL,
//    GCOVER_BT_CONNECT,
//    GCOVER_APP_CONNECT,
//};

void gui_set_cover_index(uint8_t index);
u32 func_cover_get_pic_res_addr();
u8 func_cover_get_str_idx();
s16 func_cover_get_pic_y();
s16 func_cover_get_txt_y();
void gui_cover_msg_enqueue(uint8_t index);

#endif // _FUNC_COVER_H
