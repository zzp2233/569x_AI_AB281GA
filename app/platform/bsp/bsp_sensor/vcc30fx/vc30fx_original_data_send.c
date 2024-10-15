/**********************************************************************************************
 * @Copyright(c) 2024, Chengdu vcaresensor Microelectronics Corporation. All rights reserved.
 * @Author: Tse
 * @Date: 2024-02-04 14:54:19
 * @LastEditTime: 2024-03-29 15:53:05
 * @LastEditors: Tse
 * @Description:
 **********************************************************************************************/
/* system include */
//#include "os_timer.h"
//#include <simple_ble_service.h>
//#include "wristband_global_data.h"
//extern RtkWristBandSysType_t RtkWristbandSys;
/* vcare include */
#include "vcare_device_com.h"
#include "vc30fx_sample_result.h"

static unsigned char ble_buf[2000] = {0};
static unsigned short int buf_size = 0;
static unsigned short int buf_index = 0;
#define BLE_PACK_SIZE 200
#define TIMER_BLE_GAP 10

static void debug_orginal_data_send(unsigned short int size);
/*********************************************************************************************************
 * 调试蓝牙数据格式：
 * [0~7]slot0_8bytes
 * {
 *  .[0~2]ppg_data(h,m,l)
 *  .[3]ppg_adcbit=(bit-16)/2
 *  .[4]resistance
 *  .[5]current
 *  .[6~7]env_bit14(h,l)
 * }
 * [ 8~15]slot1_8bytes{...}
 * [16~23]slot2_8bytes{...}
 * [24~25]bio0_2bytes(h,l)
 * [26~27]bio1_2bytes(h,l)
 * [28~29]ps_slot3_2bytes(h,l)
 * [30~35]gsensorx/y/z(h,l) or tempRefADC, tempExtADC, temperature
 * [36]hr
 * [37]spo2
 * [38]algo_mode
 * [39]wear_bioret(bit7) | algoret(bit6) | acceret(bit5) | wear_drv(bit3~bit0)
 *
 * #define FRAME_SIZE 40
 *********************************************************************************************************/
#define ORIGINAL_DATA_DEBUG_ENABLE (1)
#if ORIGINAL_DATA_DEBUG_ENABLE

#define HR_PPG_SLOT 0
#define IR_PPG_SLOT 1
#define RD_PPG_SLOT 2
#define PS_PPG_SLOT 3
/* 单采样帧字节数 */
#define FRAME_SIZE 40

extern void vc30fx_drv_get_wear_detail_result( unsigned char *bio_ret, unsigned char *algo_ret, unsigned char *acce_ret );

void vc30fx_send_orginal_data(vcare_ppg_device_t *pdev, gsensor_axes *pgsensor, unsigned char ppgsize)
{
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char slot_k = 0;
    unsigned char psfreq_index = 0;
    /* wear all result param */
    unsigned char bio_result=0, algo_result=0, acce_result=0;
    vc30fx_sample_info_t *presult = (vc30fx_sample_info_t *)pdev->result;
    memset(ble_buf, 0, sizeof(ble_buf));
    if( ppgsize>=64 )
    {
        ppgsize=64;
    }
    if( ppgsize>=sizeof(ble_buf)/FRAME_SIZE )
    {
        ppgsize=sizeof(ble_buf)/FRAME_SIZE;
    }
    vc30fx_drv_get_wear_detail_result( &bio_result, &algo_result, &acce_result );
    for (i = 0; i < ppgsize; i++)
    {
        j = 0;
        /* 0-23,slot_ppg：3*8=24 */
        for (slot_k = 0; slot_k < 3; slot_k++)
        {
            if (slot_k == 2)
            {
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = 0;
            }
            else
            {
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].ppg_buffer[i] >> 16) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].ppg_buffer[i] >> 8) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].ppg_buffer[i] >> 0) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = 0;
                ble_buf[i * FRAME_SIZE + j++] = presult->slot_result[slot_k].slot_resistance;
                ble_buf[i * FRAME_SIZE + j++] = presult->slot_result[slot_k].slot_current;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].env_data >> 8) & 0xff;
                ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[slot_k].env_data >> 0) & 0xff;
            }
        }
        /*24-29 bio:2*2 + ps：2 */
        psfreq_index = i / presult->ps_div;
        if (presult->extra_result.biodata_storefifo)
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_buffer[psfreq_index] >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_buffer[psfreq_index] >> 0) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_buffer[psfreq_index] >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_buffer[psfreq_index] >> 0) & 0xff;
        }
        else
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_data >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioinn_data >> 0) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_data >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->extra_result.bioext_data >> 0) & 0xff;
        }
        if (presult->slot_result[2].slot_storefifo)
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].ppg_buffer[psfreq_index] >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].ppg_buffer[psfreq_index] >> 0) & 0xff;
        }
        else
        {
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].u.ps_data >> 8) & 0xff;
            ble_buf[i * FRAME_SIZE + j++] = (presult->slot_result[2].u.ps_data >> 0) & 0xff;
        }
/* 30-35 xyz：2*3=6*/
#if 1
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->xData[i] >> 8;
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->xData[i] & 0xff;
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->yData[i] >> 8;
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->yData[i] & 0xff;
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->zData[i] >> 8;
        ble_buf[i * FRAME_SIZE + j++] = pgsensor->zData[i] & 0xff;
#else  /* temp result */
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempref_data >> 8;
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempref_data & 0xff;
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempext_data >> 8;
        ble_buf[i * FRAME_SIZE + j++] = presult->extra_result.tempext_data & 0xff;
        ble_buf[i * FRAME_SIZE + j++] = pdev->temperature >> 8;
        ble_buf[i * FRAME_SIZE + j++] = pdev->temperature & 0xff;
#endif
        /* 36-spo2,37-hr,38-algo_mode */
        ble_buf[i * FRAME_SIZE + j++] = pdev->heart_rate;
        ble_buf[i * FRAME_SIZE + j++] = pdev->spo2;
        ble_buf[i * FRAME_SIZE + j++] = pdev->heart_algo_mode;
        /* [39]wear_bioret(bit7) | algoret(bit6) | acceret(bit5) | wear_drv(bit3~bit0) */
        ble_buf[i * FRAME_SIZE + j++] = (bio_result<<7)|(algo_result<<6)|(acce_result<<5)| (pdev->wear & 0xf);
    }
//    debug_orginal_data_send(ppgsize * FRAME_SIZE);
}

#endif

//void vc30fx_send_orginal_data_debug_mark_data( void )
//{
//    for(int i=0; i<FRAME_SIZE; i++)
//    {
//        ble_buf[i]=0xff;
//    }
//    debug_orginal_data_send(FRAME_SIZE);
//}
//
///**********************************************************************************************************
// *	对苹果手机APP不能连续接收长段数据，利用软件定时器，进行分包发送
// *	创建软件定时器及数据分包处理
// */
//static void send_data_by_ble(unsigned char *pbuf, unsigned short int size)
//{
//    if (RtkWristbandSys.gap_conn_state == GAP_CONN_STATE_CONNECTED)
//    {
//        hr_ble_service_send_v3_notify(RtkWristbandSys.wristband_conn_id, hr_ser_id, pbuf, size);
//    }
//    VCARE_DBG_LOG("send_data_by_ble=%d", size);
//}

//static int timer_send_data_function(void)
//{
//    unsigned short int residue_size = 0;
//    // 数据发送已经完成
//    if (buf_index >= buf_size)
//    {
//        return 0;
//    }
//    // 判断剩余数据长度是否还需要分包
//    residue_size = buf_size - buf_index;
//    if (residue_size <= BLE_PACK_SIZE)
//    {
//        send_data_by_ble(&ble_buf[buf_index], residue_size);
//        return 0;
//    }
//    else
//    {
//        send_data_by_ble(&ble_buf[buf_index], BLE_PACK_SIZE);
//        buf_index += BLE_PACK_SIZE;
//        return 1;
//    }
//}

///* 创建软件定时器,如果数据太长通过软件定时器发送 */
//#define TIMER_ID 0xf0
//static void *debug_send_timer = NULL;
//static void debug_send_timer_start(void)
//{
//    if (debug_send_timer)
//    {
//        os_timer_restart(&debug_send_timer, TIMER_BLE_GAP);
//    }
//}
//static void debug_send_timer_stop(void)
//{
//    if (debug_send_timer)
//    {
//        os_timer_stop(&debug_send_timer);
//    }
//}
//
//static void debug_send_timer_callback(void *p_handle)
//{
//    if (timer_send_data_function())
//    {
//        /* 定时器继续循环 */
//        os_timer_restart(&debug_send_timer, TIMER_BLE_GAP);
//    }
//    else
//    {
//        /* 关闭定时器 */
//        debug_send_timer_stop();
//    }
//}

//void vc30fx_send_orginal_data_timer_init(void)
//{
//    if (debug_send_timer == NULL)
//    {
//        os_timer_create(&debug_send_timer, "debug_send_timer", TIMER_ID, TIMER_BLE_GAP, false, debug_send_timer_callback);
//    }
//    os_timer_restart(&debug_send_timer, TIMER_BLE_GAP);
//    os_timer_stop(&debug_send_timer);
//}
//
//static void debug_orginal_data_send(unsigned short int size)
//{
//    buf_index = 0;
//    buf_size = size;
//    VCARE_DBG_LOG("debug_orginal_data_send = %d", size);
//#if 1
//    if (buf_size <= BLE_PACK_SIZE)
//    {
//        send_data_by_ble(ble_buf, size);
//    }
//    else
//    {
//        send_data_by_ble(ble_buf, BLE_PACK_SIZE);
//        buf_index += BLE_PACK_SIZE;
//        debug_send_timer_start();
//    }
//#else
//    for (unsigned char i = 0; i * 8 < size; i++)
//    {
//        VCARE_DBG_LOG("[%d~%d]:%02x %02x %02x %02x , %02x %02x %02x %02x", i * 8, (i + 1) * 8,
//                   ble_buf[i * 8], ble_buf[i * 8 + 1], ble_buf[i * 8 + 2], ble_buf[i * 8 + 3], ble_buf[i * 8 + 4], ble_buf[i * 8 + 5], ble_buf[i * 8 + 6], ble_buf[i * 8 + 7]);
//    }
//#endif
//}
