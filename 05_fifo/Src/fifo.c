#include "fifo.h"

#define TX_FIFO_SIZE            1024
#define RX_FIFO_SIZE            1024
#define TX_FAIL                 0
#define TX_SUCCESS              1
#define RX_FAIL                 0
#define RX_SUCCESS              1

tx_t static TX_FIFO[TX_FIFO_SIZE];
rx_t static RX_FIFO[RX_FIFO_SIZE];

uint32_t volatile tx_put_itr;
uint32_t volatile tx_get_itr;

uint32_t volatile rx_put_itr;
uint32_t volatile rx_get_itr;

// initialise tx fifo
void tx_fifo_init(void)
{
	tx_put_itr =0;
	tx_get_itr =0;
}

// put data into tx fifo
uint8_t tx_fifo_put(tx_t data)
{
	// bound check, if `tx fifo` full
	if ((tx_put_itr - tx_get_itr) & ~(TX_FIFO_SIZE-1))
			// fifo is full
			return(TX_FAIL);

	// finally put data into `tx fifo`
	TX_FIFO[tx_put_itr & (TX_FIFO_SIZE-1)] = data;
	tx_put_itr++;
	return(TX_SUCCESS);
}

uint8_t tx_fifo_get(tx_t* data)
{
	// if `tx fifo` is empty - recently initialised hence both `tx_get_itr` and `tx_put_itr` = 0
	if (tx_put_itr == tx_get_itr)
		// fifo empty
		return(TX_FAIL);

	// finally get data from fifo
	*data = TX_FIFO[tx_get_itr & (TX_FIFO_SIZE-1)];
	tx_get_itr++;
	return(TX_SUCCESS);
}

uint32_t tx_fifo_size(void)
{
	return (uint32_t)(tx_put_itr - tx_get_itr);
}


void rx_fifo_init(void)
{
	rx_put_itr =0;
	rx_get_itr =0;
}

uint8_t rx_fifo_put(rx_t data)
{
	// bound check, if `rx fifo` full
	if ((rx_put_itr - rx_get_itr) & ~(RX_FIFO_SIZE-1))
		// fifo is full
		return(RX_FAIL);

	// finally put data into `rx fifo`
	RX_FIFO[rx_put_itr & (RX_FIFO_SIZE-1)] = data;
	rx_put_itr++;
	return(RX_SUCCESS);
}

uint8_t rx_fifo_get(rx_t* data)
{
	// if `rx fifo` is empty - recently initialised hence both `rx_get_itr` and `rx_put_itr` = 0
	if (rx_put_itr == rx_get_itr)
		// `rx fifo` empty
		return(RX_FAIL);

	// finally get from `rx fifo`
	*data = RX_FIFO[tx_get_itr & (TX_FIFO_SIZE-1)];
	rx_get_itr++;
	return(RX_SUCCESS);
}

uint32_t rx_fifo_size(void)
{
	return (uint32_t)(rx_put_itr - rx_get_itr);
}















//#include "fifo.h"
//
//
//#define TX_FIFO_SIZE              1024
//#define RX_FIFO_SIZE              1024
//#define TX_FAIL                   0
//#define TX_SUCCESS                1
//#define RX_FAIL                   0
//#define RX_SUCCESS                1
//
//tx_t static TX_FIFO[TX_FIFO_SIZE];
//rx_t static RX_FIFO[RX_FIFO_SIZE];
//
//uint32_t volatile tx_put_itr;
//uint32_t volatile tx_get_itr;
//uint32_t volatile rx_put_itr;
//uint32_t volatile rx_get_itr;
//
//
//void tx_fifo_init(void)
//{
//  tx_put_itr = 0;
//  tx_get_itr = 0;
//}
//
//
//// put data into tx fifo
//uint8_t tx_fifo_put(tx_t data)
//{
//  // check if fifo is full
//  if ((tx_put_itr - tx_get_itr) & ~(TX_FIFO_SIZE - 1))
//  {
//    return(TX_FAIL);
//  }
//
//  // put data into fifo
//  TX_FIFO[tx_put_itr & (TX_FIFO_SIZE - 1)] = data;
//
//  tx_put_itr++;
//  return(TX_SUCCESS);
//
//}
//
//uint8_t tx_fifo_get(tx_t *data)
//{
//  // check if fifo is empty
//  if (tx_put_itr == tx_get_itr)
//  {
//    // fifo empty
//    return(TX_FAIL);
//  }
//
//  // get the data
//  *data = TX_FIFO[tx_get_itr & (TX_FIFO_SIZE - 1)];
//
//  tx_get_itr++;
//
//  return(TX_SUCCESS);
//}
//
//uint32_t tx_fifo_size(void)
//{
//  return(uint32_t)(tx_put_itr - tx_get_itr);
//}
//
//
//// Initialise rx fifo
//void rx_fifo_init(void)
//{
//  rx_put_itr = 0;
//  rx_get_itr = 0;
//}
//
//// put data into rx fifo
//uint8_t rx_fifo_put(rx_t data)
//{
//  // check if fifo is full
//  if ((rx_put_itr - rx_get_itr) & ~(RX_FIFO_SIZE - 1))
//  {
//    return (RX_FAIL);
//  }
//
//  // put data into fifo
//  RX_FIFO[rx_put_itr & (RX_FIFO_SIZE - 1)] = data;
//
//  rx_put_itr++;
//
//  return (RX_SUCCESS);
//}
//
//uint8_t rx_fifo_get(rx_t *data)
//{
//  // check if fifo is empty
//  if (rx_put_itr == rx_get_itr)
//  {
//    // fifo emptty
//    return (RX_FAIL);
//  }
//
//  // get the data
//  *data = RX_FIFO[rx_get_itr & (RX_FIFO_SIZE - 1)];
//
//  rx_get_itr++;
//
//  return (RX_SUCCESS);
//}
//
//uint32_t rx_fifo_size(void)
//{
//  return (uint32_t)(rx_put_itr - rx_get_itr);
//}
//
