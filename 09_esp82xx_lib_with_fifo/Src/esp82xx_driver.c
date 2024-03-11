#include "esp82xx_driver.h"

#define GPIOAEN                   (1U << 0)
#define USART2EN                  (1U << 17)
#define USART1EN                  (1U << 4)

#define CR1_UE                    (1U << 13)
#define SYS_FREQ                  16000000U       // ==> 1 sec
#define APB1_CLK                  SYS_FREQ
#define APB2_CLK                  SYS_FREQ

#define UART_BAUDRATE             115200
#define CR1_TE                    (1U << 3)
#define CR1_RE                    (1U << 2)

#define CR1_RXENEIE               (1U << 5)

#define SYSTICK_LOAD_VAL          (SYS_FREQ/1000U) // 16000 ==> 1 milliseconds
#define CTRL_ENABLE               (1U << 0)       // SYST_CSR [GUM page: 249], enable the counter
#define CTRL_CLKSRC               (1U << 2)       // ...,  0=external clock, 1=processor clock
#define CTRL_COUNTFLAG            (1U << 16)      // ..., returns 1 if timer counted to 0 since last time was read


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
  NVIC_EnableIRQ(USART2_IRQn);

  //6. Enable the UART module
  USART2->CR1 |= CR1_UE;
}

/* Pinout:
 * UART Module      :   UART1
 * UART Pins        :   PA9 = TX, PA10 = RX
 *
 * ESP pin              STM32F4 pin
 * -----------------------------------
 * ESP82xx RS pin (RESET)    :   PA8 OR 3.3V (PA8 connection redudunt to fix NO ST Link detected error which not exist in ESP-01S; only in ESP-01)
 * ESP82xx EN pin            :   3.3v
 * ESP82xx IO1 pin (GPIO 0)  :   3.3v
 * ESP82xx IO2 pin (GPIO 2)  :   3.3v
 * ESP82xx VCC pin           :   3.3v
 * ESP82xx GND pin           :   GND
 * ESP82xx TX pin            :   PA10 (RX)
 * ESP82xx RX pin            :   PA9 (TX)
 *
 */
void esp_rs_pin_init(void)
{
	// Enable clock access to GPIOA
	RCC->AHB1ENR |= GPIOAEN;

	// Set PA8 as ouput pin
  GPIOA->MODER |= (0x1UL << (16U));
  GPIOA->MODER &= ~(0x1UL << (17U));
}


void esp_rs_pin_enable(void)
{
	// Set PA8 to high
	GPIOA->ODR |= (0x1UL << (8U));
}

void esp_rs_pin_disable(void)
{
	// Set PA8 to high
	GPIOA->ODR &= ~(0x1UL << (8U));
}


void esp_uart_init(void)
{
  // Enable clock access to GPIOA
  RCC->AHB1ENR |= GPIOAEN;

  // Set PA9 and PA10 modes to Alternate Function

  //PA9
  GPIOA->MODER &= ~(1U << 18);
  GPIOA->MODER |= (1U << 19);

  //PA10
  GPIOA->MODER &= ~(1U << 20);
  GPIOA->MODER |= (1U << 21);

  //4. PA9 Alternate function type to USART1_TX (0b0111:AF07 |= AFRH9[7:4]) [RM page: 162]
  // GPIOx_AFRH register
  GPIOA->AFR[1] |= (1U << 4);
  GPIOA->AFR[1] |= (1U << 5);
  GPIOA->AFR[1] |= (1U << 6);
  GPIOA->AFR[1] &= ~(1U << 7);

  //5. PA10 Alternate function type to USART1_RX (0b0111:AF07 |= AFRH10[11:8]) [RM page: 162]
  // GPIOx_AFRH register
  GPIOA->AFR[1] |= (1U << 8);
  GPIOA->AFR[1] |= (1U << 9);
  GPIOA->AFR[1] |= (1U << 10);
  GPIOA->AFR[1] &= ~(1U << 11);


  //                  [ Configure UART module ]

  //1. Enable clock access to UART module (0b1 |= USART2EN[BIT17]) [RM page: 119]
  RCC->APB2ENR |= USART1EN;

  //2. Disable UART module, USART_CR1
  USART1->CR1 &= ~CR1_UE;

  //3. Set UART Baud rate
  USART1->BRR = compute_uart_bd(APB2_CLK, UART_BAUDRATE);

  //4. Set tranfer direction, want either or both Tx and Rx
  USART1->CR1 = CR1_TE | CR1_RE;

  // Enable RXNEIE
  USART1->CR1 |= CR1_RXENEIE;

  //5. Enable UART2 interrupt in NVIC
//  NVIC_EnableIRQ(USART1_IRQn); // Enable it in the library instead

  //6. Enable the UART module
  USART1->CR1 |= CR1_UE;

}

void esp_uart_write_char(char ch)
{
	/* make sure the transmit data register is empty */
	while (!(USART1->SR & SR_TXE));

	/* write to transmit data register */
	USART1->DR = (ch & 0xFF);
}

void debug_uart_write(int ch)
{
  // make sure the transmit data register is empty
  while(!(USART2->SR & SR_TXE)){}

  // write to transmit data register
  USART2->DR = (ch & 0xFF);
}

void systick_delay_ms(uint32_t delay)
{
  // Systick Register names in header file are different
  // SysTick->LOAD corresponse to `SysTick Reload Value Register` [SYST_RVR]
  // SysTick->VAL    ==>   `SysTick Current Value Register` [SYST_CVR]
  // SysTick->CTRL   ==>   `SysTick Control and Status Register` [SYST_CSR]

  // 1. Reload with number of clocks per milliseconds [SYST_RVR] [GUM page: 249]
  SysTick->LOAD = SYSTICK_LOAD_VAL - 1; // for 16000 clock pulses, set RELOAD 15999 (i.e. N-1)

  // 2. Clear `Systick current value register` [SYST_CVR] [GUM page: 249]
  SysTick->VAL = 0;

  // 3. Enable systick and select internal clock source [SYST_CSR] [GUM page: 249]
  SysTick->CTRL = CTRL_CLKSRC | CTRL_ENABLE;

  // if delay=5ms; i=0 then i++ every 1ms
  for (int i=0; i < delay; i++)
  {
    // Wait until COUNTFLAG is set = 1
    // no set = 0, timeout has not occured
    while((SysTick->CTRL & CTRL_COUNTFLAG) == 0){}
  }

  SysTick->CTRL = 0;
}

static uint16_t compute_uart_bd(uint32_t pclk, uint32_t desired_baud)
{
  return ((pclk +(desired_baud/2U))/desired_baud);
}

