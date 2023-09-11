#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdint.h>
#include "stm32f4xx.h"

#define UART_BUFFER_SIZE		      512
#define INIT_VAL				          0

typedef uint8_t portType;


typedef struct
{
	unsigned char buffer[UART_BUFFER_SIZE];
	volatile uint32_t head;
	volatile uint32_t tail;
}circular_buffer;

int8_t find_str(char *str2, char *str1);
void circular_buffer_init(void);
void buffer_clear(portType uart);
int32_t buffer_peek(portType uart);
int32_t buffer_read(portType uart);
void buffer_write(unsigned char c, portType uart );
int8_t is_data(portType uart);
int8_t is_response(char *str);
int8_t get_next_strs(char *str,uint8_t num_of_chars, char *dest_buffer);
void buffer_send_string(const char *s, portType uart);
int8_t copy_up_to_string(char * str, char * dest_buffer);
void esp82_uart_callback(void);
void debug_uart_callback(void);

#endif /* CIRCULAR_BUFFER_H_ */
