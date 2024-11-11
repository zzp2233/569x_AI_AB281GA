/**
*@file
*@brief        电话相关模块
*@details    和平台接口相关
*@author       zn.zeng
*@date       2021-10-28
*@version      v1.0
*/

#include "ute_module_call.h"
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_gui_common.h"
#include "ute_application_common.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "include.h"

#ifndef GAP_DEVICE_NAME_LEN
#define GAP_DEVICE_NAME_LEN 32
#endif // !GAP_DEVICE_NAME_LEN

ute_module_call_data_t uteModuleCallData;
void *uteModuleCallOnGoingTimerPointer = NULL;
void *uteModuleCallActiveTimerPointer = NULL;
//Casen add 22-04-18
void *uteModuleCallOutGoingChangeScoTimerPointer = NULL;
uint8_t retryOutgoingChangeScoCount = 0;
uint8_t retryActiveChangeScoCount = 0;

//Casen add 22-06-23
void *uteModuleCallDialTimerPointer = NULL;

//zn.zeng add 22-07-04
void *uteModuleCallPbapTimerOutPointer = NULL;

static uint8_t uteModuleBtDeviceName[GAP_DEVICE_NAME_LEN];

ute_module_call_app_ctrl_data_t uteModuleCallAppCtrlData =
{
    .appCallForwarding = false,
    .phoneOS = 0x01,
    .btAutomaticPair = true,
    .uuid = 0x00,
};

/**
*@brief      用于判断协议HFP，A2DP是否链接
*@details    A2DP只有在用户打开开关的时候才判断，否则不判断;这个是比较好的方式用于判断BT是否链接手机
*@author     ellison
*@date       ellison add ,2022-Sep-05 21:55
*/
bool isHfpAndA2dpProfileBothConnected(void)
{
    bool result =false;
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    if(uteModuleCallData.isEntertranmentVoiceOn)
    {
        result =bt_a2dp_profile_completely_connected();
    }
#endif
    if(bt_hfp_is_connected())
    {
        result = true;
    }
    return result;
}

/**
*@brief  bt 参数保存
*@details
*@author        zn.zeng
*@date        2022-01-07
*/
void uteModuleCallBtSaveConfig(void)
{
    void *file;
    uint8_t readbuff[6];
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALLINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        memcpy(&readbuff[0],&uteModuleCallData.address[0],6);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,mac=", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_CALL_LVL,&uteModuleCallData.address[0],6);
        uteModuleFilesystemWriteData(file,&readbuff[0],6);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  bt 参数读取
*@details
*@author        zn.zeng
*@date        2022-01-07
*/
void uteModuleCallBtReadConfig(void)
{
    void *file;
    uint8_t readbuff[6];
    memset(readbuff,0,6);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALLINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],6);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,readbuff=", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_CALL_LVL,&readbuff[0],6);
    memcpy(&uteModuleCallData.address[0],&readbuff[0],6);
}

/**
*@brief  bt连接标志
*@details
*@author        zn.zeng
*@date        2021-11-04
*/
bool uteModuleCallBtIsConnected(void)
{
#if UTE_MODULE_CALL_SET_MODE_EXCEPTION_HANDLER_SUPPORT
    // uint8_t btStatus =1;
    //btStatus = uteModuleCallBtGetPowerStatus();//BT必须打开的情况下才有链接，防止异常的时候显示图标异常
    return isHfpAndA2dpProfileBothConnected();//(uteModuleCallData.isConnected && btStatus);
#else
    return bt_is_connected();
#endif
}

/**
*@brief  更新KEY获取的手机蓝牙地址
*@details  为了防止用户首次没有打开BT，在后续打开时，没有获取到蓝牙地址；而不能每隔10s的回链
*@param[in] uint8_t *addr，蓝牙地址数据，大小为6byte
*@author       ellison
*@date        2022-09-21
*/
void uteModuleCallBtUpdateKeyConnectAddress(uint8_t *addr)
{

    if(memcmp(&uteModuleCallData.address[0],&addr[0],6)!=0)
    {
        memcpy(&uteModuleCallData.address[0],&addr[0],6);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,mac=", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_CALL_LVL,&uteModuleCallData.address[0],6);
        uteModuleCallBtSaveConfig();
    }
}

/**
*@brief  call bt关机
*@details 1、当消息为UTE_BT_POWER_OFF_APP_UNBIND时，需要等清理了佩戴信息才关机
*@param[in] reason, BT关闭原因
*@author        zn.zeng
*@date        2021-11-08
*/
void uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_REASON reason)
{
    uteModuleCallData.isPowerOn = false;

    if(reason != UTE_BT_POWER_OFF_APP_UNBIND)
    {
        if (bt_is_connected())
        {
            bt_disconnect(0);
        }
        bt_scan_disable();
    }

    uteModuleCallData.callData.isAutoWakupFromBle = false;
    uteModuleCallData.isBtAutoClose = false;

}

/**
*@brief  call bt开机，可连接
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteModuleCallBtPowerOn(ute_bt_power_on_type_t type)
{
    uteModuleCallData.powerOnType = type;
    if (type != UTE_BT_POWER_OFF)
    {
        uteModuleCallData.isPowerOn = true;

        if(!bt_get_scan())
        {
            bt_scan_enable();
            bt_connect();
        }

        uteModuleCallData.callData.isAppAudio = false;
        uteModuleCallData.callData.inOutToActiveRtc = 0;
        uteModuleCallData.powerOnTimeSecond = 0;
        uteModuleCallData.idleTimerSecond = 0;
    }
    if(type==UTE_BT_POWER_ON_FORM_BLE_CALL_NOTIFY)
    {
        uteModuleCallData.callData.isAutoWakupFromBle = true;
    }
}

/**
*@brief BT自动睡眠参数保存
*@details
*@author        xjc
*@date        2022-04-27
*/
void uteModuleCallIsBtAutoCloseSaveConfig(void)
{
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleCallData.isBtAutoClose;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BT_AUTO_CLOSE_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  BT自动睡眠参数读取
*@details
*@author      casen
*@date        2022-08-04
*/
void uteModuleCallIsBtAutoCloseReadConfig(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = false;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BT_AUTO_CLOSE_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleCallData.isBtAutoClose = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,.isBtAutoClose=%d", __func__,uteModuleCallData.isBtAutoClose);
}
/**
*@brief BT自动睡眠参数获取
*@details
*@author      caesn
*@date        2022-08-17
*/
bool uteModuleCallGetIsBtAutoClose(void)
{
    return uteModuleCallData.isBtAutoClose;
}

/**
*@brief  call 初始化
*@details
*@param[in] uint16_t type ，消息类型
*@author        zn.zeng
*@date        2021-11-06
*/
void uteModuleCallInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR, NULL, NULL);
#endif
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,line=%d,reset mac address",__func__,__LINE__);
    memset(&uteModuleCallData,0,sizeof(ute_module_call_data_t));
    uteModuleCallData.isPowerOn = false;
    uteModuleCallBtReadConfig();
#if UTE_MODULE_BT_ONCE_PAIR_CONNECT_SUPPORT
    uteModuleCallBtReadCtrlAppData();
#endif
#if UTE_MODULE_BT_POWER_STATUS_SAVE_SUPPORT
    uteModuleCallReadBtPowerOnOffStatus();
    if(uteModuleCallData.isPowerOn)
    {
        uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
    }
#endif
}

/**
*@brief   获取BT蓝牙名
*@details
*@param[out] uint8_t *name
*@return    void
*@author    casen
*@date      2022-03-26
*/
void uteModuleCallGetBtDevName(uint8_t *name,uint8_t limitSize)
{
    uint8_t nameLen = strlen((char *)&uteModuleBtDeviceName);
    if(limitSize>nameLen)
    {
        limitSize = nameLen;
    }
    memcpy(name,uteModuleBtDeviceName,limitSize);
}

/**
*@brief   设置BT蓝牙名
*@details
*@param[out] uint8_t *name
*@return    void
*@author    casen
*@date      2022-03-26
*/
void uteModuleCallSetBtDevName(uint8_t *name,uint8_t size)
{
    if(size > (sizeof(xcfg_cb.bt_name)-1))
    {
        size = sizeof(xcfg_cb.bt_name) - 1;
    }
    memset(xcfg_cb.bt_name,0x00,sizeof(xcfg_cb.bt_name));
    memcpy(xcfg_cb.bt_name,name,size);
}

/**
*@brief     BT设备基础信息
*@details   用于返回给APP
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author     casen
*@date       2022-03-23
*/
void uteModuleProtocolGetBtInfo(uint8_t *response,uint8_t *length)
{
    //回应0x3801 + BT3.0Name(20byte)+BT3.0MAC(6byte)+BT开关状态（1byte）+BT配对状态（1byte）
    uint8_t totalByte = 0;
    uteModuleCallGetBtDevName(&response[totalByte],GAP_DEVICE_NAME_LEN);
    totalByte += 20;
    bt_get_local_bd_addr(&response[totalByte]);
    totalByte += 6;
    response[totalByte] = uteModuleCallBtIsPowerOn();
    totalByte += 1;
    if(memcmp(uteModuleCallData.address,"\x00\x00\x00\x00\x00\x00",6)!=0)
    {
        response[totalByte] = 1; //配对过设备
    }
    else
    {
        response[totalByte] = 0; //未配对过设备
    }
    totalByte += 1;
    response[totalByte] = isHfpAndA2dpProfileBothConnected();//uteModuleCallData.isConnected;
    totalByte += 1;
    (*length) = totalByte;
}

/*****
*@brief     获取当前链接的手机是否是ios
*@details
*@return true是iPhone，否则不是
*@author     ellison
*@date       2023-03-24
*/
bool uteModuleCallIsCurrentConnectionIphone(void)
{
    return (uteModuleCallAppCtrlData.phoneOS==0x01);
}
/**
*@brief     控制BT设备数据
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author     casen
*@date       2022-03-23
*/
void uteModuleProtocolCtrlBT(uint8_t*receive,uint8_t length)
{
    uint8_t *response ;
    response = uteModulePlatformMemoryAlloc(255);
    memset(response,0x00,255);
    memcpy(response,receive,length);
    if(receive[1]==0x01)       //首次绑定
    {
        if(length>2)
        {
            uteModuleCallAppCtrlData.phoneOS = receive[2];      // 1:IOS
        }
        if(length>3)
        {
            uteModuleCallAppCtrlData.btAutomaticPair = receive[3];      // 是否自动配对
        }
        if(length>4)               // 判断链接ID
        {
            uint32_t tempUuid = (uint32_t)receive[4]<<24|receive[5]<<16|receive[6]<<8|receive[7];
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,.uuid = 0x%x tempUuid = 0x%x",__func__,uteModuleCallAppCtrlData.uuid,tempUuid);
            if(uteModuleCallAppCtrlData.uuid != tempUuid)           // 换设备，ID更改
            {
                uteModuleCallAppCtrlData.uuid = tempUuid;
                if(memcmp(uteModuleCallData.address,"\x00\x00\x00\x00\x00\x00",6)!=0)
                {
                    if(isHfpAndA2dpProfileBothConnected() == false)
                    {
                        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.phoneOS=%d,reset mac address",__func__,uteModuleCallAppCtrlData.phoneOS);
                        // 暂时关闭这个清零，因为现在的ios会发送这个导致底层的蓝牙佩戴被清零掉
                        if(uteModuleCallAppCtrlData.phoneOS==0x02)
                        {
                            // bt_bond_delete(uteModuleCallData.address);
                            bt_nor_delete_link_info();
                            memset(uteModuleCallData.address,0x00,6);
                        }
                    }
                }
#if UTE_MODULE_BT_LOW_BAT_NOT_ALLOW_POWER_ON_SUPPORT
                if ((uteModuleCallBtIsPowerOn() == false) && (uteDrvBatteryCommonGetLvl() >= UTE_DRV_BATTERY_LOW_POWER_PERECNT))
                {
                    uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
                }
#else
                if(uteModuleCallBtIsPowerOn() ==false)
                {
                    uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
                }
#endif
                if(uteModuleCallAppCtrlData.phoneOS == 0x01)
                {
                    printf("======================>ble_bt_connect\n");
                    /**************************/
                    //一键双连代码，协议待跑通
                    app_phone_type_set(uteModuleCallIsCurrentConnectionIphone());
                    bsp_change_bt_mac();
                    ble_bt_connect();
                    /**************************/
                    // ute_ble_connect_state_t state;
                    // uteApplicationCommonGetBleConnectionState(&state);
                    // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isParired=%d", __func__,state.isParired);
                }
            }
        }
        uteModuleCallBtSaveCtrlAppData();
        uint8_t totalByte = 0;
        uteModuleProtocolGetBtInfo(&response[2],&totalByte);
        uteModuleProfileBle50SendToPhone(&response[0],totalByte+2);
    }
    else if(receive[1]==0x02)
    {
        uteModuleCallAppCtrlData.uuid = 0;
        if(receive[2] == 0x00) //turn off       //解绑
        {
            if(uteModuleCallBtIsPowerOn())
            {
                bt_nor_delete_link_info();
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_APP_UNBIND);
            }
            bt_nor_delete_link_info();
        }
        else
        {
#if UTE_MODULE_BT_LOW_BAT_NOT_ALLOW_POWER_ON_SUPPORT
            if ((uteModuleCallBtIsPowerOn() == false) && (uteDrvBatteryCommonGetLvl() >= UTE_DRV_BATTERY_LOW_POWER_PERECNT))
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            }
#else
            if(uteModuleCallBtIsPowerOn() ==false)
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            }
#endif
        }
        uteModuleProfileBle50SendToPhone(&receive[0],3);
    }
    else if(receive[1]==0x03)
    {
        // uteModuleCallAppCtrlData.appCallForwarding = receive[3];
        uint8_t totalByte = 0;
        uteModuleProtocolGetBtInfo(&response[2],&totalByte);
        uteModuleProfileBle50SendToPhone(&response[0],totalByte+2);
    }
    uteModulePlatformMemoryFree(response);
}

// void uteModuleCallBtOnceConnect(bool appIos)
// {
// #if LE_SM_SC_EN
//     if (app_phone_type_get() != ble_app_ios) {
//         app_phone_type_set(ble_app_ios);
//         bsp_change_bt_mac();
//     }

//     if (ble_app_ios && !bt_is_connected()) {        //IOS
//         if (bt_nor_get_link_info(NULL)) {           //如果存在配对信息
//             sys_cb.bt_reconn_flag = true;
//             bt_connect();
//         } else {
//             ble_bt_connect();                       //一键双联
//         }
//     }
// #endif // LE_SM_SC_EN
// }

//sdk接口
// void app_phone_type_set(u8 type)
// {
//     app_data.ble_app_ios = type;
// }

// u8 app_phone_type_get(void)
// {
//     return app_data.ble_app_ios;
// }

/**
*@brief 获取来电转手表接听
*@details
*@author   casen
*@date     2022-03-24
*/
bool uteModuleCallBtGetAppCallForwarding(void)
{
    return uteModuleCallAppCtrlData.appCallForwarding;
}

/**
*@brief  获取允许BT通过BLE配对标志
*@details
*@author   casen
*@date     2022-03-24
*/
bool uteModuleCallBtGetAutomaticPair(void)
{
    return (uteModuleCallAppCtrlData.btAutomaticPair||uteModuleCallAppCtrlData.isSetBondFlag);
    //return uteModuleCallAppCtrlData.btAutomaticPair;
}

/**
*@brief   set
*@details
*@author   zn.zeng
*@date     2022-06-30
*/
void uteModuleCallBtSetBondFlag(bool isSet)
{
    uteModuleCallAppCtrlData.isSetBondFlag = isSet;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isSetBondFlag=%d", __func__,uteModuleCallAppCtrlData.isSetBondFlag);
}

/**
*@brief  ble 连接状态接口
*@details
*@author        zn.zeng
*@date        2022-06-10
*/
void uteModuleCallBleConnectState(bool isConnected)
{
#if UTE_BT30_AUTO_POWRER_OFF_SUPPORT
    if (isConnected)
    {
        /*Casen 22-08-04 自动休眠 & 配对地址不为空*/
        if (!uteModuleCallBtIsPowerOn() && (uteModuleCallData.isBtAutoClose == true) && memcmp(uteModuleCallData.address, "\x00\x00\x00\x00\x00\x00", 6))
        {
            uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
        }
    }
#endif
}

/**
*@brief  app 控制参数保存
*@details
*@author      casen
*@date       2022-03-24
*/
void uteModuleCallBtSaveCtrlAppData(void)
{
    void *file;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALL_CTRL_APPDATA,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&uteModuleCallAppCtrlData,sizeof(uteModuleCallAppCtrlData));
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief 控制参数读取
*@details
*@author  casen
*@date     2022-03-24
*/
void uteModuleCallBtReadCtrlAppData(void)
{
    void *file;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_CALL_CTRL_APPDATA,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&uteModuleCallAppCtrlData,sizeof(uteModuleCallAppCtrlData));
        uteModuleFilesystemCloseFile(file);
    }
}

#if UTE_MODULE_BT_POWER_STATUS_SAVE_SUPPORT
/**
*@brief     保存BK开关状态
*@details
*@author    xjc
*@date      2022-04-07
*/
void uteModuleCallSaveBtPowerOnOffStatus(void)
{
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleCallData.isPowerOn;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BK_ON_OFF_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     读取BK开关状态
*@details
*@author    xjc
*@date      2022-04-07
*/
void uteModuleCallReadBtPowerOnOffStatus(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_BT_POWER_DEFAULT_ON;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BK_ON_OFF_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleCallData.isPowerOn = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,uteModuleCallData.isPowerOn=%d", __func__,uteModuleCallData.isPowerOn);
}
#endif


/*
*@brief  获取sco连接状态
*@details
*@author        xjc
*@date        2022-06-01
*/
bool uteModuleCallIsScoConnected(void)
{
    return sys_cb.sco_state;
}

/**
*@brief  bt power on 的状态
*@details
*@author      zn.zeng
*@date        2022-07-01
*/
bool uteModuleCallBtIsPowerOn(void)
{
    return uteModuleCallData.isPowerOn;
}

/**
*@brief  来电提醒填入号码和名字
*@details
*@author        zn.zeng
*@date        2021-11-06
*/
void uteModuleCallSetInComingNumberName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t nameSize)
{
    if((name!=NULL)&&(nameSize!=0))
    {
        if(nameSize>=(UTE_CALL_NAME_MAX-6))
        {
            memcpy(&name[(UTE_CALL_NAME_MAX-6)],"\x00\x2e\x00\x2e\x00\x2e",6);
            uteModuleCallData.callData.nameSize = UTE_CALL_NAME_MAX;
        }
        else
        {
            uteModuleCallData.callData.nameSize = nameSize;
        }
        memcpy(&uteModuleCallData.callData.name[0],&name[0],uteModuleCallData.callData.nameSize);
    }
    if((number!=NULL)&&(numberSize!=0))
    {
        if(numberSize>=(UTE_CALL_DIAL_NUMBERS_MAX-6))
        {
            memcpy(&number[(UTE_CALL_DIAL_NUMBERS_MAX-6)],"\x00\x2e\x00\x2e\x00\x2e",6);
            uteModuleCallData.callData.numberSize = UTE_CALL_DIAL_NUMBERS_MAX;
        }
        else
        {
            uteModuleCallData.callData.numberSize = numberSize;
        }
        memcpy(&uteModuleCallData.callData.number[0],&number[0],uteModuleCallData.callData.numberSize);
    }
    //APP_PRINT_INFO2("%s,numberSize=%d", TRACE_STRING(__func__),uteModuleCallData.callData.numberSize);
}

/**
*@brief    清除号码和联系人名信息
*@details  number :ascii name:UTF8
*@author   casen
*@date     2021-12-08
*/
void uteModuleCallClearNumberAndName(void)
{
    uteModuleCallData.callData.needClearNumberAndName = false;
    memset(&uteModuleCallData.callData.number[0],0,UTE_CALL_DIAL_NUMBERS_MAX);
    uteModuleCallData.callData.numberSize = 0;
    memset(&uteModuleCallData.callData.name[0],0,UTE_CALL_NAME_MAX);
    uteModuleCallData.callData.nameSize = 0;
}

/**
*@brief    准备同步通讯录
*@details
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallSyncAddressBookStart(void)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,dirInfo->filesCnt = %d", __func__,dirInfo->filesCnt);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,del file=%s", __func__,&path[0]);
    uteModuleFilesystemDelFile((char *)&path[0]);
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief    同步通讯录数据
*@details
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallSyncAddressBookData(uint8_t *receive,uint8_t length)
{
    uint8_t receive_onece_dataNum = receive[2];
    uint8_t *buff = (uint8_t *)uteModulePlatformMemoryAlloc(receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
    uint8_t dataIndex = 3,contactLen = 0,phoneNumLen = 0,contactSaveLen = 0;
    memset(buff,0x00,receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
    for(uint8_t i=0; i<receive_onece_dataNum; i++)
    {
        phoneNumLen = receive[dataIndex+2];
        contactLen =  receive[dataIndex+3];
        contactSaveLen = (contactLen>(UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH-1))?(UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH-1):contactLen;
        memcpy(&buff[i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH],&receive[dataIndex+2],(phoneNumLen+contactSaveLen+2));
        dataIndex = dataIndex + (phoneNumLen + contactLen + 4);
    }
    uint8_t path[40];
    memset(&path[0],0,40);
    uint16_t contactsTotalSize = 0;
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY | FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&contactsTotalSize,2);

        uteModuleFilesystemSeek(file,2+(contactsTotalSize)*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&buff[0],receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
        //APP_PRINT_INFO1("name = %b",TRACE_BINARY(receive_onece_dataNum*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH, &buff[0]));
        contactsTotalSize += receive_onece_dataNum;
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,contactsTotalSize = %d", __func__,contactsTotalSize);
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&contactsTotalSize,2);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(buff);
}

/**
*@brief    同步通讯录结束
*@details
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallSyncAddressBookEnd(void)
{
    uteModuleCallGetAddressBookSize();
#if UTE_MODULE_SCREENS_SYNC_BOOK_SUCCESS_SUPPORT
    uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_SYNC_BOOK_SUCCESS_ID);
#endif
}

/**
*@brief    全部删除通讯录数据
*@details
*@author   pcm
*@date     2022-11-30
*/
void uteModuleCallDeleteAllBookData(void)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,dirInfo->filesCnt = %d", __func__,dirInfo->filesCnt);
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR, UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,del file=%s", __func__,&path[0]);
    uteModuleFilesystemDelFile((char *)&path[0]);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
*@brief    获取通讯录联系人个数
*@details
*@author   casen
*@date     2021-12-06
*/
uint16_t uteModuleCallGetAddressBookSize(void)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    uint16_t contactsTotalSize = 0;
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR, UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&contactsTotalSize,2);
        UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,contactsTotalSize = %d", __func__,contactsTotalSize);
        uteModuleFilesystemCloseFile(file);
    }
    return contactsTotalSize;
}

/**
*@brief    解析通讯录联系人名称
*@details  data: 格式号码长度+名字长度+号码(ascii)+联系人(unicode)
           name: 名字内容，unicode 编码
                     nameLen : 名字长度
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallParseAddressBookContactNameAndNumber(uint8_t *data,uint8_t *name,uint8_t *nameLen,uint8_t *number,uint8_t *numberLen)
{
    uint8_t contactNameLen = 0, contactNumberLen = 0;
    contactNumberLen= data[0];
    contactNameLen = data[1];
    memcpy(number,&data[2],contactNumberLen);
    memcpy(name,&data[2+contactNumberLen],contactNameLen);
    *nameLen = contactNameLen;
    *numberLen = contactNumberLen;
}

/**
*@brief    获取通讯录联系人所有数据
*@details  totalLen: 记录总数
           data: 格式号码长度+名字长度+号码(ascii)+联系人(unicode)
*@author   casen
*@date     2021-12-06
*/
void uteModuleCallGetAllAddressBookContactContent(uint16_t totalLen,ute_module_call_addressbook_t *pData)
{
    uint8_t path[40];
    memset(&path[0],0,40);
    uint8_t tempBuff[UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH];
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        for(uint16_t i = 0; i<totalLen; i++)
        {
            uteModuleFilesystemSeek(file,2+i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&tempBuff[0],UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
            uteModuleCallParseAddressBookContactNameAndNumber(tempBuff,&pData[i].nameUnicode[0],&pData[i].nameUnicodeLen,&pData[i].numberAscii[0],&pData[i].numberAsciiLen);
            UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,i=%d;nameUnicodeLen=%d,numberAsciiLen=%d", __func__,i,pData[i].nameUnicodeLen,pData[i].numberAsciiLen);
        }
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief    根据号码匹配获取通讯录联系人姓名
*@details  totalLen: 记录总数
           data: 格式号码长度+名字长度+号码(ascii)+联系人(unicode)
*@author   casen
*@date     2021-12-06
*/
bool uteModuleCallGetAddressBookContactName(uint8_t *number,uint8_t numberSize,uint8_t *name,uint8_t *nameLen)
{
    uint8_t path[25];
    memset(&path[0],0,25);
    bool isMatch = false;
    uint8_t tempBuff[UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH];
    sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_CALL_DATA_DIR,UTE_MODULE_FILESYSTEM_CALL_ADDRESS_BOOK_DIR);
    void *file;
    uint16_t totalLen = uteModuleCallGetAddressBookSize();
//  &uteModuleCallData.callData.number[0],uteModuleCallData.callData.numberSize
    ute_module_call_addressbook_t tempData;
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        for(uint16_t i = 0; i<totalLen; i++)
        {
            uteModuleFilesystemSeek(file,2+i*UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH,FS_SEEK_SET);
            uteModuleFilesystemReadData(file,&tempBuff[0],UTE_MODULE_CALL_ADDRESSBOOK_ONCE_MAX_LENGTH);
            uteModuleCallParseAddressBookContactNameAndNumber(tempBuff,&tempData.nameUnicode[0],&tempData.nameUnicodeLen,&tempData.numberAscii[0],&tempData.numberAsciiLen);
            UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,i=%d;nameUnicodeLen=%d,numberAsciiLen=%d", __func__,i,tempData.nameUnicodeLen,tempData.numberAsciiLen);
            // if(isBtPbapNameUpdate == true) break;
            if(memcmp(number,tempData.numberAscii,numberSize) == 0 && numberSize!=0 && numberSize == tempData.numberAsciiLen)
            {
                *nameLen = tempData.nameUnicodeLen;
                memcpy(name,&tempData.nameUnicode[0],tempData.nameUnicodeLen);
                isMatch = true;
                break;
            }
        }
        uteModuleFilesystemCloseFile(file);
    }
    return isMatch;
}
