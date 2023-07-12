#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"

int main()
{
  debug_uart_init();

  while(1)
  {
    printf("Hello from Debug UART \n\r");
  }
}
