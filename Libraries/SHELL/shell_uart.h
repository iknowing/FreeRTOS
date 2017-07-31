#ifndef __SHELL_UART_H
#define __SHELL_UART_H

#include "stm32f10x_usart.h"

void shell_uart_init(void);
void shell_uart_loop(void);

#endif
