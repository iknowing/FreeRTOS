#include "stm32f10x.h"
#include <stdio.h>
#include "user_task.h"

ErrorStatus HSEStartUpStatus;
void RCC_Configuration(void);
void Debug_InfoInit(void);

int main(void)
{
    RCC_Configuration();
    Debug_InfoInit();
    if(SUCCESS == HSEStartUpStatus)
    {
        printf("Enable HSE sucess.\r\n");
        printf("The system CLK is 72MHz.\r\n");
    }
    else
    {
        printf("Enable HSE fail.\r\n");
        printf("The system CLK is 64MHz.\r\n");
    }
/*
 *  Hardware self-inspection
*/ 

/*Creat tasks and enable scheduler*/    
    vTaskCreat();
    vTaskStartScheduler();
    
    return 0;
}

/*******************************************************************************
* Function Name  : Debug_InfoInit
* Description    : Initialize the UART1 used to debug information output before Enter the FreeRTOS.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Debug_InfoInit(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	
    /* GPIOA Configuration:USART1_TX(PA9) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* GPIOA Configuration:USART1_RX(PA10) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    USART_InitTypeDef USART_InitStructure;    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);    
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);       
    USART_Cmd(USART1, ENABLE);  
}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
 
  // 复位RCC时钟配置(用于调试模式) 
  RCC_DeInit();
 
  // 使能外部晶振 
  RCC_HSEConfig(RCC_HSE_ON);
  
  // 等待外部晶振稳定 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    // 设置锁相环频率PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  }
  else {
    // 使能内部晶振 
    RCC_HSICmd(ENABLE);
    // 等待内置振荡器稳定 
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    // 设置锁相环频率PLLCLK = 8MHz/2 * 16 = 64 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);
  }

    // 使能FLASH的预取缓冲 
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  //设置代码延时值,FLASH_Latency_2为两个延时周期
  FLASH_SetLatency(FLASH_Latency_2);
	
  //设置系统总线时钟
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 

  //设置高速设备总线时钟，RCC_HCLK_Div1为系统时钟除以1
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  //设置低速设备总线时钟，RCC_HCLK_Div2为系统时钟除以2
  RCC_PCLK1Config(RCC_HCLK_Div2);
  
  //使能锁相环倍频
  RCC_PLLCmd(ENABLE);
  
  // 等待锁相环倍频后的频率稳定 
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  // 选择锁相环时钟为系统时钟源 
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  // 等待设置完成 
  while(RCC_GetSYSCLKSource() != 0x08);

}

int fputc(int c,FILE *f)
{
    while((USART1->SR & USART_FLAG_TXE) != USART_FLAG_TXE);
    USART_SendData(USART1,c);
    
    return c;
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number*/
  //printf("Wrong parameters value: file %s on line %d\r\n", file, line);

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
