#ifndef _PORT_TFT_H
#define _PORT_TFT_H

#define PORT_TFT_INT            IO_PA6
#define PORT_TFT_INT_VECTOR     PORT_INT1_VECTOR

//RST
#define PORT_TFT_RST            IO_PA7
#define PORT_TFT_RST_H()        GPIOASET = BIT(7);//GPIOBSET = BIT(7);
#define PORT_TFT_RST_L()        GPIOACLR = BIT(7);//GPIOBCLR = BIT(7);

//CS
#define TFT_SPI_CS_EN()         GPIOACLR = BIT(5);
#define TFT_SPI_CS_DIS()        GPIOASET = BIT(5);

#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
#define PORT_TFT_BL             GPIO_PF1
#define LCD_BL_EN()             {GPIOFDIR &= ~BIT(1); GPIOFDE  |= BIT(1); GPIOFSET  = BIT(1);}
#define LCD_BL_DIS()            {GPIOFDIR |= BIT(1); GPIOFDE  &= ~BIT(1);}
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
#define PORT_TFT_BL             GPIO_PE1
#define LCD_BL_EN()             {GPIOEDIR &= ~BIT(1); GPIOEDE  |= BIT(1); GPIOESET  = BIT(1);}
#define LCD_BL_DIS()            {GPIOEDIR |= BIT(1); GPIOEDE  &= ~BIT(1);}
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690F)
#define PORT_TFT_BL             GPIO_PF6
#define LCD_BL_EN()             {GPIOFDIR &= ~BIT(6); GPIOFDE  |= BIT(6); GPIOFSET  = BIT(6);}
#define LCD_BL_DIS()            {GPIOFDIR |= BIT(6); GPIOFDE  &= ~BIT(6);}
#else
#error CHIP_PACKAGE_SELECT error!!!
#endif

//RS
#define TFT_SPI_DATA_EN()       GPIOASET = BIT(3);
#define TFT_SPI_CMD_EN()        GPIOACLR = BIT(3);

#if (GUI_MODE_SELECT == MODE_4WIRE_8BIT)
#define   DC_ENABLE()           {GPIOAFEN &= ~BIT(3); GPIOADE |=  BIT(3);GPIOASET = BIT(3); GPIOADIR &= ~BIT(3);} // DC脚 设置输出
#define   DC_CMD_EN()           TFT_SPI_CMD_EN();           // DC 拉低
#define   DC_DATA_EN()          TFT_SPI_DATA_EN();          // DC 拉高
#elif (GUI_MODE_SELECT == MODE_3WIRE_9BIT_2LINE)
#define   DC_ENABLE()           {GPIOAFEN |= BIT(3); GPIOADE |=  BIT(3); GPIOADIR |= BIT(3);} //DC脚 设置输入
#define   DC_CMD_EN()           {DESPICON &= ~BIT(19);}     // DC 拉低
#define   DC_DATA_EN()          {DESPICON |= BIT(19);}      // DC 拉高
#endif

void port_tft_init(void);
void port_tft_exit(void);
void port_tft_reset(void);

#endif
