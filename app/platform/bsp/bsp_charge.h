#ifndef _BSP_CHARGE_H
#define _BSP_CHARGE_H


typedef enum
{
    CHARGE_NO_FULL = 0,         //���û��
    CHARGE_FULL,                //�������
    CHARGE_REFILL               //�������
} charge_sta_m;


void bsp_charge_init(void);
void bsp_charge_off(void);
u8 charge_dc_detect(void);
void charge_detect(u8 mode);
void bsp_charge_sta(u8 sta);
void bsp_charge_set_stop_time(u16 stop_time);
void bsp_charge_bled_ctrl(void);
void charge_set_detect_cnt(u8 value);


/**
 *   ���״̬����
 *   ���س��״̬
 *   0 : 5Vû����
     1 : �����
     2 ������
 */
u8 bsp_charge_sta_get(void);



#endif
