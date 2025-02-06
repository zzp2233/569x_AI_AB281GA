#ifndef _MUSIC_RES_H_
#define _MUSIC_RES_H_

void mp3_res_play(u32 addr, u32 len);
void wav_res_play(u32 addr, u32 len);
void wsbc_res_play(u32 addr, u32 len);

bool bsp_res_music_play(u32 addr, u32 len);         //播放RES MUSIC
void bsp_res_play_exit_cb(uint8_t res_idx);

#endif // _MUSIC_RES_H_
