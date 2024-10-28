/**
*@file
*@brief        模块系统消息
*@details  系统消息处理连接层
*@author       zn.zeng
*@date       Jun 29, 2021
*@version      v1.0
*/

#include "ute_module_message.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_drv_motor.h" 
#include "ute_module_notify.h"

/**
*@brief  消息模块消息处理函数
*@details  处理ute自己的消息，此函数需要接到平台消息处理函数，运行在ute application task
*@param[in] ute_task_application_message_t *msg
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleMessageUteApplicationTaskHandler(ute_task_application_message_t *msg)
{
    uint16_t type = msg->type;
    uint32_t param = msg->param;

    switch (type)
    {
        case MSG_TYPE_SYSTEM_TIME_SEC_BASE:
        {
            uteModuleSystemtimeSecondCb();
        }
        break;
        case MSG_TYPE_DRV_MOTOR_START:
        {
            uteDrvMotorStartHandlerMsg(param);
        }
        break;
        case MSG_TYPE_DRV_MOTOR_STOP:
        {
            uteDrvMotorStopHandlerMsg();
        }
        break;
        case MSG_TYPE_DRV_MOTOR_HANDLE_TIMER:
        {
            uteDrvMotorTimerHandlerMsg();
        }
        break;
        case MSG_TYPE_HNAD_SCREEN_ON_NOTIFY:
        {
            if(sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
        }
        break;
        case MSG_TYPE_HNAD_SCREEN_OFF_NOTIFY:
        {
            if(!sys_cb.gui_sleep_sta)
            {
                sys_cb.guioff_delay = 1;
            }
        }
        break;
        case MSG_TYPE_MODULE_NOTIFY_NOTIFYCATTION:
        {
            uteModuleNotifyNotifycationHandlerMsg();
        }
        break;
        default:
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,unknown msg,type=%d", __func__, type);
            break;
    }
}



