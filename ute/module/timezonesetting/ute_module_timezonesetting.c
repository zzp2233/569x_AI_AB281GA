/**
*@file
*@brief        时区设置模块
*@details
*@author       黄鹤
*@date       2022-08-12
*@version      v1.0
*/
#include "ute_module_timezonesetting.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"
#include "ute_module_crc.h"
#include "ute_module_profile_ble.h"

#if UTE_MODULE_TIME_ZONE_SETTING_SUPPORT

/*! 时区设置数据 黄鹤, 2022-03-29  */
#define UTE_MODULE_TIMEZONE_DATA_LENGTH ((sizeof(ute_module_timezonesetting_one_timezone_t) + 1) * UTE_MODULE_WORLD_CLOCK_TOTAL_CNT)
ute_module_timezonesetting_data_t uteModuleTimeZoneSettingData;

/**
*@brief  读取时区储存数据信息
*@details
*@author        kp.h
*@date        2022-08-27
*/
void uteModuleTimezoneReadConfig(void)
{
    void *file = NULL;
    uint8_t *readbuff;
    readbuff = uteModulePlatformMemoryAlloc(UTE_MODULE_TIMEZONE_DATA_LENGTH);
    memset(readbuff,0,UTE_MODULE_TIMEZONE_DATA_LENGTH);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_ZONE_DATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file,&readbuff[0],UTE_MODULE_TIMEZONE_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleTimeZoneSettingData.number = readbuff[1];
    uint16_t size_one_timezone_data = UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH+3;
    for(uint8_t i=0; i<UTE_MODULE_WORLD_CLOCK_TOTAL_CNT; i++)
    {
        uteModuleTimeZoneSettingData.info[i].countryNameLen = readbuff[2+size_one_timezone_data*i];
        memcpy(uteModuleTimeZoneSettingData.info[i].countryNameU8,&readbuff[3+size_one_timezone_data*i],UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH);
        uteModuleTimeZoneSettingData.info[i].timezone = readbuff[3+UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH+size_one_timezone_data*i];
        uteModuleTimeZoneSettingData.info[i].timezone_dec = readbuff[4+UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH+size_one_timezone_data*i];
    }
    uteModulePlatformMemoryFree(readbuff);
}

/**
*@brief  会议提醒参数初始化
*@details
*@author       kp.h
*@date        2022-08-27
*/
void uteModuleTimezoneInit(void)
{
    uteModuleTimezoneReadConfig();
}

/**
*@brief  获取时区数据
*@details
*@param[out] ute_module_timezonesetting_data_t *data  时区变量指针
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleTimeZoneSettingGetData(ute_module_timezonesetting_data_t *data)
{
    memcpy(data, &uteModuleTimeZoneSettingData, sizeof(ute_module_timezonesetting_data_t));
}
/**
*@brief  保存所有时区数据
*@details
*@param[out] ute_module_timezonesetting_data_t *data  时区变量指针
*@author        kp.h
*@date        Aug 26, 2022
*/
void uteModuleTimeZoneSettingSaveData(ute_module_timezonesetting_data_t *data)
{
    /*! 保存到文件, 2022-03-29*/
    void *file;
    uint8_t *writebuff;
    writebuff = uteModulePlatformMemoryAlloc(UTE_MODULE_TIMEZONE_DATA_LENGTH);
    memset(writebuff,0,UTE_MODULE_TIMEZONE_DATA_LENGTH);
    writebuff[1] = data->number;
    uint16_t size_one_timezone_data = UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH+3;
    for(uint8_t i=0; i<UTE_MODULE_WORLD_CLOCK_TOTAL_CNT; i++)
    {
        writebuff[2+size_one_timezone_data*i] = data->info[i].countryNameLen;
        memcpy(&writebuff[3+size_one_timezone_data*i],data->info[i].countryNameU8,UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH);
        writebuff[3+UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH+size_one_timezone_data*i] = data->info[i].timezone;
        writebuff[4+UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH+size_one_timezone_data*i] = data->info[i].timezone_dec;
    }
    //writebuff index 517 518 519 = 0 : reserve
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_ZONE_DATA,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&data[0],sizeof(ute_module_timezonesetting_data_t));
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(writebuff);
}
/**
*@brief  判断是否是闰年
*@details
*@author        黄鹤
*@date        2022-08-12
*/
bool uteModuleTimeZoneSettingIsLeapYear(uint16_t year)
{
    return (((year % 100 != 0) && (year % 4 == 0)) || (year % 400 == 0));
}

/**
*@brief  获取时区设置个数
*@details
*@param[out]  uteModuleTimeZoneSettingData.number
*@author        zn.zeng
*@date        Jun 29, 2021
*/
uint8_t uteModuleTimeZoneSettingGetNum(void)
{
    return uteModuleTimeZoneSettingData.number;
}

/**
*@brief       时区数据同步
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       huanghe
*@date       2022-08-23
*/
void uteModuleTimeZoneSettingCmd(uint8_t *receive, uint8_t length)
{
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receive=", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL,receive,length);

    if (receive[1] == 0xfa)
    {
        if (receive[3] == 0)
        {
            uteModuleTimeZoneSettingData.receiveCrc = 0;
            uteModuleTimeZoneSettingData.number = receive[2];
        }
        uteModuleCrc8Bit(&uteModuleTimeZoneSettingData.receiveCrc, &receive[2],length - 2);
        uint8_t MaxCountryNameLen = sizeof(uteModuleTimeZoneSettingData.info[receive[3]].countryNameU8) - 1;
#if 0//APP_MODULE_USE_SHARERAM_SUPPORT
        uint16_t countryNameUnicodeLen = UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH;
        uint8_t NameUnicodeBuf[UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH];
        memset(NameUnicodeBuf, 0, sizeof(NameUnicodeBuf));
        uteModuleCharencodeUtf8ConversionUnicode(&NameUnicodeBuf[0], &countryNameUnicodeLen, &receive[5], receive[4]);
        if (countryNameUnicodeLen > MaxCountryNameLen)
        {
            uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen = MaxCountryNameLen;
        }
        else
        {
            uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen = countryNameUnicodeLen;
        }
        memcpy(&uteModuleTimeZoneSettingData.info[receive[3]].countryNameU8[0], &NameUnicodeBuf[0], uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen);
#else
        if (receive[4] > MaxCountryNameLen)
        {
            uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen = MaxCountryNameLen;
        }
        else
        {
            uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen = receive[4];
        }
        memcpy(&uteModuleTimeZoneSettingData.info[receive[3]].countryNameU8[0], &receive[5], uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen);
#endif
        uteModuleTimeZoneSettingData.info[receive[3]].countryNameU8[uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen] = 0;

        if((length-(receive[4]+5)) > 1)//2byte表示时区,新协议
        {
            uteModuleTimeZoneSettingData.info[receive[3]].timezone = receive[length - 2];
            uteModuleTimeZoneSettingData.info[receive[3]].timezone_dec = receive[length - 1];
        }
        else//1byte表示时区,老协议
        {
            uteModuleTimeZoneSettingData.info[receive[3]].timezone = receive[length - 1];
            uteModuleTimeZoneSettingData.info[receive[3]].timezone_dec = 0xff;
        }
        uteModuleProfileBle50SendToPhone(&receive[0],4);
        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,timezoneParam[%d].countryNameLen=%d,timeZone=%d.%d", __func__, receive[3], uteModuleTimeZoneSettingData.info[receive[3]].countryNameLen,
                       uteModuleTimeZoneSettingData.info[receive[3]].timezone,uteModuleTimeZoneSettingData.info[receive[3]].timezone_dec);
    }
    else if (receive[1] == 0xfb)
    {
        uteModuleTimeZoneSettingData.number = 0;
        memset(&uteModuleTimeZoneSettingData,0x00,sizeof(ute_module_timezonesetting_data_t));
        uteModuleProfileBle50SendToPhone(&receive[0], 2);
    }
    else if (receive[1] == 0xfd)
    {
        uint8_t send_buff[2];
        send_buff[0] = receive[0];
        send_buff[1] = receive[1];
        if (receive[2] != uteModuleTimeZoneSettingData.receiveCrc)
        {
            uteModuleTimeZoneSettingData.number = 0;
            memset(&uteModuleTimeZoneSettingData,0x00,sizeof(ute_module_timezonesetting_data_t));
            uteModuleTimezoneReadConfig();//读取flash数据
            send_buff[1] = 0xff;
        }
        else
        {
            uteModuleTimeZoneSettingSaveData(&uteModuleTimeZoneSettingData);
        }
        uteModuleProfileBle50SendToPhone(send_buff, 2);
        uteModuleTimeZoneSettingData.receiveCrc = 0;
    }
}
#endif //UTE_MODULE_TIME_ZONE_SETTING_SUPPORT

