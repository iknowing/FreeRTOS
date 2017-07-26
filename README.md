# FreeRTOS
Study Recode Of FreeRTOS



2017-07-25 17:46:36
01.#ifndef FREERTOS_CONFIG_H  
02.#define FREERTOS_CONFIG_H  
03.   
04./*Here is a good place to include header files that are required across 
05.yourapplication. */  
06.#include "something.h"  
07.   
08.#define configUSE_PREEMPTION                    1  
09.#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0  
10.#define configUSE_TICKLESS_IDLE                 0  
11.#define configCPU_CLOCK_HZ                      60000000  
12.#define configTICK_RATE_HZ                      250  
13.#define configMAX_PRIORITIES                    5  
14.#define configMINIMAL_STACK_SIZE                128  
15.#define configTOTAL_HEAP_SIZE                   10240  
16.#define configMAX_TASK_NAME_LEN                 16  
17.#define configUSE_16_BIT_TICKS                  0  
18.#define configIDLE_SHOULD_YIELD                 1  
19.#define configUSE_TASK_NOTIFICATIONS            1  
20.#define configUSE_MUTEXES                       0  
21.#define configUSE_RECURSIVE_MUTEXES             0  
22.#define configUSE_COUNTING_SEMAPHORES           0  
23.#define configUSE_ALTERNATIVE_API               0/* Deprecated! */  
24.#define configQUEUE_REGISTRY_SIZE               10  
25.#define configUSE_QUEUE_SETS                    0  
26.#define configUSE_TIME_SLICING                  0  
27.#define configUSE_NEWLIB_REENTRANT              0  
28.#define configENABLE_BACKWARD_COMPATIBILITY     0  
29.#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5  
30.   
31./*Hook function related definitions. */  
32.#define configUSE_IDLE_HOOK                     0  
33.#define configUSE_TICK_HOOK                     0  
34.#define configCHECK_FOR_STACK_OVERFLOW          0  
35.#define configUSE_MALLOC_FAILED_HOOK            0  
36.   
37./*Run time and task stats gathering related definitions. */  
38.#define configGENERATE_RUN_TIME_STATS           0  
39.#define configUSE_TRACE_FACILITY                0  
40.#define configUSE_STATS_FORMATTING_FUNCTIONS    0  
41.   
42./*Co-routine related definitions. */  
43.#define configUSE_CO_ROUTINES                   0  
44.#define configMAX_CO_ROUTINE_PRIORITIES         1  
45.   
46./*Software timer related definitions. */  
47.#define configUSE_TIMERS                        1  
48.#define configTIMER_TASK_PRIORITY               3  
49.#define configTIMER_QUEUE_LENGTH                10  
50.#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE  
51.   
52./*Interrupt nesting behaviour configuration. */  
53.#define configKERNEL_INTERRUPT_PRIORITY        [dependent of processor]  
54.#define configMAX_SYSCALL_INTERRUPT_PRIORITY   [dependent on processor and application]  
55.#define configMAX_API_CALL_INTERRUPT_PRIORITY  [dependent on processor and application]  
56.   
57./*Define to trap errors during development. */  
58.#define configASSERT( ( x ) )     if( ( x ) == 0) vAssertCalled( __FILE__, __LINE__ )  
59.   
60./*FreeRTOS MPU specific definitions. */  
61.#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0  
62.   
63./*Optional functions - most linkers will remove unused functions anyway. */  
64.#define INCLUDE_vTaskPrioritySet                1  
65.#define INCLUDE_uxTaskPriorityGet               1  
66.#define INCLUDE_vTaskDelete                     1  
67.#define INCLUDE_vTaskSuspend                    1  
68.#define INCLUDE_xResumeFromISR                  1  
69.#define INCLUDE_vTaskDelayUntil                 1  
70.#define INCLUDE_vTaskDelay                      1  
71.#define INCLUDE_xTaskGetSchedulerState          1  
72.#define INCLUDE_xTaskGetCurrentTaskHandle       1  
73.#define INCLUDE_uxTaskGetStackHighWaterMark     0  
74.#define INCLUDE_xTaskGetIdleTaskHandle          0  
75.#define INCLUDE_xTimerGetTimerDaemonTaskHandle  0  
76.#define INCLUDE_pcTaskGetTaskName               0  
77.#define INCLUDE_eTaskGetState                   0  
78.#define INCLUDE_xEventGroupSetBitFromISR        1  
79.#define INCLUDE_xTimerPendFunctionCall          0  
80.   
81./* Aheader file that defines trace macro can be included here. */  
82.   
83.#end if/* FREERTOS_CONFIG_H*/ 


2017-07-25 16:59:21
Add FreeRTOS in the projet.
