/**
****************************************************************************************
*
* @file drv_magnetic_mmc5603.c
*
*   mmc5603驱动代码
*
*@author        pcm
*@date        2024-11-25
*
****************************************************************************************
*/
#include "ute_project_config.h"
#include "ute_application_common.h"
#include "ute_drv_magnetic_mmc5603.h"
#include "math.h"
#include "ute_module_log.h"

#if UTE_DRV_SENSOR_MAG_MMC5603NJ_SUPPORT

#define MMC5603NJ_WRITE_ADDR(ADDR)         ((ADDR) << 1)
#define MMC5603NJ_READ_ADDR_UPDATE(ADDR)   ((ADDR) << 1 | 1)
#define MMC5603NJ_READ_ADDR(ADDR)          ((ADDR) << 1 | 1) << 8 | ((ADDR) << 1)

bool drvMagnetciMmc5603Ifmode = false;
/**
*@brief           Mmc5603 写寄存器
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t data  寄存器值
*@return  返回true为成功
*@author        pcm
*@date        2024-11-25
*/
bool drvMagneticMmc5603WriteReg(uint8_t reg,uint8_t data)
{
    if(sys_cb.gsensor_iic_en)
    {
        bsp_i2c_init();
    }

    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA;
    bsp_hw_i2c_tx_buf(i2c_cfg, MMC5603NJ_WRITE_ADDR(MMC5603_ADDR), (uint16_t)reg, &data,1);


    return true;
}

/**
*@brief        Mmc5603读寄存器数据
*@details
*@param[in] uint8_t reg  寄存器
*@param[in] uint8_t *data  寄存器值
*@param[in] uint8_t size 读取长度
*@return  返回true为成功
*@author        pcm
*@date        2024-11-25
*/
bool drvMagneticMmc5603ReadReg(uint8_t reg,uint8_t size,uint8_t *data)
{
    if(sys_cb.gsensor_iic_en)
    {
        bsp_i2c_init();
    }
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
    bsp_hw_i2c_rx_buf(i2c_cfg, MMC5603NJ_READ_ADDR(MMC5603_ADDR), reg, data,size);

    return true;
}
/* Indicate working mode of sensor */
static uint8_t sensor_state = 1;

/* Function declaration */

void drvMagnetciMmc5603DelayMs(uint32_t ms)
{
    uteModulePlatformDelayMs(ms);
}
/*********************************************************************************
* decription: SET operation
*********************************************************************************/
void MMC5603_SET(void)
{
    /* Write 0x08 to register 0x1B, set SET bit high */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_SET);

    /* Delay to finish the SET operation */
    drvMagnetciMmc5603DelayMs(1);

    return;
}

/*********************************************************************************
* decription: RESET operation
*********************************************************************************/
void MMC5603_RESET(void)
{
    /* Write 0x10 to register 0x1B, set RESET bit high */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_RESET);

    /* Delay to finish the RESET operation */
    drvMagnetciMmc5603DelayMs(1);

    return;
}

/**
*@brief     工厂测试模式
*@details  decription: Factory test mode
*@param[in]
*@param[in]
*@param[in]
*@return  返回true为成功
*@author        pcm
*@date        2024-11-25
*/
int MMC5603_Factory_Test_Mode(void)
{
    int i;
    uint8_t data_reg[6] = {0};
    uint16_t data_set[3] = {0};
    uint16_t data_reset[3] = {0};
    uint32_t delta_data[3] = {0};

    const uint16_t thr_srst_low = 100;

    /* Write reg 0x1D */
    /* Set Cmm_en bit '0', Disable continuous mode */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL2, 0x00);

    drvMagnetciMmc5603DelayMs(20);

    /* Write reg 0x1B */
    /* Set Auto_SR_en bit '0', Disable the function of automatic set/reset */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, 0x00);

    /* Write reg 0x1C, Set BW<1:0> = 00 */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL1, 0x00);

    /* Do RESET operation */
    MMC5603_RESET();
    /* Write 0x01 to register 0x1B, set Take_meas_M bit '1' */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_TMM);
    /* Delay 10 ms to finish the TM operation */
    drvMagnetciMmc5603DelayMs(10);
    /* Read register data */
    drvMagneticMmc5603ReadReg( MMC5603_REG_DATA, 6, data_reg);
    /* Get high 16bits data */
    data_reset[0] = (uint16_t)(data_reg[0] << 8 | data_reg[1]); //X axis
    data_reset[1] = (uint16_t)(data_reg[2] << 8 | data_reg[3]); //Y axis
    data_reset[2] = (uint16_t)(data_reg[4] << 8 | data_reg[5]); //Z axis

    /* Do SET operation */
    MMC5603_SET();
    /* Write 0x01 to register 0x1B, set Take_meas_M bit '1' */
    drvMagneticMmc5603WriteReg(MMC5603_REG_CTRL0,MMC5603_CMD_TMM);
    /* Delay 10 ms to finish the TM operation */
    drvMagnetciMmc5603DelayMs(10);
    /* Read register data */
    drvMagneticMmc5603ReadReg( MMC5603_REG_DATA, 6, data_reg);
    /* Get high 16bits data */
    data_set[0] = (uint16_t)(data_reg[0] << 8 | data_reg[1]);   //X axis
    data_set[1] = (uint16_t)(data_reg[2] << 8 | data_reg[3]);   //Y axis
    data_set[2] = (uint16_t)(data_reg[4] << 8 | data_reg[5]);   //Z axis

    for(i=0; i<3; i++)
    {
        if(data_set[i] >= data_reset[i])
            delta_data[i] = data_set[i] - data_reset[i];
        else
            delta_data[i] = data_reset[i] - data_set[i];
    }

    /* If output < 100lsb, fail*/
    if(delta_data[0]<thr_srst_low && delta_data[1]<thr_srst_low && delta_data[2]<thr_srst_low)
        return -1;  // fail

    return 1;   //pass
}

/**
*@brief   读取Sensor的ID
*@details  Product ID check
*@param[in]
*@param[in]
*@param[in]
*@return  返回true为成功
*@author        pcm
*@date        2024-11-25
*/
int MMC5603_CheckID(void)
{
    unsigned char pro_id = 0;

    /* Read register 0x39, check product ID */
    drvMagneticMmc5603ReadReg(MMC5603_REG_PRODUCTID1,1, &pro_id);
    if((pro_id != MMC5603_PRODUCT_ID)&&(pro_id != MMC5617_PRODUCT_ID)&&(pro_id != MMC5616_PRODUCT_ID))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,init error",__func__);
        return -1;
    }

    return 1;
}

/*********************************************************************************
* decription: Auto self-test registers configuration
*********************************************************************************/
void MMC5603_Auto_SelfTest_Configuration(void)
{
    int i;
    uint8_t reg_value[3];
    int16_t st_thr_data[3]= {0};
    int16_t st_thr_new[3]= {0};

    int16_t st_thd[3]= {0};
    uint8_t st_thd_reg[3];

    /* Read trim data from reg 0x27-0x29 */
    drvMagneticMmc5603ReadReg(MMC5603_REG_ST_X_VAL,3,reg_value);
    for(i=0; i<3; i++)
    {
        st_thr_data[i] = (int16_t)(reg_value[i]-128)*32;
        if(st_thr_data[i]<0)
            st_thr_data[i] = -st_thr_data[i];
        st_thr_new[i] = st_thr_data[i]-st_thr_data[i]/5;

        st_thd[i] = st_thr_new[i]/8;
        if(st_thd[i]>255)
            st_thd_reg[i] = 0xFF;
        else
            st_thd_reg[i] = (uint8_t)st_thd[i];
    }
    /* Write threshold into the reg 0x1E-0x20 */
    drvMagneticMmc5603WriteReg( MMC5603_REG_X_THD, st_thd_reg[0]);
    drvMagneticMmc5603WriteReg( MMC5603_REG_Y_THD, st_thd_reg[1]);
    drvMagneticMmc5603WriteReg( MMC5603_REG_Z_THD, st_thd_reg[2]);

    return;
}

/*********************************************************************************
* decription: Auto self-test
*********************************************************************************/
int MMC5603_Auto_SelfTest(void)
{
    uint8_t reg_status = 0;

    /* Write 0x40 to register 0x1B, set Auto_st_en bit high */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_AUTO_ST_EN);

    /* Delay 15ms to finish the selftest process */
    drvMagnetciMmc5603DelayMs(15);

    /* Read register 0x18, check Sat_sensor bit */
    drvMagneticMmc5603ReadReg( MMC5603_REG_STATUS1, 1,&reg_status);
    if((reg_status&MMC5603_SAT_SENSOR))
    {
        return -1;
    }

    return 1;
}

/*********************************************************************************
* decription: Continuous mode configuration with auto set and reset
*********************************************************************************/
void MMC5603_Continuous_Mode_With_Auto_SR(uint8_t bandwith, uint8_t sampling_rate)
{
    /* Write reg 0x1C, Set BW<1:0> = bandwith */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL1, bandwith);

    /* Write reg 0x1A, set ODR<7:0> = sampling_rate */
    drvMagneticMmc5603WriteReg( MMC5603_REG_ODR, sampling_rate);

    /* Write reg 0x1B */
    /* Set Auto_SR_en bit '1', Enable the function of automatic set/reset */
    /* Set Cmm_freq_en bit '1', Start the calculation of the measurement period according to the ODR*/
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_CMM_FREQ_EN|MMC5603_CMD_AUTO_SR_EN);

    /* Write reg 0x1D */
    /* Set Cmm_en bit '1', Enter continuous mode */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL2, MMC5603_CMD_CMM_EN);

    return;
}

/*********************************************************************************
* decription: Do selftest operation periodically
*********************************************************************************/
int MMC5603_Saturation_Checking(void)
{
    int ret = 0; //1 pass, -1 fail, 0 elapsed time is less 5 seconds

    /* If sampling rate is 50Hz, then do saturation checking every 250 loops, i.e. 5 seconds */
    static int NumOfSamples = 250;
    static int cnt = 0;

    if((cnt++)>=NumOfSamples)
    {
        cnt = 0;
        ret = MMC5603_Auto_SelfTest();
        if(ret == -1)
        {
            /* Sensor is saturated, need to do SET operation */
            MMC5603_SET();
        }

        /* Do TM_M after selftest operation */
        drvMagneticMmc5603WriteReg(MMC5603_REG_CTRL0,MMC5603_CMD_TMM);
        drvMagnetciMmc5603DelayMs(8);
    }

    return ret;
}

/*********************************************************************************
* decription: Auto switch the working mode between Auto_SR and SETonly
*********************************************************************************/
void MMC5603_Auto_Switch(uint16_t *mag)
{
    float mag_out[3];

    mag_out[0] = ((float)mag[0] - MMC5603_16BIT_OFFSET)/MMC5603_16BIT_SENSITIVITY;
    mag_out[1] = ((float)mag[1] - MMC5603_16BIT_OFFSET)/MMC5603_16BIT_SENSITIVITY;
    mag_out[2] = ((float)mag[2] - MMC5603_16BIT_OFFSET)/MMC5603_16BIT_SENSITIVITY;

    if(sensor_state == 1)
    {
        /* If X or Y axis output exceed 10 Gauss, then switch to single mode */
        if((fabs(mag_out[0])>10.0f) || (fabs(mag_out[1])>10.0f))
        {
            sensor_state = 2;

            /* Disable continuous mode */
            drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL2, 0x00);
            drvMagnetciMmc5603DelayMs(15);//Delay 15ms to finish the last sampling

            /* Do SET operation */
            drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_SET);
            drvMagnetciMmc5603DelayMs(1);//Delay 1ms to finish the SET operation

            /* Do TM_M before next data reading */
            drvMagneticMmc5603WriteReg(MMC5603_REG_CTRL0,MMC5603_CMD_TMM);
            drvMagnetciMmc5603DelayMs(8);//Delay 8ms to finish the TM_M operation
        }
    }
    else if(sensor_state == 2)
    {
        /* If both of X and Y axis output less than 8 Gauss, then switch to continuous mode with Auto_SR */
        if((fabs(mag_out[0])<8.0f) && (fabs(mag_out[1])<8.0f))
        {
            sensor_state = 1;

            /* Enable continuous mode with Auto_SR */
            drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL0, MMC5603_CMD_CMM_FREQ_EN|MMC5603_CMD_AUTO_SR_EN);
            drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL2, MMC5603_CMD_CMM_EN);
        }
        else
        {
            /* Sensor checking */
            if(MMC5603_Saturation_Checking()==0)
            {
                /* Do TM_M before next data reading */
                drvMagneticMmc5603WriteReg(MMC5603_REG_CTRL0,MMC5603_CMD_TMM);
            }
        }
    }

    return;
}

/*********************************************************************************
* decription: Disable sensor continuous mode
*********************************************************************************/
void MMC5603_Disable(void)
{
    /* Write reg 0x1D */
    /* Set Cmm_en bit '0', Disable continuous mode */
    drvMagneticMmc5603WriteReg( MMC5603_REG_CTRL2, 0x00);

    drvMagnetciMmc5603DelayMs(15);
#if 1 //DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
//    drvMagneticCommonFunction->magSleep();
    //   MMC5603_Disable();
#endif
    return;
}

/*********************************************************************************
* decription: Enable sensor
*********************************************************************************/
void MMC5603_Enable(void)
{
    int ret = 0;

    /* Inite the sensor state */
    sensor_state = 1;

    /* Check product ID */
    ret = MMC5603_CheckID();
    if(ret<0)
        return;

    /* Auto self-test registers configuration */
    MMC5603_Auto_SelfTest_Configuration();

    /* Work mode setting */
    MMC5603_Continuous_Mode_With_Auto_SR(MMC5603_CMD_BW00, 50);

    drvMagnetciMmc5603DelayMs(15);

// #if 1 //DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
//     if(drvMagnetciMmc5603Ifmode)
//     {
//         drvMagneticCommonFunction->magSetAutoMode(MMC5603_REG_DATA);
//     }
// #endif
    return;
}

/*********************************************************************************
* decription: Read the data register and convert to magnetic field
*********************************************************************************/
void MMC5603_GetData(float *mag_out,uint8_t *rawdata)
{
    uint8_t data_reg[6] = {0};
    uint16_t data_temp[3] = {0};

    /* Read register data */
    drvMagneticMmc5603ReadReg( MMC5603_REG_DATA, 6, data_reg);
    // memcpy(data_reg,rawdata,6);
    /* Get high 16bits data */
    data_temp[0] = (uint16_t)(data_reg[0] << 8 | data_reg[1]);
    data_temp[1] = (uint16_t)(data_reg[2] << 8 | data_reg[3]);
    data_temp[2] = (uint16_t)(data_reg[4] << 8 | data_reg[5]);

    // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,data_temp[0]=%d,data_temp[1]=%d,data_temp[2]=%d",__func__,data_temp[0],data_temp[1],data_temp[2]);

    /* Transform to unit Gauss */
    mag_out[0] = ((float)data_temp[0] - MMC5603_16BIT_OFFSET)/MMC5603_16BIT_SENSITIVITY;
    mag_out[1] = ((float)data_temp[1] - MMC5603_16BIT_OFFSET)/MMC5603_16BIT_SENSITIVITY;
    mag_out[2] = ((float)data_temp[2] - MMC5603_16BIT_OFFSET)/MMC5603_16BIT_SENSITIVITY;



    MMC5603_Auto_Switch(data_temp);

// #if 1 //DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
//     if(drvMagnetciMmc5603Ifmode)
//     {
//         drvMagneticCommonFunction->magSetAutoMode(MMC5603_REG_DATA);
//     }
// #endif
    return;
}
bool drvMagnetciMmc5603GetIsIfmode(void)
{
    return drvMagnetciMmc5603Ifmode;
}
bool drvMagneticMmc5603Init(void)
{
    uint8_t id = 0;
    drvMagnetciMmc5603Ifmode = false;
    drvMagneticMmc5603ReadReg(MMC5603_REG_PRODUCTID1, 1, &id);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ifmode false ID = 0x%02x", __func__, id);
    if ((id != MMC5603_PRODUCT_ID) && (id != MMC5617_PRODUCT_ID) && (id != MMC5616_PRODUCT_ID))
    {
        drvMagnetciMmc5603Ifmode = true;
#if 1 // DRV_GSENSOR_TWI_MAGNETIC_SUPPORT
        drvMagneticMmc5603Init();
#endif
        drvMagneticMmc5603ReadReg(MMC5603_REG_PRODUCTID1, 1, &id);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ifmode true ID = 0x%02x", __func__, id);
    }

    if ((id == MMC5603_PRODUCT_ID) || (id == MMC5617_PRODUCT_ID) || (id == MMC5616_PRODUCT_ID))
    {
        MMC5603_Enable();
        return true;
    }
    else
    {
        drvMagnetciMmc5603Ifmode = false;
        return false;
    }
}

const drv_magnetic_common_config_t drvMagneticMmc5603Function=
{
    .init = drvMagneticMmc5603Init,
    .deepSleep = MMC5603_Disable,
    .getData = MMC5603_GetData,
    .isUsedIfmode = drvMagnetciMmc5603GetIsIfmode,
    .sensorSet = MMC5603_SET,
};

#endif

