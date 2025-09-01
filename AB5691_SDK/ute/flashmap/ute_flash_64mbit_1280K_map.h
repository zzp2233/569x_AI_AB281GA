
#ifndef _UTE_FLASH_64MBIT_1024K_MAP_H_
#define _UTE_FLASH_64MBIT_1024K_MAP_H_

#define UTE_FLASH_SIZE                       0x01000000

#define UTE_BOOT1_ADDRESS                    0x00000000
#define UTE_BOOT1_SIZE                       0x00001000
#define UTE_BOOT2_ADDRESS                    0x00001000
#define UTE_BOOT2_SIZE                       0x00001000
#define UTE_CODE_ADDRESS                     0x00002000
#define UTE_CODE_SIZE                        0x001F0000
#define UTE_UI_ADDRESS                       0x001F2000
#define UTE_UI_SIZE                          0x00BA0000
#define UTE_OTA_TMP_ADDRESS                  0x00D92000
#define UTE_OTA_TMP_SIZE                     0x00146000
#define UTE_MODULE_WATCHONLINE_START_ADDRESS 0x00ED8000
#define UTE_MODULE_WATCHONLINE_MAX_SIZE      0x00090000
#define UTE_MODULE_FILESYSTEM_START_ADDRESS  0x00F68000
#define UTE_MODULE_FILESYSTEM_MAX_SIZE       0x0008F000
#define UTE_USER_PARAM_ADDRESS               0x00FF7000
#define UTE_USER_PARAM_SIZE                  0x00002000
#define UTE_FACTORY_PARAM_ADDRESS            0x00FF9000
#define UTE_FACTORY_PARAM_SIZE               0x00007000

#define UTE_SYSTEM_USER_DATA_START_ADDRESS UTE_MODULE_WATCHONLINE_START_ADDRESS
#endif /* _UTE_FLASH_64MBIT_1024K_MAP_H_ */
