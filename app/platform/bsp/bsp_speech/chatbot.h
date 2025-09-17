#ifndef CHATBOT_H__
#define CHATBOT_H__

typedef enum
{
    CHATEVT_IS_CONN,
    CHATEVT_EXIT,

    CHATEVT_TTS_PLAYING,
    CHATEVT_LISTENING,
    CHATEVT_THINKING,


    CHATEVT_ERROR_RECONN,
    CHATEVT_ERROR_BROKEN,
} chatbot_event_t;

typedef enum
{
    CHATMCP_IDLE,
    CHATMCP_POWERUP,
    CHATMCP_POWERDOWN,
} chatbot_mcp_t;

typedef void (*chatbot_text_callback_t)(const char *str);
typedef void (*chatbot_event_callback_t)(chatbot_event_t event);
typedef void (*chatbot_mcp_callback_t)(chatbot_mcp_t event, void *param);

//void chatbot_start_mic(void);
//void chatbot_stop_mic(void);
bool chatbot_init(bool is_foreign);
void chatbot_deinit(void);
void chatbot_start(void);
void chatbot_set_default_vol(u8 vol);
//void chatbot_stop(void);
void chatbot_set_text_callback(chatbot_text_callback_t cb);
void chatbot_set_event_callback(chatbot_event_callback_t cb);
void chatbot_set_mcp_callback(chatbot_mcp_callback_t cb);
void chatbot_set_mac(bd_addr_t *mac);
void chatbot_send_vape_record(u32 today_puff, u32 week_puff);


#endif
