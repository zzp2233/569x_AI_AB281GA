#include "spt5113c.h"
#include "bsp_i2c.h"
#include "ute_module_newFactoryTest.h"

#if (CTP_SELECT == CTP_SPT5113C)

#if UTE_DRV_TFT_S320_X380_ST77916_TRULY_191_QSPI_SUPPORT
#include "spt5113c_firmware_YCY_W16Y_5613B01_A1_01.h"
#elif UTE_DRV_TFT_S240X296_JD9853_HKOE19124B01_QSPI_SUPPORT
// #include "spt5113c_firmware_YCY_5758B01_A0_01.h"
#include "spt5113c_firmware_YCY_5758B01_03.h"
#elif UTE_DRV_TFT_S360X360_GC9B71_HY139074A_QSPI_SUPPORT
#include "spt5113c_firmware_YCY_JY139012AV0_01.h"
#elif UTE_DRV_AMOLED_S368X448_CH13620_193WY01012_QSPI_SUPPORT
#include "spt5113c_firmware_YCY_AZ192012A1_01.h"
#elif (UTE_DRV_TFT_S240X284_I183_JD9853_0185A035_QSPI_SUPPORT || UTE_DRV_TFT_S240X284_I183_JD9853_SXT180G1911_QSPI_SUPPORT || UTE_DRV_TFT_S240X284_C_JD9853_WV018LZQ_QSPI_SUPPORT)
#include "spt5113c_firmware_YCY_SXT_180G1911V1_01.h"
#elif UTE_DRV_TFT_S360X360_I172_ST77916_QSPI_SUPPORT
#include "spt5113c_firmware_YCY_5798B01_A0_PTX2506_01.h"
#elif DRV_BOE_S240X296_I183_JD9853_HKOE1912534B01_QSPI_SUPPORT
// #include "spt5113c_firmware_YCY_5826B01_A0_01.h"
#include "spt5113c_firmware_YCY_5826B01_02.h"
#else
#include "spt5113c_firmware_YCY_5758B01_A0_01.h"
#endif

static bool spt5113c_firmware_update(void);
static bool spt5113c_is_init = false;
typedef enum
{
    EXEC_OK = 0,
    CHIP_CODE_ERR,
    UPGRADE_HOLD_ERR,
    WRITE_CODE_ERR,
    CRC_CHECK_ERR,

} FIRMWARE_UPDATE_STATUS;

#define MAX_CTP_UPDATE_LEN          (0x2800)
static uint8_t ctp_fw_temp_buffer[MAX_CTP_UPDATE_LEN] AT(.ctp_ota);

#define UPDATE_INFO_EN              1
#if (UPDATE_INFO_EN)
#define UPDATE_INFO(fmt, ...)       printf(fmt, ##__VA_ARGS__)
#else
#define UPDATE_INFO(fmt, ...)
#endif

#define ASU_DEV_ADDR                0x60
#define APP_DEV_ADDR                0x13
#define CHIP_CODE                   0x53

#define I2C_WRITE_ADDR(ADDR)        ((ADDR) << 1)               //CTP IIC写地址
#define I2C_READ_ADDR(ADDR)         ((ADDR) << 1 | 1)           //CTP IIC读地址

#if (CHIP_PACKAGE_SELECT == CHIP_5691G || CHIP_PACKAGE_SELECT == CHIP_5691C_F)
static i2c_t __SPT5113C_IIC1 =
{
    .sfr = (i2c_sfr_t *)&IIC1CON0,
    .map = (i2c_map_t *)&FUNCMCON2,
};
i2c_t *SPT5113C_IICx = &__SPT5113C_IIC1;
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690F)
static i2c_t __SPT5113C_IIC0 =
{
    .sfr = (i2c_sfr_t *)&IIC0CON0,
    .map = (i2c_map_t *)&FUNCMCON2,
};
i2c_t *SPT5113C_IICx = &__SPT5113C_IIC0;
#else
#error CHIP_PACKAGE_SELECT select error!!!
#endif

//触摸坐标获取
static uint8_t spt5113c_buff[7];

static void spt5113c_i2c_read_16bit(uint8_t dev_addr, uint16_t reg_addr, uint8_t *read_data, uint16_t read_len)
{
    u32 ticks;

    SPT5113C_IICx->sfr->IICxCMDA = (I2C_READ_ADDR(dev_addr) << 24) | (reg_addr << 8) | I2C_WRITE_ADDR(dev_addr);       //0x2A
    SPT5113C_IICx->sfr->IICxCON1 = BIT(12) | BIT(11) | BIT(9) | BIT(8) | BIT(7) | BIT(6) | BIT(5) | BIT(4) | BIT(3) | read_len;
    SPT5113C_IICx->sfr->IICxDMAADR = DMA_ADR(read_data);
    SPT5113C_IICx->sfr->IICxDMACNT = ((read_len - 1) << 16) | BIT(0);
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(28);                //KICK

    ticks = tick_get();
    while (!(SPT5113C_IICx->sfr->IICxCON0 & BIT(31)))
    {
        if (tick_check_expire(ticks, 200))
        {
            UPDATE_INFO("spt5113c_i2c_read 16bit time out ERROR\n");
            return;
        }
    }
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(29);                //Clear Pending
}

static void spt5113c_i2c_write_16bit(uint8_t dev_addr, uint16_t reg_addr, uint8_t *write_data, uint16_t write_len)
{
    u32 ticks;

    SPT5113C_IICx->sfr->IICxCMDA = (reg_addr << 8) | I2C_WRITE_ADDR(dev_addr);       //0x2A
    SPT5113C_IICx->sfr->IICxCON1 = BIT(12) | BIT(11) | BIT(10) | BIT(6) | BIT(5) | BIT(4) | BIT(3);
    SPT5113C_IICx->sfr->IICxDMAADR = DMA_ADR(write_data);
    SPT5113C_IICx->sfr->IICxDMACNT = ((write_len - 1) << 16) | BIT(1) |BIT(0);
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(28);                //KICK

    ticks = tick_get();
    while (!(SPT5113C_IICx->sfr->IICxCON0 & BIT(31)))
    {
        if (tick_check_expire(ticks, 2000))
        {
            UPDATE_INFO("spt5113c_i2c_write 16bit time out ERROR\n");
            return;
        }
    }
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(29);                //Clear Pending
}

AT(.com_text.ctp)
static void spt5113c_i2c_read_8bit_kick(uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, int read_len)
{
    SPT5113C_IICx->sfr->IICxCMDA = (I2C_READ_ADDR(dev_addr) << 24) | (reg_addr << 8) | I2C_WRITE_ADDR(dev_addr);       //0x2A
    SPT5113C_IICx->sfr->IICxCON1 = BIT(12) | BIT(11) | BIT(9) | BIT(8) | BIT(7) | BIT(5) | BIT(4) | BIT(3) | read_len;
    SPT5113C_IICx->sfr->IICxDMAADR = DMA_ADR(read_data);
    SPT5113C_IICx->sfr->IICxDMACNT = ((read_len - 1) << 16) | BIT(0);
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(28);                //KICK
}

AT(.com_text.ctp)
static void spt5113c_i2c_write_8bit(uint8_t dev_addr, uint8_t reg_addr, uint8_t *write_data, int write_len)
{
    u32 ticks;

    SPT5113C_IICx->sfr->IICxCMDA = (reg_addr << 8) | I2C_WRITE_ADDR(dev_addr);       //0x2A
    SPT5113C_IICx->sfr->IICxCON1 = BIT(12) | BIT(11) | BIT(10) | BIT(5) | BIT(4) | BIT(3);
    SPT5113C_IICx->sfr->IICxDMAADR = DMA_ADR(write_data);
    SPT5113C_IICx->sfr->IICxDMACNT = ((write_len - 1) << 16) | BIT(1) | BIT(0);
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(28);                //KICK

    ticks = tick_get();
    while (!(SPT5113C_IICx->sfr->IICxCON0 & BIT(31)))
    {
        if (tick_check_expire(ticks, 1))
        {
            //UPDATE_INFO("spt5113c_i2c_write 8bit time out ERROR\n");
            return;
        }
    }
    SPT5113C_IICx->sfr->IICxCON0 |= BIT(29);                //Clear Pending
}



/******************************** SPT5113C 驱动 ****************************************/

/*
 * 函数名：spt5113c_init
 * 描述  ：SPT5113C触摸初始化
 * 输入  ：void
 * 输出  ：bool
 */
bool spt5113c_init(void)
{
    return spt5113c_firmware_update();
}

/*
 * 函数名：spt5113c_reset
 * 描述  ：SPT5113C 触摸硬件复位
 * 输入  ：void
 * 输出  ：void
 */
static void spt5113c_reset(void)
{
    PORT_CTP_RST_H();
    delay_ms(10);
    PORT_CTP_RST_L();
    delay_ms(12);
    PORT_CTP_RST_H();
    delay_ms(12);
}

static FIRMWARE_UPDATE_STATUS spt5113c_asu_connect(void)
{
    uint8_t reg_temp;

    //CHIP CODE verify
    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC000, &reg_temp, 1);
    if(reg_temp != CHIP_CODE)
    {
        return CHIP_CODE_ERR;
    }

    //Enter upgrade
    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
    reg_temp &= 0xf0;// low 4 bits clear hold upgrade
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);//re-write

    return EXEC_OK;
}

static void spt5113c_write_code_length(void)
{
    uint8_t length_lo = (FIRMWARE_SIZE - 1) & 0xFF;
    uint8_t length_hi = ((FIRMWARE_SIZE - 1) >> 8) & 0xFF;

    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC130, &length_hi, 1);
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC131, &length_lo, 1);
}

static void spt5113c_write_code_crc(void)
{
    uint8_t crc_lo = CRC16_CHECK_CODE & 0xFF;
    uint8_t crc_hi = (CRC16_CHECK_CODE >> 8) & 0xFF;

    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC132, &crc_hi, 1);
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC133, &crc_lo, 1);
}

static FIRMWARE_UPDATE_STATUS spt5113c_write_code(uint16_t write_addr, uint8_t *write_buffer, uint16_t write_length)
{
    if((write_buffer == NULL) || (write_length == 0) || ((write_addr + write_length) > 0x2800))
    {
        return WRITE_CODE_ERR;
    }

    memcpy(ctp_fw_temp_buffer, write_buffer, write_length);
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, write_addr, ctp_fw_temp_buffer, write_length);

    return EXEC_OK;
}

static FIRMWARE_UPDATE_STATUS spt5113c_crc_check(void)
{
    uint8_t inc, byte_lo, byte_hi;
    uint8_t crc_check_cnt = 0;
    uint16_t crc_code;

    spt5113c_write_code_length();
    spt5113c_write_code_crc();

    inc = 0x01; //soft start crc check
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC136, &inc, 1); //send inc
    //delay_ms(10); //wait for check complete

    while(crc_check_cnt < 100)
    {
        inc = 0;
        spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC136, &inc, 1);
        if((inc & 0x01) == 0)
        {
            //CRC计算完成
            break;
        }
        crc_check_cnt++;
        delay_us(100);
    }

    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC134, &byte_hi, 1); //read crc hi byte
    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC135, &byte_lo, 1); //read crc lo byte

    crc_code = ((uint16_t)byte_hi << 8) + byte_lo;
    uteModuleNewFactoryTestSetTpVersion(crc_code);
    if(crc_code == CRC16_CHECK_CODE)
    {
        return EXEC_OK;
    }
    else
    {
        return CRC_CHECK_ERR;
    }
}

//启动后扫频跳频
void spt5113c_start_work_with_scan(void)
{
    uint8_t reg_temp;
    uint8_t hopping_ctrl[3] = {0x01, 0x00, 0x00};

    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0x2FB0, hopping_ctrl, 3);

    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
    reg_temp &= 0xDF;
    reg_temp |= 0x03;
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
}

//启动后按主控给的频率运行
void spt5113c_start_work_with_freqset(uint16_t freqset)
{
    uint8_t reg_temp;

    reg_temp = 0;
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0x2FB0, &reg_temp, 1);
    reg_temp = (uint8_t)(freqset & 0xff);
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0x2FB1, &reg_temp, 1);
    reg_temp = (uint8_t)((freqset >> 8) & 0xff);
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0x2FB2, &reg_temp, 1);

    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
    reg_temp &= 0xDF;
    reg_temp |= 0x03;
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
}

//启动扫频后状态查询
bool spt5113c_frequency_check_cmd_finish_get(void)
{
    uint8_t check_cmd_finish = 0x01;

    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0x2FB0, &check_cmd_finish, 1);

    return (check_cmd_finish == 0x00 ? true : false);
}

//扫频后频点获取
uint16_t spt5113c_frequency_get(void)
{
    uint16_t tp_freq = 0;

    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0x2FB1, &tp_freq, 2);

    return tp_freq;
}

static void spt5113c_start_work(void)
{
    uint8_t reg_temp;
    uint8_t hopping_ctrl[3] = {0};

    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0x2FB0, hopping_ctrl, 3);
    spt5113c_i2c_read_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
    reg_temp |= 0x03;
    spt5113c_i2c_write_16bit(ASU_DEV_ADDR, 0xC103, &reg_temp, 1);
}

/*
 * 函数名：spt5113c_firmware_update
 * 描述  ：SPT5113C 固件升级
 * 输入  ：void
 * 输出  ：void
 */
static bool spt5113c_firmware_update(void)
{
    FIRMWARE_UPDATE_STATUS status = EXEC_OK;

    UPDATE_INFO("\r\n#######################plxc spt5113c firmware update#######################\r\n");

    //reset
    spt5113c_reset();
    UPDATE_INFO("# spt5113c reset.\r\n");

    spt5113c_is_init = true;
    //connect
    status = spt5113c_asu_connect();
    if(status == EXEC_OK)
    {
        UPDATE_INFO("# spt5113c connect success!\r\n");
    }
    else
    {
        UPDATE_INFO("# spt5113c connect fail. fail code:%d\r\n", status);
        goto exit;
    }

    //check crc code to determine updating
    status = spt5113c_crc_check();//run code 11ms.
    if(status == EXEC_OK)
    {
        UPDATE_INFO("## spt5113c firmware is already latest!\r\n");

        spt5113c_start_work(); //exit must reboot
        return true;
    }
    else
    {
        UPDATE_INFO("## spt5113c firmware version verify different, updating\r\n");
    }
    //write code to rom
    status = spt5113c_write_code(0, (uint8_t *)spt5113c_firmware, FIRMWARE_SIZE);//run code 79ms.

    //check code
    status = spt5113c_crc_check();
    if(status == EXEC_OK)
    {
        UPDATE_INFO("### spt5113c update success!\r\n");
    }
    else
    {
        UPDATE_INFO("### spt5113c update fail!\r\n");
        goto exit;
    }

    spt5113c_start_work(); //exit must reboot
    return true;

exit:
    spt5113c_start_work(); //exit must reboot
    return false;
}

/*
 * 函数名：spt5113c_enter_sleep
 * 描述  ：SPT5113C 进入休眠模式
 * 输入  ：void
 * 输出  ：void
 */
AT(.com_text.ctp)
void spt5113c_enter_sleep(void)
{
    uint8_t temp_data;

    temp_data = 0xFF;
    spt5113c_i2c_write_8bit(APP_DEV_ADDR, 0xFF, &temp_data, 1);

    temp_data = 0x03;
    spt5113c_i2c_write_8bit(APP_DEV_ADDR, 0xE5, &temp_data, 1);

//    temp_data = 0x01;
//    spt5113c_i2c_write_8bit(APP_DEV_ADDR, 0xFE, &temp_data, 1);
}

/*
 * 函数名：spt5113c_readkick
 * 描述  ：SPT5113C 中断内读触摸数据
 * 输入  ：void
 * 输出  ：void
 */
AT(.com_text.ctp)
void spt5113c_readkick(void)
{
    spt5113c_i2c_read_8bit_kick(APP_DEV_ADDR, 0x00, spt5113c_buff, sizeof(spt5113c_buff));
}

AT(.com_text.ctp)
bool spt5113c_get_point(int32_t *x, int32_t *y)
{
    uint8_t temp_data = 0x01;
    bool press = false;

#if CTP_SUPPORT_COVER
    if (spt5113c_buff[0] == 0x80)
    {
        memset(spt5113c_buff, 0, sizeof(spt5113c_buff));

        *x = -1;
        *y = -1;

        return false;
    }
#endif

    *x = ((spt5113c_buff[3] & 0x0F) << 8) + spt5113c_buff[4];
    *y = ((spt5113c_buff[5] & 0x0F) << 8) + spt5113c_buff[6];

    press = ((spt5113c_buff[3] & 0xC0) == 0x80) ? true : false;

    return press;
}


/*
 * 函数名：spt5113c_wake_up
 * 描述  ：SPT5113C 休眠唤醒
 * 输入  ：void
 * 输出  ：void
 */
AT(.com_text.ctp)
void spt5113c_wake_up(void)
{
    spt5113c_firmware_update();
}

#endif
