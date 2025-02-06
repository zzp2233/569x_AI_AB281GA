#ifndef _WATNING_PLAY_H
#define _WATNING_PLAY_H
#if 0

#include "piano.h"
#include "../codec/wsbc.h"

#define  MAX_SERIAL_SIZE        (60)  //60=24kbps, 40=16kbps
#define  SAMPLES_LEN            320
#define  CACHE_BUF_SIZE         240

typedef struct
{
    ring_buf_t res_ring_buf;
    s16 obuf[SAMPLES_LEN];          //ʹ��gpdma����Ҫ���ֽڶ���
    u8 ibuf[CACHE_BUF_SIZE];
    u8 frame_size;
    u16 samples;
    ///���ڼ�¼����ƴ�ӵ���Դ�ļ���������
    u8 *res_ptr;
    u32 res_size;
    u32 rd_pos;
} warning_cb_t;

extern warning_cb_t warning_cb;


void warning_dec_check_kick(void);
void warning_dec_proc(void);

int warning_play_init(u8 *ptr, u32 size, bool kick_start);
void warning_play_kick(void);
int warning_play_proc(void);
void warning_play_exit(void);

#endif //_WATNING_PLAY_H
#endif
