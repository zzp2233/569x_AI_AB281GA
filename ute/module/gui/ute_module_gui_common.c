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
#include "ute_module_emotionPressure.h"
#include "ute_module_appbinding.h"
#include "ute_module_newFactoryTest.h"
#if UTE_MODULE_SCREENS_SCREEN_LIFT_WRIST_HANDLE_SUPPORT
#include "ute_module_liftwrist.h"
#endif
#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT
#include "ute_module_one_click_measure.h"
#endif
#include "ute_module_findphone.h"
#include "ute_module_notdisturb.h"

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
    if(index >= UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,error index:%d",__func__,index);
        return;
    }
    uteModuleGuiCommonData.displayCtrl.currScreenSaverIndex = index;
    uteModuleGuiCommonSaveConfig();
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

/**
 * @brief        设置是否需要刷新显示
 * @details
 * @param[in]    isUpdate true:需要刷新显示;false:不需要刷新显示
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-07-16
 */
void uteModuleGuiCommonSetUpdateScreenSaver(bool isUpdate)
{
    uteModuleGuiCommonData.isScreenSaverUpdate = isUpdate;
}

/**
 * @brief        获取是否需要刷新显示
 * @details
 * @return       bool
 * @author       Wang.Luo
 * @date         2025-07-16
 */
bool uteModuleGuiCommonIsUpdateScreenSaver(void)
{
    return uteModuleGuiCommonData.isScreenSaverUpdate;
}

/**
 * @brief        息屏显示更新显示
 * @details      0秒时调用一次
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-07-31
 */
void uteModuleGuiCommonScreenSaverUpdateCb(void)
{
    if (uteModuleGuiCommonIsInScreenSaver())
    {
        if(uteModuleGuiCommonCurrIsScreenSaverTimeBucket())
        {
            uteModuleGuiCommonSetUpdateScreenSaver(true);
        }
        else
        {
            tft_exit();
            ctp_exit();
            power_gate_3v3_off();
            uteDrvScreenCommonDisplayOff();
        }
    }
    else
    {
        if (!uteModuleGuiCommonIsDisplayOn() && uteModuleGuiCommonCurrIsScreenSaverTimeBucket())
        {
            // uteTaskGuiStartScreen(FUNC_CLOCK_SAVER, 0, __func__);
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HNAD_SCREEN_ON_NOTIFY,0);
        }
    }
}

/**
 * @brief        当前是否在息屏显示时间段
 * @details
 * @return       true 当前在息屏显示时间段
 * @author       Wang.Luo
 * @date         2025-07-31
 */
bool uteModuleGuiCommonCurrIsScreenSaverTimeBucket(void)
{
    bool screenSaverTimeBucket = false;
    if (uteModuleGuiCommonIsScreenSaverSetOn())
    {
        if (uteModuleGuiCommonData.displayCtrl.screenSaverTimeBucket)
        {
            ute_module_systemtime_time_t time;
            uteModuleSystemtimeGetTime(&time);
            uint16_t currentTime = time.hour * 60 + time.min;
            if (uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin == uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin)
            {
                // 如果开始和结束时间相等，可以视作全天启用或全天禁用
                screenSaverTimeBucket = true;
            }
            else if (uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin > uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin)
            {
                if ((currentTime >= uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin) && (currentTime <= uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin))
                {
                    screenSaverTimeBucket = true;
                }
            }
            else
            {
                if ((currentTime >= uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin) || (currentTime <= uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin))
                {
                    screenSaverTimeBucket = true;
                }
            }
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,currentTime:%d,StartTimeHourMin:%d,EndTimeHourMin:%d,screenSaverTimeBucket:%d\n", __func__, currentTime, uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin, uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin, screenSaverTimeBucket);
        }
        else
        {
            screenSaverTimeBucket = true;
        }
    }
    return screenSaverTimeBucket;
}

/**
 * @brief        切换息屏表盘时间段开关
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-01
 */
void uteModuleGuiCommonSwitchScreenSaverTimeBucket(void)
{
    uteModuleGuiCommonData.displayCtrl.screenSaverTimeBucket = !uteModuleGuiCommonData.displayCtrl.screenSaverTimeBucket;
    uteModuleGuiCommonSaveConfig();
}

/**
 * @brief        获取息屏表盘时间段开关
 * @details
 * @return       uint16_t
 * @author       Wang.Luo
 * @date         2025-08-01
 */
uint16_t uteModuleGuiCommonScreenSaverTimeBucketIsOpen(void)
{
    return uteModuleGuiCommonData.displayCtrl.screenSaverTimeBucket;
}

/**
 * @brief        设置当前修改的时间是开始时间还是结束时间
 * @details
 * @param[in]    isStart true:开始时间  false:结束时间
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-01
 */
void uteModuleGuiCommonSetScreenSaverTimeBucketType(bool isStart)
{
    uteModuleGuiCommonData.displayCtrl.setScreenSaverType = isStart;
}

/**
 * @brief        获取当前修改的时间是开始时间还是结束时间
 * @details
 * @return       bool true:开始时间  false:结束时间
 * @author       Wang.Luo
 * @date         2025-08-01
 */
bool uteModuleGuiCommonCurrSetTimeBucketTypeIsStart(void)
{
    return uteModuleGuiCommonData.displayCtrl.setScreenSaverType;
}

/**
 * @brief        设置息屏表盘生效时间段
 * @details
 * @param[in]    hour 小时
 * @param[in]    min 分钟
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-01
 */
void uteModuleGuiCommonSetScreenSaverTimeBucket(uint8_t hour, uint8_t min)
{
    uint16_t timeBucket = hour * 60 + min;
    if (uteModuleGuiCommonData.displayCtrl.setScreenSaverType)
    {
        if (timeBucket != uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin)
        {
            uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin = timeBucket;
            uteModuleGuiCommonSaveConfig();
        }
    }
    else
    {
        if (timeBucket != uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin)
        {
            uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin = timeBucket;
            uteModuleGuiCommonSaveConfig();
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,startMin:%d,endMin:%d", __func__, uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin, uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin);
}

/**
 * @brief        获取息屏表盘时间段
 * @details
 * @param[out]   startMin 息屏开始时间 hour*60+min
 * @param[out]   endMin 息屏结束时间 hour*60+min
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-01
 */
void uteModuleGuiCommonGetScreenSaverTimeBucket(uint16_t *startMin, uint16_t *endMin)
{
    if (startMin)
    {
        *startMin = uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin;
    }
    if (endMin)
    {
        *endMin = uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin;
    }
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

#if UTE_MODULE_SCREENS_MINI_CARD_SUPPORT
/**
 * @brief        保存小卡片设置
 * @details
 * @param[in]    tblSort 卡片排序
 * @param[in]    sortCnt 卡片排序个数
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-09-09
 */
void uteModuleGuiCommonSaveMiniCardTblSort(uint8_t *tblSort, uint8_t sortCnt)
{
    if (memcmp(tblSort, &uteModuleGuiCommonData.displayCtrl.miniCardTblSort, MAX_FUNC_MINI_SORT_CNT) != 0)
    {
        void *file;
        if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_MINI_CARD_TBL_SORT, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
        {
            uint8_t writeBuff[MAX_FUNC_MINI_SORT_CNT + 1];
            memset(&writeBuff[0], 0, sizeof(writeBuff));
            writeBuff[0] = sortCnt;
            memcpy(&writeBuff[1], &tblSort[0], sortCnt);
            uteModuleFilesystemWriteData(file, &writeBuff[0], sizeof(writeBuff));
            uteModuleFilesystemCloseFile(file);
        }
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,save mini card ctl bits", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL, tblSort, MAX_FUNC_MINI_SORT_CNT);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,mini card not change", __func__);
    }
}

/**
 * @brief        读取小卡片排序
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
void uteModuleGuiCommonReadMiniCardTblSort(void)
{
    void *file;
    static const uint8_t defaultMiniCardTblSort[UTE_GUI_MINI_CARD_TBL_SORT_CNT_DEFAULT] = UTE_GUI_MINI_CARD_TBL_SORT_ARRAY_DEFAULT;
    memcpy(&uteModuleGuiCommonData.displayCtrl.miniCardTblSort[0], &defaultMiniCardTblSort[0], sizeof(defaultMiniCardTblSort));
    uteModuleGuiCommonData.displayCtrl.miniCardSortCnt = UTE_GUI_MINI_CARD_TBL_SORT_CNT_DEFAULT;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_MINI_CARD_TBL_SORT, &file, FS_O_RDONLY))
    {
        uint8_t readBuff[MAX_FUNC_MINI_SORT_CNT + 1];
        memset(&readBuff[0], 0, sizeof(readBuff));
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readBuff[0], sizeof(readBuff));
        uteModuleFilesystemCloseFile(file);

        uteModuleGuiCommonData.displayCtrl.miniCardSortCnt = readBuff[0];
        memcpy(&uteModuleGuiCommonData.displayCtrl.miniCardTblSort[0], &readBuff[1], MAX_FUNC_MINI_SORT_CNT);

        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,read mini card ctl bits", __func__);
        UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL, &uteModuleGuiCommonData.displayCtrl.miniCardTblSort[0], MAX_FUNC_MINI_SORT_CNT);
    }
}

/**
 * @brief        外部获取小卡片排序
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-16
 */
void uteModuleGuiCommonGetMiniCardTblSort(uint8_t *tblSort, uint8_t *sortCnt)
{
    if (tblSort != NULL && sortCnt != NULL && memcmp(tblSort, &uteModuleGuiCommonData.displayCtrl.miniCardTblSort[0], MAX_FUNC_MINI_SORT_CNT))
    {
        *sortCnt = uteModuleGuiCommonData.displayCtrl.miniCardSortCnt;
        memcpy(tblSort, &uteModuleGuiCommonData.displayCtrl.miniCardTblSort[0], uteModuleGuiCommonData.displayCtrl.miniCardSortCnt);
    }
}
#endif

#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
/**
 * @brief        读取俄罗斯方块历史最高分
 * @details
 * @return       u32
 * @author       zhenhua.cai
 * @date         2025-10-17
 */
u32 uteModuleGuiCommonReadTetrisMaxScore(void)
{
    return uteModuleGuiCommonData.tetrisMaxScore;
}
/**
 * @brief        设置俄罗斯方块历史最高分
 * @details
 * @return       u32
 * @author       zhenhua.cai
 * @date         2025-10-17
 */
void uteModuleGuiCommonSaverTetrisMaxScore(u32 score)
{
    if (uteModuleGuiCommonData.tetrisMaxScore)
    {
        if (score != uteModuleGuiCommonData.tetrisMaxScore)
        {
            uteModuleGuiCommonData.tetrisMaxScore = score;
            uteModuleGuiCommonSaveConfig();
        }
    }
    else
    {
        if (score != uteModuleGuiCommonData.tetrisMaxScore)
        {
            uteModuleGuiCommonData.tetrisMaxScore = score;
            uteModuleGuiCommonSaveConfig();
        }
    }
}

#endif

/**
*@brief        读取显示参数
*@details
*@author       zn.zeng
*@date       2021-10-23
*/
void uteModuleGuiCommonReadConfig(void)
{
    void *file;
    uint8_t readbuff[22];
    memset(readbuff,0,sizeof(readbuff));
    /*! 显示参数zn.zeng, 2021-08-20  */
    readbuff[0] = DEFAULT_SCREEN_ON_TIME_SECOND;
    readbuff[1] = DEFAULT_TEMPERATURE_IS_FAHRENHEIT;
    readbuff[2] = DEFAULT_SCREEN_BACKLIGHT_PERCENT;
    readbuff[3] = DEFAULT_WATCH_INDEX;
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    readbuff[8] = UTE_MODULE_DIAL_SCREEN_DEFAULT_STATUS;
    readbuff[9] = 0;
#endif
#if UTE_MODULE_ENCODER_SWITCH_WATCHMAIN_LOCK_SUPPORT
    readbuff[11] = DEFAULT_SWITCHOVER_WATCHMAIN_LOCK;
#endif
    readbuff[12] = DEFAULT_SWITCH_MODE;
    readbuff[13] = DEFAULT_COVER_DISPLAY_OFF;
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    readbuff[14] = UTE_MODULE_SCREEN_SAVE_DEFAULT_TIME_BUCKET_OPEN;
    readbuff[15] = (UTE_MODULE_SCREEN_SAVE_DEFAULT_TIME_BUCKET_START >> 8) & 0xff;
    readbuff[16] = (UTE_MODULE_SCREEN_SAVE_DEFAULT_TIME_BUCKET_START) & 0xff;
    readbuff[17] = (UTE_MODULE_SCREEN_SAVE_DEFAULT_TIME_BUCKET_END >> 8) & 0xff;
    readbuff[18] = (UTE_MODULE_SCREEN_SAVE_DEFAULT_TIME_BUCKET_END) & 0xff;
#endif
#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
    readbuff[19] = 0;
    readbuff[20] = 0;
    readbuff[21] = 0;
#endif

    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DISPLAYINFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],sizeof(readbuff));
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
    uteModuleGuiCommonData.displayCtrl.screenSaverTimeBucket = readbuff[14];
    uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin = readbuff[15] << 8 | readbuff[16];
    uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin = readbuff[17] << 8 | readbuff[18];
#endif
    uteModuleGuiCommonData.isPowerSavingOpen = readbuff[10];
#if UTE_MODULE_ENCODER_SWITCH_WATCHMAIN_LOCK_SUPPORT
    uteModuleGuiCommonData.displayCtrl.isWatchMainLock = readbuff[11];
#endif
    uteModuleGuiCommonData.displayCtrl.switchMode = readbuff[12];
#if UTE_MODULE_COVER_DISPLAY_OFF_SUPPORT
    uteModuleGuiCommonData.isCoverDisplayOffOpen = readbuff[13];
#endif
#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
    uteModuleGuiCommonData.tetrisMaxScore = readbuff[19]<<16|readbuff[20]<<8|readbuff[21];
#endif
#if EN_CUM_SWITCH_LR_MODE_CHOOSE
    switch (uteModuleGuiCommonData.displayCtrl.switchMode)
    {
        case SWITCH_PODE_SINGLE_SCALE:
            sys_cb.switch_lr_left_mode_choose = FUNC_SWITCH_LR_UNIL_ZOOM_LEFT;
            sys_cb.switch_lr_rigth_mode_choose = FUNC_SWITCH_LR_UNIL_ZOOM_RIGHT;
            break;

        case SWITCH_MODE_BILATERAL_SCALE:
            sys_cb.switch_lr_left_mode_choose = FUNC_SWITCH_LR_ZOOM_LEFT;
            sys_cb.switch_lr_rigth_mode_choose = FUNC_SWITCH_LR_ZOOM_RIGHT;
            break;

        default:
            sys_cb.switch_lr_left_mode_choose = FUNC_SWITCH_LR_LEFT;
            sys_cb.switch_lr_rigth_mode_choose = FUNC_SWITCH_LR_RIGHT;
            break;
    }
#endif
    uteModuleGuiCommonReadScreenTblSort();
    uteModuleGuiCommonReadMiniCardTblSort();
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

    if (bt_cb.disp_status == BT_STA_OTA || func_cb.sta == FUNC_OTA_UI_MODE || is_fot_start() || func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL)
    {
        return;
    }

    UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,isAllClear = %d", __func__, isAllClear);

    uint16_t screenId = FUNC_NULL;

    if (isAllClear)
    {
        task_stack_init();

        if (0)
        {
            // pass
        }
#if UTE_USER_ID_FOR_BINDING_SUPPORT
        else if (uteModuleAppBindingGetHasBindingBefore() != HAS_BEEN_CONNECTED)
        {

            ute_module_systemtime_time_t time;
            uteModuleSystemtimeGetTime(&time);
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,Not bound, HasBindingBefore=0x%x, isWatchSetLangage=%d", __func__, uteModuleAppBindingGetHasBindingBefore(), time.isWatchSetLangage);
            screenId = FUNC_POWER_ON_SCAN;
            if (!time.isWatchSetLangage)
            {
                screenId = FUNC_POWER_ON_LANGUAGE;
            }
        }
#endif
        else
        {
            screenId = FUNC_CLOCK;
        }
    }
    else
    {
        screenId = task_stack_pop();
        if (!screenId)
        {
            screenId = FUNC_CLOCK; // 异常返回表盘
        }
    }

    if (uteModuleSportMoreSportIsRuning() && screenId != FUNC_SPORT_SUB_RUN)
    {
        screenId = FUNC_SPORT_SUB_RUN;
    }

    if (uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE)
    {
        screenId = FUNC_CHARGE;
    }

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    if (uteModuleGuiCommonIsInScreenSaver())
    {
        uteModuleGuiCommonData.lastScreenId = screenId;
    }
    else
#endif
    {
        func_cb.sta = screenId;
        task_stack_push(func_cb.sta);
    }
    UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,screenId = %d", __func__, screenId);
}

#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
/**
*@brief        获取临时保存的屏幕id
*@details
*@author       xjc
*@date       2022-10-31
*/
uint16_t uteModuleGuiCommonGetTempScreenId(void)
{
    if (uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE)
    {
        UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,UTE_MOUDLE_SCREENS_CHARGER_ID", __func__);
        return UTE_MOUDLE_SCREENS_CHARGER_ID;
    }
    else if (uteModuleSportMoreSportIsRuning())
    {
        UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID", __func__);
        return UTE_MOUDLE_SCREENS_SPORTS_DETAIL_ID;
    }
    if (uteModuleGuiCommonData.lastScreenId != UTE_MOUDLE_SCREENS_SCREEN_SAVER_ID)
    {
        UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%s,pTempUIConfig->screenId=%d", __func__, uteModuleGuiCommonData.lastScreenId);
        return uteModuleGuiCommonData.lastScreenId;
    }
}

void uteModuleGuiCommonSetTempScreenId(uint16_t screenId)
{
    uteModuleGuiCommonData.lastScreenId = screenId;
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
        // uteDrvScreenCommonDisplayOff();
        // uteDrvTpCommonSleep();
        uteModuleGuiCommonData.isDisplayOn = false;
        // uteModulePlatformStopTimer(&displayOffTimerPointer);
        uteModuleSprotResetRovllverScreenMode();
    }
    else
    {
        uteModulePlatformStopTimer(&clearDepthAfterOffTimerPointer);
        uteModuleGuiCommonData.isDisplayOn = true;
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
    if(!sys_cb.gui_sleep_sta && sys_cb.guioff_delay && uteModuleGuiCommonIsAllowHandGestureDisplayOff())
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
    uteModuleWatchOnlineDataInit();
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
    uint8_t writebuff[22];
    memset(writebuff,0,sizeof(writebuff));
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
#if UTE_MODULE_ENCODER_SWITCH_WATCHMAIN_LOCK_SUPPORT
    writebuff[11] = uteModuleGuiCommonData.displayCtrl.isWatchMainLock;
#endif
    writebuff[12] = uteModuleGuiCommonData.displayCtrl.switchMode;
#if UTE_MODULE_COVER_DISPLAY_OFF_SUPPORT
    writebuff[13] = uteModuleGuiCommonData.isCoverDisplayOffOpen;
#endif
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    writebuff[14] = uteModuleGuiCommonData.displayCtrl.screenSaverTimeBucket;
    writebuff[15] = (uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin>>8)&0xff;
    writebuff[16] = (uteModuleGuiCommonData.displayCtrl.screenSaverStartTimeHourMin)&0xff;
    writebuff[17] = (uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin>>8)&0xff;
    writebuff[18] = (uteModuleGuiCommonData.displayCtrl.screenSaverEndTimeHourMin)&0xff;
#endif
#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
    writebuff[19] = (uteModuleGuiCommonData.tetrisMaxScore>>16)&0xff;
    writebuff[20] = (uteModuleGuiCommonData.tetrisMaxScore>>8)&0xff;
    writebuff[21] = (uteModuleGuiCommonData.tetrisMaxScore)&0xff;
#endif
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_DISPLAYINFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],sizeof(writebuff));
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

#if UTE_MODULE_ENCODER_SWITCH_WATCHMAIN_LOCK_SUPPORT
/**
 * @brief        设置编码器切换表盘锁定开关
 * @details
 * @param[in]    isLock true 锁定，false 不锁定
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-06-18
 */
void uteModuleGuiCommonSwitchEncoderKeysSwitchoverWatchMainLock(void)
{
    uteModuleGuiCommonData.displayCtrl.isWatchMainLock = !uteModuleGuiCommonData.displayCtrl.isWatchMainLock;
    uteModuleGuiCommonSaveConfig();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isWatchMainLock=%d",__func__,uteModuleGuiCommonData.displayCtrl.isWatchMainLock);
}

/**
 * @brief        获取编码器切换表盘锁定状态
 * @details
 * @return       bool 锁定状态 false:未锁定 true:锁定
 * @author       Wang.Luo
 * @date         2025-06-18
 */
bool uteModuleGuiCommonGetEncoderKeysSwitchoverWatchMainLock(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isWatchMainLock=%d",__func__,uteModuleGuiCommonData.displayCtrl.isWatchMainLock);
    return uteModuleGuiCommonData.displayCtrl.isWatchMainLock;
}

#endif

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

    if (func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL || func_cb.sta == FUNC_OTA_UI_MODE || is_fot_start() || screenId == FUNC_NULL)
    {
        return;
    }

    if (func_cb.sta != screenId)
    {
        if (func_cb.flag_sort
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
            && screenId != FUNC_CLOCK_SAVER
#endif
           ) // 在快捷任务界面跳转到其他界面
        {
            func_cb.flag_sort_jump = true;
            func_cb.flag_sort = false;
        }
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
        if (screenId == FUNC_CLOCK_SAVER)
        {
            uteModuleGuiCommonData.lastScreenId = func_cb.sta;
        }
#endif
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
 * @brief        开始显示弹窗
 * @details      可填参数查看msgbox.h
 * @param[in]    msg 弹窗消息文本
 * @param[in]    title 弹窗标题文本
 * @param[in]    time 弹窗时间文本
 * @param[in]    type 弹窗类型
 * @param[in]    index 弹窗索引
 * @param[in]    animat 是否需要动画 true:需要动画 false:不需要动画
 * @return       result 弹窗结果
 * @author       Wang.Luo
 * @date         2025-07-11
 */
int uteModuleGuiCommonStartMsgbox(char *msg, char *title, char *time, int mode, char type, uint8_t index, bool animat)
{

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);

    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = true;
    }
    reset_sleep_delay_all();

    if (func_cb.msg_cb)
    {
        msg_cb_t *msg_cb = func_cb.msg_cb;
        if (msg_cb->frm)
        {
            compo_form_destroy(msg_cb->frm);
            func_free(msg_cb);
            func_cb.msg_cb = NULL;
            msg_cb->frm = NULL;
        }
    }

    if (index)
    {
        sys_cb.cover_index = index;
    }

    sys_cb.cover_animat_dis = !animat;

    return msgbox(msg, title, time, mode, type);
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
AT(.com_text.ute_gui)
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

/**
 * @brief        获取当前切换样式
 * @details
 * @return       切换样式 ute_gui_switch_mode
 * @author       Wang.Luo
 * @date         2025-07-25
 */
uint8_t uteModuleGuiCommonGetDisplayCtrlSwitchMode(void)
{
    return uteModuleGuiCommonData.displayCtrl.switchMode;
}

/**
 * @brief        设置切换样式
 * @details
 * @param[in]    switchMode 切换样式 ute_gui_switch_mode
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-07-25
 */
void uteModuleGuiCommonSetDisplayCtrlSwitchMode(uint8_t switchMode)
{
    if (switchMode != uteModuleGuiCommonData.displayCtrl.switchMode)
    {
        uteModuleGuiCommonData.displayCtrl.switchMode = switchMode;
        uteModuleGuiCommonSaveConfig();
    }
#if EN_CUM_SWITCH_LR_MODE_CHOOSE
    switch (switchMode)
    {
        case SWITCH_PODE_SINGLE_SCALE:
            sys_cb.switch_lr_left_mode_choose = FUNC_SWITCH_LR_UNIL_ZOOM_LEFT;
            sys_cb.switch_lr_rigth_mode_choose = FUNC_SWITCH_LR_UNIL_ZOOM_RIGHT;
            break;

        case SWITCH_MODE_BILATERAL_SCALE:
            sys_cb.switch_lr_left_mode_choose = FUNC_SWITCH_LR_ZOOM_LEFT;
            sys_cb.switch_lr_rigth_mode_choose = FUNC_SWITCH_LR_ZOOM_RIGHT;
            break;

        default:
            sys_cb.switch_lr_left_mode_choose = FUNC_SWITCH_LR_LEFT;
            sys_cb.switch_lr_rigth_mode_choose = FUNC_SWITCH_LR_RIGHT;
            break;
    }
#endif
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
    bool result = false;
    int id = uteModuleGuiCommonGetCurrentScreenId();
#if UTE_MODULE_HEART_SUPPORT
    if ((id == FUNC_HEARTRATE) && (uteModuleHeartIsWear()))
    {
        result = true;
    }
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
    if ((id == FUNC_BLOOD_OXYGEN) && (uteModuleBloodoxygenIsTesting()))
    {
        result = true;
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
        result = true;
    }
#endif
#if UTE_BT30_CALL_SUPPORT
    if (id == FUNC_BT_CALL)
    {
        result = true;
    }
    if (id == FUNC_BT_RING)
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT
    if (id == UTE_MOUDLE_SCREENS_SYNC_WATCH_ONLINE_ID)
    {
        result = true;
    }
#endif
#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT
    if (id == FUNC_MEASURE && uteModuleOneClickMeasureIsTesting())
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_BREATH_RATE_SUPPORT
    if ((id == UTE_MOUDLE_SCREENS_BREATH_RATE_ID) && (uteModuleBreathrateIsTesting()))
    {
        result = true;
    }
#endif
#if UTE_MOUDLE_SCREENS_BREATH_TRAINING_ID
    ute_breathtraining_data_t breathtrainingData;
    uteModuleGetBreathTrainingData(&breathtrainingData);
    if ((id == UTE_MOUDLE_SCREENS_BREATH_TRAINING_ID) && (breathtrainingData.isBreathTraining))
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_QUICK_REPLY_LIST_SUPPORT
    if (id == UTE_MOUDLE_SCREENS_QUICK_REPLY_LIST_ID)
    {
        result = true;
    }
#endif
#if UTE_MODULE_PLAYBACK_SUPPORT
    if (id == UTE_MOUDLE_SCREENS_MIC_RECORDING_ID || id == UTE_MOUDLE_SCREENS_MIC_RECORD_PLAYING_ID || id == UTE_MOUDLE_SCREENS_MIC_RECORD_LIST_ID)
    {
        result = true;
    }
#endif
#if UTE_MODULE_GAME_FLYBIRD_SUPPORT
    if (id == UTE_MOUDLE_SCREENS_GAME_FLYBIRD_ID)
    {
        result = true;
    }
#endif
#if UTE_MODULE_GAME_NUMBER_KLOTSKI_SUPPORT
    if (id == UTE_MOUDLE_SCREENS_GAME_NUMBER_KLOTSKI_ID)
    {
        result = true;
    }
#endif
#if UTE_MODULE_ALI_UPAY_SUPPORT || UTE_MODULE_ALI_UPAY_V02_SUPPORT
    if (id == UTE_MOUDLE_SCREENS_ALI_UPAY_BINDING_ID)
    {
        result = true;
    }
    if (id == UTE_MOUDLE_SCREENS_ALI_UPAY_STARTUP_PAGE_ID)
    {
        result = true;
    }
    if (id == UTE_MOUDLE_SCREENS_ALI_UPAY_PAYMENT_ID)
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
    if ((id == FUNC_FINDPHONE) && (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING))
    {
        result = true;
    }
#endif
#if UTE_MODULE_NOTIFY_CHARGING_NOT_REMIND_SUPPORT
    if (uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE)
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    if (id == FUNC_BREATHE_RUN)
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_BIRD_SUPPORT
    if (id == FUNC_BIRD)
    {
        result = true;
    }
#endif
#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
    if (id == FUNC_TETRIS)
    {
        result = true;
    }
#endif
    if (!uteModuleGuiCommonData.isDisplayOn)
    {
        result = false;
    }
    return result;
}
#endif // end UTE_MODULE_GUI_TESTING_NOT_GOTO_NOTIFICATION_SCREEN_SUPPORT

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
#if 0//UTE_MODULE_SCREENS_SCREEN_LIFT_WRIST_HANDLE_SUPPORT
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !isPowerSavingModeOpen;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(quick.isTurnTheWrist,uteModuleSportGetStepsTargetCnt());
    if ((uteModuleLiftWristGetOldScheduledOpenStatus() || uteModuleLiftWristGetOldOpenStatus()) && (!uteModuleNotDisturbGetOpenStatus() && !uteModuleNotDisturbIsTimeBucket()))
    {
        uteApplicationCommonSetQuickSwitchStatus(&quick);
        uteApplicationCommonSendQuickSwitchStatus();

        if (isPowerSavingModeOpen)
        {
            uteModuleLiftWristRecordOpenStatus();
        }

        if (quick.isTurnTheWrist)
        {
            if(uteModuleLiftWristGetOldScheduledOpenStatus())
            {
                uteModuleLiftWristSetScheduled(true);
            }
            else
            {
                uteModuleLiftWristSetOpenStatus(true);
            }
        }
        else
        {
            uteModuleLiftWristSetOpenStatus(false);
            uteModuleLiftWristSetScheduled(false);
            uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_CLOSE);
        }
    }
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

#if UTE_MODULE_COVER_DISPLAY_OFF_SUPPORT
/**
*@brief  设置覆盖息屏开关
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
void uteModuleGuiCommonSetCoverDisplayOffSwitch(bool isCoverDisplayOffOpen)
{
    if (isCoverDisplayOffOpen != uteModuleGuiCommonData.isCoverDisplayOffOpen)
    {
        uteModuleGuiCommonData.isCoverDisplayOffOpen = isCoverDisplayOffOpen;
        uteModuleGuiCommonSaveConfig();
    }
}
/**
*@brief  获取覆盖息屏开关
*@details
*@author      dengli.lu
*@date        2022-06-17
*/
bool uteModuleGuiCommonGetCoverDisplayOffSwitch(void)
{
    return uteModuleGuiCommonData.isCoverDisplayOffOpen;
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
        if(id == FUNC_BT_RING)
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
#if UTE_MODULE_SCREENS_STOP_WATCH_SUPPORT
        if(id == UTE_MOUDLE_SCREENS_STOP_WATCH_ID)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
        ute_ble_connect_state_t connectStatus;
        uteApplicationCommonGetBleConnectionState(&connectStatus);
        if ((id == FUNC_FINDPHONE) &&
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT
            (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING) &&
#endif
            (connectStatus.isConnected))
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
        if(id == FUNC_CAMERA)
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
        if (uteModuleFactoryTestGetCurrTestItem() != TEST_ITEM_NONE)
        {
            return false;
        }
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
        if (uteModuleNewFactoryTestGetMode() != FACTORY_TEST_MODE_NULL)
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
#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT
        if(id == FUNC_MEASURE && uteModuleOneClickMeasureIsTesting())
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
        if (id == FUNC_BREATHE_RUN)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_BIRD_SUPPORT
        if (id == FUNC_BIRD)
        {
            return false;
        }
#endif
#if UTE_MODULE_SCREENS_TETRIS_SUPPORT
        if (id == FUNC_TETRIS)
        {
            return false;
        }
#endif
    }
    return true;
}

#if UTE_DRV_TOUCH_MULTI_CLICK_SPECIFY_SCREEN_SUPPORT
/**
 * @brief        是否允许TP多击事件
 * @details      只在特定界面使用双击，以免影响点击速度
 * @return       true:允许 false:不允许
 * @author       Wang.Luo
 * @date         2025-08-25
 */
AT(.com_text.ctp)
bool uteModuleGuiCommonIsAllowTouchMultiClickScreen(void)
{
    bool allowTouchMultiClick = false;
    uint16_t id = uteModuleGuiCommonGetCurrentScreenId();
    if(id == UTE_MOUDLE_SCREENS_WATCHMAIN_ID)
    {
        allowTouchMultiClick = true;
    }
    return allowTouchMultiClick;
}
#endif
