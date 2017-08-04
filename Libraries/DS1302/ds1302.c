/**********************************************************
** Copyright(C)2014 杭州美思特电子科技有限公司
**********************************************************
** 项    目：
** 文    件：
** 描    述：
** 当前版本：1.0.0
** 作    者：董艳军
** 完成时间：
** 历史版本：无
** 修改内容：无
**********************************************************/

/**********************************************************
**		             文件引用
**********************************************************/
#include "DS1302.h"

/**********************************************************
**		    DS1302相关寄存器地址
**********************************************************/
#define R_Sec	0x81		 //秒寄存器读操作地址
#define W_Sec	0x80		 //秒寄存器写操作地址
#define R_Min	0x83		 //分寄存器读操作地址
#define W_Min	0x82		 //分寄存器写操作地址
#define R_Hour	0x85		 //时寄存器读操作地址7
#define W_Hour	0x84		 //时寄存器写操作地址
#define R_Day	0x87		 //天寄存器读操作地址
#define W_Day	0x86		 //天寄存器写操作地址
#define R_Mon	0x89		 //月寄存器读操作地址
#define W_Mon	0x88		 //月寄存器写操作地址
#define R_Year	0x8D		 //年寄存器读操作地址
#define W_Year	0x8C		 //年寄存器写操作地址
#define WP_EN	0x8E		 //寄存器写保护操作地址

/**********************************************************
**		             变量定义
**********************************************************/
typedef unsigned char	BYTE;

/**********************************************************
**		             函数定义
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
*名称：void IIC_Delay(void)
*功能：DS1302 SPI延时
*输入：无
*输出：无
**************************************************/
void IIC_Delay(void)
{
	uint16_t delay=10000;
    while(delay--);
}

/**************************************************
*名称：void DS1302WriteByte(BYTE Temp)
*功能：SPI字节写
*输入：BYTE  要写入的字节
*输出：无
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
*名称：BYTE DS1302ReadByte()
*功能：SPI字节读
*输入：无
*输出：BYTE	读出的字节
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
*名称：BYTE DS1302Read(BYTE addr)
*功能：DS1302 寄存器读
*输入：BYTE  寄存器地址
*输出：BYTE	 读出的字节
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
*名称：void DS1302Write(BYTE addr,BYTE Temp)
*功能：DS1302 寄存器写
*输入：BYTE  寄存器地址；BYTE	 写入的字节
*输出：无
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
*名称：uint8_t DS1302_SetTime(uint8_t* date);
*功能：DS1302时间校准
*输入：需要校准的时间
*输出：无
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
*名称：uint8_t DS1302_GetTime(uint8_t* date);
*功能：DS1302 时间读取
*输入：无
*输出：无
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
*名称：void DS1302_EN(void)
*功能：DS1302 起振
*输入：无
*输出：无
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

















