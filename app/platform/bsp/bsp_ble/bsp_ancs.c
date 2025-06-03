#include "include.h"
#include "bsp_ancs.h"
#include "ute_module_message.h"

#if (LE_ANCS_CLIENT_EN && LE_EN)

void ble_ancs_client_notifiy_callback(u8 id, const char *att_name, const char *att_content)
{
    printf("id:%d\n", id);
    printf("Notification:%s - %s\n", att_name, att_content);

    ble_app_ancs_client_notifiy_callback(id, att_name, att_content);
}

void ble_ancs_notifiy_info_callback(uint32_t uid, uint8_t event_id, uint8_t event_flags, uint8_t category_id, uint8_t category_count)
{
    ble_cb.ansc_uid = uid;
    printf("uid:%x, event_id:%x, event_flags:%x, category_id:%x, category_count:%x\n", uid, event_id, event_flags, category_id, category_count);

    ble_app_watch_set_wakeup(true);

    // 过滤掉历史通知
    if ((event_flags & ANCS_EVENT_FLAG_PRE_EXISTING) == 0)
    {
        // 处理来电事件
        if (event_id == ANCS_EVENT_ID_ADDED && category_id == ANCS_CATEGORY_ID_STATUS_UPDATE)
        {
            // “通话中”状态更新也不响应
            sys_cb.ancs_missed_call = true;
        }
        else if (event_id == ANCS_EVENT_ID_ADDED && category_id == ANCS_CATEGORY_ID_INCOMING_CALL)
        {
            // 来电响铃中
            sys_cb.ancs_missed_call = false;
        }
        else if (event_id == ANCS_EVENT_ID_ADDED && category_id == ANCS_CATEGORY_ID_MISSED_CALL)
        {
            // 未接来电
            sys_cb.ancs_missed_call = true;
        }
        else if (event_id == ANCS_EVENT_ID_REMOVED && category_id == ANCS_CATEGORY_ID_INCOMING_CALL)
        {
            // 来电结束（可能是接听或挂断）
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_CALL_DISABLE_NOTIFY, 0);
        }
    }
}

#endif // (LE_ANCS_CLIENT_EN && LE_EN)

