#include <stdio.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"

static void esp82xx_reset(void);
static void esp82xx_startup_test(void);
static void esp82xx_sta_mode(void);
static void esp82xx_ap_connect(char *ssid, char *password);

extern portType esp82xx_port;
extern portType debug_port;


void esp82xx_server_init(char *ssid, char *password)
{
	circular_buffer_init();
	esp82xx_reset();
	esp82xx_startup_test();
	esp82xx_sta_mode();
	esp82xx_ap_connect(ssid, password);
}

// Reset/Restart module (esp8266) - RM page:13
static void esp82xx_reset(void)
{
	buffer_send_string("AT+RST\r\n", esp82xx_port);
	systick_delay_ms(1000);

	// Response: "OK"
	while (!(is_response("OK\r\n")));

	buffer_send_string("Reset was successful...\n\r", debug_port);
}


// Test AT startup (esp8266) - RM page:13
static void esp82xx_startup_test(void)
{
	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	// Send Test command
	buffer_send_string("AT\r\n", esp82xx_port);

	// Response: "OK"
	while (! (is_response("OK\r\n")));

	buffer_send_string("AT Startup test successful...\n\r", debug_port);
}


// Enable STA mode (Station mode) - RM page: 22
static void esp82xx_sta_mode(void)
{
	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	// Send STA command
	buffer_send_string("AT+CWMODE_CUR=1\r\n", esp82xx_port);

	// Response: "OK"
	while (! (is_response("OK\r\n")));

	buffer_send_string("STA mode set successfully...\n\r", debug_port);
}


// Connect to Access point (AP)
static void esp82xx_ap_connect(char *ssid, char *password)
{
	char data[80];

	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	buffer_send_string("Connecting to Access Point...\n\r", debug_port);

	// Put ssid, password and command into single packet
	sprintf(data, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);

	// Send entire packet to esp uart
	buffer_send_string(data, esp82xx_port);

	// Response: "OK"
	while (! (is_response("OK\r\n")));

	sprintf(data, "Connected to \"%s\"\r\n", ssid);

	buffer_send_string(data, debug_port);
}


void USART1_IRQHandler(void)
{
	esp82_uart_callback();
}


void USART2_IRQHandler(void)
{
	debug_uart_callback();
}
