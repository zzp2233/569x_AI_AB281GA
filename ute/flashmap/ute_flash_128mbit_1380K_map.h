
#ifndef _UTE_FLASH_128MBIT_1380K_MAP_H_
#define _UTE_FLASH_128MBIT_1380K_MAP_H_

#define UTE_FLASH_SIZE                                  0x01000000  //16MB

#define UTE_BOOT1_ADDRESS                               0x00000000
#define UTE_BOOT1_SIZE                                  0x00001000  //4K Bytes
#define UTE_BOOT2_ADDRESS                               0x00001000
#define UTE_BOOT2_SIZE                                  0x00001000  //4K Bytes
#define UTE_CODE_ADDRESS                                0x00002000
#define UTE_CODE_SIZE                                   0x00159000  //1380K Bytes
#define UTE_UI_ADDRESS                                  0x0015B000
#define UTE_UI_SIZE                                     0x00B3E000  //11512K Bytes
#define UTE_OTA_TMP_ADDRESS                             0x00C99000
#define UTE_OTA_TMP_SIZE                                0x000E1000  //897K Bytes  UTE_CODE_SIZE*0.65
#define UTE_MODULE_WATCHONLINE_START_ADDRESS            0x00D7A000
#define UTE_MODULE_WATCHONLINE_MAX_SIZE                 0x00180000  //1536K Bytes
#define UTE_MODULE_FILESYSTEM_START_ADDRESS             0x00EFA000
#define UTE_MODULE_FILESYSTEM_MAX_SIZE                  0x00100000  //1024K Bytes
#define UTE_USER_PARAM_ADDRESS                          0x00FFA000
#define UTE_USER_PARAM_SIZE                             0x00001000  //4K Bytes
#define UTE_FACTORY_PARAM_ADDRESS                       0x00FFB000
#define UTE_FACTORY_PARAM_SIZE                          0x00005000  //20K Bytes

#define UTE_SYSTEM_USER_DATA_START_ADDRESS UTE_MODULE_WATCHONLINE_START_ADDRESS
#endif /* _UTE_FLASH_128MBIT_1024K_MAP_H_ */
