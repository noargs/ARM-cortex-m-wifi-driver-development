#include "esp82xx_driver.h"

#define GPIOAEN                   (1U << 0)
#define USART2EN                  (1U << 17)
#define CR1_UE                    (1U << 13)
#define SYS_FREQ                  16000000
#define APB1_CLK                  SYS_FREQ
#define UART_BAUDRATE             115200
#define CR1_TE                    (1U << 3)
#define CR1_RE                    (1U << 2)

static uint16_t compute_uart_bd(uint32_t pclk, uint32_t desired_baud);

int __io_putchar(int ch)
{
  debug_uart_write(ch);
  return ch;
}

// ABP1
// PA2 ==> USART2_TX (AF07)
// PA3 ==> USART2_RX (AF07)
void debug_uart_init(void)
{
  //1. enable clock access to UART pins' GPIO port A (0b1 |=  GPIOAEN[BIT0]) [RM page: 117]
  RCC->AHB1ENR |= GPIOAEN;

  //2. PA2 mode to Alternate function mode (0b10 |= MODER2[5:4]) [RM page: 157]
  GPIOA->MODER &= ~(1U << 4);
  GPIOA->MODER |= (1U << 5);

  //3. PA3 mode to Alternate function mode (0b10 |= MODER3[7:6]) [RM page: 157]
  GPIOA->MODER &= ~(1U << 6);
  GPIOA->MODER |= (1U << 7);

  //4. PA2 Alternate function type to USART2_TX (0b0111:AF07 |= AFRL2[11:8]) [RM page: 161]
  // GPIOx_AFRL register
  GPIOA->AFR[0] |= (1U << 8);
  GPIOA->AFR[0] |= (1U << 9);
  GPIOA->AFR[0] |= (1U << 10);
  GPIOA->AFR[0] &= ~(1U << 11);

  //5. PA3 Alternate function type to USART2_RX (0b0111:AF07 |= AFRL2[15:12]) [RM page: 161]
  // GPIOx_AFRL register
  GPIOA->AFR[0] |= (1U << 12);
  GPIOA->AFR[0] |= (1U << 13);
  GPIOA->AFR[0] |= (1U << 14);
  GPIOA->AFR[0] &= ~(1U << 15);


  //                  [ Configure UART module ]

  //1. Enable clock access to UART module (0b1 |= USART2EN[BIT17]) [RM page: 119]
  RCC->APB1ENR |= USART2EN;

  //2. Disable UART module, USART_CR1
  USART2->CR1 &= ~CR1_UE;

  //3. Set UART Baud rate
  USART2->BRR = compute_uart_bd(APB1_CLK, UART_BAUDRATE);

  //4. Set tranfer direction, want either or both Tx and Rx
  USART2->CR1 = CR1_TE | CR1_RE;

  //5. Enable UART2 interrupt in NVIC

  //6. Enable the UART module
  USART2->CR1 |= CR1_UE;
}

void debug_uart_write(int ch)
{
  // make sure the transmit data register is empty
  while(!(USART2->SR & SR_TXE)){}

  // write to transmit data register
  USART2->DR = (ch & 0xFF);
}

static uint16_t compute_uart_bd(uint32_t pclk, uint32_t desired_baud)
{
  return ((pclk +(desired_baud/2U))/desired_baud);
}

