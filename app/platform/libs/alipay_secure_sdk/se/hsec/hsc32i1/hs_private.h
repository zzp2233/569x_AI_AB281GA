#ifndef __HS_PRIVATE_H__
#define __HS_PRIVATE_H__
void IIC_TEST_DEMO(void);

extern unsigned char *g_HSI2CBuf;
extern unsigned char gHSEncTransSign;
extern unsigned char g_DefTransey[16];
extern unsigned char g_TransTmpKey[16];


//CRC���ֽڼ��㡪��ѭ�����ƣ��������bit
extern unsigned short LeftShift_CRC(unsigned char byValue, unsigned short *lpwCrc);
extern void ComputeCRC(unsigned char *Data, unsigned short Length, unsigned char *pbyOut);
extern void lmemset(unsigned char *pBuf, unsigned char byVal, unsigned short wLen);
extern void lmemcpy(unsigned char *pDst, unsigned char *pSrc, unsigned short wLen);
extern unsigned char lmemcmp(unsigned char *pBuf1, unsigned char *pBuf2, unsigned short wLen);


extern void HS_IIC_Init(void);
extern void HS_IIC_Send_Cmd(unsigned char *pData, unsigned short wLen);
extern unsigned char HS_IIC_Recv_Resp(unsigned char *pData, unsigned short wLen);
extern void IIC_Master_Send(unsigned char byAddr, unsigned char *pData, unsigned short wLen);
extern void IIC_Master_Receive(unsigned char byAddr, unsigned char *pData, unsigned short wLen);

extern void Delay_Ms(unsigned char byMilliSec);
extern void Set_GPIO_State(unsigned char byState );

#endif
