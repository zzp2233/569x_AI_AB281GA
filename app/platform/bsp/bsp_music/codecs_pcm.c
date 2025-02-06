/*
 *  codecs_pcm.c
 *
 *  Created by simon on 2022.6.9
 *  各种音频格式解码后PCM输出接口, 解码后的pcm算法处理统一在此文件调用。
 *
 */
#include "include.h"


void mp3_pcm_out_start(void) {}
AT(.mp3dec.pcm)
void mp3_pcm_out_process(u32 samples) {}
