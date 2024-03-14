#include <stdio.h>
#include <string.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"

static void esp82xx_reset(void);
static void esp82xx_startup_test(void);
static void esp82xx_sta_mode(void);
static void esp82xx_ap_connect(char *ssid, char *password);
static void esp82xx_single_connection_enable(void);
static void buffer_reset(char* buffer);


extern port_t esp82xx_port;
extern port_t debug_port;


void esp82xx_device_init(char *ssid, char *password)
{
	buffer_init();
	esp82xx_reset();
	esp82xx_startup_test();
	esp82xx_sta_mode();
	esp82xx_ap_connect(ssid, password);
	esp82xx_single_connection_enable();
}

// Reset/Restart module (esp8266) - RM 2021.08 page: 8
static void esp82xx_reset(void)
{
	buffer_send_str("AT+RST\r\n", esp82xx_port);
	systick_delay_ms(1000);

	// Response: "OK"
	while (!(buffer_isresponse("OK\r\n")));

	buffer_send_str("Reset was successful...\n\r", debug_port);
}


// Test AT startup (esp8266) - RM 2021.08 page: 8
static void esp82xx_startup_test(void)
{
	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	// Send Test command
	buffer_send_str("AT\r\n", esp82xx_port);

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	buffer_send_str("AT Startup test successful...\n\r", debug_port);
}


// Enable STA mode (Station mode) - RM page: 21
static void esp82xx_sta_mode(void)
{
	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	// Send STA command
	buffer_send_str("AT+CWMODE_CUR=1\r\n", esp82xx_port);

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	buffer_send_str("STA mode set successfully...\n\r", debug_port);
}


// Connect to Access point (AP) - RM page: 22
static void esp82xx_ap_connect(char *ssid, char *password)
{
	char data[80];

	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	buffer_send_str("Connecting to Access Point...\n\r", debug_port);

	// Put ssid, password and command into single packet
	sprintf(data, "AT+CWJAP_CUR=\"%s\",\"%s\"\r\n", ssid, password);

	// Send entire packet to esp uart
	buffer_send_str(data, esp82xx_port);

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	sprintf(data, "Connected to \"%s\"\n\r", ssid);

	buffer_send_str(data, debug_port);
}

// Enable single connection
static void esp82xx_single_connection_enable(void)
{
	buffer_clear(esp82xx_port);

	// Send 'Enable single connection' command - RM 2021.08 page: 50
	buffer_send_str("AT+CIPMUX=0\r\n", esp82xx_port);

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	buffer_send_str("Single connection enabled...\n\r", debug_port);
}


static void buffer_reset(char* buffer)
{
	int len = strlen(buffer);

	for (int i=0; i<len; i++)
	{
		buffer[i] = '\0';
	}
}

void esp82xx_thingspeak_send(char* apikey, int field_no, uint32_t sensor_value)
{
	char local_buff1[100] = {0};
	char local_buff2[100] = {0};

	// establish TCP connection with ThingSpeak
	buffer_send_str("AT+CIPSTART=\"TCP\",\"34.231.74.177\",80\r\n", esp82xx_port);   // or 184.106.153.149

	// wait for "OK" response
	while (! (buffer_isresponse("OK\r\n")));

	sprintf(local_buff1, "GET /update?api_key=%s&field&%d=%lu\r\n", apikey, field_no, sensor_value);

	int len = strlen(local_buff1);
	sprintf(local_buff2, "AT+CIPSEND=%d\r\n", len);

	// send buffer length
	buffer_send_str(local_buff2, esp82xx_port);

	// wait for ">" response
	while (! (buffer_isresponse(">")));

	// send parameters
	buffer_send_str(local_buff1, esp82xx_port);

	// wait for "SEND OK" response
	while (! (buffer_isresponse("SEND OK\r\n")));

	// wait for "CLOSED" response
	while (! (buffer_isresponse("CLOSED")));

	// clear buffers
	buffer_reset(local_buff1);
	buffer_reset(local_buff2);

	// re-initialise the Circluar Buffer
	buffer_init();
}

void USART1_IRQHandler(void)
{
	esp82_uart_callback();
}


void USART2_IRQHandler(void)
{
	debug_uart_callback();
}



