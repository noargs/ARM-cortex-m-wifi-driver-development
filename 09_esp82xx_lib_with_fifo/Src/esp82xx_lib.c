#include "esp82xx_lib.h"
#include <stdbool.h>
#include "fifo.h"

#define SR_RXNE                           (1U << 5)
#define SR_TXE                            (1U << 7)

#define SERVER_RESPONSE_SIZE               1024
#define BUFFER_SIZE                        1024

uint32_t RXBUFFERIndex = 0;
uint32_t LastReturnIndex = 0;
uint32_t CurrentReturnIndex = 0;

char RXBuffer[BUFFER_SIZE];
char TXBuffer[BUFFER_SIZE];

// Buffer to hold characters after "+IPD," substring
char server_response_buffer[SERVER_RESPONSE_SIZE];

char sub_str[32];
volatile bool searching          =  false;
volatile bool is_response        =  false;
volatile int search_index        =  0;

char server_response_sub_str[16]              =  "+ipd,";
volatile bool server_search_response_complete =  false;
volatile int server_response_search_index     =  0;
volatile int server_response_searching        =  0;

int server_response_index                     =   0;


// Initialise string search for server response
static void esp_server_response_search_start(void)
{
	strcpy(sub_str, "+ipd,");
	server_response_search_index    = 0;
	server_response_searching       = 1;
	server_search_response_complete = false;
	server_response_index           = 0;
}


// Initialise string search in Rx data stream
static void wait_for_response(char *pt)
{
	strcpy(server_response_sub_str, pt);
	search_index = 0;
	is_response  = false;
	searching    = true;
}


// Convert string to lowercase
char lc(char letter)
{
	if ((letter >= 'A') && (leter <= 'Z'))
		letter |= 0x20;

	return letter;
}


// Search for string in Rx data stream
static void search_check(char letter)
{
	if (searching)
	{
		// Check if characters match
    if (sub_str[search_index == lc(letter)])
    {
    	search_index++;

    	// Check if strings match
    	if (sub_str[search_index] == 0)
    	{
    		is_response = true;
    		searching   = false;
    	}
    }
    else
    {
    	// Start over
    	search_index = 0;
    }
	}
}


// Search for server response in Rx data stream
static void esp_server_response_search_check(char letter)
{
	if (server_response_searching == 1)
	{
		// Check if characters match
		if (server_response_sub_str[server_response_search_index] == lc(letter))
		{
			server_response_search_index++;

			// Check if strings match
			if (server_response_sub_str[server_response_index] == 0)
			{
				server_response_searching = 2;
				strcpy(server_response_sub_str, "\n\rok\r\n");
				server_response_search_index = 0;
			}
		}
		else
		{
			// Start over
			server_response_search_index = 0;
		}
	}
	else if (server_response_searching == 2)
	{
		if (server_response_index < SERVER_RESPONSE_SIZE)
		{
			server_response_buffer[server_response_index] = lc(letter);
			server_response_index++;
		}
		// Check if characters match
		if (server_response_sub_str[server_response_search_index] == lc(letter))
		{
			 server_response_search_index++;

			 // Check if strings match
			 if (server_response_sub_str[server_response_search_index] == 0)
			 {
				 server_search_response_complete = true;
				 server_response_searching = 0;
			 }
		}
		else
		{
			// Start over
			server_response_search_index = 0;
		}

	}
}


// Copy content of tx_fifo into Debug UART DR
static void copy_software_to_hardware(void)
{
	char letter;
	while ((USART2->SR & SR_TXE) && tx_fifo_size() > 0)
	{
		tx_fifo_get(&letter);
		USART2->DR = letter;
	}
}


// Ouput UART characters
void uart_output_char(char data)
{
	if (tx_fifo_put(data) == FIFO_FAIL)
		return;

	copy_software_to_hardware();
}

void esp_82xx_uart_to_fifo(void)
{
	char letter;
	if (USART1->SR & SR_RXNE)
	{
		letter = USART1->DR;
		uart_output_char(letter);

		if (RXBUFFERIndex >= BUFFER_SIZE)
		{
			RXBUFFERIndex = 0;
		}
		RXBuffer[RXBUFFERIndex] = letter;
		RXBUFFERIndex++;

		// Check for end of command
		search_check(letter);

		// Check for server response
		esp_server_response_search_check(letter);

		// Check for new line character
		if (letter == '\n')
		{

		}
	}
}


// Send command to esp82xx

// Look for server response in Rx data stream

// Copy content of UART DR into FIFO



// Callback function for esp82xx UART

// esp82xx UART IRQHandler
