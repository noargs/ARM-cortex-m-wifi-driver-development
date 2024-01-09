#ifndef FIFO_H_
#define FIFO_H_
#include <stdint.h>

typedef char tx_t;
typedef char rx_t;

void tx_fifo_init(void);
uint8_t tx_fifo_put(tx_t data);
uint8_t tx_fifo_get(tx_t* data);
uint32_t tx_fifo_size(void);
void rx_fifo_init(void);
uint8_t rx_fifo_put(rx_t data);
uint8_t rx_fifo_get(rx_t* data);
uint32_t rx_fifo_size(void);

#endif /* FIFO_H_ */
