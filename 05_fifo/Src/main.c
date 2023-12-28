#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"

int tx_data_buffer[100];
int rx_data_buffer[100];

int main()
{
  char tx_data, rx_data;

  debug_uart_init();

  tx_fifo_init();
  rx_fifo_init();

  for(int i=0; i<100; i++)
  {
    tx_fifo_put(i*2);
    rx_fifo_put(i+3);
  }

  for (int i=0; i<100; i++)
  {
    tx_fifo_get(&tx_data);
    rx_fifo_get(&rx_data);

    tx_data_buffer[i] = tx_data;
    rx_data_buffer[i] = rx_data;
  }

  printf("--------------------- [TX Test] ---------------------\n\r");

  for (int i=0; i<100; i++)
  {
    printf("Data at `tx_data_buffer` idx %d = %d:\n\r",i, tx_data_buffer[i]);
  }

  printf("--------------------- [RX Test] ---------------------\n\r");
  for (int i=0; i<100; i++)
  {
    printf("Data at `rx_data_buffer` idx %d = %d:\n\r",i, rx_data_buffer[i]);
  }

  while(1)
  {
//    printf("A second just past. \n\r");
//    systick_delay_ms(1000);
  }
}
