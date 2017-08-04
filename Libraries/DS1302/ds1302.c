/**********************************************************
** Copyright(C)2014 ������˼�ص��ӿƼ����޹�˾
**********************************************************
** ��    Ŀ��
** ��    ����
** ��    ����
** ��ǰ�汾��1.0.0
** ��    �ߣ����޾�
** ���ʱ�䣺
** ��ʷ�汾����
** �޸����ݣ���
**********************************************************/

/**********************************************************
**		             �ļ�����
**********************************************************/
#include "DS1302.h"

/**********************************************************
**		    DS1302��ؼĴ�����ַ
**********************************************************/
#define R_Sec	0x81		 //��Ĵ�����������ַ
#define W_Sec	0x80		 //��Ĵ���д������ַ
#define R_Min	0x83		 //�ּĴ�����������ַ
#define W_Min	0x82		 //�ּĴ���д������ַ
#define R_Hour	0x85		 //ʱ�Ĵ�����������ַ7
#define W_Hour	0x84		 //ʱ�Ĵ���д������ַ
#define R_Day	0x87		 //��Ĵ�����������ַ
#define W_Day	0x86		 //��Ĵ���д������ַ
#define R_Mon	0x89		 //�¼Ĵ�����������ַ
#define W_Mon	0x88		 //�¼Ĵ���д������ַ
#define R_Year	0x8D		 //��Ĵ�����������ַ
#define W_Year	0x8C		 //��Ĵ���д������ַ
#define WP_EN	0x8E		 //�Ĵ���д����������ַ

/**********************************************************
**		             ��������
**********************************************************/
typedef unsigned char	BYTE;

/**********************************************************
**		             ��������
**********************************************************/

#define RT_CLK_L()        GPIO_ResetBits(RT_CLK)
#define RT_SDA_L()        GPIO_ResetBits(RT_SDA)
#define RT_CE_L()         GPIO_ResetBits(RT_CE)

#define RT_CLK_H()         GPIO_SetBits(RT_CLK)
#define RT_SDA_H()         GPIO_SetBits(RT_SDA)
#define RT_CE_H()          GPIO_SetBits(RT_CE)

#define RT_SDA_IN()        	{GPIO_InitTypeDef GPIO_InitStructure;\
                            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);\
                            \
                            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;\
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;\
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                            GPIO_Init(GPIOB, &GPIO_InitStructure);}



#define RT_SDA_OUT()        {GPIO_InitTypeDef GPIO_InitStructure;\
                            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);\
                            \
                            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;\
                            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;\
                            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
                            GPIO_Init(GPIOB, &GPIO_InitStructure);}

#define RT_SDA_IS_H()       GPIO_ReadInputDataBit(RT_SDA)


/**************************************************
*���ƣ�void IIC_Delay(void)
*���ܣ�DS1302 SPI��ʱ
*���룺��
*�������
**************************************************/
void IIC_Delay(void)
{
	uint16_t delay=10000;
    while(delay--);
}

/**************************************************
*���ƣ�void DS1302WriteByte(BYTE Temp)
*���ܣ�SPI�ֽ�д
*���룺BYTE  Ҫд����ֽ�
*�������
**************************************************/
void DS1302WriteByte(BYTE Temp)
{
	BYTE i;
	RT_CLK_L();
	IIC_Delay();
	for(i=0;i<8;i++)
	{
		if(Temp&0x01)
        {
            RT_SDA_H();
        }
        else
        {
            RT_SDA_L();
        }
		IIC_Delay();
		RT_CLK_H();
		IIC_Delay();
		RT_CLK_L();
		Temp>>=1;
	}
}

/**************************************************
*���ƣ�BYTE DS1302ReadByte()
*���ܣ�SPI�ֽڶ�
*���룺��
*�����BYTE	�������ֽ�
**************************************************/
BYTE DS1302ReadByte()
{
	BYTE i,Temp;
    
    RT_SDA_IN();
	IIC_Delay();
	for(i=0;i<8;i++)
	{
		Temp>>=1;
		if(RT_SDA_IS_H())
			Temp |= 0x80;
		RT_CLK_H();
		IIC_Delay();
		RT_CLK_L();
		IIC_Delay();
	}
    RT_SDA_OUT();
	return Temp;
}

/**************************************************
*���ƣ�BYTE DS1302Read(BYTE addr)
*���ܣ�DS1302 �Ĵ�����
*���룺BYTE  �Ĵ�����ַ
*�����BYTE	 �������ֽ�
**************************************************/
BYTE DS1302Read(BYTE addr)
{
	BYTE Temp;
    
	RT_CE_L();
	RT_CLK_L();
	RT_CE_H();
	DS1302WriteByte(addr);
	Temp=DS1302ReadByte();
	RT_CLK_H();
	RT_CE_L();
	return Temp;
}

/**************************************************
*���ƣ�void DS1302Write(BYTE addr,BYTE Temp)
*���ܣ�DS1302 �Ĵ���д
*���룺BYTE  �Ĵ�����ַ��BYTE	 д����ֽ�
*�������
**************************************************/
void DS1302Write(BYTE addr,BYTE Temp)
{
	RT_CE_L();
	RT_CLK_L();
	RT_CE_H();
	DS1302WriteByte(addr);
	DS1302WriteByte(Temp);
	RT_CLK_H();
	RT_CE_L();
}

#define  ToBCD(x)   (((x/10)<<4)+(x%10))
/**************************************************
*���ƣ�uint8_t DS1302_SetTime(uint8_t* date);
*���ܣ�DS1302ʱ��У׼
*���룺��ҪУ׼��ʱ��
*�������
**************************************************/
uint8_t DS1302_SetTime(uint8_t* date)
{

	DS1302Write(WP_EN,0x00);
	DS1302Write(W_Sec, ToBCD(date[0]));
	DS1302Write(W_Min, ToBCD(date[1]));
	DS1302Write(W_Hour,ToBCD(date[2]));
	DS1302Write(W_Day, ToBCD(date[3]));
	DS1302Write(W_Mon, ToBCD(date[4]));
	DS1302Write(W_Year,ToBCD(date[5]));
	DS1302Write(WP_EN,0x80);
    
    return 0;
}

#define ToHEX(x)  ((x>>4)*10+(x&0x0F))
/**************************************************
*���ƣ�uint8_t DS1302_GetTime(uint8_t* date);
*���ܣ�DS1302 ʱ���ȡ
*���룺��
*�������
**************************************************/
uint8_t DS1302_GetTime(uint8_t* date)	 
{
	date[5]=ToHEX(DS1302Read(R_Year));
	date[4]=ToHEX(DS1302Read(R_Mon));
	date[3]=ToHEX(DS1302Read(R_Day));
	date[2]=ToHEX(DS1302Read(R_Hour));
	date[1]=ToHEX(DS1302Read(R_Min));
	date[0]=ToHEX(DS1302Read(R_Sec));
    
    return 0;
}

/**************************************************
*���ƣ�void DS1302_EN(void)
*���ܣ�DS1302 ����
*���룺��
*�������
**************************************************/
void DS1302_EN(void)	
{
     BYTE Temp;

     GPIO_InitTypeDef GPIO_InitStructure;
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
     GPIO_Init(GPIOB, &GPIO_InitStructure);

     Temp = DS1302Read(R_Sec);
     if((Temp&0x80) == 0x80)
     {
          DS1302Write(WP_EN,0x00);
          DS1302Write(W_Sec,(Temp&0x7F));
          DS1302Write(WP_EN,0x80);   
     }
}

















