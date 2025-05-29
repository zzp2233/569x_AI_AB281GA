#ifndef _API_FLASH_H
#define _API_FLASH_H

uint os_spiflash_read(void *buf, u32 addr, uint len);
void os_spiflash_program(void *buf, u32 addr, uint len);
void os_spiflash_erase(u32 addr);           //addr需要4k对齐
void os_spiflash_erase_32k(u32 addr);       //addr需要32k对齐
void os_spiflash_erase_64k(u32 addr);       //addr需要64k对齐

/**
 * @brief 判断第num个OTP的block[1K]是否已经被固化
 * @param[in] num: 检测哪个块, 取值 1~3
 */
bool os_otp_is_locked(u8 num);

/**
 * @brief 固化第num个OTP的block[1K]
 * @param[in] num: 固化哪个块, 取值 1~3
 */
void os_otp_lock(u8 num);

/**
 * @brief 读取第num个OTP的block[1K]中addr地址数据到buf
 * @param[out] buf: 用于接收数据buf
 * @param[in] addr: 取值范围 0x0~0x400, 需要读取otp中addr地址
 * @param[in] len: 读取数据的长度
 * @param[in] num: 读取哪个块, 取值 1~3
 */
void os_otp_read(void *buf, u32 addr, uint len, u8 num);

/**
 * @brief 写入buf到第num个OTP的block[1K]中addr地址
 * @param[in] buf: 需要写入数据
 * @param[in] addr: 取值范围 0x0~0x400, 需要写入到otp中addr地址
 * @param[in] len: 写入数据的长度
 * @param[in] num: 写入到哪个块, 取值 1~3
 */
void os_otp_write(void *buf, u32 addr, uint len, u8 num);

//读取flash厂家型号
u32 spiflash_id_read(void);

//读取4byte flash唯一id
void spiflash_readuid(uint8_t *buf);

#endif // _API_FLASH_H
