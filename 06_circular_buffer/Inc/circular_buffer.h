#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include <stdint.h>
#include "stm32f4xx.h"

#define UART_BUFFER_SIZE          512
#define INIT_VAL                  0

typedef uint8_t port_t;

typedef struct
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile uint32_t head;
  volatile uint32_t tail;
}circular_buffer;

int8_t find_string(char *substring, char *string_to_check);

#endif /* CIRCULAR_BUFFER_H_ */
