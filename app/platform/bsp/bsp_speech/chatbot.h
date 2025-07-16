#ifndef CHATBOT_H__
#define CHATBOT_H__

typedef enum
{
    CHATEVT_IS_CONN,
    CHATEVT_EXIT,

    CHATEVT_TTS_PLAYING,
    CHATEVT_LISTENING,

    CHATEVT_ERROR_RECONN,
    CHATEVT_ERROR_BROKEN,
} chatbot_event_t;

typedef void (*chatbot_text_callback_t)(const char *str);
typedef void (*chatbot_event_callback_t)(chatbot_event_t event);

void chatbot_start_mic(void);
void chatbot_stop_mic(void);
bool chatbot_init(void);
void chatbot_deinit(void);
void chatbot_start(void);
void chatbot_stop(void);
void chatbot_set_text_callback(chatbot_text_callback_t cb);
void chatbot_set_event_callback(chatbot_event_callback_t cb);

#endif
