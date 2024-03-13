#include <stdio.h>
#include <string.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "hardware_modules.h"

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

char* HtmlRes_HomePage = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
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
		<a class=\"button button-on\" href=\"/valve\">ON</a>\
		<a class=\"button button-on\" href=\"/gripper\">ON</a>\
		<a class=\"button button-on\" href=\"/pump\">ON</a>\
		<a class=\"button button-on\" href=\"/light\">ON</a>\
		</body></html>";

char* HtmlRes_ValvePage = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
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

char* HtmlRes_PumpPage = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
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
		<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: ON &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
		<a class=\"button button-on\" href=\"/valve\">ON</a>\
		<a class=\"button button-on\" href=\"/gripper\">ON</a>\
		<a class=\"button button-off\" href=\"/\">OFF</a>\
		<a class=\"button button-on\" href=\"/light\">ON</a>\
		</body></html>";

char* HtmlRes_LightPage = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
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
		<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: ON</p>\
		<a class=\"button button-on\" href=\"/valve\">ON</a>\
		<a class=\"button button-on\" href=\"/gripper\">ON</a>\
		<a class=\"button button-on\" href=\"/pump\">ON</a>\
		<a class=\"button button-off\" href=\"/\">OFF</a>\
		</body></html>";

char* HtmlRes_GripperPage = " <html><head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\
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
		<p>VALVE  State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp GRIPPER  State: ON &nbsp;&nbsp;&nbsp;&nbsp;&nbsp PUMP State: OFF &nbsp;&nbsp;&nbsp;&nbsp;&nbsp LIGHT  State: OFF</p>\
		<a class=\"button button-on\" href=\"/valve\">ON</a>\
		<a class=\"button button-off\" href=\"/\">OFF</a>\
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


void esp82xx_process_server_data(char* str, int link_id)
{
	if (strcmp(str, "/valve") == 0)
	{
		esp82xx_send_server_data(HtmlRes_ValvePage, link_id);
	}
	else if (strcmp(str, "/gripper") == 0)
	{
		esp82xx_send_server_data(HtmlRes_GripperPage, link_id);
	}
	else if (strcmp(str, "/pump") == 0)
	{
		esp82xx_send_server_data(HtmlRes_PumpPage, link_id);
	}
	else if (strcmp(str, "/light") == 0)
	{
		esp82xx_send_server_data(HtmlRes_LightPage, link_id);
	}
	else
	{
		esp82xx_send_server_data(HtmlRes_HomePage, link_id);
	}
}


void esp82xx_server_begin(void)
{
	char dest_buffer[64] = {0};
	char link_id;

	while (! buffer_get_next_str("+IPD,",1,&link_id));

	link_id -= 48;

	//	esp82xx_send_server_data(HtmlRes_HomePage, link_id);

	while (!(buffer_copy_up_to_str("HTTP/1.1", dest_buffer)));

	if (find_substr_in_str("/valve", dest_buffer) == 1)
	{
		esp82xx_process_server_data("/valve", link_id);
		valve_on();
		pump_off();
		light_off();
		gripper_off();

		buffer_send_str("\r\nValve module diagnostics starting ...\r\n", debug_port);
	}
	else if (find_substr_in_str("/gripper", dest_buffer) == 1)
	{
		esp82xx_process_server_data("/gripper", link_id);
		valve_off();
		pump_off();
		light_off();
		gripper_on();

		buffer_send_str("\r\nGripper module diagnostics starting ...\r\n", debug_port);
	}
	else if (find_substr_in_str("/light", dest_buffer) == 1)
	{
		esp82xx_process_server_data("/light", link_id);
		valve_off();
		pump_off();
		light_on();
		gripper_off();

		buffer_send_str("\r\nLight module diagnostics starting ...\r\n", debug_port);
	}
	else if (find_substr_in_str("/pump", dest_buffer) == 1)
	{
		esp82xx_process_server_data("/pump", link_id);
		valve_off();
		pump_on();
		light_off();
		gripper_off();

		buffer_send_str("\r\nPump module diagnostics starting ...\r\n", debug_port);
	}
	else
	{
		esp82xx_process_server_data("/", link_id);
	}

}

static void buffer_reset(char* buffer)
{
	int len = strlen(buffer);

	for (int i=0; i<len; i++)
	{
		buffer[i] = '\0';
	}
}

void esp82xx_thingspeak_send(char* apikey, int field_no, uint32_t value)
{
	char local_buff1[100] = {0};
	char local_buff2[100] = {0};

	// establish TCP connection with ThingSpeak
	buffer_send_str("AT+CIPSTART=\"TCP\",\"34.231.74.177\",80\r\n", esp82xx_port);   // or 184.106.153.149

	// wait for "OK" response
	while (! (buffer_isresponse("OK\r\n")));

	sprintf(local_buff1, "GET /update?api_key=%s&field&%d=%u\r\n", apikey, field_no, value);

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



