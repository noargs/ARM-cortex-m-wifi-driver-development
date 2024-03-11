#ifndef ESP82XX_DRIVER_H_
#define ESP82XX_DRIVER_H_

#include <stdint.h>
#include "stm32f4xx.h"

#define SR_TXE                    (1U << 7)
#define SR_RXNE                   (1U << 5)

void debug_uart_init(void);
void debug_uart_write(int ch);
void esp_uart_write_char(char ch);
void esp_uart_init(void);
void systick_delay_ms(uint32_t delay);
void esp_rs_pin_init(void);
void esp_rs_pin_enable(void);
void esp_rs_pin_disable(void);

#endif /* ESP82XX_DRIVER_H_ */
