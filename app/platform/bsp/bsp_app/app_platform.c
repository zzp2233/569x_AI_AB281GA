#include "include.h"

#if (USE_APP_TYPE == USE_AB_APP)
void app_ab_data_storage_process(void);
void app_ab_data_storage_init(void);
#endif

void app_platform_process(void)
{
#if (USE_APP_TYPE == USE_AB_APP)
    app_ab_data_storage_process();
#endif
}

void app_platform_init(void)
{
#if (USE_APP_TYPE == USE_AB_APP)
    app_ab_data_storage_init();
#endif
}
