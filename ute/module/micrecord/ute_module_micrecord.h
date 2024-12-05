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

void uteModuleMicRecordFactoryEnter(void);
void uteModuleMicRecordFactoryExit(void);
void uteModuleMicRecordFactoryStart(void);
bool uteModuleMicRecordFactoryIsHaveData(void);
void uteModuleMicRecordFactoryPlay(void);

#endif //_UTE_MODULE_CRC_H_

