/**
*@file
*@brief        天气模块
*@details
*@author       zn.zeng
*@date       2021-08-25
*@version      v1.0
*/
#include "ute_module_weather.h"
#include "ute_module_log.h"
#include "ute_module_filesystem.h"
#include "ute_module_charencode.h"

/*! 天气数据 zn.zeng, 2021-08-25  */
ute_module_weather_data_t uteModuleWeatherData;
/* 天气互斥量 zn.zeng 2022-02-14*/
void *uteModuleWeatherMute;
/**
*@brief            读取天气数据
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteModuleWeatherReadConfig(void)
{
    void *file = NULL;
    memset(&uteModuleWeatherData,0,sizeof(ute_module_weather_data_t));
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_WEATHERINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,(void*)&uteModuleWeatherData,sizeof(ute_module_weather_data_t));
        uteModuleFilesystemCloseFile(file);
    }
#if UTE_MODULE_LOG_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,currDay=%d,pm25=%d,aqi=%d,%d C", __func__,uteModuleWeatherData.currDay,uteModuleWeatherData.fristDayPm25,uteModuleWeatherData.fristDayAqi,uteModuleWeatherData.fristDayCurrTemperature);
    for(uint8_t i=0; i<7; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,weather=0x%02x,min=%d C,max=%d C", __func__,uteModuleWeatherData.DayWeather[i],uteModuleWeatherData.dayTemperatureMin[i],uteModuleWeatherData.dayTemperatureMax[i]);
    }
#endif

}
/**
*@brief            初始化
*@details
*@author        zn.zeng
*@date       2022-02-14
*/
void uteModuleWeatherInit(void)
{
    uteModuleWeatherReadConfig();
    uteModulePlatformCreateMutex(&uteModuleWeatherMute);
}
/**
*@brief            保存天气数据
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteModuleWeatherSaveData(void)
{
    void *file = NULL;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_WEATHERINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,uteModuleWeatherSaveData", __func__);
        uteModuleFilesystemWriteData(file,(void*)&uteModuleWeatherData,sizeof(ute_module_weather_data_t));
        uteModuleFilesystemCloseFile(file);
    }

}
/**
*@brief            获取天气数据
*@details
*@author        zn.zeng
*@date        2021-08-25
*/
void uteModuleWeatherGetData(ute_module_weather_data_t *data)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
#if APP_MODULE_WEATHER_HOURS_INFO_SUPPORT
    uint8_t index = 0;
    ute_module_systemtime_time_t nextTime;

    memset(&nextTime, 0, sizeof(ute_module_systemtime_time_t));
    memcpy(&nextTime, &uteModuleWeatherData.updateTime, sizeof(ute_module_systemtime_time_t));
    uteModuleSystemtimeFixDateIncreaseDecreaseDay(&nextTime, 1);
#endif
    /*! 跨天更新天气数zn.zeng, 2021-08-26  */
    uteModulePlatformTakeMutex(uteModuleWeatherMute);
    if((time.day!=uteModuleWeatherData.currDay)&&(uteModuleWeatherData.currDay!=0))
    {
        for(uint8_t i=0; i<6; i++)
        {
            uteModuleWeatherData.DayWeather[i] = uteModuleWeatherData.DayWeather[i+1];
            uteModuleWeatherData.dayTemperatureMin[i] = uteModuleWeatherData.dayTemperatureMin[i+1];
            uteModuleWeatherData.dayTemperatureMax[i] = uteModuleWeatherData.dayTemperatureMax[i+1];
#if APP_MODULE_WEATHER_HOURS_INFO_SUPPORT
            uteModuleWeatherData.dayHumidity[i] = uteModuleWeatherData.dayHumidity[i + 1];
            uteModuleWeatherData.nightHumidity[i] = uteModuleWeatherData.nightHumidity[i + 1];
            uteModuleWeatherData.dayUltraviolet[i] = uteModuleWeatherData.dayUltraviolet[i + 1];
            uteModuleWeatherData.nightUltraviolet[i] = uteModuleWeatherData.nightUltraviolet[i + 1];
#endif
        }
        uteModuleWeatherData.fristDayCurrTemperature = (uteModuleWeatherData.dayTemperatureMin[0]+uteModuleWeatherData.dayTemperatureMax[0])/2;
        uteModuleWeatherData.DayWeather[6] = 0;
        uteModuleWeatherData.dayTemperatureMin[6] = 0;
        uteModuleWeatherData.dayTemperatureMax[6] = 0;
#if APP_MODULE_WEATHER_HOURS_INFO_SUPPORT
        uteModuleWeatherData.dayHumidity[6] = 0;
        uteModuleWeatherData.nightHumidity[6] = 0;
        uteModuleWeatherData.dayUltraviolet[6] = 0;
        uteModuleWeatherData.nightUltraviolet[6] = 0;
#endif
        uteModuleWeatherData.currDay = time.day;
        if(uteModuleWeatherData.DayWeather[0]==0)
        {
            uteModuleWeatherData.currDay = 0;
        }
    }
#if APP_MODULE_WEATHER_HOURS_INFO_SUPPORT
    if (time.hour != uteModuleWeatherData.currHour && uteModuleWeatherData.currHour < 24 && time.day == uteModuleWeatherData.updateTime.day)//at the same day
    {
        if (time.hour > uteModuleWeatherData.currHour)//must bigger than the sync hour
        {
            index = time.hour - uteModuleWeatherData.currHour - 1;
            // 检查小时天气数据是否有效，只有有效时才更新当前天气
            if (uteModuleWeatherData.hourTemperature[index] != 0xFF)
                uteModuleWeatherData.fristDayCurrTemperature = uteModuleWeatherData.hourTemperature[index];
            if (uteModuleWeatherData.hourHumidity[index] != 0xFF)
                uteModuleWeatherData.fristDayWeatherHumidity = uteModuleWeatherData.hourHumidity[index];
            if (uteModuleWeatherData.hourUltraviolet[index] != 0xFF)
                uteModuleWeatherData.fristDayWeatherUltraviolet = uteModuleWeatherData.hourUltraviolet[index];
            if (uteModuleWeatherData.hourWeather[index] != 0xFF)
                uteModuleWeatherData.DayWeather[0] = uteModuleWeatherData.hourWeather[index];

            if (uteModuleWeatherData.fristDayCurrTemperature < uteModuleWeatherData.dayTemperatureMin[0])
                uteModuleWeatherData.fristDayCurrTemperature = uteModuleWeatherData.dayTemperatureMin[0];
            if (uteModuleWeatherData.fristDayCurrTemperature > uteModuleWeatherData.dayTemperatureMax[0])
                uteModuleWeatherData.fristDayCurrTemperature = uteModuleWeatherData.dayTemperatureMax[0];
        }
    }
    else if (time.day == nextTime.day && uteModuleWeatherData.currHour < 24)//the next day base on the sync day
    {
        if (time.hour < uteModuleWeatherData.currHour)
        {
            index = 23 - uteModuleWeatherData.currHour + time.hour;
            if (index < 24)
            {
                // 检查小时天气数据是否有效，只有有效时才更新当前天气
                if (uteModuleWeatherData.hourTemperature[index] != 0xFF)
                    uteModuleWeatherData.fristDayCurrTemperature = uteModuleWeatherData.hourTemperature[index];
                if (uteModuleWeatherData.hourHumidity[index] != 0xFF)
                    uteModuleWeatherData.fristDayWeatherHumidity = uteModuleWeatherData.hourHumidity[index];
                if (uteModuleWeatherData.hourUltraviolet[index] != 0xFF)
                    uteModuleWeatherData.fristDayWeatherUltraviolet = uteModuleWeatherData.hourUltraviolet[index];
                if (uteModuleWeatherData.hourWeather[index] != 0xFF)
                    uteModuleWeatherData.DayWeather[0] = uteModuleWeatherData.hourWeather[index];

                if (uteModuleWeatherData.fristDayCurrTemperature < uteModuleWeatherData.dayTemperatureMin[0])
                    uteModuleWeatherData.fristDayCurrTemperature = uteModuleWeatherData.dayTemperatureMin[0];
                if (uteModuleWeatherData.fristDayCurrTemperature > uteModuleWeatherData.dayTemperatureMax[0])
                    uteModuleWeatherData.fristDayCurrTemperature = uteModuleWeatherData.dayTemperatureMax[0];
            }
        }
    }
#endif
//    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,cityName:", __func__);
//    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&uteModuleWeatherData.cityString[0],uteModuleWeatherData.cityNameLen);
    memcpy(data,&uteModuleWeatherData,sizeof(ute_module_weather_data_t));
    uteModulePlatformGiveMutex(uteModuleWeatherMute);
}
/**
*@brief            设置天气数据
*@details
*@author        zn.zeng
*@date        2022-02-14
*/
void uteModuleWeatherSetData(ute_module_weather_data_t data)
{
    uteModulePlatformTakeMutex(uteModuleWeatherMute);
    memcpy(&uteModuleWeatherData,&data,sizeof(ute_module_weather_data_t));
    uteModulePlatformGiveMutex(uteModuleWeatherMute);
}

/**
*@brief         获取当天日期
*@details
*@author        dengli.lu
*@date        2021-10-27
*/
uint8_t uteModuleWeatherGetCurrDay(void)
{
    return uteModuleWeatherData.currDay;
}
#if UTE_MODULE_WEATHER_HUMIDITY_ULTRAVIOLET_SUPPORT || APP_MODULE_WEATHER_HOURS_INFO_SUPPORT
/**
*@brief         紫外数据转换为等级
*@details
*@param[in] uint8_t ultraviolet,紫外数据值
*@return uint8_t 返回等级，0~4，分别为 很弱、弱、中等、强、很强
*@author       zn.zeng
*@date        2022-09-22
*/
uint8_t uteModuleWeatherGetUltravioletLevel(uint8_t ultraviolet)
{
    uint8_t ultravioletLevel = 0;
    if (ultraviolet <= 2) //最弱
    {
        ultravioletLevel = 0;
    }
    else if ((ultraviolet == 3) || (ultraviolet == 4)) //弱
    {
        ultravioletLevel = 1;
    }
    else if ((ultraviolet == 5) || (ultraviolet == 6)) //中等
    {
        ultravioletLevel = 2;
    }
    else if ((ultraviolet >= 7) && (ultraviolet <= 9)) //强
    {
        ultravioletLevel = 3;
    }
    else //很强
    {
        ultravioletLevel = 4;
    }
    return ultravioletLevel;
}
#endif
#if  UTE_MODULE_WEATHER_CITY_NAME_NEW_SUPPORT
/**
*@brief        设置城市名
*@details
*@author        Txj
*@date        2022-11-10
***/
void uteModuleWeatherSetCityName(uint8_t *WeatherData,uint16_t size)
{
    uteModuleWeatherData.cityNameLen=size;
    memset(&uteModuleWeatherData.cityString[0],0,UTE_WEATHER_CITY_NAME_TITLE_MAX_SIZE);
    memcpy(&uteModuleWeatherData.cityString[0],&WeatherData[0],size);
    uteModuleWeatherSaveData();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,cityName:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&uteModuleWeatherData.cityString[0],uteModuleWeatherData.cityNameLen);

}
/**
*@brief  获取城市名
*@details  输入编码为utf-8
*@param[in] uint8_t *data,数据
*@param[in] uint16_t size,长度
*@author        Txj
*@date        2022-11-10
*/
void uteModuleWeatherGetCityName(uint8_t *WeatherData,uint16_t *size)
{
    ute_module_weather_data_t data;
    uteModuleWeatherGetData(&data);
    uteModuleCharencodeGetUtf8String(data.cityString, data.cityNameLen, WeatherData, size);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,cityNameLen=%d,size=%d", __func__,data.cityNameLen,*size);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&data.cityString[0],data.cityNameLen);
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,WeatherData,*size);
}
#endif
#if APP_MODULE_WEATHER_HOURS_INFO_SUPPORT
/**
*@brief         获取未来24小时天气的起始时间
*@details
*@author        wuhuowang
*@date        2022-11-19
*/
uint8_t uteModuleWeatherGetCurrHour(void)
{
    return uteModuleWeatherData.currHour;
}

/**
*@brief  保存所有天气
*@details
*@param[in] uint8_t *WeatherData 天气数据
*@param[in] uint16_t length 数据长度
*@author        wuhuowang
*@date        2022-12-19
*/
void uteModuleWeatherSaveAllWeatherInfo(uint8_t *WeatherData, uint16_t length)
{
    uint8_t i = 0;
    uint8_t dayCnt = 0;

    uteModuleSystemtimeGetTime(&uteModuleWeatherData.updateTime);
    uteModuleWeatherData.currDay = uteModuleWeatherData.updateTime.day;
    uteModuleWeatherData.currHour = uteModuleWeatherData.updateTime.hour;
    uteModuleWeatherData.DayWeather[0] = (WeatherData[0] << 8) | WeatherData[1];
    uteModuleWeatherData.fristDayPm25 = (WeatherData[5] << 8) | WeatherData[6];
    uteModuleWeatherData.fristDayAqi= (WeatherData[7] << 8) | WeatherData[8];
    if (WeatherData[2] & 0x80)
    {
        uteModuleWeatherData.fristDayCurrTemperature = -(WeatherData[2] & ~0x80);
    }
    else
    {
        uteModuleWeatherData.fristDayCurrTemperature = WeatherData[2];
    }
    if (WeatherData[3] & 0x80)
    {
        uteModuleWeatherData.dayTemperatureMax[0] = -(WeatherData[3] & ~0x80);
    }
    else
    {
        uteModuleWeatherData.dayTemperatureMax[0] = WeatherData[3];
    }
    if (WeatherData[4] & 0x80)
    {
        uteModuleWeatherData.dayTemperatureMin[0] = -(WeatherData[4] & ~0x80);
    }
    else
    {
        uteModuleWeatherData.dayTemperatureMin[0] = WeatherData[4];
    }
    for (i = 0; i < 24; i ++)
    {
        if (WeatherData[i * 5 + 9] & 0x80)
            uteModuleWeatherData.hourTemperature[i] = -(WeatherData[i * 5 + 9] & (~0x80));
        else
            uteModuleWeatherData.hourTemperature[i] = WeatherData[i * 5 + 9];
        uteModuleWeatherData.hourHumidity[i] = WeatherData[i * 5 + 10];
        uteModuleWeatherData.hourUltraviolet[i] = WeatherData[i * 5 + 11];
        uteModuleWeatherData.hourWeather[i] = (WeatherData[i * 5 + 12] << 8) | WeatherData[i * 5 + 13];
        if (i == 0)
        {
            uteModuleWeatherData.dayHumidity[0] = WeatherData[i * 5 + 10];
            uteModuleWeatherData.nightHumidity[0] = WeatherData[i * 5 + 10];
            uteModuleWeatherData.dayUltraviolet[0] = WeatherData[i * 5 + 11];
            uteModuleWeatherData.nightUltraviolet[0] = WeatherData[i * 5 + 11];
        }
    }
    uteModuleWeatherData.fristDayWeatherHumidity = uteModuleWeatherData.hourHumidity[0];
    uteModuleWeatherData.fristDayWeatherUltraviolet = uteModuleWeatherData.hourUltraviolet[0];
    dayCnt = (length - 129) / 8;
    if (dayCnt > 6)
        dayCnt = 6;
    for (i = 0; i < dayCnt; i ++)
    {
        uteModuleWeatherData.DayWeather[i + 1] = (WeatherData[i * 8 + 129] << 8) | WeatherData[i * 8 + 130];
        if (WeatherData[i * 8 + 131] & 0x80)
        {
            uteModuleWeatherData.dayTemperatureMax[i + 1] = -(WeatherData[i * 8 + 131] & (~0x80));
        }
        else
        {
            uteModuleWeatherData.dayTemperatureMax[i + 1] = WeatherData[i * 8 + 131];
        }
        if (WeatherData[i * 8 + 132] & 0x80)
        {
            uteModuleWeatherData.dayTemperatureMin[i + 1] = -(WeatherData[i * 8 + 132] & (~0x80));
        }
        else
        {
            uteModuleWeatherData.dayTemperatureMin[i + 1] = WeatherData[i * 8 + 132];
        }
        uteModuleWeatherData.dayHumidity[i + 1] = WeatherData[i * 8 + 133];
        uteModuleWeatherData.nightHumidity[i + 1] = WeatherData[i * 8 + 134];
        uteModuleWeatherData.dayUltraviolet[i + 1] = WeatherData[i * 8 + 135];
        uteModuleWeatherData.nightUltraviolet[i + 1] = WeatherData[i * 8 + 136];
    }
}
#endif
