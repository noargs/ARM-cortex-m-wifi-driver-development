#include <string.h>
#include "circular_buffer.h"

#define CR1_RXNEIE		            (1U<<5)
#define CR1_TXEIE		              (1U<<7)
#define SR_RXNE		                (1U<<5)
#define SR_TXE		                (1U<<7)

portType debug_port 	= 	0;
portType esp82xx_port =  	1;


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


void circular_buffer_init(void)
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


static void buff_store_char(unsigned char c, circular_buffer * buffer)
{
	 int loc =  (uint32_t)(buffer->head +1)% UART_BUFFER_SIZE;

	/*Check if no overflow will occur*/
	 if(loc != buffer->tail){
		 /*Store character*/
		 buffer->buffer[buffer->head]  = c ;
		 /*Update head*/
		 buffer->head = loc;
	 }
}


/*Find a substring within a string*/
int8_t find_str(char *str2, char *str1)
{
	int l,i,j;

	/*Count substring i.r str2*/
	for(l= 0 ; str2[l] !='\0' ; l++){}

	for(i = 0, j = 0 ; str1[i] !='\0' && str2[j] !='\0';i++)
	{
		if(str1[i] ==  str2[j])
		{
			j++;
		}
		else
		{
			j = 0;
		}
	}

	if(j ==  l)
	{
		/*success*/
		return 1;
	}
	else
	{
      return -1;
	}

}


void buffer_clear(portType uart)
{
	if(uart == esp82xx_port )
	{
		/*Set buffer content to  '\0'*/
		memset(_rx_buffer1->buffer,'\0',UART_BUFFER_SIZE);
		_rx_buffer1->head = 0 ;
	}

	if(uart == debug_port )
	{
		/*Set buffer content to  '\0'*/
		memset(_rx_buffer2->buffer,'\0',UART_BUFFER_SIZE);
		_rx_buffer2->head = 0 ;
	}
}


int32_t buffer_peek(portType uart)
{
	if(uart == esp82xx_port )
	{
		 if(_rx_buffer1->head  ==  _rx_buffer1->tail)
		 {
			 return -1;
		 }
		 else
		 {
			 return _rx_buffer1->buffer[_rx_buffer1->tail];
		 }
	}

	else if(uart == debug_port )
	{
	   if(_rx_buffer2->head  ==  _rx_buffer2->tail)
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


int32_t buffer_read(portType uart)
{
	if(uart == esp82xx_port )
	{
		 if(_rx_buffer1->head  ==  _rx_buffer1->tail)
			 {
				 return -1;
			 }
		 else
		 {
			unsigned char c =  _rx_buffer1->buffer[_rx_buffer1->tail];
			_rx_buffer1->tail =  (uint32_t)(_rx_buffer1->tail +1)%UART_BUFFER_SIZE;

			return c;
		 }
	}

	else if(uart == debug_port )
	{
		 if(_rx_buffer2->head  ==  _rx_buffer2->tail)
		 {
				 return -1;
		 }
		 else
		 {
				unsigned char c =  _rx_buffer2->buffer[_rx_buffer2->tail];
				_rx_buffer2->tail =  (uint32_t)(_rx_buffer2->tail +1)%UART_BUFFER_SIZE;
				return c;

		 }
	}
	else
	{
		 return -1;
	}
}



void buffer_write(unsigned char c, portType uart )
{
	if(uart == esp82xx_port )
	{
		int loc =  (_tx_buffer1->head +1)%UART_BUFFER_SIZE;

		while(loc == _tx_buffer1->tail ){}

		_tx_buffer1->buffer[_tx_buffer1->head] =  c;

		_tx_buffer1->head =  loc;

		/*Enable UART Transmit interrupt*/
		USART1->CR1 |= CR1_TXEIE;

	}

	if(uart == debug_port)
	{
		int loc =  (_tx_buffer2->head +1)%UART_BUFFER_SIZE;

		while(loc == _tx_buffer2->tail ){}

		_tx_buffer2->buffer[_tx_buffer2->head]=  c;

		_tx_buffer2->head =  loc;

		/*Enable UART Transmit interrupt*/
		USART2->CR1 |= CR1_TXEIE;

	}
}


int8_t is_data(portType uart)
{
	if(uart ==  esp82xx_port)
	{
		return(uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail)%UART_BUFFER_SIZE;
	}
	else if(uart ==  debug_port)
	{
		return(uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail)%UART_BUFFER_SIZE;

	}
	else
	{
		return -1;
	}

}



/*Function to the position of the first character of a string in the
 * the buffer*/
static void get_first_char(char *str)
{
	/*Make sure there is data in the buffer*/
	while(!is_data(esp82xx_port)){}

	while(buffer_peek(esp82xx_port)!= str[0]){

		_rx_buffer1->tail =  (uint16_t)(_rx_buffer1->tail +1)%UART_BUFFER_SIZE;

		while(!is_data(esp82xx_port)){}

	}

}


int8_t is_response(char *str)
{
	int curr_pos = 0;
	int len = strlen(str);

	while( curr_pos != len)
	{
		curr_pos  = 0;
		get_first_char(str);

		while(buffer_peek(esp82xx_port) == str[curr_pos])
		{
			curr_pos++;

			buffer_read(esp82xx_port);

			if(curr_pos ==  len)
			{
				/*success*/
				return 1;
			}

			while(!is_data(esp82xx_port)){}

		}

	}

	if(curr_pos  == len)
	{
		/*success*/
	   return 1;
	}
	else
	{
	   return -1;
	}
}


int8_t get_next_strs(char *str,uint8_t num_of_chars, char *dest_buffer)
{
	/*Make sure the string exists*/
	while(!is_response(str)){}

	for(int indx=0; indx < num_of_chars ; indx++)
	{
		while(!is_data(esp82xx_port)){}
		dest_buffer[indx] =  buffer_read(esp82xx_port);

	}

	return 1;
}


void buffer_send_string(const char *s, portType uart)
{
	while(*s != '\0')
	{
		buffer_write(*s++,uart);
	}
}


static int8_t process_copy(char *str, char *dest_buffer, int start_pos)
{
	int curr_pos = 0;
	int len =  strlen(str);
	int indx = start_pos;

	while(!is_data(esp82xx_port)){}

	while(buffer_peek(esp82xx_port) != str[curr_pos] )
	{
		dest_buffer[indx] =  _rx_buffer1->buffer[_rx_buffer1->tail];
		_rx_buffer1->tail =  (uint16_t)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
		indx++;

		while(!is_data(esp82xx_port)){}
	}

	while(buffer_peek(esp82xx_port) == str[curr_pos])
	{
		curr_pos++;
		dest_buffer[indx++] = buffer_read(esp82xx_port);

		if(curr_pos == len)
			return 1;

		while(!is_data(esp82xx_port)){}
	}

	if(curr_pos != len)
	{
		curr_pos = 0;
		process_copy(str, dest_buffer, indx);
	}

	if(curr_pos  ==  len)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}


int8_t copy_up_to_string(char * str, char * dest_buffer)
{
	/*Process copy*/
	return (process_copy(str,dest_buffer, 0));
}


void esp82_uart_callback(void)
{
	/*Check if RXNE is raised and also if RXNEIE is enabled*/
	if(((USART1->SR & SR_RXNE) != 0) && ((USART1->CR1 & CR1_RXNEIE) != 0))
	{
		unsigned char c = USART1->DR;
		buff_store_char(c,_rx_buffer1);
	}

	/*Check if TXE is raised and also if TXEIE is enabled*/
	if(((USART1->SR & SR_TXE) != 0) && ((USART1->CR1 & CR1_TXEIE) != 0))
	{
		if(tx_buffer1.head  == tx_buffer1.tail)
		{
			USART1->CR1 &=~CR1_TXEIE;
		}
		else
		{
			unsigned char c  = tx_buffer1.buffer[tx_buffer1.tail];
			tx_buffer1.tail =  (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;

			USART1->DR = c;

		}
	}
}


void debug_uart_callback(void)
{
	/*Check if RXNE is raised and also if RXNEIE is enabled*/
	if(((USART2->SR & SR_RXNE) != 0) && ((USART2->CR1 & CR1_RXNEIE) != 0))
	{
		unsigned char c = USART2->DR;
		buff_store_char(c,_rx_buffer2);
	}

	/*Check if TXE is raised and also if TXEIE is enabled*/
	if(((USART2->SR & SR_TXE) != 0) && ((USART2->CR1 & CR1_TXEIE) != 0))
	{
		if(tx_buffer2.head  == tx_buffer2.tail)
		{
			USART2->CR1 &=~CR1_TXEIE;
		}
		else
		{
			unsigned char c  = tx_buffer2.buffer[tx_buffer2.tail];
			tx_buffer2.tail =  (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;

			USART2->DR = c;
		}
	}
}
