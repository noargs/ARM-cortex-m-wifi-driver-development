#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"
#include "hardware_modules.h"

#define SSID_NAME                 "__hw_wifi__"
#define PASSKEY                   "dude@dude.com"



int main()
{
	debug_uart_init();

	esp_uart_init();

	// RS pin is connected to PA8 instead of 3.3v
	esp_rs_pin_init();
	esp_rs_pin_enable();

	// Initialise hardware modules
	light_init();
	valve_init();
	pump_init();
	gripper_init();

	esp82xx_server_init(SSID_NAME, PASSKEY);

	while(1)
	{
		// Activate the server
		esp82xx_server_begin();
	}
}
