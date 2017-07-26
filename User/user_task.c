#include "user_task.h"

void vTaskFunction(void *pvParameters)
{
    vTaskDelay(1000/portTICK_RATE_MS);
    while(1)
    {
    }

//    vTaskDelete(NULL);  //must call vTaskDelete(NULL) when quit the task.
}




