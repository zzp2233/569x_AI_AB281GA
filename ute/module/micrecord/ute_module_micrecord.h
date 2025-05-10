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

void uteModuleMicRecordFactoryEnter(void);
void uteModuleMicRecordFactoryExit(void);
void uteModuleMicRecordFactoryStart(void);
void uteModuleMicRecordFactoryPlay(void);
void uteModuleMicRecordFactoryPlayStart(void);
uint8_t uteModuleMicRecordFactoryGetrecordState(void);
void uteModuleMicRecordFactoryWriteDataToFlash(void);
void uteModuleMicRecordFactorySetrecordState(uint8_t state);

#endif //_UTE_MODULE_CRC_H_

