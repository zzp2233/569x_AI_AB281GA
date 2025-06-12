/**
*@file
*@brief        录音模块头文件
*@details
*@author       zn.zeng
*@date       2021-12-21
*@version      v1.0
*/
#ifndef _UTE_MODULE_MICRECORD_H_
#define _UTE_MODULE_MICRECORD_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "include.h"
enum
{
    FACTORY_TEST_RECORD_IDLE,
    FACTORY_TEST_RECORD_RECORDING,   //录音中
    FACTORY_TEST_RECORD_RECORDED,    //录音完成
    FACTORY_TEST_RECORD_PLAYING,
};
//mic?????????
typedef struct
{
    u8 rec_buf[MIC_TEST_BUF_SIZE];
    u32 rec_datalen;
    bool rec_buf_full;
    volatile u32 samples;
    volatile int ch_mode;
} mic_testbuf_t;

void uteModuleMicRecordFactoryEnter(void);
void uteModuleMicRecordFactoryExit(void);
void uteModuleMicRecordFactoryStart(void);
bool uteModuleMicRecordFactoryIsHaveData(void);
void uteModuleMicRecordFactoryPlay(void);
mic_testbuf_t *uteModuleMicRecordFactoryGetMicBuf(void);
bool uteModuleMicRecordFactoryIsPlaying(void);
bool uteModuleMicRecordFactoryIsRecording(void);
void uteModuleMicRecordFactorySetrecordState(uint8_t state);
uint8_t uteModuleMicRecordFactoryGetRecordState(void);
#endif //_UTE_MODULE_CRC_H_

