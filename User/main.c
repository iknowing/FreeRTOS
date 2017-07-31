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
 
  // ��λRCCʱ������(���ڵ���ģʽ) 
  RCC_DeInit();
 
  // ʹ���ⲿ���� 
  RCC_HSEConfig(RCC_HSE_ON);
  
  // �ȴ��ⲿ�����ȶ� 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    // �������໷Ƶ��PLLCLK = 8MHz * 9 = 72 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  }
  else {
    // ʹ���ڲ����� 
    RCC_HSICmd(ENABLE);
    // �ȴ����������ȶ� 
    while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

    // �������໷Ƶ��PLLCLK = 8MHz/2 * 16 = 64 MHz 
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);
  }

    // ʹ��FLASH��Ԥȡ���� 
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

  //���ô�����ʱֵ,FLASH_Latency_2Ϊ������ʱ����
  FLASH_SetLatency(FLASH_Latency_2);
	
  //����ϵͳ����ʱ��
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 

  //���ø����豸����ʱ�ӣ�RCC_HCLK_Div1Ϊϵͳʱ�ӳ���1
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  //���õ����豸����ʱ�ӣ�RCC_HCLK_Div2Ϊϵͳʱ�ӳ���2
  RCC_PCLK1Config(RCC_HCLK_Div2);
  
  //ʹ�����໷��Ƶ
  RCC_PLLCmd(ENABLE);
  
  // �ȴ����໷��Ƶ���Ƶ���ȶ� 
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
  
  // ѡ�����໷ʱ��Ϊϵͳʱ��Դ 
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  
  // �ȴ�������� 
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
