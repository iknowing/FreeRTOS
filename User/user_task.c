#include "user_task.h"
#include "stm32f10x.h"

TaskHandle_t  xHandleTaskFunction = NULL;
void vTaskFunction(void *pvParameters)
{
    TickType_t xBlockTime = pdMS_TO_TICKS(1000);
    
    GPIO_InitTypeDef GPIO_InitStructure;    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5;    //abcd OE ST LT0Êä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    vTaskDelay(1000/portTICK_RATE_MS);
    while(1)
    {
        ulTaskNotifyTake(pdTRUE,xBlockTime);
        if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1))
        {
            GPIO_ResetBits(GPIOA,GPIO_Pin_1);
        }
        else
        {
            GPIO_SetBits(GPIOA,GPIO_Pin_1);
        }
    }

//    vTaskDelete(NULL);  //must call vTaskDelete(NULL) when quit the task.
}


void vTaskCreat(void)
{
    xTaskCreate(vTaskFunction,
                "vTaskFunction",
                64,
                NULL,
                5,
                &xHandleTaskFunction);
}

