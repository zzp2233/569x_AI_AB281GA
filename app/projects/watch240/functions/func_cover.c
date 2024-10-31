#include "include.h"
#include "func.h"
#include "func_cover.h"
#include "ute_module_notify.h"
#include "ute_drv_motor.h"
#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
///闹钟蓝牙连接提醒等简单界面弹窗
#define COVER_REMIND_ITEM_CNT            ((int)(sizeof(tbl_cover_remind_item) / sizeof(tbl_cover_remind_item[0])))
typedef struct f_cover_remind_item_t_
{
    u32 res_addr;
    u8 str_idx;
    s16 pic_y;
    s16 txt_y;
    s16 title_y;
} f_cover_remind_item_t;

//和func_cover.h里的宏定义顺序一致
const f_cover_remind_item_t tbl_cover_remind_item[] =
{
    /*id                               res_addr,                              str_idx,                pic_y,                txt_y                  title_y   */
    [REMIND_COVER_ALARM]            = {UI_BUF_ICON_ALARM_CLOCK_BIN,             STR_ALARM_CLOCK,        GUI_SCREEN_HEIGHT/5+30, GUI_SCREEN_HEIGHT/5,  GUI_SCREEN_CENTER_Y},
//    [REMIND_COVER_HEALTH_SEDENTARY] = {UI_BUF_POP_UP_SEDENTARY_CLOCK_BIN,         STR_SEDENTARY_REMIND,   102,            190},
//    [REMIND_COVER_HEALTH_DRINK]     = {UI_BUF_POP_UP_WATER_CLOCK_BIN,             STR_DRINK_REMIND,       102,            190},
    [REMIND_COVER_FIND_WATCH]       = {UI_BUF_REMIND_FINDWATCH_BIN,             STR_FIND_WATCH,         GUI_SCREEN_CENTER_Y,    GUI_SCREEN_HEIGHT*4/5,  0},
//    [REMIND_COVER_GOAL]             = {UI_BUF_POP_UP_GOAL_ACHIEVEMENT_BIN,        STR_GOAL_ACHIEVE,       175,            290},
//    [REMIND_GCOVER_BT_CONNECT]      = {UI_BUF_POP_UP_BLUETOOTH_CONNECTION_BIN,    STR_BT_CONNECT,         175,            290},
//    [REMIND_GCOVER_APP_CONNECT]     = {UI_BUF_POP_UP_APP_CONNECTION_BIN,          STR_APP_CONNECT,        175,            290},
};

///消息弹窗界面（简略）
#define COVER_BRIEF_MSG_ITEM_CNT            ((int)(sizeof(tbl_cover_brief_msg_item) / sizeof(tbl_cover_brief_msg_item[0])))
const f_cover_brief_msg_item_t tbl_cover_brief_msg_item[] =
{
    /*id                       res_addr,                              pic_y,                txt_y,                title_y                  time_txt_y   */
    [MSG_QQ]                = {UI_BUF_MSG_ICON_QQ_BIN,        GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_QQ
    [MSG_WECHAT]            = {UI_BUF_MSG_ICON_WECHAT_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WECHAT
    [MSG_MMS]               = {UI_BUF_MSG_ICON_MESSAGE_BIN,   GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MMS
    [MSG_OTHER]             = {UI_BUF_MSG_ICON_OTHER_BIN,     GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_OTHER
    [MSG_Facebook]          = {UI_BUF_MSG_ICON_FACEBOOK_BIN,  GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Facebook
    [MSG_Twitter]           = {UI_BUF_MSG_ICON_X_BIN,         GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Twitter
    [MSG_WhatsApp]          = {UI_BUF_MSG_ICON_WHATSAPP_BIN,  GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp
    [MSG_Skype]             = {UI_BUF_MSG_ICON_SKYPE_BIN,     GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Skype
    [MSG_FacebookMessenger] = {UI_BUF_MSG_ICON_MESSENGER_BIN, GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_FacebookMessenger
    [MSG_Hangouts]          = {UI_BUF_MSG_ICON_HANGOUTS_BIN,  GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hangouts
    [MSG_Line]              = {UI_BUF_MSG_ICON_LINE_BIN,      GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Line
    [MSG_Linkedin]          = {UI_BUF_MSG_ICON_LINK_BIN,      GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Linkedin
    [MSG_Instagram]         = {UI_BUF_MSG_ICON_INS_BIN,       GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Instagram
    [MSG_Viber]             = {UI_BUF_MSG_ICON_VIBER_BIN,     GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Viber
    [MSG_KakaoTalk]         = {UI_BUF_MSG_ICON_KAKAO_BIN,     GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_KakaoTalk
    [MSG_VKontakte]         = {UI_BUF_MSG_ICON_VKONTAKTE_BIN, GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_VKontakte
    [MSG_Snapchat]          = {UI_BUF_MSG_ICON_SNAPCHAT_BIN,  GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Snapchat
    [MSG_Googleplus]        = {UI_BUF_MSG_ICON_GOOGLEPLUS_BIN,GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Googleplus
    [MSG_Gmail]             = {UI_BUF_MSG_ICON_GMAIL_BIN,     GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gmail
    [MSG_Flickr]            = {UI_BUF_MSG_ICON_FLICKR_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flickr
    [MSG_Tumblr]            = {UI_BUF_MSG_ICON_TUMBLR_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Tumblr
    [MSG_Pinterest]         = {UI_BUF_MSG_ICON_PINTEREST_BIN, GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Pinterest
    [MSG_YouTube]           = {UI_BUF_MSG_ICON_YOUTUBE_BIN,   GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YouTube
    [MSG_Telegram]          = {UI_BUF_MSG_ICON_TELEGRAM_BIN,  GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Telegram
    [MSG_Truecaller]        = {UI_BUF_MSG_ICON_TRUECALLER_BIN,GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Truecaller
    [MSG_Paytm]             = {UI_BUF_MSG_ICON_PAYTM_BIN,     GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Paytm
    [MSG_Zalo]              = {UI_BUF_MSG_ICON_ZALO_BIN,      GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zalo
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
    [MSG_imo]               = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_imo
    [MSG_MicrosoftTeams]    = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MicrosoftTeams
    [MSG_Outlook]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Outlook
    [MSG_Swiggy]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Swiggy
    [MSG_Zomato]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zomato
    [MSG_Gpay]              = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gpay
    [MSG_PhonePe]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PhonePe
    [MSG_Hotstar]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hotstar
    [MSG_PrimeVideo]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PrimeVideo
    [MSG_Flipkart]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flipkart
    [MSG_Amazon]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Amazon
    [MSG_Myntra]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Myntra
    [MSG_NoiseApp]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_NoiseApp
    [MSG_DailyHunt]         = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_DailyHunt
    [MSG_Inshorts]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Inshorts
    [MSG_BookMyShow]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_BookMyShow
    [MSG_Calendar]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Calendar
    [MSG_JioTv]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_JioTv
    [MSG_MakeMyTrip]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MakeMyTrip
    [MSG_Netflix]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Netflix
    [MSG_Ola]               = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Ola
    [MSG_ReflexApp]         = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_ReflexApp
    [MSG_Uber]              = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Uber
    [MSG_YTMusic]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YTMusic
    [MSG_WhatsApp_Business] = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp_Business
    [MSG_Dunzo]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Dunzo
    [MSG_Gaana]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gaana
    [MSG_Google_Drive]      = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Google_Drive
    [MSG_googlechat]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_googlechat
    [MSG_Wynkmusic]         = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Wynkmusic
    [MSG_Yahoo]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Yahoo
    [MSG_TitanSmartWorld]   = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_TitanSmartWorld
    [MSG_Slack]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Slack
    [MSG_Spotify]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5+10,     GUI_SCREEN_HEIGHT/2 - 20,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Spotify
#endif // APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
};

///消息弹窗界面（详细）
#define COVER_DETAIL_MSG_ITEM_CNT            ((int)(sizeof(tbl_cover_detail_msg_item) / sizeof(tbl_cover_detail_msg_item[0])))
const f_cover_detail_msg_item_t tbl_cover_detail_msg_item[] =
{
    /*   res_addr,                   pic_y,                   txt_y,                     title_y                  time_txt_y   */
    [MSG_QQ]                = {UI_BUF_MSG_ICON_QQ_BIN,        GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_QQ
    [MSG_WECHAT]            = {UI_BUF_MSG_ICON_WECHAT_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WECHAT
    [MSG_MMS]               = {UI_BUF_MSG_ICON_MESSAGE_BIN,   GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MMS
    [MSG_OTHER]             = {UI_BUF_MSG_ICON_OTHER_BIN,     GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_OTHER
    [MSG_Facebook]          = {UI_BUF_MSG_ICON_FACEBOOK_BIN,  GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Facebook
    [MSG_Twitter]           = {UI_BUF_MSG_ICON_X_BIN,         GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Twitter
    [MSG_WhatsApp]          = {UI_BUF_MSG_ICON_WHATSAPP_BIN,  GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp
    [MSG_Skype]             = {UI_BUF_MSG_ICON_SKYPE_BIN,     GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Skype
    [MSG_FacebookMessenger] = {UI_BUF_MSG_ICON_MESSENGER_BIN, GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_FacebookMessenger
    [MSG_Hangouts]          = {UI_BUF_MSG_ICON_HANGOUTS_BIN,  GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hangouts
    [MSG_Line]              = {UI_BUF_MSG_ICON_LINE_BIN,      GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Line
    [MSG_Linkedin]          = {UI_BUF_MSG_ICON_LINK_BIN,      GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Linkedin
    [MSG_Instagram]         = {UI_BUF_MSG_ICON_INS_BIN,       GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Instagram
    [MSG_Viber]             = {UI_BUF_MSG_ICON_VIBER_BIN,     GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Viber
    [MSG_KakaoTalk]         = {UI_BUF_MSG_ICON_KAKAO_BIN,     GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_KakaoTalk
    [MSG_VKontakte]         = {UI_BUF_MSG_ICON_VKONTAKTE_BIN, GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_VKontakte
    [MSG_Snapchat]          = {UI_BUF_MSG_ICON_SNAPCHAT_BIN,  GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Snapchat
    [MSG_Googleplus]        = {UI_BUF_MSG_ICON_GOOGLEPLUS_BIN,GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Googleplus
    [MSG_Gmail]             = {UI_BUF_MSG_ICON_GMAIL_BIN,     GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gmail
    [MSG_Flickr]            = {UI_BUF_MSG_ICON_FLICKR_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flickr
    [MSG_Tumblr]            = {UI_BUF_MSG_ICON_TUMBLR_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Tumblr
    [MSG_Pinterest]         = {UI_BUF_MSG_ICON_PINTEREST_BIN, GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Pinterest
    [MSG_YouTube]           = {UI_BUF_MSG_ICON_YOUTUBE_BIN,   GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YouTube
    [MSG_Telegram]          = {UI_BUF_MSG_ICON_TELEGRAM_BIN,  GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Telegram
    [MSG_Truecaller]        = {UI_BUF_MSG_ICON_TRUECALLER_BIN,GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Truecaller
    [MSG_Paytm]             = {UI_BUF_MSG_ICON_PAYTM_BIN,     GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Paytm
    [MSG_Zalo]              = {UI_BUF_MSG_ICON_ZALO_BIN,      GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zalo
#if APP_DYNAMIC_ADDITIONAL_SOCIAL_APP_SUPPORT
    [MSG_imo]               = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_imo
    [MSG_MicrosoftTeams]    = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MicrosoftTeams
    [MSG_Outlook]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Outlook
    [MSG_Swiggy]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Swiggy
    [MSG_Zomato]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Zomato
    [MSG_Gpay]              = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gpay
    [MSG_PhonePe]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PhonePe
    [MSG_Hotstar]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Hotstar
    [MSG_PrimeVideo]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_PrimeVideo
    [MSG_Flipkart]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Flipkart
    [MSG_Amazon]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Amazon
    [MSG_Myntra]            = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Myntra
    [MSG_NoiseApp]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_NoiseApp
    [MSG_DailyHunt]         = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_DailyHunt
    [MSG_Inshorts]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Inshorts
    [MSG_BookMyShow]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_BookMyShow
    [MSG_Calendar]          = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Calendar
    [MSG_JioTv]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_JioTv
    [MSG_MakeMyTrip]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_MakeMyTrip
    [MSG_Netflix]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Netflix
    [MSG_Ola]               = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Ola
    [MSG_ReflexApp]         = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_ReflexApp
    [MSG_Uber]              = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Uber
    [MSG_YTMusic]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_YTMusic
    [MSG_WhatsApp_Business] = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_WhatsApp_Business
    [MSG_Dunzo]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Dunzo
    [MSG_Gaana]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Gaana
    [MSG_Google_Drive]      = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Google_Drive
    [MSG_googlechat]        = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_googlechat
    [MSG_Wynkmusic]         = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Wynkmusic
    [MSG_Yahoo]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Yahoo
    [MSG_TitanSmartWorld]   = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_TitanSmartWorld
    [MSG_Slack]             = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Slack
    [MSG_Spotify]           = {UI_BUF_ICON_MESSAGE_BIN,    GUI_SCREEN_HEIGHT/5,     GUI_SCREEN_HEIGHT/2-12,    GUI_SCREEN_HEIGHT/2-20,  GUI_SCREEN_HEIGHT/2-50},       //MSG_Spotify
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

typedef struct f_cover_t_
{

} f_cover_t;


u32 func_cover_get_pic_res_addr(u8 type)
{
    switch (type)
    {

        case MSGBOX_MSG_TYPE_DETAIL:
            return tbl_cover_detail_msg_item[sys_cb.msg_index].res_addr;
            break;

        case MSGBOX_MSG_TYPE_BRIEF:
            return tbl_cover_brief_msg_item[sys_cb.msg_index].res_addr;
            break;

        case MSGBOX_MSG_TYPE_REMIND_COVER:
            return tbl_cover_remind_item[sys_cb.cover_index].res_addr;
            break;

        default:
            break;
    }
    return 0;
}

u8 func_cover_get_str_idx(u8 type)
{
    switch (type)
    {
        case MSGBOX_MSG_TYPE_REMIND_COVER:
            return tbl_cover_remind_item[sys_cb.cover_index].str_idx;
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
    switch (type)
    {
        case MSGBOX_MSG_TYPE_REMIND_COVER:
            return tbl_cover_remind_item[sys_cb.cover_index].pic_y;
            break;

        case MSGBOX_MSG_TYPE_DETAIL:
            return tbl_cover_detail_msg_item[sys_cb.msg_index].pic_y;
            break;

        case MSGBOX_MSG_TYPE_BRIEF:
            return tbl_cover_brief_msg_item[sys_cb.msg_index].pic_y;
            break;

        default:
            break;
    }
    return 0;
}

s16 func_cover_get_txt_y(u8 type)
{
    switch (type)
    {
        case MSGBOX_MSG_TYPE_REMIND_COVER:
            return tbl_cover_remind_item[sys_cb.cover_index].txt_y;
            break;

        case MSGBOX_MSG_TYPE_DETAIL:
            return tbl_cover_detail_msg_item[sys_cb.msg_index].txt_y;
            break;

        case MSGBOX_MSG_TYPE_BRIEF:
            return tbl_cover_brief_msg_item[sys_cb.msg_index].txt_y;
            break;

        default:
            break;
    }
    return 0;
}

s16 func_cover_get_title_txt_y(u8 type)
{
    switch (type)
    {
        case MSGBOX_MSG_TYPE_REMIND_COVER:
            return tbl_cover_remind_item[sys_cb.cover_index].title_y;
            break;

        case MSGBOX_MSG_TYPE_DETAIL:
            return tbl_cover_detail_msg_item[sys_cb.msg_index].title_y;
            break;

        case MSGBOX_MSG_TYPE_BRIEF:
            return tbl_cover_brief_msg_item[sys_cb.msg_index].title_y;
            break;

        default:
            break;
    }
    return 0;
}

s16 func_cover_get_time_txt_y(u8 type)
{
    switch (type)
    {
        case MSGBOX_MSG_TYPE_REMIND_COVER:
            break;

        case MSGBOX_MSG_TYPE_DETAIL:
            return tbl_cover_detail_msg_item[sys_cb.msg_index].time_txt_y;
            break;

        case MSGBOX_MSG_TYPE_BRIEF:
            return tbl_cover_brief_msg_item[sys_cb.msg_index].time_txt_y;
            break;

        default:
            break;
    }
    return 0;
}

void app_msg_pop_up(uint8_t index)
{
    if (sys_cb.gui_sleep_sta)
    {
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

    if ((sys_cb.cover_index > MSG_CALL) && (sys_cb.cover_index < MSG_MAX_CNT))
    {

        ute_module_notify_data_t *ble_msg = ab_zalloc(sizeof(ute_module_notify_data_t));
        if (ble_msg == NULL)
        {
            printf("%s malloc err!!\n", __func__);
            return;
        }
        uteModuleNotifyGetData(ble_msg);

        char *msg = (char*)ble_msg->currNotify.content;
        char *title = NULL;
        char time[30]= {0};
        sprintf(time, "%04d-%02d-%02d %02d:%02d", ble_msg->currNotify.year, ble_msg->currNotify.month, ble_msg->currNotify.day, ble_msg->currNotify.hour,
                ble_msg->currNotify.min);
        int res = msgbox(msg, title, time, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_BRIEF);
        if (res == MSGBOX_RES_ENTER_DETAIL_MSG)         //点击进入详细消息弹窗
        {
            printf("enter MSGBOX_RES_ENTER_DETAIL_MSG\n");
            int res = msgbox(msg, title, time, MSGBOX_MODE_BTN_DELETE, MSGBOX_MSG_TYPE_DETAIL);
            if (res == MSGBOX_RES_DELETE)
            {
                uteModuleNotifySetDisplayIndex(0);
                uteModuleNotifyDelAllHistoryData(false);
            }
        }
        ab_free(ble_msg);
    }
}

void gui_cover_msg_enqueue(uint8_t index)
{
    if (bt_cb.disp_status >= BT_STA_INCOMING && bt_cb.disp_status <= BT_STA_INCALL)
    {
        return;
    }
    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();
    sys_cb.cover_index = index;
    msg_enqueue(EVT_MSGBOX_EXIT);
    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
    msg_enqueue(EVT_WATCH_SET_COVER);
}

void gui_set_cover_index(uint8_t index)
{
    printf("%s:%d\n", __func__, index);

//    ute_module_systemtime_one_alarm_t alarm = {0};
    ute_module_systemtime_one_alarm_t* alarm_p = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm_p == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return;
    }

    if (index > COVER_REMIND_ITEM_CNT)
    {
        printf("cover index exceed!\n");
        ab_free(alarm_p);
        return;
    }
    if (!COVER_REMIND_ITEM_CNT)
    {
        ab_free(alarm_p);
        return;
    }
    sys_cb.cover_index = index;
    u8 mode = MSGBOX_MODE_BTN_NONE;
    if (sys_cb.cover_index)
    {
        //char *msg = (char *)i18n[tbl_cover_remind_item[sys_cb.cover_index - 1].str_idx];
        char *msg = (char *)i18n[func_cover_get_str_idx(MSGBOX_MSG_TYPE_REMIND_COVER)];
        char title[8] = {'\0'};
        if ((sys_cb.cover_index >= REMIND_COVER_ALARM) && (sys_cb.cover_index <= REMIND_COVER_HEALTH_DRINK))
        {
            if (sys_cb.cover_index == REMIND_COVER_ALARM)
            {

                //获取用户闹钟
                if (uteModuleSystemtimeGetAlarmRingIndex() != 0xff)
                {
                    uteModuleSystemtimeGetAlarm(alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
                }
//                alarm_p = &alarm;
                if (alarm_p->isRepeatRemindOpen)
                {
                    snprintf(title, sizeof(title), "%02d:%02d", alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                }
                else
                {
                    snprintf(title, sizeof(title), "%02d:%02d", alarm_p->hour, alarm_p->min);
                }
                //开启马达 喇叭
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
            }
            else if (sys_cb.cover_index == REMIND_COVER_FIND_WATCH)
            {
                //snprintf(msg, sizeof(msg), "寻找手表");
            }
//            else if (sys_cb.cover_index == COVER_HEALTH_DRINK || sys_cb.cover_index == COVER_HEALTH_SEDENTARY) {
//                tm_t tm = time_to_tm(RTCCNT);
//                snprintf(title, sizeof(title), "%02d:%02d", tm.hour, tm.min);
//            }
            mode = MSGBOX_MODE_BTN_REMIND_LATER_CLOSE;
        }

        int res = msgbox(msg, title, NULL, mode,  MSGBOX_MSG_TYPE_REMIND_COVER);


        if (res == MSGBOX_RES_REMIND_LATER)         //稍后提醒
        {
            if (sys_cb.cover_index == REMIND_COVER_ALARM)
            {
                printf("COVER_ALARM MSGBOX_RES_REMIND_LATER\n");
                //开启配置贪睡时钟 参数
                if (alarm_p != NULL)
                {
                    alarm_p->isRepeatRemindOpen = true;

                    uint8_t hour_later = alarm_p->repeatRemindHour;
                    uint8_t min_later = alarm_p->repeatRemindMin + ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN;
                    if (min_later > 59)
                    {
                        hour_later += 1;
                        min_later -= 60;
                    }

                    if (hour_later > 24)
                    {
                        hour_later -= 24;
                    }

                    alarm_p->repeatRemindHour = hour_later;
                    alarm_p->repeatRemindMin = min_later;
                    uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
                    printf("repeat alarm[%d] ring, [%02d:%02d]\n", uteModuleSystemtimeGetAlarmRingIndex(), alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                }
                //关闭 喇叭 马达
                uteDrvMotorStop();
            }
        }
        else if (res == MSGBOX_RES_EXIT)                   //强制退出弹窗
        {
            if (sys_cb.cover_index == REMIND_COVER_ALARM)
            {
                printf("COVER_ALARM MSGBOX_RES_EXIT\n");
                //关闭 喇叭 马达
                uteDrvMotorStop();
            }
        }
        else if (res == MSGBOX_RES_TIMEOUT_EXIT)            //提醒界面超时退出
        {
            if (sys_cb.cover_index == REMIND_COVER_ALARM)
            {
                printf("COVER_ALARM MSGBOX_RES_TIMEOUT_EXIT\n");
                //关闭 喇叭 马达
                uteDrvMotorStop();
            }
        }
        else
        {
            if (sys_cb.cover_index == REMIND_COVER_ALARM)
            {
                alarm_p->isRepeatRemindOpen = false;
                uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
                //关闭 喇叭 马达
                uteDrvMotorStop();
            }
        }

    }

    ab_free(alarm_p);
}
