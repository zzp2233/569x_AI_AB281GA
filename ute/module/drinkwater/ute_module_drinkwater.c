/**
*@file
*@brief        喝水提醒模块
*@details
*@author       黄鹤
*@date       2022-03-29
*@version      v1.0
*/
#include "ute_module_drinkwater.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"
#include "ute_drv_motor.h"
#include "func_cover.h"

/*! 喝水消息 黄鹤, 2022-03-29  */
ute_module_drink_water_data_t uteModuleDrinkWaterData;

#define UTE_MODULE_DRINK_WATER_DATA_LENGTH 11

/**
*@brief  读取喝水参数信息
*@details
*@author        黄鹤
*@date        2022-03-29
*/
void uteModuleDrinkWaterReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[UTE_MODULE_DRINK_WATER_DATA_LENGTH];
    /*! 喝水配置黄鹤, 2022-03-29  */
    readbuff[0] = DEFAULT_DRINK_WATER_OPNE;
    readbuff[1] = DEFAULT_DRINK_WATER_TIME_INTERVAL;
    readbuff[2] = (DEFAULT_DRINK_WATER_START_TIME>>8)&0xff;
    readbuff[3] = DEFAULT_DRINK_WATER_START_TIME&0xff;;
    readbuff[4] = (DEFAULT_DONOT_DISTURB_END_TIME>>8)&0xff;
    readbuff[5] = DEFAULT_DONOT_DISTURB_END_TIME&0xff;
    readbuff[6] = DEFAULT_NOON_SLEEP_OPNE;
    readbuff[7] = (DEFAULT_NOON_SLEEP_START_TIME>>8)&0xff;
    readbuff[8] = DEFAULT_NOON_SLEEP_START_TIME&0xff;;
    readbuff[9] = (DEFAULT_NOON_SLEEP_END_TIME>>8)&0xff;
    readbuff[10] = DEFAULT_NOON_SLEEP_END_TIME&0xff;

    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DRINKWATER,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,&readbuff[0],UTE_MODULE_DRINK_WATER_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleDrinkWaterData.isOpen = readbuff[0];
    uteModuleDrinkWaterData.min = readbuff[1];
    uteModuleDrinkWaterData.startPeriodHour=readbuff[2];
    uteModuleDrinkWaterData.startPeriodMin=readbuff[3];
    uteModuleDrinkWaterData.endPeriodHour=readbuff[4];
    uteModuleDrinkWaterData.endPeriodMin= readbuff[5];
    uteModuleDrinkWaterData.isOpenNoonSleep=readbuff[6];
    uteModuleDrinkWaterData.startNoonSleepHour = readbuff[7];
    uteModuleDrinkWaterData.startNoonSleepMin = readbuff[8];
    uteModuleDrinkWaterData.endNoonSleepHour =  readbuff[9];
    uteModuleDrinkWaterData.endNoonSleepMin  =  readbuff[10];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isOpen=%,startHour=%d,endHour=%d", __func__,uteModuleDrinkWaterData.isOpen,uteModuleDrinkWaterData.startPeriodHour,uteModuleDrinkWaterData.endNoonSleepHour);
}
/**
*@brief  保存喝水信息
*@details
*@param[in] (ute_not_disturb_param_t param)
*@author        黄鹤
*@date        2022-03-29
*/
void uteModuleDrinkWaterSaveParam(ute_module_drink_water_data_t *param)
{
    if(param)memcpy(&uteModuleDrinkWaterData,param,sizeof(ute_module_drink_water_data_t));
    uteModuleDrinkWaterData.RemindSecCnt=0;

    void *file;
    uint8_t writebuff[UTE_MODULE_DRINK_WATER_DATA_LENGTH];
    writebuff[0] = uteModuleDrinkWaterData.isOpen;
    writebuff[1] = uteModuleDrinkWaterData.min;
    writebuff[2] = uteModuleDrinkWaterData.startPeriodHour;
    writebuff[3] = uteModuleDrinkWaterData.startPeriodMin;
    writebuff[4] = uteModuleDrinkWaterData.endPeriodHour;
    writebuff[5] = uteModuleDrinkWaterData.endPeriodMin;
    writebuff[6] = uteModuleDrinkWaterData.isOpenNoonSleep;
    writebuff[7] = uteModuleDrinkWaterData.startNoonSleepHour;
    writebuff[8] = uteModuleDrinkWaterData.startNoonSleepMin;
    writebuff[9] = uteModuleDrinkWaterData.endNoonSleepHour;
    writebuff[10] = uteModuleDrinkWaterData.endNoonSleepMin;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DRINKWATER,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],UTE_MODULE_DRINK_WATER_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  喝水提醒参数初始化
*@details
*@author       黄鹤
*@date        2022-03-31
*/
void uteModuleDrinkWaterInit(void)
{
    uteModuleDrinkWaterReadConfig();
}
/**
*@brief  喝水提醒处理
*@details
*@param[in] (ute_not_disturb_param_t param)
*@author        黄鹤
*@date        2022-03-29
*/
void uteModuleDrinkWaterHandler(ute_module_systemtime_time_t time)
{
    if(uteModuleDrinkWaterData.isOpen == true)
    {
        uint32_t start_time = uteModuleDrinkWaterData.startPeriodHour*3600 + uteModuleDrinkWaterData.startPeriodMin*60;
        uint32_t end_time   = uteModuleDrinkWaterData.endPeriodHour*3600 + uteModuleDrinkWaterData.endPeriodMin*60;
        uint32_t NoonSleep_start_time = uteModuleDrinkWaterData.startNoonSleepHour*3600 + uteModuleDrinkWaterData.startNoonSleepMin*60;
        uint32_t NoonSleep_end_time   = uteModuleDrinkWaterData.endNoonSleepHour*3600 + uteModuleDrinkWaterData.endNoonSleepMin*60;
        uint32_t totalSec   = time.hour*3600 + time.min*60 + time.sec;

        if(uteModuleDrinkWaterData.isOpenNoonSleep && ((totalSec>=NoonSleep_start_time) && (totalSec<=NoonSleep_end_time)))
        {
            //printf("Noonsleeping!!!dont Remind!\nuteModuleDrinkWaterData.RemindSecCnt = %d\n", uteModuleDrinkWaterData.RemindSecCnt);
        }
        else
        {
            if(start_time<end_time)// 8:00 - 18 :00
            {
                if( (totalSec>=start_time) && (totalSec<=end_time) )
                {
                    uteModuleDrinkWaterData.RemindSecCnt ++;
                }
                else
                {
                    uteModuleDrinkWaterData.RemindSecCnt = 0;
                }
            }
            else// 18:00 -8:00
            {
                if((start_time<=totalSec) || (end_time>=totalSec) )
                {
                    uteModuleDrinkWaterData.RemindSecCnt ++;
                }
                else
                {
                    uteModuleDrinkWaterData.RemindSecCnt = 0;
                }
            }

        }
        if(uteModuleDrinkWaterData.RemindSecCnt>=(uteModuleDrinkWaterData.min*60))
        {
            uteModuleDrinkWaterStart();
            uteModuleDrinkWaterData.RemindSecCnt = 0;
        }
    }
    else
    {
        uteModuleDrinkWaterData.RemindSecCnt = 0;
    }
}

bool uteModuleDrinkWaterOpenCtrl(bool isSet,bool isOpen)
{
    if(isSet)
    {
        uteModuleDrinkWaterData.isOpen=isOpen;
        uteModuleDrinkWaterSaveParam(NULL);
    }

    return uteModuleDrinkWaterData.isOpen;
}
void uteModuleDrinkWaterOpenSwitch(void)
{
    uteModuleDrinkWaterData.isOpen=!uteModuleDrinkWaterData.isOpen;
    uteModuleDrinkWaterSaveParam(NULL);
}


/**
*@brief    开始喝水提醒
*@details
*@author        黄鹤
*@date        2022-03-30
*/
void uteModuleDrinkWaterStart(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
#if QUICK_SWITCH_DRINK_WATER_SUPPORT
    if(uteModuleDrinkWaterData.isOpen)
#endif
    {
        sys_cb.cover_index = REMIND_COVER_HEALTH_DRINK;
        sys_cb.remind_tag = true;
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,3);
    }
}

