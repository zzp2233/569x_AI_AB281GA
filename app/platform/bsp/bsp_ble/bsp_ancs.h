#ifndef __BSP_ANCS_H
#define __BSP_ANCS_H

// event_id 定义
#define ANCS_EVENT_ID_ADDED     0x00    // 新通知到来
#define ANCS_EVENT_ID_MODIFIED  0x01    // 通知被修改
#define ANCS_EVENT_ID_REMOVED   0x02    // 通知被移除（接听/挂断）

// event_flags 标志位定义
#define ANCS_EVENT_FLAG_SILENT              (1 << 0)  // 静音通知
#define ANCS_EVENT_FLAG_IMPORTANT           (1 << 1)  // 重要通知
#define ANCS_EVENT_FLAG_PRE_EXISTING        (1 << 2)  // 已存在的历史通知
#define ANCS_EVENT_FLAG_POSITIVE_ACTION     (1 << 3)  // 正面操作（如接听）
#define ANCS_EVENT_FLAG_NEGATIVE_ACTION     (1 << 4)  // 负面操作（如拒接）

// category_id 类别定义（补充完整）
#define ANCS_CATEGORY_ID_INCOMING_CALL      0x01    // 来电
#define ANCS_CATEGORY_ID_MISSED_CALL        0x02    // 未接来电
#define ANCS_CATEGORY_ID_VOICE_MAIL         0x03    // 语音邮件
#define ANCS_CATEGORY_ID_SOCIAL             0x04    // 社交类（微信/QQ等）
#define ANCS_CATEGORY_ID_SCHEDULE           0x05    // 日程提醒
#define ANCS_CATEGORY_ID_EMAIL              0x06    // 邮件
#define ANCS_CATEGORY_ID_NEWS               0x07    // 新闻
#define ANCS_CATEGORY_ID_PHONEBOOK          0x08    // 通讯录同步（少见）
#define ANCS_CATEGORY_ID_POMA_REMINDER      0x09    // Pomodoro 提醒（番茄钟）

// 下面是非官方但广泛验证有效的类别
#define ANCS_CATEGORY_ID_HIGH_PRIORITY      0x0F    // 高优先级通知（如闹钟）
#define ANCS_CATEGORY_ID_PHYSICAL_ACTIVITY  0x10    // 运动健身类通知
#define ANCS_CATEGORY_ID_SOS                0x11    // 紧急联系人通知
#define ANCS_CATEGORY_ID_STATUS_UPDATE      0x0C    // “通话中”状态更新（iOS 内部使用）

#endif //__BSP_ANCS_H
