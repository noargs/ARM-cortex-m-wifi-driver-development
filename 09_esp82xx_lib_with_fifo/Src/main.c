#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "fifo.h"
#include "esp82xx_lib.h"

#define SSID_NAME                 "TP-LINK_apt4"
#define PASSKEY                   "321321321"



int main()
{

	// Initiailise debug uart
	debug_uart_init();

	// Initialise esp82xx uart
	esp_uart_init();

	// Initialise RS pin
	// esp_rs_pin_init();

	// Enable RS pin
	// esp_rs_pin_enable();

	// Initialise server
	esp82xx_server_init(SSID_NAME, PASSKEY);


	while(1)
	{
		// Activate the server
		server_begin();
	}
}
