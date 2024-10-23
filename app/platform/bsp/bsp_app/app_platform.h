#ifndef _APP_PLATFROM_H_
#define _APP_PLATFROM_H_

void app_platform_init(void);                                         //app平台相关初始化
void app_platform_process(void);                                      //app平台进程

enum {
    //这个顺序更改,需要同步到func_cover.h那边
    COVER_NULL,
    COVER_ALARM,
    //这一段HEALTH按顺序,需要与analysis那边一致
    COVER_HEALTH_SEDENTARY,
    COVER_HEALTH_DRINK,

    COVER_FIND_WATCH,
    COVER_GOAL,
    GCOVER_BT_CONNECT,
    GCOVER_APP_CONNECT,
};


#endif  //_APP_PLATFROM_H_
