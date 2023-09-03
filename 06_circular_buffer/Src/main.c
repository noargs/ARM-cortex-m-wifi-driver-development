#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "circular_buffer.h"


extern portType debug_port;

int main()
{

	 /*Initialize debug uart*/
   debug_uart_init();

   /*Initialize circular buffer*/
   circular_buffer_init();

   /*Send string*/
   buffer_send_string("Hello from DEBUG UART ...\n\r",debug_port);


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
