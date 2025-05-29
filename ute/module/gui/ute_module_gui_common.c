/**
*@file
*@brief        gui 公共层
*@details
*@author       zn.zeng
*@date       2021-09-03
*@version      v1.0
*/
#include "include.h"
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_drv_screen_common.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"
#include "ute_module_heart.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_sport.h"
#include "ute_module_filesystem.h"
#include "ute_module_watchonline.h"
#include "ute_drv_battery_common.h"

/*! gui的数据结构 zn.zeng, 2021-09-03  */
ute_module_gui_common_t uteModuleGuiCommonData AT(.com_text.ute_gui_comdata);
// void *displayOffTimerPointer;
void *clearDepthAfterOffTimerPointer;

/**
*@brief        自动灭屏定时器回调
*@details
*@author       zn.zeng
*@date       2021-09-03
*/
// void uteModuleGuiCommonDisplayOffTimerCallback(void *pxTimer)
// {
//     UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s", __func__);
//     if(uteModuleGuiCommonData.isDisplayOn)
//     {
//         // ute_task_gui_message_t msg;
//         // msg.type = MSG_TYPE_MODULE_GUI_DISPLAY_OFF_TIMER;
//         // uteTaskGuiSendMsg(&msg);
//     }
//     else
//     {
//         uteModulePlatformStopTimer(&displayOffTimerPointer);
//     }
// }

/**
*@brief        延时清除显示深度定时器回调
*@details       关闭显示后开始定时
*@author       zn.zeng
*@date       2021-11-03
*/
void uteModuleGuiCommonClearDepthTimerCallback(void *pxTimer)
{
    UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s", __func__);
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_GUI_CLEAR_DEPTH,0);
}

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
/**
*@brief        熄屏表盘初始化
*@details
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonScreenSaverConfigInit(void)
{
    uint32_t size = strlen(UTE_MODULE_DIAL_SCREEN_SAVER_SORT_STRINGS);
    uint8_t *string = UTE_MODULE_DIAL_SCREEN_SAVER_SORT_STRINGS;
    uint8_t tmp[20];
    uint32_t cpyStartIndex = 0;
    uint8_t configIndex = 0;
    for(uint32_t i=0; i<size; i++)
    {
        if(string[i]==0x2d)//'-'
        {
            memset(&tmp[0],0,20);
            memcpy(&tmp[0],&string[cpyStartIndex],i-cpyStartIndex);
            uint32_t id = atoi((const char *)&tmp[0]);
            cpyStartIndex = i+1;
            UI_WatchConfigTypeDef *p = uteModuleGuiCommonFindWatchWithId(id+UTE_MODULE_GUI_DIAL_SCREEN_SAVER_ID);
            if((p!=NULL)&&(configIndex<UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX))
            {
                uteModuleGuiCommonData.pScreenSaverConfigs[configIndex]=p;
                configIndex++;
            }
        }
    }
    //uteModuleWatchOnlineReadConfigFromFlash();
    uteModuleGuiCommonSetCurrSreenSaverIndex(uteModuleGuiCommonData.displayCtrl.currScreenSaverIndex);
}

/**
*@brief        获取熄屏表盘的配置指针
*@details
*@param[out] UI_WatchConfigTypeDef **config
*@param[in] uint8_t index
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonGetScreenSaverConfig(UI_WatchConfigTypeDef **config,uint8_t index)
{
    UTE_MODULE_LOG(UTE_LOG_GUI_LVL,"%s,index = %d",__func__,index);
    if(index<UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX)
    {
        *config = uteModuleGuiCommonData.pScreenSaverConfigs[index];
    }
}
/**
*@brief        获取当前表盘的配置索引
*@details
*@param[out] uint8_t *index
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonGetCurrScreenSaverIndex(uint8_t *index)
{
    *index = uteModuleGuiCommonData.displayCtrl.currScreenSaverIndex;
}
/**
*@brief        设置当前熄屏表盘的配置索引
*@details
*@param[out] uint8_t *index
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonSetCurrSreenSaverIndex(uint8_t index)
{
    uint8_t maxIndex = UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX;
    uteModuleGuiCommonData.displayCtrl.currScreenSaverIndex = index;
    uteModuleGuiCommonData.screenSaverPreview1Index = index;
    uteModuleGuiCommonData.screenSaverPreview2Index = index+1;
    if(uteModuleGuiCommonData.screenSaverPreview2Index>=maxIndex)
    {
        uteModuleGuiCommonData.screenSaverPreview2Index = 0;
    }
    if(uteModuleGuiCommonData.screenSaverPreview1Index>0)
    {
        uteModuleGuiCommonData.screenSaverPreview0Index = uteModuleGuiCommonData.screenSaverPreview1Index-1;
    }
    else
    {
        uteModuleGuiCommonData.screenSaverPreview0Index = maxIndex-1;
    }
    uteModuleGuiCommonSaveConfig();
}
/**
*@brief        获取当前表盘预览的配置索引指针
*@details
*@param[out] uint8_t **index0
*@param[out] uint8_t **index1
*@param[out] uint8_t **index2
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonGetSreenSaverPreviewsIndex(uint8_t **index0,uint8_t **index1,uint8_t **index2)
{
    if(index0!=NULL)
    {
        *index0 = &uteModuleGuiCommonData.screenSaverPreview0Index;
    }
    if(index1!=NULL)
    {
        *index1 = &uteModuleGuiCommonData.screenSaverPreview1Index;
    }
    if(index2!=NULL)
    {
        *index2 = &uteModuleGuiCommonData.screenSaverPreview2Index;
    }
}

/**
*@brief        熄屏表盘功能是否开启
*@author       cxd
*@date       2022-05-03
*/
bool uteModuleGuiCommonIsScreenSaverSetOn(void)
{
#if UTE_MODULE_POWER_SAVING_SUPPORT
    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        return false;
    }
#endif
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
    if(uteModuleNewFactoryTestGetMode() == FACTORY_TEST_MODE_AGING)
    {
        return false;
    }
#endif
    return uteModuleGuiCommonData.displayCtrl.isScreenSaverSetOn;
}
/**
*@brief        熄屏表盘功能开关设置
*@details
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonScreenSaverSetSwitch(void)
{
    uteModuleGuiCommonData.displayCtrl.isScreenSaverSetOn = !uteModuleGuiCommonData.displayCtrl.isScreenSaverSetOn;
    uteModuleGuiCommonSaveConfig();
}

/**
*@brief        进入熄屏表盘前关闭几个定时器
*@details
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonStopTimerForScreenSaver(void)
{
    uteModulePlatformStopTimer(&pageSwitchTimerPointer);
    uteModulePlatformStopTimer(&displayOffTimerPointer);
    uteModulePlatformStopTimer(&animationTimerPointer);
}

/**
*@brief        设置息屏界面状态
*@details
*@author       cxd
*@date       2022-05-03
*/
void uteModuleGuiCommonSetInScreenSaver(bool isScreenSaver)
{
    uteModuleGuiCommonData.isScreenSaver = isScreenSaver;
}

/**
*@brief        是否在息屏界面
*@details
*@author       cxd
*@date       2022-05-03
*/
bool uteModuleGuiCommonIsInScreenSaver(void)
{
    return uteModuleGuiCommonData.isScreenSaver;
}
#endif

/**
 * @brief        保存一级界面显示控制参数
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
void uteModuleGuiCommonSavescreenTblSort(uint8_t *tblSort, uint8_t sortCnt)
{
    if(memcmp(tblSort,&uteModuleGuiCommonData.displayCtrl.screenTblSort,MAX_FUNC_SORT_CNT) != 0)
    {
        void *file;
        if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SCREEN_TBL_SORT,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
        {
            uint8_t writeBuff[MAX_FUNC_SORT_CNT + 1];
            memset(&writeBuff[0],0,sizeof(writeBuff));
            writeBuff[0] = sortCnt;
            memcpy(&writeBuff[1],&tblSort[0],sortCnt);
            uteModuleFilesystemWriteData(file,&writeBuff[0],sizeof(writeBuff));
            uteModuleFilesystemCloseFile(file);
        }
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,save screen ctl bits",__func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,tblSort,MAX_FUNC_SORT_CNT);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,screen ctl bits not change",__func__);
    }
}

/**
 * @brief        读取一级界面显示控制参数
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
void uteModuleGuiCommonReadScreenTblSort(void)
{
    void *file;
    static const uint8_t defaultScreenTblSort[UTE_CUI_SCREEN_TBL_SORT_CNT_DEFAULT] = UTE_CUI_SCREEN_TBL_SORT_ARRAY_DEFAULT;
    memcpy(&uteModuleGuiCommonData.displayCtrl.screenTblSort[0],&defaultScreenTblSort[0],sizeof(defaultScreenTblSort));
    uteModuleGuiCommonData.displayCtrl.screenSortCnt = UTE_CUI_SCREEN_TBL_SORT_CNT_DEFAULT;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SCREEN_TBL_SORT,&file,FS_O_RDONLY))
    {
        uint8_t readBuff[MAX_FUNC_SORT_CNT + 1];
        memset(&readBuff[0],0,sizeof(readBuff));
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],sizeof(readBuff));
        uteModuleFilesystemCloseFile(file);

        uteModuleGuiCommonData.displayCtrl.screenSortCnt =  readBuff[0];
        memcpy(&uteModuleGuiCommonData.displayCtrl.screenTblSort[0], &readBuff[1], MAX_FUNC_SORT_CNT);

        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,read screen ctl bits",__func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,&uteModuleGuiCommonData.displayCtrl.screenTblSort[0],MAX_FUNC_SORT_CNT);
    }
}

/**
 * @brief        外部获取一级界面显示控制参数
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
AT(.com_text.ute_gui)
void uteModuleGuiCommonGetScreenTblSort(uint8_t *tblSort, uint8_t *sortCnt)
{
    if(tblSort != NULL && sortCnt != NULL && memcmp(tblSort, &uteModuleGuiCommonData.displayCtrl.screenTblSort[0], MAX_FUNC_SORT_CNT))
    {
        *sortCnt = uteModuleGuiCommonData.displayCtrl.screenSortCnt;
        memcpy(tblSort, &uteModuleGuiCommonData.displayCtrl.screenTblSort[0], uteModuleGuiCommonData.displayCtrl.screenSortCnt);
    }
}

/**
*@brief        读取显示参数
*@details
*@author       zn.zeng
*@date       2021-10-23
*/
void uteModuleGuiCommonReadConfig(void)
{
    void *file;
    uint8_t readbuff[11];
    memset(readbuff,0,11);
    /*! 显示参数zn.zeng, 2021-08-20  */
    readbuff[0] = DEFAULT_SCREEN_ON_TIME_SECOND;
    readbuff[1] = DEFAULT_TEMPERATURE_IS_FAHRENHEIT;
    readbuff[2] = DEFAULT_SCREEN_BACKLIGHT_PERCENT;
    readbuff[3] = DEFAULT_WATCH_INDEX;
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    readbuff[8] = UTE_MODULE_DIAL_SCREEN_DEFAULT_STATUS;
    readbuff[9] = 0;
#endif
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DISPLAYINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],11);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleGuiCommonData.displayCtrl.displayOffTimeSecond = readbuff[0];
    uteModuleGuiCommonData.displayCtrl.isFahrenheit = readbuff[1];
    uteModuleGuiCommonData.displayCtrl.backLightPercent = readbuff[2];
    uteModuleGuiCommonData.displayCtrl.currWatchIndex = readbuff[3];
    uteModuleGuiCommonData.themeTypeId = readbuff[4]<<24|readbuff[5]<<16|readbuff[6]<<8|readbuff[7];
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    uteModuleGuiCommonData.displayCtrl.isScreenSaverSetOn = readbuff[8];
    uteModuleGuiCommonData.displayCtrl.currScreenSaverIndex = readbuff[9];
#endif
    uteModuleGuiCommonData.isPowerSavingOpen = readbuff[10];
    uteModuleGuiCommonReadScreenTblSort();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,displayOffTime=%d,isFahrenheit=%d,backLightPercent=%d,currWatchIndex=%d,themeTypeId=%d", __func__,uteModuleGuiCommonData.displayCtrl.displayOffTimeSecond,uteModuleGuiCommonData.displayCtrl.isFahrenheit,
                   uteModuleGuiCommonData.displayCtrl.backLightPercent,uteModuleGuiCommonData.displayCtrl.currWatchIndex,uteModuleGuiCommonData.themeTypeId);
}

/**
*@brief        gui init
*@details
*@author       zn.zeng
*@date       2021-09-03
*/
void uteModuleGuiCommonInit(void)
{
    memset(&uteModuleGuiCommonData,0,sizeof(ute_module_gui_common_t));
    /*! 创建定时器 zn.zeng, 2021-09-03  */
    // uteModulePlatformCreateTimer(&displayOffTimerPointer, "display off",1, 5000, false, uteModuleGuiCommonDisplayOffTimerCallback);
    uteModulePlatformCreateTimer(&clearDepthAfterOffTimerPointer, "clear depth",1, 5000, false, uteModuleGuiCommonClearDepthTimerCallback);
    uteModuleGuiCommonReadConfig();
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    uteModuleGuiCommonScreenSaverConfigInit();
#endif
    uteModuleGuiCommonData.isGoBackDisplay = true;
    func_cb.menu_style = (uint8_t)uteModuleGuiCommonGetThemeTypeId();
    uteModuleGuiCommonWatchConfigInit();
}

/**
*@brief        外部用清除所有深度的方法
*@details
*@param[in]
*@author      casen
*@date        2021-12-23
*/
void uteModuleGuiCommonDisplayExternalClearDepth(void)
{
    task_stack_init();
    // latest_task_init(); //最近任务
    task_stack_push(FUNC_CLOCK);
    func_cb.sta = FUNC_CLOCK;
}

/**
*@brief        清除显示深度
*@details
*@param[in] bool isAllClear  是否全部清除，否只清除最后一级
*@author       zn.zeng
*@date       2021-10-22
*/
void uteModuleGuiCommonDisplayDepthClearTop(bool isAllClear)
{
    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
    msg_enqueue(EVT_MSGBOX_EXIT);
    msg_enqueue(EVT_CLOCK_SUB_SIDE_EXIT);

    if ((bt_cb.disp_status >= BT_STA_INCOMING && bt_cb.disp_status <= BT_STA_OTA) || func_cb.sta == FUNC_OTA_UI_MODE || is_fot_start())
    {
        return;
    }

    if(uteModuleSportMoreSportIsRuning() && func_cb.sta != FUNC_SPORT_SUB_RUN)
    {
        func_cb.sta = FUNC_SPORT_SUB_RUN;
        return;
    }

    UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,isAllClear = %d", __func__,isAllClear);
    if (isAllClear)
    {
        task_stack_init();
        // latest_task_init(); // 最近任务
        if (uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE)
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,return FUNC_CHARGE", __func__);
            func_cb.sta = FUNC_CHARGE;
            task_stack_push(FUNC_CHARGE);
        }
        else
        {
            task_stack_push(FUNC_CLOCK);
            func_cb.sta = FUNC_CLOCK;
        }
    }
    else
    {
        func_cb.sta = task_stack_pop();
        if (!func_cb.sta)
        {
            func_cb.sta = FUNC_CLOCK;                                 //异常返回表盘
        }
    }
}

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
/**
*@brief        获取临时保存的屏幕id
*@details
*@author       xjc
*@date       2022-10-31
*/
int uteModuleGuiCommonGetTempUIConfigId(void)
{
    if ((uteModuleGuiCommonData.pTempUIConfig != NULL))
    {
        if(uteModuleGuiCommonData.pTempUIConfig->screenId != UTE_MOUDLE_SCREENS_SCREEN_SAVER_ID)
        {
            UTE_MODULE_LOG(UTE_LOG_GUI_LVL,"%s,pTempUIConfig->screenId=%d",__func__,uteModuleGuiCommonData.pTempUIConfig->screenId);
            return uteModuleGuiCommonData.pTempUIConfig->screenId;
        }
    }
    if (uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE)
    {
        UTE_MODULE_LOG(UTE_LOG_GUI_LVL,"%s,UTE_MOUDLE_SCREENS_CHARGER_ID",__func__);
        return UTE_MOUDLE_SCREENS_CHARGER_ID;
    }
    else if(uteModuleSportMoreSportIsRuning())
    {
        UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID", __func__);
        return UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID;
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_GUI_LVL,"%s,UTE_MOUDLE_SCREENS_WATCHMAIN_ID",__func__);
        return UTE_MOUDLE_SCREENS_WATCHMAIN_ID;
    }
}
#endif
/**
*@brief        关闭显示
*@details
*@author       zn.zeng
*@date       2021-09-18
*/
void uteModuleGuiCommonDisplayOff(bool isPowerOff)
{
    UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,.isDisplayOn = %d", __func__,uteModuleGuiCommonData.isDisplayOn);
    if(isPowerOff)
    {
        if(uteModuleGuiCommonData.isGoBackDisplay)
        {
            uteModulePlatformRestartTimer(&clearDepthAfterOffTimerPointer,UTE_MODULE_GUI_CLEAR_DEPTH_AFTER_TIME_SECOND*1000);
        }
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
        uteDrvScreenEsdTeIntErrorCheckTimerStop();
#endif
        // uteDrvScreenCommonDisplayOff();
        // uteDrvTpCommonSleep();
        uteModuleGuiCommonData.isDisplayOn = false;
        // uteModulePlatformStopTimer(&displayOffTimerPointer);
        uteModuleSprotResetRovllverScreenMode();

        //充电中，返回充电界面
        if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE && func_cb.sta != FUNC_CHARGE)
        {
            func_cb.sta = FUNC_CHARGE;
            task_stack_push(FUNC_CHARGE);
        }
    }
    else
    {
        uteModulePlatformStopTimer(&clearDepthAfterOffTimerPointer);
        uteModuleGuiCommonData.isDisplayOn = true;
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
        uteDrvScreenEsdTeIntErrorCheckTimerStart(1000);
#endif
    }
}

/**
 * @brief        设置熄屏是否允许返回表盘
 * @details
 * @param[in]    allow true 允许，false 不允许
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-10-29
 */
void uteModuleGuiCommonDisplayOffAllowGoBack(bool allow)
{
    uteModuleGuiCommonData.isGoBackDisplay = allow;
}

/**
*@brief    抬手亮屏msg处理函数
*@details
*@author        zn.zeng
*@date        2021-08-06
*/
void uteModuleGuiCommonHandScreenOnMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, gui_sleep_sta=%d", __func__, sys_cb.gui_sleep_sta);
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    if((!uteModuleGuiCommonIsDisplayOn())||(uteModuleGuiCommonIsInScreenSaver()))
#else
    if(!uteModuleGuiCommonIsDisplayOn())
#endif
    {
#if UTE_CUSTOM_HAND_SCREEN_ON_DISPLAY_OFF_TIME
        uteModuleGuiCommonData.isHandScreenOn = true;
#endif
        sys_cb.hand_screen_on = true;
        // uteModuleSprotResetRovllverScreenMode();
    }
}
/**
*@brief    抬下灭屏msg处理函数
*@details
*@author        zn.zeng
*@date        2021-08-06
*/
void uteModuleGuiCommonHandScreenOffMsg(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, gui_sleep_sta=%d,guioff_delay=%d", __func__, sys_cb.gui_sleep_sta, sys_cb.guioff_delay);
    // uteModuleGuiCommonDisplayOff(false);
    if(!sys_cb.gui_sleep_sta && sys_cb.guioff_delay)
    {
        sys_cb.guioff_delay = 1;
        // gui_sleep();                //仅熄屏
        uteModuleSprotResetRovllverScreenMode();
    }
}

#if UTE_CUSTOM_HAND_SCREEN_ON_DISPLAY_OFF_TIME
/**
* @brief        重置抬手亮屏标志
* @details      用于自定义抬手亮屏时长时，重置抬手亮屏标志
* @author       Wang.Luo
*@date          2022-09-02
* @return       void*
*/
void uteResetHandScreenOnFlag(void)
{
    uteModuleGuiCommonData.isHandScreenOn = false;
}
#endif

/**
*@brief        重置亮屏timer
*@details
*@param[in] UI_ConfigTypeDef *pMenu，界面指针
*@param[in] int16_t displayOffTimeSecond     If the value is not 0, the display off time is the set value (unit: second)
*@author       wuhuowang
*@date       2023-04-25
*/
// void uteModuleGuiCommonResetTimer(int16_t displayOffTimeSecond)
// {
//     ute_display_ctrl_t info = uteModuleGuiCommonData.displayCtrl;
//     uteModulePlatformRestartTimer(&displayOffTimerPointer, displayOffTimeSecond * 1000);
// }

/**
*@brief        获取当前表盘的配置索引
*@details
*@param[out] uint8_t *index
*@author       zn.zeng
*@date       2021-10-25
*/
void uteModuleGuiCommonGetCurrWatchIndex(uint8_t *index)
{
    *index = uteModuleGuiCommonData.displayCtrl.currWatchIndex;
}

/**
*@brief        获取当前总表盘个数
*@details
*@param[out] WatchMaxIndex  返回总个数（包括在线表盘个数）
*@author       pcm
*@date       2022-8-11
*/
uint8_t uteModuleGuiCommonGetCurrWatchMaxIndex(void)
{
    uint8_t WatchMaxIndex = 0;
    WatchMaxIndex = uteModuleWatchOnlineGetVailWatchCnt() + UTE_MODULE_SCREENS_WATCH_CNT_MAX;
    return WatchMaxIndex;
}

/**
*@brief        设置当前表盘的配置索引
*@details
*@param[out] uint8_t *index
*@author       zn.zeng
*@date       2021-10-25
*/
void uteModuleGuiCommonSetCurrWatchIndex(uint8_t index)
{
    uteModuleGuiCommonData.displayCtrl.currWatchIndex = index;
    sys_cb.dialplate_index = index;
    if(index >= UTE_MODULE_SCREENS_WATCH_CNT_MAX)
    {
        uteModuleWatchOnlineUpateConfigFromFlash();
    }
    uteModuleGuiCommonSaveConfig();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,index=%d",__func__,index);
}

/**
*@brief      watch config init
*@details
*@author     zn.zeng
*@date       2021-10-23
*/
void uteModuleGuiCommonWatchConfigInit(void)
{
    uteModuleWatchOnlineUpateConfigFromFlash();
    uteModuleGuiCommonSetCurrWatchIndex(uteModuleGuiCommonData.displayCtrl.currWatchIndex);
}

/**
*@brief  设置亮屏时间和温度单位
*@details
*@param[in] displayOffTime,isOpenHandScreenOn,isFahrenheit
*@author        zn.zeng
*@date        2021-08-20
*/
void uteModuleGuiCommonSetDisplayTemperatureUtil(bool isFahrenheit)
{
    uteModuleGuiCommonData.displayCtrl.isFahrenheit = isFahrenheit;
    uteModuleGuiCommonSaveConfig();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,disFahrenheit=%d", __func__,isFahrenheit);
}

/**
*@brief  获取控制显示内容相关参数
*@details
*@param[in] displayOffTimeSecond,backLightPercent,isFahrenheit，currWatchIndex
*@author    dengli.lu
*@date      2021-10-27
*/
void uteModuleGuiCommonGetDisplayInfo(ute_display_ctrl_t *displayInfo)
{
    memcpy(displayInfo, &uteModuleGuiCommonData.displayCtrl, sizeof(ute_display_ctrl_t));
}
/**
*@brief  保存显示参数信息
*@details
*@author        zn.zeng
*@date        2021-08-20
*/
void uteModuleGuiCommonSaveConfig(void)
{
    /*! 保存到文件zn.zeng, 2021-08-20*/
    void *file;
    uint8_t writebuff[11];
    memset(writebuff,0,11);
    writebuff[0] = uteModuleGuiCommonData.displayCtrl.displayOffTimeSecond;
    writebuff[1] = uteModuleGuiCommonData.displayCtrl.isFahrenheit;
    writebuff[2] = uteModuleGuiCommonData.displayCtrl.backLightPercent;
    writebuff[3] = uteModuleGuiCommonData.displayCtrl.currWatchIndex;
    writebuff[4] = (uteModuleGuiCommonData.themeTypeId>>24)&0xff;
    writebuff[5] = (uteModuleGuiCommonData.themeTypeId>>16)&0xff;
    writebuff[6] = (uteModuleGuiCommonData.themeTypeId>>8)&0xff;
    writebuff[7] = (uteModuleGuiCommonData.themeTypeId>>0)&0xff;
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    writebuff[8] = uteModuleGuiCommonData.displayCtrl.isScreenSaverSetOn;
    writebuff[9] = uteModuleGuiCommonData.displayCtrl.currScreenSaverIndex;
#endif
#if UTE_MODULE_POWER_SAVING_SUPPORT
    writebuff[10] = uteModuleGuiCommonData.isPowerSavingOpen;
#endif
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DISPLAYINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],11);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief        设置背光亮度百分比
*@details
*@author       dengli.lu
*@date       2021-10-30
*/
void uteModuleGuiCommonSetBackLightPercent(uint8_t setBackLightPercent)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,setBackLightPercent:%d",__func__,setBackLightPercent);
    uteModuleGuiCommonData.displayCtrl.backLightPercent = setBackLightPercent;
    uteModuleGuiCommonSaveConfig();
}

/**
*@brief        获取背光亮度百分比
*@details
*@author       xiaodong.chen
*@date       2022.4.15
*/
uint8_t uteModuleGuiCommonGetBackLightPercent(void)
{
#if UTE_MODULE_POWER_SAVING_SUPPORT
    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        return DEFAULT_BACK_LIGHT_PERCENT_MIN;
    }
    else
#endif
    {
        return uteModuleGuiCommonData.displayCtrl.backLightPercent;
    }
}
/**
*@brief        设置亮屏时长
*@details
*@author       dengli.lu
*@date       2021-12-02
*/
void uteModuleGuiCommonSetDisplayOffTime(uint8_t displayOffTimeSecond)
{
    uteModuleGuiCommonData.displayCtrl.displayOffTimeSecond = displayOffTimeSecond;
    uteModuleGuiCommonSaveConfig();
}

/**
*@brief        获取当前亮屏时长
*@details
*@author       dengli.lu
*@date       2021-12-02
*/
AT(.com_text.ute_gui)
uint8_t uteModuleGuiCommonGetDisplayOffTime(void)
{
    return uteModuleGuiCommonData.displayCtrl.displayOffTimeSecond;
}
/**
*@brief        返回上一级界面
*@detail
*@author       zn.zeng
*@date       2021-11-03
*/
void uteModuleGuiCommonGoBackLastScreen(void)
{
    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
    msg_enqueue(EVT_MSGBOX_EXIT);
    msg_enqueue(EVT_CLOCK_SUB_SIDE_EXIT);

    if ((bt_cb.disp_status >= BT_STA_INCOMING && bt_cb.disp_status <= BT_STA_OTA) || func_cb.sta == FUNC_OTA_UI_MODE || is_fot_start())
    {
        return;
    }

    func_directly_back_to();
}

/**
*@brief        开始显示界面
*@detail
*@param[in] int screenId 界面唯一id
*@author       zn.zeng
*@date       2021-09-03
*/
void uteTaskGuiStartScreen(uint8_t screenId, uint16_t switchMode, const char *format, ...)
{
#if UTE_LOG_SYSTEM_LVL
    if (format != NULL)
    {
        va_list args;
        va_start(args, format);
        char debugInfo[256];
        vsnprintf(debugInfo, sizeof(debugInfo), format, args);
        va_end(args);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s:%s", __func__, debugInfo);
    }
#endif

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,%d->%d,mode:%d", __func__, func_cb.sta, screenId, switchMode);

    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = true;
    }
    reset_sleep_delay_all();

    if ((bt_cb.disp_status >= BT_STA_INCOMING && bt_cb.disp_status <= BT_STA_OTA) || func_cb.sta == FUNC_OTA_UI_MODE || is_fot_start() || screenId == FUNC_NULL)
    {
        return;
    }

    if(func_cb.flag_sort) //在快捷任务界面跳转到其他界面
    {
        func_cb.flag_sort_jump = true;
        func_cb.flag_sort = false;
    }

    if (func_cb.sta != screenId)
    {
        msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
        msg_enqueue(EVT_MSGBOX_EXIT);
        msg_enqueue(EVT_CLOCK_SUB_SIDE_EXIT);
        if (switchMode != 0 && !(switchMode & FUNC_SWITCH_CANCEL) && !(switchMode & FUNC_SWITCH_DIRECT))
        {
            func_switch_to(screenId, switchMode);
        }
        else
        {
            uteModuleGuiCommonSetSwitchToMenu(true);
            func_cb.sta = screenId;
            task_stack_push(screenId);
        }
    }
}

bool uteModuleGuiCommonIsSwitchToMenu(void)
{
    return uteModuleGuiCommonData.isSwitchToMenu;
}

void uteModuleGuiCommonSetSwitchToMenu(bool isSwitchToMenu)
{
    uteModuleGuiCommonData.isSwitchToMenu = isSwitchToMenu;
}

/**
 * @brief        从栈中删除某个界面
 * @param[in]    screenId 界面ID
 * @author       Wang.Luo
 * @date         2025-03-14
 */
void uteTaskGuiStackRemoveScreenId(uint8_t screenId)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,screenId;%d",__func__,screenId);
    if(func_cb.sta != screenId && task_stack_contains(screenId))
    {
        task_stack_remove(screenId);
    }
}

/**
*@brief        设置主题类型，如有需要，可增加type
*@details
*@author       dengli.lu
*@date       2021-11-08
*/
void uteModuleGuiCommonSetThemeTypeId(int themeTypeId)
{
    if(uteModuleGuiCommonData.themeTypeId != themeTypeId)
    {
        uteModuleGuiCommonData.themeTypeId = themeTypeId;
        uteModuleGuiCommonSaveConfig();
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,themeTypeId=%d",__func__,themeTypeId);
    }
}
/**
*@brief        获取主题类型，如有需要，可增加type
*@details
*@author       dengli.lu
*@date       2021-11-17
*/
int uteModuleGuiCommonGetThemeTypeId(void)
{
    return uteModuleGuiCommonData.themeTypeId;
}

/**
*@brief        获取当前亮屏状态
*@details
*@author       zn.zeng
*@date       2021-11-30
*/
bool uteModuleGuiCommonIsDisplayOn(void)
{
    return uteModuleGuiCommonData.isDisplayOn;
}

/**
*@brief        获取当前显示界面的ID
*@details
*@author       xjc
*@date       2022-01-11
*/
int uteModuleGuiCommonGetCurrentScreenId(void)
{
    return func_cb.sta;//gui_get_stack_top();
}

/**
*@brief        获取上个显示界面的ID
*@details
*@author       xjc
*@date       2022-01-11
*/
int uteModuleGuiCommonGetLastScreenId(void)
{
    return task_stack_get_last();//gui_get_stack_top();
}


#if UTE_MODULE_GUI_TESTING_NOT_GOTO_NOTIFICATION_SCREEN_SUPPORT
/**
*@brief     是否需要显示消息提醒界面
*@details   在这些界面功能测试中不需要消息提醒（但是会保存到历史列表里面）
*@author    xjc
*@date      2022-05-23
*/
bool uteModuleGuiCommonIsDontNeedNotificationGuiScreen(void)
{
    bool result =false;
    int id = uteModuleGuiCommonGetCurrentScreenId();
#if UTE_MODULE_HEART_SUPPORT
    if((id == UTE_MOUDLE_SCREENS_HEART_RATE_ID) && (uteModuleHeartIsWear()))
    {
        result =true;
    }
#endif
#if UTE_MODULE_SCREENS_BLOOD_PRESSURE_SUPPORT
    if((id == UTE_MOUDLE_SCREENS_BLOOD_PRESSURE_ID) && (uteModuleBloodpressureIsTesting()))
    {
        result =true;
    }
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
    if((id == UTE_MOUDLE_SCREENS_BLOOD_OXYGEN_ID) && (uteModuleBloodoxygenIsTesting()))
    {
        result =true;
    }
#endif
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
    if((id == FUNC_PRESSURE || id == FUNC_MOOD || id == UTE_MOUDLE_SCREENS_FATIGUE_ID)\
       &&(uteModuleEmotionPressureIsTesting()))
    {
        result =true;
    }
#endif
#if UTE_MODULE_SCREENS_BREATH_RATE_SUPPORT
    if((id == UTE_MOUDLE_SCREENS_BREATH_RATE_ID)&&(uteModuleBreathrateIsTesting()))
    {
        result =true;
    }
#endif
#if UTE_MOUDLE_SCREENS_BREATH_TRAINING_ID
    ute_breathtraining_data_t breathtrainingData;
    uteModuleGetBreathTrainingData(&breathtrainingData);
    if((id == UTE_MOUDLE_SCREENS_BREATH_TRAINING_ID)&&(breathtrainingData.isBreathTraining))
    {
        result =true;
    }
#endif
#if UTE_MODULE_SCREENS_QUICK_REPLY_LIST_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_QUICK_REPLY_LIST_ID)
    {
        result =true;
    }
#endif
#if UTE_MODULE_PLAYBACK_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_MIC_RECORDING_ID || id == UTE_MOUDLE_SCREENS_MIC_RECORD_PLAYING_ID || id == UTE_MOUDLE_SCREENS_MIC_RECORD_LIST_ID)
    {
        result =true;
    }
#endif
#if UTE_MODULE_GAME_FLYBIRD_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_GAME_FLYBIRD_ID)
    {
        result =true;
    }
#endif
#if UTE_MODULE_GAME_NUMBER_KLOTSKI_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_GAME_NUMBER_KLOTSKI_ID)
    {
        result =true;
    }
#endif
#if UTE_MODULE_ALI_UPAY_SUPPORT || UTE_MODULE_ALI_UPAY_V02_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_ALI_UPAY_BINDING_ID)
    {
        result =true;
    }
    if(id == UTE_MOUDLE_SCREENS_ALI_UPAY_STARTUP_PAGE_ID)
    {
        result =true;
    }
    if (id == UTE_MOUDLE_SCREENS_ALI_UPAY_PAYMENT_ID)
    {
        result =true;
    }
#endif
#if UTE_DRV_BLOODPRESSURE_VITA_UART_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_VITA_BLOODPRESSURE_CALIB_ID && BP_CALIB_UNFINISHED == uteModuleBloodpressureVitaGetCalibResult())
    {
        result =true;
    }
#endif
#if UTE_MODULE_SCREENS_ECG_SAMPLING_SUPPORT
    if(id == UTE_MOUDLE_SCREENS_ECG_SAMPLING_ID && uteModuleEcgIsTesting())
    {
        result =true;
    }
#endif
    if(!uteModuleGuiCommonData.isDisplayOn)
    {
        result = false;
    }
    return result;
}
#endif //end UTE_MODULE_GUI_TESTING_NOT_GOTO_NOTIFICATION_SCREEN_SUPPORT

/**
*@brief     获取当前有效表盘数
*@details
*@author    zn.zeng
*@date      2022-06-28
*/
uint8_t uteModuleGuiCommonGetVailWatchCnt(void)
{
    uint8_t maxCnt = UTE_MODULE_SCREENS_WATCH_CNT_MAX;
    if(uteModuleWatchOnlineGetVailWatchCnt())
    {
        maxCnt = maxCnt + uteModuleWatchOnlineGetVailWatchCnt();
    }
    UTE_MODULE_LOG(UTE_LOG_GUI_LVL,"%s,maxCnt=%d",__func__,maxCnt);
    return maxCnt;
}
/*
*@brief     计算点到圆心的距离
*@details
*@author    cxd
*@date      2022-05-30
*/

#if 0
int16_t uteModuleGuiGetDistanceFromCenter(UTE_UI_Point_t p)
{
    return (int16_t)(sqrt((UTE_DRV_SCREEN_WIDTH/2-p.x)*(UTE_DRV_SCREEN_WIDTH/2-p.x)+(UTE_DRV_SCREEN_HEIGHT/2-p.y)*(UTE_DRV_SCREEN_HEIGHT/2-p.y)));
}

/*
*@brief     在圆形屏幕显示范围内获取某个矩形的随机起始位置
*@details
*@author    cxd
*@date      2022-05-30
*/
void uteModuleGuiGetRandPositionInCircularScreen(UTE_UI_Point_t *pOutPoint,int16_t width,int16_t height)
{
    UTE_UI_Point_t sPoint;

RESTART_LINE:
    sPoint.x=platform_random(UTE_DRV_SCREEN_WIDTH-width);
    sPoint.y=platform_random(UTE_DRV_SCREEN_HEIGHT-height);

    if(uteModuleGuiGetDistanceFromCenter(sPoint) >(UTE_DRV_SCREEN_WIDTH/2))goto RESTART_LINE;
    sPoint.x += width;
    if(uteModuleGuiGetDistanceFromCenter(sPoint) >(UTE_DRV_SCREEN_WIDTH/2))goto RESTART_LINE;
    sPoint.y += height;
    if(uteModuleGuiGetDistanceFromCenter(sPoint) >(UTE_DRV_SCREEN_WIDTH/2))goto RESTART_LINE;
    sPoint.x -= width;
    if(uteModuleGuiGetDistanceFromCenter(sPoint) >(UTE_DRV_SCREEN_WIDTH/2))goto RESTART_LINE;

    sPoint.y -= height;
    memcpy(pOutPoint,&sPoint,sizeof(UTE_UI_Point_t));
}
#endif

#if UTE_MODULE_POWER_SAVING_SUPPORT
/**
*@brief  设置省电模式开关
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
void uteModuleGuiCommonSetPowerSavingModeOpen(bool isPowerSavingModeOpen)
{
    uteModuleGuiCommonData.isPowerSavingOpen = isPowerSavingModeOpen;
#if UTE_MODULE_SCREENS_SCREEN_LIFT_WRIST_HANDLE_SUPPORT
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !isPowerSavingModeOpen;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(quick.isTurnTheWrist,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
#endif
    uteModuleGuiCommonSaveConfig();
}
/**
*@brief  获取省电模式开关
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
bool uteModuleGuiCommonGetPowerSavingModeOpen(void)
{
    return uteModuleGuiCommonData.isPowerSavingOpen;
}
#endif
/**
*@brief  是否允许手势息屏,下列状态不允许息屏（可运用于落腕息屏、覆盖息屏）,其他界面可自行添加
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
bool uteModuleGuiCommonIsAllowHandGestureDisplayOff(void)
{
    int id = uteModuleGuiCommonGetCurrentScreenId();
    if(uteModuleGuiCommonData.isDisplayOn)
    {
#if UTE_MODULE_HEART_SUPPORT
        if((id == FUNC_HEARTRATE) && (uteModuleHeartIsWear()))
        {
            return false;
        }
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
        if((id == FUNC_BLOOD_OXYGEN) && (uteModuleBloodoxygenIsTesting()))
        {
            return false;
        }
#endif
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
        if ((
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT
                (id == FUNC_PRESSURE)
#endif
#if UTE_MODULE_SCREENS_EMOTION_SUPPORT
                || (id == FUNC_MOOD)
#endif
#if UTE_MODULE_SCREENS_FATIGUE_SUPPORT
                || (id == UTE_MOUDLE_SCREENS_FATIGUE_ID)
#endif
            ) &&
            uteModuleEmotionPressureIsTesting())
        {
            return false;
        }
#endif
#if UTE_BT30_CALL_SUPPORT
        if (id == FUNC_BT_CALL)
        {
            return false;
        }
#endif
#if UTE_MOUDLE_SCREENS_BREATH_TRAINING_ID
        if(id == UTE_MOUDLE_SCREENS_BREATH_TRAINING_ID)
        {
            ute_breathtraining_data_t breathtrainingData;
            uteModuleGetBreathTrainingData(&breathtrainingData);
            if(breathtrainingData.isBreathTraining)
            {
                return false;
            }
        }
#endif
#if UTE_MODULE_SCREENS_POWERON_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_POWERON_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_POWEROFF_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_POWEROFF_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
        if(id == FUNC_FLASHLIGHT)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_OTA_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_OTA_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_SYNC_WATCH_ONLINE_ID)
        {
            return false;
        }
#endif
#if 0//UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
        if((id == UTE_MOUDLE_SCREENS_NEW_FACTORY_MODE_SELECT_ID) || (id == UTE_MOUDLE_SCREENS_NEW_FACTORY_AGING_ID) || (id == UTE_MOUDLE_SCREENS_NEW_FACTORY_MODULE_ID))
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_STOP_WATCH_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_STOP_WATCH_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_FIND_PHONE_SUPPORT
        ute_ble_connect_state_t connectStatus;
        uteApplicationCommonGetBleConnectionState(&connectStatus);
        if((id == UTE_MOUDLE_SCREENS_FIND_PHONE_ID) &&
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
           (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING) &&
#endif // UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
           (connectStatus.isConnected))
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_TAKE_PICTURE_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_TAKE_PICTURE_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_SPORTS_DETAIL_SUPPORT
        if((id == UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID)&&(uteModuleSportMoreSportGetCountZeroIndex() > 0))
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_FACTORY_TEST_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_FACTORY_TEST_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_BREATH_RATE_SUPPORT
        if((id == UTE_MOUDLE_SCREENS_BREATH_RATE_ID)&&(uteModuleBreathrateIsTesting()))
        {
            return false;
        }
#endif
#if UTE_MODULE_PLAYBACK_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_MIC_RECORDING_ID || id == UTE_MOUDLE_SCREENS_MIC_RECORD_PLAYING_ID || id == UTE_MOUDLE_SCREENS_MIC_RECORD_LIST_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_GAME_FLYBIRD_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_GAME_FLYBIRD_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_GAME_NUMBER_KLOTSKI_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_GAME_NUMBER_KLOTSKI_ID)
        {
            return false;
        }
#endif
#ifdef UTE_MODULE_SCREENS_COUNT_DOWN_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_COUNT_DOWN_ID)
        {
            return false;
        }
#endif
    }
    return true;
}
