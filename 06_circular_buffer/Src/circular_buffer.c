#include <string.h>
#include "circular_buffer.h"


static void buffer_store_char(circular_buffer* buffer, unsigned char c);
static int8_t process_copy(char* str, char* dest_buffer, int start_position);

#define CR1_RXNEIE		            (1U<<5)
#define CR1_TXEIE		              (1U<<7)
#define SR_RXNE		                (1U<<5)
#define SR_TXE		                (1U<<7)

port_t debug_port 	= 	0;
port_t esp82xx_port =  	1;


// Buffers for ESP_UART
circular_buffer rx_buffer1 = {{INIT_VAL},INIT_VAL,INIT_VAL};
circular_buffer tx_buffer1 = {{INIT_VAL},INIT_VAL,INIT_VAL};

// Buffers for DEBUG_UART
circular_buffer rx_buffer2 = {{INIT_VAL},INIT_VAL,INIT_VAL};
circular_buffer tx_buffer2 = {{INIT_VAL},INIT_VAL,INIT_VAL};

circular_buffer *_rx_buffer1;
circular_buffer *_tx_buffer1;
circular_buffer *_rx_buffer2;
circular_buffer *_tx_buffer2;


void buffer_init(void)
{
	// Initialise buffers
	_rx_buffer1 = &rx_buffer1;
	_tx_buffer1 = &tx_buffer1;
	_rx_buffer2 = &rx_buffer2;
	_tx_buffer2 = &tx_buffer2;

	// Initialise UART interrupt
	USART1->CR1 |=CR1_RXNEIE;  				// ESP_UART
	USART2->CR1 |=CR1_RXNEIE;  				// DEBUG_UART
}

static void buffer_store_char(circular_buffer* buffer, unsigned char c)
{
	// check for overflow - new data stored before the tail (head will advance from current location of the tail

	int loc = (uint32_t)(buffer->head + 1) % UART_BUFFER_SIZE;

	if (loc != buffer->tail)
	{
		buffer->buffer[buffer->head] = c;
		buffer->head = loc;
	}
}

// get the position of first character of a string in the buffer
// this function is only needed to process our esp data, hence debug doesn't need
void buffer_get_first_char(char *str)
{
	// make sure data is there in the buffer
	while (!buffer_isdata_on_rx(esp82xx_port));
	while (buffer_peek(esp82xx_port) != str[0])
	{
		_rx_buffer1->tail = (uint16_t)(_rx_buffer1->tail +1) % UART_BUFFER_SIZE;
		while (!buffer_isdata_on_rx(esp82xx_port));
	}
}

int8_t buffer_get_next_str(char* str, uint8_t num_of_chars, char* dest_buffer)
{
	// make sure string exists
	while (!buffer_isresponse(str));

	for (int index =0; index < num_of_chars; index++)
	{
		while (!buffer_isdata_on_rx(esp82xx_port));
		dest_buffer[index] = buffer_read(esp82xx_port);
	}
	return 1;
}

void buffer_send_str(const char* s, port_t uart)
{
	while (*s != '\0')
	{
		buffer_write(*s++, uart);
	}
}

static int8_t process_copy(char* str, char* dest_buffer, int start_position)
{
	int current_position =0;
	int string_length = strlen(str);
	int index = start_position;

	while (!buffer_isdata_on_rx(esp82xx_port));

	while (buffer_peek(esp82xx_port) != str[current_position])
	{
		dest_buffer[index] = _rx_buffer1->buffer[_rx_buffer1->tail];
		_rx_buffer1->tail = (uint16_t)(_rx_buffer1->tail +1) % UART_BUFFER_SIZE;
		index++;

		while (!buffer_isdata_on_rx(esp82xx_port));
	}

	while (buffer_peek(esp82xx_port) == str[current_position])
	{
		current_position++;
		dest_buffer[index++] = buffer_read(esp82xx_port);

		if (current_position == string_length)
		{
			return 1;
		}
		while (!buffer_isdata_on_rx(esp82xx_port));
	}

	if (current_position != string_length)
	{
		current_position =0;
		process_copy(str, dest_buffer, index);
	}

	if (current_position == string_length)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int8_t buffer_copy_up_to_str(char* str, char* dest_buffer)
{
	return (process_copy(str, dest_buffer, 0));
}

int8_t find_substr_in_str(char* sub_str, char* str)
{
	// `str` main string i.e. 'homework' and find `sub_str` substring 'home'

	int l, i, j;

	// count substring
	for (l =0; sub_str[l] != '\0'; l++) { }

	for (i =0, j =0; str[i] != '\0' && sub_str[j] != '\0'; i++)
	{
		if (str[i] == sub_str[j])
			j++;
		else
			j =0;

		if (j == l)
			return 1;         // success
		else
			return -1;
	}
}

void buffer_clear(port_t uart)
{
	if (uart == esp82xx_port)
	{
		memset(_rx_buffer1->buffer, '\0', UART_BUFFER_SIZE);
		_rx_buffer1->head =0;
	}

	if (uart == esp82xx_port)
	{
		memset(_rx_buffer2->buffer, '\0', UART_BUFFER_SIZE);
		_rx_buffer2->head =0;
	}
}

int32_t buffer_peek(port_t uart)
{
	if (uart == esp82xx_port)
	{
		if (_rx_buffer1->head == _rx_buffer1->tail)
		{
			return -1;
		}
		else
		{
			return _rx_buffer1->buffer[_rx_buffer1->tail];
		}
	}

	else if (uart == debug_port)
	{
		if (_rx_buffer2->head == _rx_buffer2->tail)
		{
			return -1;
		}
		else
		{
			return _rx_buffer2->buffer[_rx_buffer2->tail];
		}
	}

	else
	{
		return -1;
	}
}


int32_t buffer_read(port_t uart)
{
	if (uart == esp82xx_port)
	{
		if (_rx_buffer1->head == _rx_buffer1->tail)
		{
			return -1;
		}
		else
		{
			unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
			_rx_buffer1->tail = (uint32_t)(_rx_buffer1->tail +1) % UART_BUFFER_SIZE;
			return c;
		}
	}

	else if (uart == debug_port)
	{
		if (_rx_buffer2->head == _rx_buffer2->tail)
		{
			return -1;
		}
		else
		{
			unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
			_rx_buffer2->tail = (uint32_t)(_rx_buffer2->tail +1) % UART_BUFFER_SIZE;
			return c;
		}
	}

	else
	{
		return -1;
	}
}

void buffer_write(unsigned char c, port_t uart)
{
	if (uart == esp82xx_port)
	{
		int loc = (_tx_buffer1->head +1) % UART_BUFFER_SIZE;
		while (loc == _tx_buffer1->tail);
		_tx_buffer1->buffer[_tx_buffer1->head] = c;
		_tx_buffer1->head = loc;

		// enable UART transmit interrupt
		USART1->CR1 |= CR1_TXEIE;
	}

	if (uart == debug_port)
	{
		int loc = (_tx_buffer2->head +1) % UART_BUFFER_SIZE;
		while (loc == _tx_buffer2->tail);
		_tx_buffer2->buffer[_tx_buffer2->head] = c;
		_tx_buffer2->head = loc;

		// Enable UART transmit interrupt
		USART2->CR1 |= CR1_TXEIE;
	}
}


// check RX contains data
int8_t buffer_isdata_on_rx(port_t uart)
{
	if (uart == esp82xx_port)
	{
		return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail) % UART_BUFFER_SIZE;
	}
	else if (uart == debug_port)
	{
		return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;
	}
	else
	{
		return -1;
	}
}

int8_t buffer_isresponse(char* str)
{
	int current_position =0;
	int string_length = strlen(str);

	while (current_position != string_length)
	{
		current_position =0;
		buffer_get_first_char(str);

		while (buffer_peek(esp82xx_port) == str[current_position])
		{
			current_position++;
			buffer_read(esp82xx_port);

			if (current_position == len)
			{
				return 1;
			}

			while (!buffer_isdata_on_rx(esp82xx_port));
		}
	}

	if (current_position)
	{
		return 1;
	}
	else
	{
		return -1;
	}

}

void esp82_uart_callback(void)
{
	// check RXNE is raised and RXNEIE is enabled
	if (((USART1->SR & USART_SR_RXNE) != 0) && (USART1->CR1 & USART_CR1_RXNEIE != 0))
	{
		unsigned char c = USART1->DR;
		buffer_store_char(_rx_buffer1, c);
	}

	// check TXE is raised and TXEIE is enabled
	if (((USART1->SR & USART_SR_TXE) != 0) && (USART1->CR1 & USART_CR1_TXEIE) != 0)
	{
		if (tx_buffer1.head == tx_buffer1.tail) // buffer empty; nothing there to transmit
		{
			USART1->CR1 &= ~USART_CR1_TXEIE;
		}
		else
		{
			unsigned char c = tx_buffer1.buffer[tx_buffer.tail];
			tx_buffer1.tail = (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;

			USART1->DR = c;
		}
	}
}


//
//
//void debug_uart_callback(void)
//{
//	/*Check if RXNE is raised and also if RXNEIE is enabled*/
//	if(((USART2->SR & SR_RXNE) != 0) && ((USART2->CR1 & CR1_RXNEIE) != 0))
//	{
//		unsigned char c = USART2->DR;
//		buff_store_char(c,_rx_buffer2);
//	}
//
//	/*Check if TXE is raised and also if TXEIE is enabled*/
//	if(((USART2->SR & SR_TXE) != 0) && ((USART2->CR1 & CR1_TXEIE) != 0))
//	{
//		if(tx_buffer2.head  == tx_buffer2.tail)
//		{
//			USART2->CR1 &=~CR1_TXEIE;
//		}
//		else
//		{
//			unsigned char c  = tx_buffer2.buffer[tx_buffer2.tail];
//			tx_buffer2.tail =  (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;
//
//			USART2->DR = c;
//		}
//	}
//}
