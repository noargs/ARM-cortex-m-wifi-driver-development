#include "stm32f4xx.h"

// (1U << 0)  =  0b  0000 0000 0000 0000 0000 0000 0000 0001
// (1U << 7)  =  0b  0000 0000 0000 0000 0000 0000 1000 0000
#define GPIOAEN              (1U << 0)

// Initial state: AHBIENR   =   0b  0000 0000 0000 0000 0000 0001 0000 1110
// AHB1ENR = GPIOAEN        =   0b  0000 0000 0000 0000 0000 0000 0000 0001
// AHB1ENR |= GPIOAEN       =   0b  0000 0000 0000 0000 0000 0001 0000 1111

int main()
{
  // Enable clock access to GPIOA [User LED `LD2` PA5]
  RCC->AHB1ENR = GPIOAEN;

  // set PA5 to output pin
  GPIOA->MODER |= (1U << 10);
  GPIOA->MODER &= ~(1U << 11);

  // set PA5 high
  GPIOA->ODR |= (1U << 5);

  while(1);

  return 0;
}
