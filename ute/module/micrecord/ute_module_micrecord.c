/**
*@file
*@brief        录音模块
*@details
*@author       zn.zeng
*@date       2021-12-21
*@version      v1.0
*/

#include "include.h"
#include "ute_module_micrecord.h"
#include "ute_module_log.h"
#include "ute_module_charencode.h"
#include "ute_module_systemtime.h"
// #include "ute_module_music.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if MIC_TEST_EN

#define MIC_TEST_VOL        (VOL_MAX / 2)
#define MIC_BUF_IS_FULL     mic_cb.rec_buf_full

//mic数据缓冲区
typedef struct {
    u8 rec_buf[MIC_TEST_BUF_SIZE];
    u32 rec_datalen;
    bool rec_buf_full;
    volatile u32 samples;
    volatile int ch_mode;
} mic_testbuf_t;

AT(.mic_test_buf)
mic_testbuf_t mic_cb;

AT(.com_text.str)
char mic_test_str[] = "data full!!!\n";

AT(.com_text.func.mic_test)
bool put_mic_obuf(u8 *ptr, u32 samples, int ch_mode)
{
    mic_cb.samples = samples;
    mic_cb.ch_mode = ch_mode;

    if (mic_cb.rec_datalen > MIC_TEST_BUF_SIZE - samples * 2) {
        TRACE(mic_test_str);
        return true;
    }

    memcpy(&mic_cb.rec_buf[mic_cb.rec_datalen], ptr, samples * 2);
    mic_cb.rec_datalen += samples * 2;
    return false;
}

AT(.com_text.func.mic_test)
void mic_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    if (mic_cb.rec_buf_full == false && put_mic_obuf(ptr, samples, ch_mode)) {
        mic_cb.rec_buf_full = true;
    }
}

void mic_test_init(void)
{
    TRACE("-->%s\n",__func__);
    bt_audio_bypass();  //复用aram，bypass蓝牙音乐
    memset(&mic_cb, 0, sizeof(mic_testbuf_t));
}

void mic_test_start(void)
{
    TRACE("-->%s\n",__func__);
    mic_cb.rec_datalen = 0;
    mic_cb.rec_buf_full = false;
    audio_path_init(AUDIO_PATH_SPEAKER);
    audio_path_start(AUDIO_PATH_SPEAKER);
}

void mic_test_exit(void)
{
    TRACE("-->%s\n",__func__);
    audio_path_exit(AUDIO_PATH_SPEAKER);
    bt_audio_enable();
}

void mic_test_outp(void)
{
    TRACE("-->%s\n",__func__);
    bool mute_bkp;
    bsp_change_volume(MIC_TEST_VOL);
    mute_bkp = bsp_get_mute_sta();
    if (mute_bkp) {
        bsp_sys_unmute();
    } else {
        dac_fade_in();
    }
    dac_spr_set(SPR_16000);

//    u32 tick = tick_get();
    for (int i = 0; i < mic_cb.rec_datalen; i += 2) { //阻塞播放
        s16 *ptr16 = (s16 *)&mic_cb.rec_buf[i];
        obuf_put_one_sample(*ptr16, *ptr16);
    }
//    printf("tick[%d]\n", tick_get() - tick);

    dac_fade_out();
    dac_fade_wait();

    bsp_change_volume(sys_cb.vol);
    if (mute_bkp) {
        bsp_sys_mute();
    }
}

#else

#define MIC_BUF_IS_FULL     0
#define mic_test_init()
#define mic_test_start()
#define mic_test_exit()
#define mic_test_outp()

#endif  //MIC_TEST_EN

void uteModuleMicRecordFactoryEnter(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL,"%s",__func__);
    mic_test_init();
}

/**
*@brief  工厂模式结束测试
*@details
*@author        zn.zeng
*@date        2022-01-10
*/
void uteModuleMicRecordFactoryExit(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL,"%s",__func__);
    mic_test_exit();
}

/**
*@brief  工厂模式开始测试
*@details
*@author        zn.zeng
*@date        2021-12-21
*/
void uteModuleMicRecordFactoryStart(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL,"%s",__func__);
    mic_test_start();
}

bool uteModuleMicRecordFactoryIsHaveData(void)
{
    return mic_cb.rec_buf_full;
}

void uteModuleMicRecordFactoryPlay(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL,"%s",__func__);
    mic_test_outp();
}
