#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"

#define SSID_NAME                 "__hw_wifi__"
#define PASSKEY                   "dude@dude.com"


int main()
{

	// Initiailise debug uart
	debug_uart_init();

	// Initialise esp82xx uart
	esp_uart_init();

	// Initialise server
	esp82xx_server_init(SSID_NAME, PASSKEY);


	while(1);
}
