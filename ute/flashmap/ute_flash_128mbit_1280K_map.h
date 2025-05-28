
#ifndef _UTE_FLASH_128MBIT_1280K_MAP_H_
#define _UTE_FLASH_128MBIT_1280K_MAP_H_

#define UTE_FLASH_SIZE                                  0x01000000  //16MB

#define UTE_BOOT1_ADDRESS                               0x00000000
#define UTE_BOOT1_SIZE                                  0x00001000  //4K Bytes
#define UTE_BOOT2_ADDRESS                               0x00001000
#define UTE_BOOT2_SIZE                                  0x00001000  //4K Bytes
#define UTE_CODE_ADDRESS                                0x00002000
#define UTE_CODE_SIZE                                   0x00140000  //1280K Bytes
#define UTE_UI_ADDRESS                                  0x00142000
#define UTE_UI_SIZE                                     0x0073D000  //7492KB Bytes
#define UTE_OTA_TMP_ADDRESS                             0x0087F000
#define UTE_OTA_TMP_SIZE                                0x000D0000  //832K Bytes  UTE_CODE_SIZE*0.65
#define UTE_MODULE_WATCHONLINE_START_ADDRESS            0x0094F000
#define UTE_MODULE_WATCHONLINE_MAX_SIZE                 0x00100000  //1024K Bytes
#define UTE_MODULE_FILESYSTEM_START_ADDRESS             0x00A4F000
#define UTE_MODULE_FILESYSTEM_MAX_SIZE                  0x00100000  //1024K Bytes
#define UTE_USER_PARAM_ADDRESS                          0x00B4F000
#define UTE_USER_PARAM_SIZE                             0x00001000  //4K Bytes
#define UTE_FACTORY_PARAM_ADDRESS                       0x00B50000
#define UTE_FACTORY_PARAM_SIZE                          0x00005000  //20K Bytes

#define UTE_SYSTEM_USER_DATA_START_ADDRESS UTE_MODULE_WATCHONLINE_START_ADDRESS
#endif /* _UTE_FLASH_128MBIT_1024K_MAP_H_ */
