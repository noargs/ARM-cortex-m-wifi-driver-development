#include <stdio.h>
#include <string.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"

static void esp82xx_reset(void);
static void esp82xx_startup_test(void);
static void esp82xx_sta_mode(void);
static void esp82xx_ap_connect(char *ssid, char *password);
static void esp82xx_get_local_ip(void);
static void esp82xx_multiple_connection_enable(void);
static void esp82xx_create_tcp_server(void);
static int8_t esp82xx_send_server_data(char *str, int link_id);


extern port_t esp82xx_port;
extern port_t debug_port;

char* homepage_htmlres = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
<title>HARDWARE CONTROL SERVER </title><style>\
 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
	   text-decoration: none;\
	   font-size: 25px;\
	   margin: 30px 30px 30px 30px;\
	   cursor: pointer;\
	   border-radius: 4px;\
	   float: left;\
  }\
  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
  .button-off {background-color: #ff2200;}\
  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
  .module_label{margin: 30px 30px 30px 30px;}\
 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
<a class=\"button button-on\" href=\"/#\">ON</a>\
<a class=\"button button-on\" href=\"/#\">ON</a>\
<a class=\"button button-on\" href=\"/#\">ON</a>\
<a class=\"button button-on\" href=\"/#\">ON</a>\
</body></html>";

char* valvepage_htmlres = "<html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
		<title>HARDWARE CONTROL SERVER </title><style>\
		 html { font-family: Helvetica;display: inline-block;margin: 0px auto;text-align: center;width:100%}\
		 body{ margin-top: 50px;float: left;}  h1{color: #444444;margin: 50px auto 30px;}h3{color: #444444;margin-bottom: 50px;}\
		 .button{display: block;width: 80px;background-color:#1abc9c;border: none;color: white;padding: 13px 30px;\
			   text-decoration: none;\
			   font-size: 25px;\
			   margin: 30px 30px 30px 30px;\
			   cursor: pointer;\
			   border-radius: 4px;\
			   float: left;\
		  }\
		  .button-on{ background-color: #1abc77;}.button-on:active{background-color: #16a085;}\
		  .button-off {background-color: #ff2200;}\
		  .button-off:active{background-color: #ff0000;}{ font-size: 14px;color: #888;margin-bottom: 10px;}\
		  .module_label{margin: 30px 30px 30px 30px;}\
		 </style></head><body><h1>HARDWARE CONTROL SERVER </h1>\
		<p>VALVE  State: ON &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
		<a class=\"button button-off\" href=\"/\">OFF</a>\
		<a class=\"button button-on\" href=\"/gripper\">ON</a>\
		<a class=\"button button-on\" href=\"/pump\">ON</a>\
		<a class=\"button button-on\" href=\"/light\">ON</a>\
		</body></html>";

void esp82xx_server_init(char *ssid, char *password)
{
	buffer_init();
	esp82xx_reset();
	esp82xx_startup_test();
	esp82xx_sta_mode();
	esp82xx_ap_connect(ssid, password);
	esp82xx_get_local_ip();
	esp82xx_multiple_connection_enable();
	esp82xx_create_tcp_server();
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


// Get device local IP address  - RM 2021.08 page: 49
static void esp82xx_get_local_ip(void)
{
	char data[80];
	char ip_buffer[20];
	int len;

	// Clear esp uart buffer
	buffer_clear(esp82xx_port);

	// Send 'get ip address' command
	buffer_send_str("AT+CIFSR\r\n", esp82xx_port);

	// Response: "CIFSR:STAIP,<Station IP address>
	while(! (buffer_isresponse("CIFSR:STAIP,\"")));

	// Copy IP address portion
	while (! (buffer_copy_up_to_str("\"", ip_buffer)));

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	len = strlen(ip_buffer);

	ip_buffer[len - 1] = '\0';

	sprintf(data, "Local IP Address: %s\n\r", ip_buffer);

	buffer_send_str(data, debug_port);
}


// Enable multiple connections
static void esp82xx_multiple_connection_enable(void)
{
	buffer_clear(esp82xx_port);

	// Send 'Enable or disable mulitple connection'  - RM 2021.08 page: 50
	buffer_send_str("AT+CIPMUX=1\r\n", esp82xx_port);

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	buffer_send_str("Multiple connections enabled...\n\r", debug_port);
}


// Create TCP server  - RM 2021.08 page: 50
static void esp82xx_create_tcp_server(void)
{
	buffer_clear(esp82xx_port);

	// Send 'create tcp server' cmd
	buffer_send_str("AT+CIPSERVER=1,80\r\n", esp82xx_port);

	// Response: "OK"
	while (! (buffer_isresponse("OK\r\n")));

	buffer_send_str("Please connect to the Local IP address above...\n\r", debug_port);

}


// Send data to the server - RM 2021.08 page: 45 and 49
static int8_t esp82xx_send_server_data(char *str, int link_id)
{
	char data[80];

	int len = strlen(str);

	// Concatenate link_id,data length
	sprintf(data, "AT+CIPSEND=%d,%d\r\n", link_id, len);

	buffer_send_str(data, esp82xx_port);

	// Response: ">"
	while (! (buffer_isresponse(">")));

	buffer_send_str(str, esp82xx_port);

	while (! (buffer_isresponse("SEND OK")));

	// Close connection (5=close all connections)
	sprintf(data, "AT+CIPCLOSE=5\r\n");

	buffer_send_str(data, esp82xx_port);

	while (! (buffer_isresponse("OK\r\n")));

	return 1;
}


void esp82xx_server_begin(void)
{
	char link_id;

	while (! buffer_get_next_str("+IPD,",1,&link_id));

	link_id -= 48;

	esp82xx_send_server_data(homepage_htmlres, link_id);
}


void USART1_IRQHandler(void)
{
	esp82_uart_callback();
}


void USART2_IRQHandler(void)
{
	debug_uart_callback();
}



