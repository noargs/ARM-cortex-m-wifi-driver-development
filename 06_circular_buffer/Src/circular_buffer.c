#include <string.h>
#include "circular_buffer.h"

#define CR1_RXNEIE                (1U << 5)

port_t debug_port   = 0;
port_t esp82xx_port = 1;

// Buffers for ESP_UART
circular_buffer rx_buffer1 = {{INIT_VAL}, INIT_VAL, INIT_VAL};
circular_buffer tx_buffer1 = {{INIT_VAL}, INIT_VAL, INIT_VAL};

// Buffers for DEBUG_UART
circular_buffer rx_buffer2 = {{INIT_VAL}, INIT_VAL, INIT_VAL};
circular_buffer tx_buffer2 = {{INIT_VAL}, INIT_VAL, INIT_VAL};

circular_buffer *_rx_buffer1;
circular_buffer *_tx_buffer1;

circular_buffer *_rx_buffer2;
circular_buffer *_tx_buffer2;

void circular_buffer_init(void)
{
  // initialise buffers
  _rx_buffer1 = &rx_buffer1;
  _tx_buffer1 = &tx_buffer1;
  _rx_buffer2 = &rx_buffer2;
  _tx_buffer2 = &tx_buffer2;

  // initialise UART interrupts
  USART1->CR1 |= CR1_RXNEIE;        /* esp_uart */
  USART2->CR1 |= CR1_RXNEIE;        /* debug_uart */
}

static void buffer_store_char(unsigned char c, circular_buffer *buffer)
{
  int location = (uint32_t)(buffer->head + 1) % UART_BUFFER_SIZE;

  // check for overflow
  if (location != buffer->tail)
  {
    // store character
    buffer->buffer[buffer->head] = c;
    // update head
    buffer->head = location;
  }
}

// find a substring within a string
int8_t find_string(char *substring, char *string_to_check)
{
  int l,i,j;

  // count substring
  for(l=0; substring[l] != '\0'; l++){}

  for(i=0, j=0; string_to_check[i] != '\0' && substring[j] != '\0'; i++)
  {
    if (string_to_check[i] == substring[j])
    {
      j++;
    }
    else
    {
      j = 0;
    }
  }

  if (j==1)
  {
    // success
    return 1;
  }
  else
  {
    return -1;
  }
}

void buffer_clear(port_t uart)
{
  if (uart == esp82xx_port)
  {
    // set buffer content to '\0'
    memset(_rx_buffer1->buffer, '\0', UART_BUFFER_SIZE);
    _rx_buffer1->head = 0;
  }

  if (uart == debug_port)
  {
    memset(_rx_buffer2->buffer, '\0', UART_BUFFER_SIZE);
    _rx_buffer2->head = 0;
  }

}

int32_t buffer_peek(port_t uart)
{
  if (uart == esp82xx_port)
  {
    if(_rx_buffer1->head == _rx_buffer1->tail)
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
    if(_rx_buffer2->head == _rx_buffer2->tail)
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

int32_t buffer_read(port_uart)
{
  if (uart == esp82xx_port)
  {
    if(_rx_buffer1->head == _rx_buffer1->tail)
    {
      return -1;
    }
    else
    {
      unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
      _rx_buffer1->tail = (uint32_t)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
      retur c;
    }
  }
  else if (uart == debug_port)
  {
    if(_rx_buffer2->head == _rx_buffer2->tail)
    {
      return -1;
    }
    else
    {
      unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
      _rx_buffer2->tail = (uint32_t)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
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
		int location = (uint32_t)(_tx_buffer1->head + 1) % UART_BUFFER_SIZE;
		while(location == _tx_buffer1->tail) {}
		_tx_buffer1->buffer[_tx_buffer1->head] = c;
		_tx_buffer1->head = location;

		// Enable UART Transmit interrupt here
	}
}
