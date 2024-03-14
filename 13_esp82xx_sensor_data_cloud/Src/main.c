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
#define FIELD_NUMBER              1

uint32_t sensor_value;

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

	// Initialise ADC
	pa1_adc_init();

	// start conversion ADC
	start_conversion();

	// initialise device
	esp82xx_device_init(SSID_NAME, PASSKEY);

	while(1)
	{
		sensor_value = adc_read();
		esp82xx_thingspeak_send(WRITE_API_KEY, FIELD_NUMBER, sensor_value);
		systick_delay_ms(3000);
	}
}
