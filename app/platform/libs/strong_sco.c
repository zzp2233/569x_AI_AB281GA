/**********************************************************************
*
*   strong_sco.c
*   定义库里面通话算法部分WEAK函数的Strong函数，动态关闭库代码
***********************************************************************/
#include "include.h"

/*****************************************************************************
 * Module    : AEC算法强定义
 *****************************************************************************/
#if !BT_AEC_EN
AT(.com_text.isr.aec)
void aec_isr(void) {}
AT(.bt_voice.aec)
void aec_nlms_process(void) {}
AT(.bt_voice.aec)
void aec_nlp_process(void) {}
void aec_init(void) {}
#endif

#if !BT_ALC_EN
void alc_init(void) {}
void alc_fade_in(s16 *buf) {}
void alc_fade_out(s16 *buf) {}
AT(.bt_voice.alc)
void bt_alc_process(u8 *ptr, u32 samples, int ch_mode) {};
#endif

#if !BT_SCO_AINS3_EN
void bt_ains3_init(void *alg_cb) {}
void bt_ains3_exit(void) {}
void bt_ains3_nr_process(void) {}
#endif

#if !BT_SCO_DNN_EN
void bt_dnn_init(void *alg_cb) {}
void bt_dnn_exit(void) {}
void bt_dnn_nr_process(void) {}
AT(.com_text.bt_sco)
bool bt_sco_dnn_is_en(void) {return false;}
void dnn_far_upsample(s16 *out, s16 *in, u32 samples, u8 step) {}
u32 dnn_near_downsample(s16 *ptr, u32 samples) {return 0;}
#endif


