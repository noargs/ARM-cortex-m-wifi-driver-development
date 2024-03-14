#include <stdio.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"
#include "hardware_modules.h"
#include "adc.h"

#define SSID_NAME                 "__hw_wifi__"
#define PASSKEY                   "dude@dude.com"
#define WRITE_API_KEY             "YOUR_WRITE_API_KEY_HERE"
#define NUMBER_OF_FIELDS          2

uint32_t sensor_value, filtered_value;
uint32_t packet[2];

int main()
{
	debug_uart_init();

	esp_uart_init();

	// RS pin is connected to PA8 instead of 3.3v
	esp_rs_pin_init();
	esp_rs_pin_enable();

	// Initialise hardware modules
//	light_init();
//	valve_init();
//	pump_init();
//	gripper_init();

	// Initialise ADC
	pa1_adc_init();

	// start conversion ADC
	start_conversion();

	// initialise device
	esp82xx_device_init(SSID_NAME, PASSKEY);

	while(1)
	{
		sensor_value = adc_read();
		filtered_value = sensor_value + 100; // simulate another sensor to send multiple fields to ThingSpeak

		packet[0] = sensor_value;
		packet[1] = filtered_value;

		esp82xx_thingspeak_send_multi_field(WRITE_API_KEY, NUMBER_OF_FIELDS, packet);

		systick_delay_ms(3000);
	}
}
