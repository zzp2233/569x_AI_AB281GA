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
#include "ute_module_message.h"
// #include "ute_module_music.h"

#define TRACE_EN UTE_LOG_MICRECORD_LVL

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FACTORY_RECORD_DATA_START (UTE_OTA_TMP_ADDRESS + 100 * 1024)
#define FACTORY_RECORD_DATA_LENGTH (128 * 1024)
#define FACTORY_RECORD_DATA_END (FACTORY_RECORD_DATA_START + FACTORY_RECORD_DATA_LENGTH)

#if (FACTORY_RECORD_DATA_LENGTH + 100 * 1024) > UTE_OTA_TMP_SIZE
#error "FACTORY_RECORD_DATA_LENGTH > UTE_OTA_TMP_SIZE"
#endif

#define RECORD_FLASH_BUF_LENGTH 4096

AT(.mic_test_buf) // 复用aram
static uint8_t record_flash_buf[RECORD_FLASH_BUF_LENGTH];

void *uteModuleMicRecordTimerPointer = NULL;

typedef struct
{
    uint16_t record_flash_buf_write_count;
    uint32_t record_flash_data_write_length;
    uint16_t record_flash_buf_read_count;
    uint32_t record_flash_data_read_length;
    uint8_t record_state;
} factory_test_earphone_data_t;

static factory_test_earphone_data_t factory_test_earphone_data;

bool factory_record_addr_check(uint32_t addr)
{
#if 0
    TRACE("%s, start %x, end %x, length %x\n",
          __func__,
          FACTORY_RECORD_DATA_START,
          FACTORY_RECORD_DATA_END,
          FACTORY_RECORD_DATA_LENGTH);
#endif

    if (addr < FACTORY_RECORD_DATA_START)
    {
        return false;
    }

    if (addr > FACTORY_RECORD_DATA_END)
    {
        return false;
    }

    return true;
}

void factory_record_erase(uint32_t addr)
{
    bool check = factory_record_addr_check(addr);
    if (check == false)
    {
        printf("erase addr error");
        return;
    }

    os_spiflash_erase(addr);
}

void factory_record_write(uint8_t *buffer, uint32_t addr, uint32_t length)
{
    bool check = factory_record_addr_check(addr);
    if (check == false)
    {
        printf("write addr error");
        return;
    }

    os_spiflash_program(buffer, addr, length);
}

void factory_record_read(uint8_t *buffer, uint32_t addr, uint32_t length)
{
    bool check = factory_record_addr_check(addr);
    if (check == false)
    {
        printf("read addr error");
        return;
    }

    os_spiflash_read(buffer, addr, length);
}

void factory_test_init_record_data()
{
    memset(&factory_test_earphone_data, 0, sizeof(factory_test_earphone_data_t));
    memset(record_flash_buf, 0, RECORD_FLASH_BUF_LENGTH);

    for (uint32_t addr = FACTORY_RECORD_DATA_START;
         addr < (FACTORY_RECORD_DATA_START + FACTORY_RECORD_DATA_LENGTH);
         addr += 4096)
    {
        TRACE("%s, erase start %x,length %d, addr %x\n",
              __func__,
              FACTORY_RECORD_DATA_START,
              4096,
              addr);

        factory_record_erase(addr);
    }
}

void uteModuleMicRecordFactoryWriteDataToFlash(void)
{
    uint32_t write_addr = FACTORY_RECORD_DATA_START + factory_test_earphone_data.record_flash_data_write_length;

    factory_record_write(record_flash_buf, write_addr, RECORD_FLASH_BUF_LENGTH);

    factory_test_earphone_data.record_flash_data_write_length += RECORD_FLASH_BUF_LENGTH;
}

AT(.com_text.mic_test)
void factory_test_write_record_data(uint8_t *data, uint32_t len)
{
    if (factory_test_earphone_data.record_state != FACTORY_TEST_RECORD_RECORDING)
    {
        return;
    }
    //   printf("write data:\n");
    //   print_r(data, 256);
    memcpy(&record_flash_buf[factory_test_earphone_data.record_flash_buf_write_count], data, len);
    factory_test_earphone_data.record_flash_buf_write_count += len;
    if (factory_test_earphone_data.record_flash_buf_write_count >= RECORD_FLASH_BUF_LENGTH)
    {
        factory_test_earphone_data.record_flash_buf_write_count = 0;
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_FACTORY_WRITE_RECORD_DATA_TO_FLASH, 0);
    }
}

uint8_t *factory_test_read_record_data(uint32_t len)
{
    if (factory_test_earphone_data.record_flash_data_read_length > factory_test_earphone_data.record_flash_data_write_length)
    {
        return NULL;
    }

    if (factory_test_earphone_data.record_flash_buf_read_count == 0)
    {
        uint32_t write_addr = FACTORY_RECORD_DATA_START + factory_test_earphone_data.record_flash_data_read_length;
        factory_record_read(record_flash_buf, write_addr, RECORD_FLASH_BUF_LENGTH);
        factory_test_earphone_data.record_flash_data_read_length += RECORD_FLASH_BUF_LENGTH;
    }
    uint8_t *ret = &record_flash_buf[factory_test_earphone_data.record_flash_buf_read_count];
    factory_test_earphone_data.record_flash_buf_read_count += len;
    if (factory_test_earphone_data.record_flash_buf_read_count == RECORD_FLASH_BUF_LENGTH)
    {
        factory_test_earphone_data.record_flash_buf_read_count = 0;
    }

    return ret;
}

#if MIC_TEST_EN

AT(.com_text.str)
char mic_test_str[] = "data full!!!\n";

AT(.com_text.func.mic_test)
void mic_test_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    factory_test_write_record_data(ptr, samples * 2);
}

void mic_test_init(void)
{
    TRACE("-->%s\n", __func__);
    bt_audio_bypass(); // 复用aram，bypass蓝牙音乐
}

void mic_test_start(void)
{
    printf("-->%s\n", __func__);
    audio_path_init(AUDIO_PATH_SPEAKER);
    audio_path_start(AUDIO_PATH_SPEAKER);
    dac_fade_in();
}

void mic_test_exit(void)
{
    TRACE("-->%s\n", __func__);
    audio_path_exit(AUDIO_PATH_SPEAKER);
    bt_audio_enable();
    bsp_change_volume(sys_cb.vol);
    dac_fade_out();
    delay_5ms(2);
    bsp_loudspeaker_mute();
}

void uteModuleMicRecordFactoryPlay(void)
{
    TRACE("-->%s\n", __func__);
    bool mute_bkp;
    bsp_change_volume(UTE_MODULE_MIC_FACTORY_TEST_PLAY_VOLUME);
    mute_bkp = bsp_get_mute_sta();
    if (mute_bkp)
    {
        bsp_sys_unmute();
    }
    else
    {
        dac_fade_in();
    }
    dac_spr_set(SPR_16000);

    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_PLAYING;

    //    u32 tick = tick_get();
    while (1)
    {
        uint8_t *ptr = factory_test_read_record_data(128 * 2);
        if (!ptr)
        {
            break;
        }

        WDT_CLR();

        for (int i = 0; i < 256; i += 2)
        {
            s16 *ptr16 = (s16 *)&ptr[i];
            obuf_put_one_sample(*ptr16, 0);
        }
    }
    //    printf("tick[%d]\n", tick_get() - tick);

    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_IDLE;
    factory_test_earphone_data.record_flash_data_read_length = 0;

    bsp_change_volume(sys_cb.vol);
    if (mute_bkp)
    {
        bsp_sys_mute();
    }
    // else {
    //     dac_fade_out();
    //     dac_fade_wait();
    // }
}

#else

#define MIC_BUF_IS_FULL 0
#define mic_test_init()
#define mic_test_start()
#define mic_test_exit()
#define mic_test_outp()

#endif // MIC_TEST_EN

void uteModuleMicRecordFactoryEnter(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL, "%s", __func__);
    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_IDLE;
    mic_test_init();
    // 提前打开dac，防止播放时阻塞导致dac打开失败
    bsp_change_volume(UTE_MODULE_MIC_FACTORY_TEST_PLAY_VOLUME);
    bool mute_bkp;
    mute_bkp = bsp_get_mute_sta();
    if (mute_bkp)
    {
        bsp_sys_unmute();
    }
    else
    {
        dac_fade_in();
    }
    dac_spr_set(SPR_16000);
}

/**
 *@brief  工厂模式结束测试
 *@details
 *@author        zn.zeng
 *@date        2022-01-10
 */
void uteModuleMicRecordFactoryExit(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL, "%s", __func__);
    mic_test_exit();
}

void uteModuleMicRecordFactoryTimerCallback(void *pxTimer)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL, "%s", __func__);
    uteModulePlatformDeleteTimer(&uteModuleMicRecordTimerPointer);
    uteModuleMicRecordTimerPointer = NULL;
    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_RECORDED;
}

/**
 *@brief  工厂模式开始测试
 *@details
 *@author        zn.zeng
 *@date        2021-12-21
 */
void uteModuleMicRecordFactoryStart(void)
{
    printf( "%s", __func__);
    factory_test_init_record_data();
    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_RECORDING;
    if (uteModuleMicRecordTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleMicRecordTimerPointer, "MicRecord", 1, UTE_MODULE_MIC_FACTORY_TEST_RECORDING_TIME * 1000, false, uteModuleMicRecordFactoryTimerCallback);
    }
    uteModulePlatformRestartTimer(&uteModuleMicRecordTimerPointer, UTE_MODULE_MIC_FACTORY_TEST_RECORDING_TIME * 1000);
    mic_test_start();
    // printf("1111111111111\r\n");
}

/**
 * @brief        工厂模式开始播放测试数据
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-05-10
 */
void uteModuleMicRecordFactoryPlayStart(void)
{
    UTE_MODULE_LOG(UTE_LOG_MICRECORD_LVL, "%s", __func__);
    factory_test_earphone_data.record_state = FACTORY_TEST_RECORD_PLAYING;
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_FACTORY_PLAY_RECORD, 0);
}

/**
 * @brief        工厂模式获取录音状态
 * @details
 * @return       uint8_t
 * @author       Wang.Luo
 * @date         2025-05-10
 */
uint8_t uteModuleMicRecordFactoryGetRecordState(void)
{
    return factory_test_earphone_data.record_state;
}

void uteModuleMicRecordFactorySetrecordState(uint8_t state)
{
    factory_test_earphone_data.record_state = state;
}
