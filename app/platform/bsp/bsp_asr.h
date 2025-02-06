#ifndef __BSP_ASR_H
#define __BSP_ASR_H

void bsp_speech_recognition_enter(void);
void bsp_speech_recognition_process(void);
void bsp_asr_init(void);
void bsp_asr_start(void);
void bsp_asr_stop(void);
u8 bsp_asr_init_sta(void);
void thread_asr_create(void);
void bsp_asr_vddon(void);
void bsp_asr_tone_play(u32 addr, u32 len);
void bsp_asr_func_switch(u8 func);

/* 华镇 相关函数*/

/* 友杰 相关函数*/
void et_init(void);
void et_start_asr();
void et_stop_asr();
void set_et_frame_refreshing(uint8_t flag);
int third_func_message(u16 msg);
int third_func_process(void);
#endif
