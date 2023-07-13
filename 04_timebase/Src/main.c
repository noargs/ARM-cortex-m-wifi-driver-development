#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"

int main()
{
  debug_uart_init();

  while(1)
  {
    printf("A second just past. \n\r");
    systick_delay_ms(1000);
  }
}
