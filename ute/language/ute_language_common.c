/**
*@file
*@brief        多国语言
*@details
*@author       zn.zeng
*@date       2021-10-23
*@version      v1.0
*/
#include "ute_language_common.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"

const char * const *i18n;

/**
 * @brief        切换语言
 * @details
 * @param[in]    langId 语言ID
 * @return       void*
 * @author       Wang.Luo
 * @date         2024-11-08
 */
void uteLanguageCommonSelect(MULTIPLE_LANGUAGE_ID langId)
{
    switch (langId)
    {
#if SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
        case CHINESE_LANGUAGE_ID:
            i18n = i18n_zh_rcn;
            break;
#endif
#if SCREEN_TITLE_MULTIPLE_ENGLISH_LANGUAGE_SUPPORT
        case ENGLISH_LANGUAGE_ID:
            i18n = i18n_en_rus;
            break;
#endif
        default :
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,Language ID is not supported",__func__);
            break;
    }
}
