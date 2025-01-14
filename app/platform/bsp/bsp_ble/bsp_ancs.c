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
    // uid:9, event_id:2, event_flags:1a, category_id:1, category_count:0
    if(event_id == 2 && event_flags == 0x1a && category_id == 1) //来电挂断
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_CALL_DISABLE_NOTIFY,0);
    }
}

#endif // (LE_ANCS_CLIENT_EN && LE_EN)

