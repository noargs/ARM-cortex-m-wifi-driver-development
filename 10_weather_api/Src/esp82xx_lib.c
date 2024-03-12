#include "esp82xx_lib.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "fifo.h"

static void esp_server_response_search_start(void);
static uint8_t esp82xx_set_wifi_mode(uint8_t mode);
static void esp_wait_response(char* pt);
static void esp_search_check(char letter);
static void esp_wait_response(char* pt);
static void esp_copy_software_to_hardware(void);
static void esp_uart_callback(void);
static void esp82xx_process_data(void);
static void esp_uart_output_char(char data);

static uint8_t esp82xx_reset(void);
static uint8_t esp82xx_list_access_points(void);
static uint8_t esp82xx_join_wifi_access_point(const char* ssid, const char* password);
static uint8_t esp82xx_get_local_ip_addr(void);
uint8_t esp82xx_create_tcp_conn(char* ip_addr);
static uint8_t esp82xx_dns_get_ip(char* website);

#define SR_RXNE                           (1U << 5)
#define SR_TXE                            (1U << 7)
#define FIFO_FAIL                         0

#define SERVER_RESPONSE_SIZE              1024

#define ESP8266_WIFI_MODE_STA             1
#define ESP8266_WIFI_MODE_AP              2
#define ESP8266_WIFI_MODE_STA_AND_AF      3

#define MAX_TRIES                         10

char Sub_str[32];
volatile bool Searching          =  false;
volatile bool Is_response        =  false;
volatile int Search_index        =  0;

char Server_response_sub_str[16]               =  "+ipd,";  // `Internet Protocol Datagram`s data, consist of `header` and actual `data`
volatile bool Server_response_search_completed =  false;
volatile int Server_response_search_index      =  0;
volatile int Server_response_searching         =  0;
int Server_response_index                      =  0;

// Buffer to hold characters after "+IPD," substring
char ServerResponseBuffer[SERVER_RESPONSE_SIZE];

char Temp_Buffer[1024];

void esp82xx_init(const char* ssid, const char* password)
{
	/* enable fifos */
	tx_fifo_init();
	rx_fifo_init();

	/* enable rs pin */
	esp_rs_pin_init();

	/* enable esp uart */
	esp_uart_init();

	/* enable debud uart */
	debug_uart_init();

	/* initialise flags */
	Searching = false;
	Is_response = false;
	Server_response_searching = 0;
	Server_response_search_completed = 0;

	printf("ESP8266 Initialisation...\r\n");

	/* Enable interrupt */
	NVIC_EnableIRQ(USART1_IRQn);

	/* Reset esp module */
	if (esp82xx_reset() == 0)
		printf("Reset failure, could not reset \n\r");
	else
		printf("Reset was successful...\n\r");

	if (esp82xx_set_wifi_mode(ESP8266_WIFI_MODE_STA) == 0)
	{
		printf("SetWifiMode Failed\n\r");
	}
	else
	{
		printf("Wifi mode set successfully...\n\r");
	}

	esp82xx_list_access_points();

	if (esp82xx_join_wifi_access_point( ssid, password) == 0)
		printf("Couldn't join the wifi [%s]\n\r", ssid);
	else
		printf("Wifi [%s] joined successfully...\n\r", ssid);

	esp82xx_dns_get_ip("https://www.google.co.uk/");
}

/* Reset esp module */
static uint8_t esp82xx_reset(void)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		/* set reset pin low */
		esp_rs_pin_disable();

		/* wait a bit */
		systick_delay_ms(10);

		/* set reset pin high */
		esp_rs_pin_enable();

		/* send RST command */
		esp82xx_send_cmd("AT+RST\r\n");

		/* wait */
		systick_delay_ms(500);

		/* check for response */
		if (Is_response) return 1;  // success

		num_of_try--;
	}

	/* failed */
	return 0;
}

/* Set wifi mode */
static uint8_t esp82xx_set_wifi_mode(uint8_t mode)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		sprintf((char*)Temp_Buffer, "AT+CWMODE=%d\r\n", mode);
		esp82xx_send_cmd((const char*)Temp_Buffer);
		systick_delay_ms(500);

		if (Is_response) return 1; // success
		num_of_try--;
	}

	return 0;
}

/* List access point */
static uint8_t esp82xx_list_access_points(void)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try){
		esp82xx_send_cmd("AT+CWLAP\r\n");
		systick_delay_ms(5000);

		if (Is_response) return 1;

		num_of_try--;
	}
	return 0;
}

/* Join access point */
static uint8_t esp82xx_join_wifi_access_point(const char* ssid, const char* password)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		sprintf((char*)Temp_Buffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
		esp82xx_send_cmd((const char*)Temp_Buffer);
		systick_delay_ms(3000);

		if (Is_response) return 1; // success
		num_of_try--;
	}
	return 0;
}

/* Get local ip address */
static uint8_t esp82xx_get_local_ip_addr(void)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		esp82xx_send_cmd("AT+CIFSR\r\n");
		systick_delay_ms(3000);

		if (Is_response) return 1; // success
		num_of_try--;
	}
	return 0;
}

/* create tcp connection */
uint8_t esp82xx_create_tcp_conn(char* ip_addr)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		sprintf((char*)Temp_Buffer, "AT+CIPSTART=\"TCP\",\"%s\",80\r\n", ip_addr);
		esp82xx_send_cmd((const char*)Temp_Buffer);
		systick_delay_ms(3000);

		if (Is_response) return 1; // success
		num_of_try--;
	}
	return 0;
}

/* send tcp packets to remote server */
uint8_t esp82xx_send_tcp_pckt(char* pckt)
{
	/* combine packet length and command */
	sprintf((char*)Temp_Buffer, "AT+CIPSEND=%d\r\n", strlen(pckt));

	/* send packet length and command */
	esp82xx_send_cmd((const char*)Temp_Buffer);
	systick_delay_ms(50);

	/* send packet */
	esp82xx_send_cmd(pckt);

	/* initialise server response search */
	esp_server_response_search_start();

	while (Server_response_search_completed == false)
	{
		systick_delay_ms(1);
	}
	if (Server_response_search_completed == false)
		return 0;
	else
		return 1;
}


/* close tcp connection */
uint8_t esp82xx_close_tcp_connection(void)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		esp82xx_send_cmd("AT+CIPCLOSE\r\n");
		systick_delay_ms(3000);

		if (Is_response) return 1; // success
		num_of_try--;
	}
	return 0;
}

/* get domain name ip address */
static uint8_t esp82xx_dns_get_ip(char* website)
{
	uint8_t num_of_try = MAX_TRIES;
	esp_wait_response("ok\r\n");

	while (num_of_try)
	{
		sprintf((char*)Temp_Buffer, "AT+CIPDOMAIN=\"%s\"\r\n", website);
		esp82xx_send_cmd((const char*)Temp_Buffer);
		systick_delay_ms(3000);

		if (Is_response) return 1; // success
		num_of_try--;
	}
	return 0;
}


/* Initialise string search for server response before searching in Rx stream */
static void esp_server_response_search_start(void)
{
	strcpy(Server_response_sub_str, "+ipd,");
	Server_response_search_index     = 0;
	Server_response_searching        = 1;
	Server_response_search_completed = false;
	Server_response_index            = 0;
}


/*
 * Initialise string search in Rx stream
 *
 * Whenver we start a search (i.e. searching substring) we
 *   copy the substring and place into `Server_response_sub_str[16]`
 */
static void esp_wait_response(char* pt)
{
	strcpy(Sub_str, pt);
	Search_index = 0;
	Is_response  = false;
	Searching    = true;
}


/*
 * Convert to lowercase
 */
char esp_lc(char letter)
{
	if ((letter >= 'A') && (letter <= 'Z'))
	{
		letter |= 0x20;
	}
	return letter;
}


/*
 * Search for string in Rx data stream
 */
static void esp_search_check(char letter)
{
	if (Searching)
	{
		if (Sub_str[Search_index] == esp_lc(letter)) // Check if characters match
		{
			Search_index++;

			if (Sub_str[Search_index] == 0)         // Check if string match
			{
				Is_response = true;
				Searching   = false;
			}
		}
		else
		{
			// Start over
			Search_index = 0;
		}
	}
}


/*
 * Search for server response in Rx data stream
 */
static void esp_server_response_search_check(char letter)
{
	if (Server_response_searching == 1)
	{
		// check if characters match
		if (Server_response_sub_str[Server_response_search_index] == esp_lc(letter))
		{
			Server_response_search_index++;

			// check if strings match
			if (Server_response_sub_str[Server_response_search_index] == 0)
			{
				Server_response_searching = 2;
				strcpy(Server_response_sub_str, "\n\rok\r\n");
				Server_response_search_index = 0;
			}
		}
		else
		{
			// Start over
			Server_response_search_index = 0;
		}
	}
	else if (Server_response_searching == 2)
	{
		if (Server_response_index < SERVER_RESPONSE_SIZE)
		{
			ServerResponseBuffer[Server_response_index] = esp_lc(letter);
		}
		// Check if characters match
		if (Server_response_sub_str[Server_response_index] == esp_lc(letter))
		{
			Server_response_index++;

			// Check if strings match
			if (Server_response_sub_str[Server_response_index] == 0)
			{
				Server_response_search_completed = true;
				Server_response_searching = 0;
			}
		}
		else
		{
			// Start over
			Server_response_search_index = 0;
		}
	}
}


/*
 * Copy content of Fifo in to UART DR
 */
static void esp_copy_software_to_hardware(void)
{
	char letter;
	while ((USART2->SR & SR_TXE) && tx_fifo_size() > 0)
	{
		tx_fifo_get(&letter);
		USART2->DR = letter;
	}
}

/*
 * ouput uart character
 */
static void esp_uart_output_char(char data)
{
	if (tx_fifo_put(data) == FIFO_FAIL)
	{
		return;
	}
	esp_copy_software_to_hardware();
}

static void esp82xx_process_data(void)
{
	char letter;

	/* check if there is new data in uart data register */
	if (USART1->SR && SR_RXNE)
	{
		/* store data from wifi uart data register in local variable */
		letter = USART1->DR;

		/* print data from wifi uart data register to debug uart i.e. computer */
		esp_uart_output_char(letter);

		/* check for end of command */
		esp_search_check(letter);

		/* check for server response*/
		esp_server_response_search_check(letter);

	}
}

/* callback function for exp82xx uart */
static void esp_uart_callback(void)
{
	esp82xx_process_data();
}

/* esp82xx uart irqhandler */
void USART1_IRQHandler(void)
{
	esp_uart_callback();
}

/* send command to esp82xx */
void esp82xx_send_cmd(const char* cmd)
{
	int index = 0;
	while (cmd[index] != 0)
	{
		esp_uart_write_char(cmd[index++]);
	}
}
