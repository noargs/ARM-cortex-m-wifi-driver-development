#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdint.h>
#include "stm32f4xx.h"

#define UART_BUFFER_SIZE		      512
#define INIT_VAL				          0

typedef uint8_t port_t;


typedef struct
{
	unsigned char buffer[UART_BUFFER_SIZE];
	volatile uint32_t head;
	volatile uint32_t tail;
}circular_buffer;

void buffer_init(void);
void buffer_clear(port_t uart);
int32_t buffer_peek(port_t uart);
int32_t buffer_read(port_t uart);
void buffer_write(unsigned char c, port_t uart );
int32_t buffer_isdata_on_rx(port_t uart);
int8_t buffer_isresponse(char *str);

int8_t buffer_get_next_str(char* str, uint8_t num_of_chars, char* dest_buffer);
void buffer_send_str(const char *s, port_t uart);
int8_t buffer_copy_up_to_str(char* str, char* dest_buffer);
int8_t find_substr_in_str(char* sub_str, char* str);

void esp82_uart_callback(void);
void debug_uart_callback(void);

#endif /* CIRCULAR_BUFFER_H_ */
