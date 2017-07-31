#ifndef __SHELL_H
#define __SHELL_H

#include <stdio.h>

/********************到的数据格式定义***********************/
#define u8 unsigned char
#define s8 signed char
#define u16 unsigned short int
#define s16 signed short int
#define u32 unsigned long int
#define s32 signed long int
#define f32 float
#define f64 double

#include "FreeRTOS.h"
#include "semphr.h"
#define printsh(mesg, args...)		{extern SemaphoreHandle_t xSemaphorePrint; \
																	xSemaphoreTake(xSemaphorePrint, portMAX_DELAY);\
																	fprintf(stdout, mesg, ##args); \
																	xSemaphoreGive(xSemaphorePrint);}

#define SHELLCMDSTRMAX			100    //勿改动

typedef struct __shellCmdTable
{
	//(void )(*func)(void);
	void *func;								//
	const u8 *cmdstr;					//
	const u8 *cmdparamtype;		//
	s16 cmdhasreturn;					//
	s16 cmdparamnum;					//
	const u8 *cmdhelp;				//
}_shellCmdTable;

extern void shell_rec(u8 dat);//指令接收循环接收函数,传入值为dat
extern void shell_loop(void);//循环解析函数,周期调用用来解析指令
#endif
