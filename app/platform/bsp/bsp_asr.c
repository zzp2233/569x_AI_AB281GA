#include "include.h"

#if ASR_SELECT

#if (ASR_SELECT == ASR_YJ && ET_WAKEUP_EN)
#include <lib_et_asr.h>
#endif // ET_WAKEUP_EN

#define HUART_DUMP_ADC_PCM 0
#define MEASURE_IO 0

#if REC_DATA_SPI_PRINT
AT(.com_text.ret.bb.putc)
void user_spi_putc(u8 ch)
{
    GLOBAL_INT_DISABLE();
    u8 i;
    for(i=0; i<8; i++)
    {
        GPIOBCLR = BIT(3);
        if(ch & 0x80)
        {
            GPIOBSET = BIT(4);
        }
        else
        {
            GPIOBCLR = BIT(4);
        }
        ch <<= 1;
        GPIOBSET = BIT(3);
    }
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    GLOBAL_INT_RESTORE();
}
#endif // REC_DATA_SPI_PRINT

#define RECORD_BUFFER_SIZE              0x1000      //mic数据流缓存大小

#if (ASR_SELECT == ASR_WS)
#define SPEECH_BUF_SIZE                 640 * 2     //从数据流中获取1280byte数据，每个样点2byte
#elif (ASR_SELECT == ASR_YJ)
#define SPEECH_BUF_SIZE                 ET_VAD_ONCE_BUF_SIZE * 4 * 2 //从数据流中获取1280byte数据，每个样点2byte
#endif

//通用数据流管理
au_stm_t record_pcm_stream AT(.asr_record);

//数据流内部缓存4K byte
u8 rec_obuf[RECORD_BUFFER_SIZE] AT(.asr_record);

//取数据临时buffer
unsigned char speech_buf[SPEECH_BUF_SIZE] AT(.asr_record);

void huart_init_do(u8 tx_port, u8 rx_port, u32 baud_rate, u8 *buf, u16 buf_size);
#define AUBUFDATA       SFR_RW (SFR1_BASE + 0x01*4)

volatile u8 asr_init_flag;
volatile u8 asr_api_sta;

#if (ET_ASR_VAD_EN)

void et_asr_vad_callback(int16_t *ptr, int32_t samples, uint8_t step)
{
    et_asr_wakeup_buf_write((int16_t*)ptr, samples, step);
}

void et_asr_wordstart()
{
    sys_clk_set(SYS_192M);
    printf("word start\n");

    //PWM need reinit
#if HUART_EN
    //need reinit
    //udet_init(1500000);
#endif // HUART_EN
#if USER_UART0_EN
    //udet_init(115200);
#endif // USER_UART0_EN
}

void et_asr_wordend()
{
    printf("word end\n");
#if 0
    if(!is_et_frame_refreshing())
    {
        sys_clk_set(SYS_88M);
    }
#endif // 0
    //PWM need reinit
#if HUART_EN
    //need reinit
    //udet_init(1500000);
#endif // HUART_EN
#if USER_UART0_EN
    //udet_init(115200);
#endif // USER_UART0_EN
}

#endif // ET_ASR_VAD_EN

//在此安全获取原始mic数据(main线程)
AT(.text.record)
void bsp_speech_recognition_process(void)
{
    if (asr_init_flag == 0)
    {
        return;
    }

#if ASR_API_CHECK_TIME
    u32 start_time = tick_get();
#endif

    u8 *buf = speech_buf;
    if (!gets_stm_buf(&record_pcm_stream, buf, SPEECH_BUF_SIZE))  //从数据流中获取1280byte数据，每个样点2byte
    {
        // printf("pipe line data less than 640 * 2 byte\n");
        return;
    }

#if HUART_DUMP_ADC_PCM
    // huart_tx(buf, 1280);
    huart_tx(buf, ET_VAD_ONCE_BUF_SIZE * 4 * 2);
    //asr_huart_putcs(buf, ET_VAD_ONCE_BUF_SIZE * 4 * 2);
#endif

#if 0
    //直推DAC，测试mic原始数据
    u16 i;
    u32 sample;
    u16 *rptr = (u16 *)buf;
    for(i = 0; i < 256; i++)
    {
        sample = rptr[i];
        sample = (sample << 16) | sample;
        AUBUFDATA = sample;
    }
#endif // 0

#if REC_DATA_SPI_PRINT
    //SPI输出，测试mic原始数据
    for(int i = 0; i < 512 ; i++)
    {
        user_spi_putc(buf[i]);
    }
#endif // REC_DATA_SPI_PRINT

    //这里调用识别库:
#if MEASURE_IO
    GPIOB &= ~BIT(3);
#if (ASR_SELECT == ASR_WS)          //华镇
    asr_alg_process((int16_t *)buf);
#elif (ASR_SELECT == ASR_YJ)        //友杰
    //----------------------------------------------------------------
#if 0
    extern int getPcmDB(int16_t* data, int size);
    int tempDb = getPcmDB((int16_t*)buf, ET_VAD_ONCE_BUF_SIZE * 4);
    printf("tempDb:%d\n", tempDb);
#endif // 0
#if ET_ASR_VAD_EN
    et_bsp_vad_detect(buf, ET_VAD_ONCE_BUF_SIZE * 4, ET_VAD_ONCE_BUF_SIZE, et_asr_vad_callback, et_asr_wordstart, et_asr_wordend);
#else
    et_asr_buf_write((int16_t*)buf, ET_VAD_ONCE_BUF_SIZE * 4, 3);
#endif // ET_ASR_VAD_EN
    //-----------------------------------------------------------------
#endif
    GPIOB |= BIT(3);
#else
#if (ASR_SELECT == ASR_WS)          //华镇
    asr_alg_process((int16_t *)buf);
#if ASR_API_CHECK_TIME
    printf("asr_ws_time:%d\n", tick_get() - start_time);
#endif //ASR_API_CHECK_TIME
#elif (ASR_SELECT == ASR_YJ)        //友杰
#if 0
    extern int getPcmDB(int16_t* data, int size);
    int tempDb = getPcmDB((int16_t*)buf, ET_VAD_ONCE_BUF_SIZE * 4);
    printf("tempDb:%d\n", tempDb);
#endif // 0
#if ET_WAKEUP_EN
#if ET_ASR_VAD_EN
    et_bsp_vad_detect((int16_t*)buf, ET_VAD_ONCE_BUF_SIZE * 4, ET_VAD_ONCE_BUF_SIZE, et_asr_vad_callback, et_asr_wordstart, et_asr_wordend);
#else
    et_asr_buf_write((int16_t*)buf, ET_VAD_ONCE_BUF_SIZE * 4, 3);
#endif // ET_ASR_VAD_EN
#endif // ET_WAKEUP_EN
#if ASR_API_CHECK_TIME
    printf("asr_yj_time:%d\n", tick_get() - start_time);
#endif //ASR_API_CHECK_TIME
#endif // ASR_SELECT
#endif // MEASURE_IO
}

//mic数据输入数据流
AT(.com_text.record)
bool puts_mic_obuf(u8 *inbuf, u16 len)
{
    bool puts_ok = puts_stm_buf(&record_pcm_stream, inbuf, len);
    return puts_ok;
}

//sdadc数据回调，不做耗时操作（aupcm线程）
AT(.com_text.sdadc)
void bsp_speech_recognition_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    puts_mic_obuf(ptr, (u16)(samples << (1 + ch_mode)));
}

//用于语音识别纯净mic数据采集
void bsp_speech_recognition_enter(void)
{
    printf(">>>enter %s\n", __func__);
#if REC_DATA_SPI_PRINT
    //逻辑分析仪连接时应当多点共地、直接连接、最短回流路径
    //SPI CLK   PB3
    GPIOBDE |= BIT(3);
    GPIOBFEN &= ~BIT(3);
    GPIOBDIR &= ~BIT(3);
    GPIOBSET = BIT(3);

    //SPI DATA  PB4
    GPIOBDE |= BIT(4);
    GPIOBFEN &= ~BIT(4);
    GPIOBDIR &= ~BIT(4);
    GPIOBSET = BIT(4);
#endif // REC_DATA_SPI_PRINT

    //数据流初始化
    memset(&record_pcm_stream, 0, sizeof(record_pcm_stream));
    record_pcm_stream.buf = 0;
    record_pcm_stream.buf = record_pcm_stream.rptr = record_pcm_stream.wptr = rec_obuf;
    record_pcm_stream.size = REC_OBUF_SIZE;

#if (ASR_SELECT == ASR_WS)      //初始化华镇语音识别库
#if WS_DONGLE_AUTH_EN
    bool check_uid_entryption(void);
    if (check_uid_entryption() == false)
    {
        printf("Dongle authorization verification failed!\n");
    }
    else
    {
        printf("Dongle authorization verification successful!\n");
    }
#endif
    asr_alg_start();
#elif (ASR_SELECT == ASR_YJ)    //初始化友杰语音识别库
#if ET_WAKEUP_EN
    et_start_asr();
#endif // ET_WAKEUP_EN
#endif

#if HUART_DUMP_ADC_PCM
    huart_init_do(HUART_TR_PB3, HUART_TR_PF5, 3000000, NULL, 0);
#endif

#if MEASURE_IO
    GPIOBDE |= BIT(3);
    GPIOBFEN &= ~BIT(3);
    GPIOBDIR &= ~BIT(3);
    GPIOBSET = BIT(3);
#endif
}

void os_task_sleep(uint32_t ms);
void thread_asr_entry(void *param)
{
    printf("enter asr thread\n");
    // bsp_speech_recognition_enter();
    // os_task_sleep(1000);

    while (1)
    {
        bsp_speech_recognition_process();
    }
}

///创建asr线程
void thread_asr_create(void)
{
    static os_thread_t pthd_asr;
    pthd_asr = os_thread_create(
                   "asr",
                   thread_asr_entry,
                   0,
                   OS_THREAD_ASR_STACK,
                   OS_THREAD_ASR_PRIORITY,
                   OS_THREAD_ASR_TICK);

    if (pthd_asr != 0)
    {
        printf("startup asr thread\n");
        os_thread_startup(pthd_asr);
    }
#if (ASR_SELECT == ASR_YJ)
    int8_t tem[8];
    extern void matmul_s(int8_t *A, int8_t *B, int32_t *C, int A_rows, int A_cols, int B_cols);
    matmul_s(tem, tem, (int32_t*)tem, 1, 8, 1);
#endif
}

// void asr_loop_test(void) {
//     printf("enter asr loop test\n");
//     bsp_speech_recognition_enter();

//     while (1) {
//         WDT_CLR();
//         bsp_speech_recognition_process();
//     }
// }

void bsp_asr_vddon(void)
{
    dac_set_power_on_off(1);
}

//初始化
void bsp_asr_init(void)
{
#if (ASR_SELECT == ASR_YJ)
    et_init();
#endif
#if ASR_FULL_SCENE
    bsp_asr_start();
#endif
}

//开始
void bsp_asr_start(void)
{
    if (asr_init_flag)
    {
        return;
    }

    printf("bsp_asr_start\n");
    CLKGAT1 |= BIT(4)|BIT(5)|BIT(27);
    CLKGAT2 |= BIT(18);

    audio_path_init(AUDIO_PATH_ASR);
    audio_path_start(AUDIO_PATH_ASR);
    bsp_asr_vddon();
    bsp_speech_recognition_enter();
    asr_init_flag = true;
}

//结束
void bsp_asr_stop(void)
{
    if ((!asr_init_flag))
    {
        return ;
    }
    printf("bsp_asr_stop\n");
    if(get_music_dec_sta() == MUSIC_PLAYING ||(sys_cb.sco_state))
    {

    }
    else
    {
        dac_fade_out();
    }
    dac_aubuf_clr();
    audio_path_exit(AUDIO_PATH_ASR);
#if (ASR_SELECT == ASR_WS)
    asr_alg_stop();
#elif (ASR_SELECT == ASR_YJ)
    et_stop_asr();
#endif
    asr_init_flag = false;
}

u8 bsp_asr_init_sta(void)
{
    return asr_init_flag;
}

//播放提示音
void bsp_asr_tone_play(u32 addr, u32 len)
{
    sys_cb.asr_gain = sys_cb.gain_offset;
    dac_set_anl_offset(0);
    func_bt_mp3_res_play(addr, len);
    dac_set_anl_offset(sys_cb.asr_gain);
}

//切换任务
void bsp_asr_func_switch(u8 func)
{
    if (func_cb.sta != func)
    {
        func_switch_to(func, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
    }
}

#endif //ASR_SELECT