#include "include.h"
#include "func.h"
#include "func_cover.h"
#include "ute_module_notify.h"
#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
///闹钟蓝牙连接提醒等简单界面弹窗
#define COVER_REMIND_ITEM_CNT            ((int)(sizeof(tbl_cover_remind_item) / sizeof(tbl_cover_remind_item[0])))
typedef struct f_cover_remind_item_t_ {
    u32 res_addr;
    u8 str_idx;
    s16 pic_y;
    s16 txt_y;
    s16 title_y;
} f_cover_remind_item_t;

//和func_cover.h里的宏定义顺序一致
const f_cover_remind_item_t tbl_cover_remind_item[] = {
    /*   res_addr,                              str_idx,                pic_y,                txt_y                  title_y   */
    {UI_BUF_ICON_ALARM_CLOCK_BIN,             STR_ALARM_CLOCK,        GUI_SCREEN_HEIGHT/5+30, GUI_SCREEN_HEIGHT/5,  GUI_SCREEN_CENTER_Y},
//    {UI_BUF_POP_UP_SEDENTARY_CLOCK_BIN,         STR_SEDENTARY_REMIND,   102,            190},
//    {UI_BUF_POP_UP_WATER_CLOCK_BIN,             STR_DRINK_REMIND,       102,            190},
//    {UI_BUF_POP_UP_FIND_WATCH_BIN,              STR_FIND_WATCH,         175,            290},
//    {UI_BUF_POP_UP_GOAL_ACHIEVEMENT_BIN,        STR_GOAL_ACHIEVE,       175,            290},
//    {UI_BUF_POP_UP_BLUETOOTH_CONNECTION_BIN,    STR_BT_CONNECT,         175,            290},
//    {UI_BUF_POP_UP_APP_CONNECTION_BIN,          STR_APP_CONNECT,        175,            290},
};

///消息弹窗界面（简略）
#define COVER_BRIEF_MSG_ITEM_CNT            ((int)(sizeof(tbl_cover_brief_msg_item) / sizeof(tbl_cover_brief_msg_item[0])))
const f_cover_brief_msg_item_t tbl_cover_brief_msg_item[] = {
    /*   res_addr,                              pic_y,                txt_y,                title_y                  time_txt_y   */
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_QQ
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WECHAT
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MMS
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_OTHER
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Facebook
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Twitter
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Skype
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_FacebookMessenger
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hangouts
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Line
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Linkedin
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Instagram
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Viber
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_KakaoTalk
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_VKontakte
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Snapchat
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Googleplus
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gmail
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flickr
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Tumblr
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Pinterest
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YouTube
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Telegram
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Truecaller
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Paytm
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zalo
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_imo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MicrosoftTeams
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Outlook
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Swiggy
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zomato
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gpay
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PhonePe
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hotstar
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PrimeVideo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flipkart
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Amazon
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Myntra
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_NoiseApp
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_DailyHunt
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Inshorts
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_BookMyShow
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Calendar
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_JioTv
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MakeMyTrip
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Netflix
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Ola
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_ReflexApp
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Uber
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YTMusic
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp_Business
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Dunzo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gaana
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Google_Drive
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_googlechat
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Wynkmusic
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Yahoo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_TitanSmartWorld
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Slack
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Spotify
#endif // APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
};

///消息弹窗界面（详细）
#define COVER_DETAIL_MSG_ITEM_CNT            ((int)(sizeof(tbl_cover_detail_msg_item) / sizeof(tbl_cover_detail_msg_item[0])))
const f_cover_detail_msg_item_t tbl_cover_detail_msg_item[] = {
    /*   res_addr,                   pic_y,                   txt_y,                     title_y                  time_txt_y   */
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_QQ
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WECHAT
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MMS
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_OTHER
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Facebook
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Twitter
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Skype
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_FacebookMessenger
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hangouts
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Line
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Linkedin
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Instagram
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Viber
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_KakaoTalk
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_VKontakte
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Snapchat
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Googleplus
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gmail
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flickr
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Tumblr
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Pinterest
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YouTube
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Telegram
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Truecaller
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Paytm
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zalo
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_imo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MicrosoftTeams
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Outlook
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Swiggy
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zomato
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gpay
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PhonePe
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hotstar
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PrimeVideo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flipkart
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Amazon
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Myntra
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_NoiseApp
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_DailyHunt
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Inshorts
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_BookMyShow
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Calendar
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_JioTv
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MakeMyTrip
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Netflix
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Ola
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_ReflexApp
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Uber
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YTMusic
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp_Business
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Dunzo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gaana
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Google_Drive
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_googlechat
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Wynkmusic
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Yahoo
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_TitanSmartWorld
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Slack
    {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Spotify
#endif // APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
};

u32 func_cover_get_detail_msg_cnt(void)
{
    return COVER_DETAIL_MSG_ITEM_CNT;
}

u32 func_cover_get_detail_msg_ui(u8 index)
{
    return tbl_cover_detail_msg_item[index].res_addr;
}

typedef struct f_cover_t_ {

} f_cover_t;


u32 func_cover_get_pic_res_addr(u8 type)
{
    switch (type) {

    case MSGBOX_MSG_TYPE_DETAIL:
        return tbl_cover_detail_msg_item[sys_cb.msg_index - 1].res_addr;
        break;

    case MSGBOX_MSG_TYPE_BRIEF:
        return tbl_cover_brief_msg_item[sys_cb.msg_index - 1].res_addr;
        break;

    case MSGBOX_MSG_TYPE_REMIND_COVER:
        return tbl_cover_remind_item[sys_cb.cover_index - 1].res_addr;
        break;

    default:
        break;
    }
    return 0;
}

u8 func_cover_get_str_idx(u8 type)
{
    switch (type) {
    case MSGBOX_MSG_TYPE_REMIND_COVER:
        return tbl_cover_remind_item[sys_cb.cover_index - 1].str_idx;
        break;

    case MSGBOX_MSG_TYPE_DETAIL:
        break;

    case MSGBOX_MSG_TYPE_BRIEF:
        break;

    default:
        break;
    }
    return 0;
}

s16 func_cover_get_pic_y(u8 type)
{
    switch (type) {
    case MSGBOX_MSG_TYPE_REMIND_COVER:
        return tbl_cover_remind_item[sys_cb.cover_index - 1].pic_y;
        break;

    case MSGBOX_MSG_TYPE_DETAIL:
        return tbl_cover_detail_msg_item[sys_cb.msg_index - 1].pic_y;
        break;

    case MSGBOX_MSG_TYPE_BRIEF:
        return tbl_cover_brief_msg_item[sys_cb.msg_index - 1].pic_y;
        break;

    default:
        break;
    }
    return 0;
}

s16 func_cover_get_txt_y(u8 type)
{
    switch (type) {
    case MSGBOX_MSG_TYPE_REMIND_COVER:
        return tbl_cover_remind_item[sys_cb.cover_index - 1].txt_y;
        break;

    case MSGBOX_MSG_TYPE_DETAIL:
        return tbl_cover_detail_msg_item[sys_cb.msg_index - 1].txt_y;
        break;

    case MSGBOX_MSG_TYPE_BRIEF:
        return tbl_cover_brief_msg_item[sys_cb.msg_index - 1].txt_y;
        break;

    default:
        break;
    }
    return 0;
}

s16 func_cover_get_title_txt_y(u8 type)
{
    switch (type) {
    case MSGBOX_MSG_TYPE_REMIND_COVER:
        return tbl_cover_remind_item[sys_cb.cover_index - 1].title_y;
        break;

    case MSGBOX_MSG_TYPE_DETAIL:
        return tbl_cover_detail_msg_item[sys_cb.msg_index - 1].title_y;
        break;

    case MSGBOX_MSG_TYPE_BRIEF:
        return tbl_cover_brief_msg_item[sys_cb.msg_index - 1].title_y;
        break;

    default:
        break;
    }
    return 0;
}

s16 func_cover_get_time_txt_y(u8 type)
{
    switch (type) {
    case MSGBOX_MSG_TYPE_REMIND_COVER:
        break;

    case MSGBOX_MSG_TYPE_DETAIL:
        return tbl_cover_detail_msg_item[sys_cb.msg_index - 1].time_txt_y;
        break;

    case MSGBOX_MSG_TYPE_BRIEF:
        return tbl_cover_brief_msg_item[sys_cb.msg_index - 1].time_txt_y;
        break;

    default:
        break;
    }
    return 0;
}

void app_msg_pop_up(uint8_t index)
{
    if (sys_cb.gui_sleep_sta) {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();

    sys_cb.msg_index = index;

    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
    msg_enqueue(EVT_MSGBOX_EXIT);
    msg_enqueue(EVT_WATCH_MSG_POP_UP);
}

void app_ute_msg_pop_up(uint8_t index)
{
    sys_cb.cover_index = index;

    if ((sys_cb.cover_index > MSG_CALL) && (sys_cb.cover_index < MSG_MAX_CNT)) {

        ute_module_notify_data_t ble_msg = {0};
        uteModuleNotifyGetData(&ble_msg);

        char *msg = (char*)ble_msg.currNotify.content;
        char *title = NULL;
        char time[30]= {0};
        sprintf(time, "%04d-%02d-%02d %02d:%02d", ble_msg.currNotify.year, ble_msg.currNotify.month, ble_msg.currNotify.day, ble_msg.currNotify.hour, ble_msg.currNotify.min);
        int res = msgbox(msg, title, time, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_BRIEF);
        if (res == MSGBOX_RES_ENTER_DETAIL_MSG) {       //点击进入详细消息弹窗
            printf("enter MSGBOX_RES_ENTER_DETAIL_MSG\n");
            int res = msgbox(msg, title, time, MSGBOX_MODE_BTN_DELETE, MSGBOX_MSG_TYPE_DETAIL);
            if (res == MSGBOX_RES_DELETE) {
                uteModuleNotifySetDisplayIndex(0);
                uteModuleNotifyDelAllHistoryData(false);
            }
        }
    }
}

void gui_cover_msg_enqueue(uint8_t index)
{
    if (bt_cb.disp_status >= BT_STA_INCOMING && bt_cb.disp_status <= BT_STA_INCALL) {
        return;
    }
    sys_cb.cover_index = index;
    msg_enqueue(EVT_MSGBOX_EXIT);
    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
    msg_enqueue(EVT_WATCH_SET_COVER);
}

void gui_set_cover_index(uint8_t index)
{
    printf("%s:%d\n", __func__, index);

    ute_module_systemtime_one_alarm_t alarm = {0};
    ute_module_systemtime_one_alarm_t* alarm_p = NULL;

    if (index > COVER_REMIND_ITEM_CNT) {
        printf("cover index exceed!\n");
        return;
    }
    if (!COVER_REMIND_ITEM_CNT) {
        return;
    }
    sys_cb.cover_index = index;
    u8 mode = MSGBOX_MODE_BTN_NONE;
    if (sys_cb.cover_index) {
        char *msg = (char *)i18n[tbl_cover_remind_item[sys_cb.cover_index - 1].str_idx];
        char title[8] = {'\0'};
        if ((sys_cb.cover_index >= REMIND_COVER_ALARM) && (sys_cb.cover_index <= REMIND_COVER_HEALTH_DRINK)) {
            if (sys_cb.cover_index == REMIND_COVER_ALARM) {

                //获取用户闹钟
                if (uteModuleSystemtimeGetAlarmRingIndex() != 0xff) {
                    uteModuleSystemtimeGetAlarm(&alarm, uteModuleSystemtimeGetAlarmRingIndex());
                }
                alarm_p = &alarm;
                if (alarm_p->isRepeatRemindOpen) {
                    snprintf(title, sizeof(title), "%02d:%02d", alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                } else {
                    snprintf(title, sizeof(title), "%02d:%02d", alarm_p->hour, alarm_p->min);
                }
                //开启马达 喇叭
            }
//            else if (sys_cb.cover_index == COVER_HEALTH_DRINK || sys_cb.cover_index == COVER_HEALTH_SEDENTARY) {
//                tm_t tm = time_to_tm(RTCCNT);
//                snprintf(title, sizeof(title), "%02d:%02d", tm.hour, tm.min);
//            }
            mode = MSGBOX_MODE_BTN_REMIND_LATER_CLOSE;
        }

        int res = msgbox(msg, title, NULL, mode,  MSGBOX_MSG_TYPE_REMIND_COVER);


        if (res == MSGBOX_RES_REMIND_LATER) {       //稍后提醒
            if (sys_cb.cover_index == REMIND_COVER_ALARM) {
                printf("COVER_ALARM MSGBOX_RES_REMIND_LATER\n");
                //开启配置贪睡时钟 参数
                if (alarm_p != NULL) {
                    alarm_p->isRepeatRemindOpen = true;

                    uint8_t hour_later = alarm_p->repeatRemindHour;
                    uint8_t min_later = alarm_p->repeatRemindMin + ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN;
                    if (min_later > 59) {
                        hour_later += 1;
                        min_later -= 60;
                    }

                    if (hour_later > 24) {
                        hour_later -= 24;
                    }

                    alarm_p->repeatRemindHour = hour_later;
                    alarm_p->repeatRemindMin = min_later;
                    uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
                    printf("repeat alarm[%d] ring, [%02d:%02d]\n", uteModuleSystemtimeGetAlarmRingIndex(), alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                }
                //关闭 喇叭 马达
           }
        } else if (res == MSGBOX_RES_EXIT) {               //强制退出弹窗
            if (sys_cb.cover_index == REMIND_COVER_ALARM) {
                printf("COVER_ALARM MSGBOX_RES_EXIT\n");
                //关闭 喇叭 马达

            }
        } else if (res == MSGBOX_RES_TIMEOUT_EXIT) {        //提醒界面超时退出
            if (sys_cb.cover_index == REMIND_COVER_ALARM) {
                printf("COVER_ALARM MSGBOX_RES_TIMEOUT_EXIT\n");
                //关闭 喇叭 马达
            }
        } else {
            if (sys_cb.cover_index == REMIND_COVER_ALARM) {
                alarm_p->isRepeatRemindOpen = false;
                uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
                //关闭 喇叭 马达
            }
        }

    }
}
