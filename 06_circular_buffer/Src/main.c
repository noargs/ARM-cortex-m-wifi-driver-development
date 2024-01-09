#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "circular_buffer.h"


extern port_t debug_port;

int main()
{
	// Test `find_substr_in_str` API
	char main_str[] = "Let's do tHo Homework";
	char substr_to_find[] = "Home";
	//find_substr_in_str(substr_to_find, main_str);

   debug_uart_init();
   buffer_init();
   printf("find string? %d\n\r", find_substr_in_str(substr_to_find, main_str));
   buffer_send_str("Hello from DEBUG UART ...\n\r",debug_port);

	while(1);
}

void USART1_IRQHandler(void)
{
	esp82_uart_callback();
}


void USART2_IRQHandler(void)
{
	debug_uart_callback();
}
