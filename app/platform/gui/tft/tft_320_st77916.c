#include "include.h"
// 320 * 385

//#define Delay(ms) bsp_spide_cs(1);delay_ms(ms)
#if (GUI_SELECT == GUI_TFT_320_ST77916)


#define WriteData(v) tft_write_data(v)
#define CommEnd()    tft_write_end()
// cmd 12h 1:cmd      4:addr/data
// cmd 32h 1:cmd/addr 4:data
// cmd 02h 1:cmd      1:addr/data
#define WriteComm12(v) \
({                   \
bsp_spide_cs(1);         \
bsp_spide_cs(0);         \
bsp_spide_bus_mode(SPIDE_1IO);   \
bsp_spide_snd_byte(0x12);\
bsp_spide_bus_mode(SPIDE_4IO);   \
bsp_spide_snd_byte(0x00);\
bsp_spide_snd_byte(v);   \
bsp_spide_snd_byte(0x00);\
})
#define WriteComm32(v) \
({                   \
bsp_spide_cs(1);         \
bsp_spide_cs(0);         \
bsp_spide_bus_mode(SPIDE_1IO);   \
bsp_spide_snd_byte(0x32);\
bsp_spide_snd_byte(0x00);\
bsp_spide_snd_byte(v);   \
bsp_spide_snd_byte(0x00);\
bsp_spide_bus_mode(SPIDE_4IO);   \
})
#define WriteComm02(v) \
({                   \
bsp_spide_cs(1);         \
bsp_spide_cs(0);         \
bsp_spide_bus_mode(SPIDE_1IO);   \
bsp_spide_snd_byte(0x02);\
bsp_spide_snd_byte(0x00);\
bsp_spide_snd_byte(v);   \
bsp_spide_snd_byte(0x00);\
})
#define ReadComm03(v) \
({                   \
bsp_spide_cs(1);         \
bsp_spide_cs(0);         \
bsp_spide_bus_mode(SPIDE_1IO);   \
bsp_spide_snd_byte(0x03);\
bsp_spide_snd_byte(0x00);\
bsp_spide_snd_byte(v);   \
bsp_spide_snd_byte(0x00);\
})

//0x02: 1CMD 1ADDR 1DATA
AT(.com_text.tft_spi)
void tft_write_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x02);
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

//0x12: 1CMD 4ADDR 4DATA
AT(.com_text.tft_spi)
void tft_write_cmd12(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x12);
    DESPICON |= BIT(3);                         //4BIT
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

//0x03: 1CMD 1ADDR 1DATA
AT(.com_text.tft_spi)
void tft_read_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON &= ~BIT(3);                        //1BIT
    tft_spi_sendbyte(0x03);
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

AT(.com_text.tft_spi)
void tft_write_data(u8 data)
{
    tft_spi_sendbyte(data);
}

AT(.com_text.tft_spi)
void tft_write_end()
{
    TFT_SPI_CS_DIS();
}

AT(.com_text.tft_spi)
void tft_set_window(u16 x0, u16 y0, u16 x1, u16 y1)
{
    x0 += GUI_SCREEN_OFS_X;
    x1 += GUI_SCREEN_OFS_X;
    y0 += GUI_SCREEN_OFS_Y;
    y1 += GUI_SCREEN_OFS_Y;

    tft_write_cmd(0x2A);        //TFT_CASET
    tft_write_data(BYTE1(x0));
    tft_write_data(BYTE0(x0));
    tft_write_data(BYTE1(x1));
    tft_write_data(BYTE0(x1));

    tft_write_cmd(0x2B);        //TFT_PASET
    tft_write_data(BYTE1(y0));
    tft_write_data(BYTE0(y0));
    tft_write_data(BYTE1(y1));
    tft_write_data(BYTE0(y1));
    tft_write_end();
}

AT(.com_text.tft_spi)
void tft_write_data_start(void)
{
    tft_write_cmd12(0x2C);      //TFT_RAMWR
}



void tft_write_cmd(u8 cmd);
void tft_write_data(u8 data);
void tft_write_end(void);

#define WriteComm(v) tft_write_cmd(v)
#define ReadComm(v)  ReadComm03(v)

//0x03: 2bit
void tft_read_id_cmd(u8 cmd)
{
    TFT_SPI_CS_DIS();
    delay_us(1);
    TFT_SPI_CS_EN();
    DESPICON = (DESPICON & ~(0x03<<2)) | (0x00<<2); //1data in 1data out
    tft_spi_sendbyte(0x03);
    tft_spi_sendbyte(0x00);
    tft_spi_sendbyte(cmd);
    tft_spi_sendbyte(0x00);
}

uint32_t tft_read_id(void)
{
    u32 despicon = DESPICON;
    uint32_t id = 0;
    //tft_read_cmd(0x04);
    tft_read_id_cmd(0x04);
    id = tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    id = (id << 8) + tft_spi_getbyte();
    tft_write_end();
    DESPICON = despicon;
    return id;
}

void tft_320_st77916_init(void)
{
    printf("tft_st77916_init\n");

    WriteComm(0xDF);
    WriteData(0x98);
    WriteData(0x53);

    WriteComm(0xDE);
    WriteData(0x00);

    WriteComm(0xB2);
    WriteData(0x23);

    WriteComm(0xB7);
    WriteData(0x00);
    WriteData(0x2D);
    WriteData(0x00);
    WriteData(0x55);

    WriteComm(0xBB);
    WriteData(0x1C);
    WriteData(0x1A);
    WriteData(0x55);
    WriteData(0x73);
    WriteData(0x6F);
    WriteData(0xF0);

    WriteComm(0xC0);
    WriteData(0x44);
    WriteData(0xA4);

    WriteComm(0xC1);
    WriteData(0x12);

    WriteComm(0xC2);
    WriteData(0x00);

    WriteComm(0xC3);
    WriteData(0x7D);
    WriteData(0x07);
    WriteData(0x14);
    WriteData(0x06);
    WriteData(0xC8);
    WriteData(0x71);
    WriteData(0x6C);
    WriteData(0x77);

    WriteComm(0xC4);
    WriteData(0x00);//00=60Hz 04=52Hz 08=42Hz
    WriteData(0x00);
    WriteData(0xA0);
    WriteData(0x79);
    WriteData(0x13);
    WriteData(0x1A);
    WriteData(0x16);
    WriteData(0x79);
    WriteData(0x13);
    WriteData(0x1A);
    WriteData(0x16);
    WriteData(0x82);

    //SET_R_GAMMA Fit G2.5 20230822
    WriteComm(0xC8);    //G2.2
    WriteData(0x3F);    //0x3F
    WriteData(0x35);    //0x36
    WriteData(0x2E);    //0x30
    WriteData(0x27);    //0x29
    WriteData(0x2D);    //0x30
    WriteData(0x2E);    //0x32
    WriteData(0x27);    //0x2B
    WriteData(0x28);    //0x2C
    WriteData(0x26);    //0x2A
    WriteData(0x24);    //0x28
    WriteData(0x1F);    //0x26
    WriteData(0x10);    //0x17
    WriteData(0x0B);    //0x12
    WriteData(0x08);    //0x0D
    WriteData(0x03);    //0x07
    WriteData(0x01);    //0x02
    WriteData(0x3F);    //0x3F
    WriteData(0x35);    //0x36
    WriteData(0x2E);    //0x30
    WriteData(0x27);    //0x29
    WriteData(0x2D);    //0x30
    WriteData(0x2E);    //0x32
    WriteData(0x27);    //0x2B
    WriteData(0x28);    //0x2C
    WriteData(0x26);    //0x2A
    WriteData(0x24);    //0x28
    WriteData(0x1F);    //0x26
    WriteData(0x10);    //0x17
    WriteData(0x0B);    //0x12
    WriteData(0x08);    //0x0D
    WriteData(0x03);    //0x07
    WriteData(0x01);    //0x02


    WriteComm(0xD0);
    WriteData(0x04);
    WriteData(0x06);
    WriteData(0x6B);
    WriteData(0x0F);
    WriteData(0x00);

    WriteComm(0xD7);
    WriteData(0x30);
    WriteData(0x30);

    WriteComm(0xE6);
    WriteData(0x14);

    WriteComm(0xDE);
    WriteData(0x01);

    WriteComm(0xB7);
    WriteData(0x03);
    WriteData(0x13);
    WriteData(0xEF);
    WriteData(0x35);
    WriteData(0x35);

    WriteComm(0xC1);
    WriteData(0x14);
    WriteData(0x15);
    WriteData(0xC0);

    WriteComm(0xC2);
    WriteData(0x06);
    WriteData(0x3A);

    WriteComm(0xC4);
    WriteData(0x72);
    WriteData(0x12);

    WriteComm(0xBE);
    WriteData(0x00);

    WriteComm(0xDE);
    WriteData(0x00);

    WriteComm(0x35);
    WriteData(0x00);

    WriteComm(0x3A);
    WriteData(0x05);//06=RGB666；05=RGB565

    WriteComm(0x2A);
    WriteData(0x00);
    WriteData(0x00);//Start_X=00
    WriteData(0x00);
    WriteData(0xEF);//End_X=239

    WriteComm(0x2B);
    WriteData(0x00);
    WriteData(0x0C);//Start_Y=12
    WriteData(0x01);
    WriteData(0x33);//End_Y=307


    WriteComm(0x11);
    CommEnd();
    delay_ms(120);


    WriteComm(0x29);
    delay_ms(50);
    CommEnd();

}
#endif

