/**
*@file
*@brief     模块系统消息头文件
*@details   系统消息写在此文件
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/

#ifndef _UTE_MODULE_MESSAGE_H_
#define _UTE_MODULE_MESSAGE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_task_application.h"

/*!消息枚举，没有的功能也保持，枚举内容不能使用宏控制 zn.zeng, 2021-07-13  */
enum
{
    MSG_TYPE_SYSTEM_TIME_SEC_BASE,
    MSG_TYPE_SYSTEM_START_POWER_OFF,
    MSG_TYPE_SYSTEM_REAL_POWER_OFF,
    MSG_TYPE_SYNC_DATA_TIMER,
    TO_APP_TASK_MSG_UPDATE_ADV_DATA,
    TO_APP_TASK_MSG_UPDATE_DEV_NAME,
    TO_APP_TASK_MSG_STOP_ADV,
};

void uteModuleMessageUteApplicationTaskHandler(ute_task_application_message_t *msg);

#endif //_UTE_MODULE_MESSAGE_H_

