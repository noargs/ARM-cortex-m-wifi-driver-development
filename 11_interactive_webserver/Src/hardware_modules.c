#include "stm32f4xx.h"

void light_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= (1U << 0);

	/* Set PA5 as output pin */
	GPIOA->MODER |= (1U << 10);
	GPIOA->MODER &= ~(1U << 11);
}

void valve_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= (1U << 0);

	/* Set PA6 as output pin */
	GPIOA->MODER |= (1U << 12);
	GPIOA->MODER &= ~(1U << 13);
}

void pump_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= (1U << 0);

	/* Set PA4 as output pin */
	GPIOA->MODER |= (1U << 8);
	GPIOA->MODER &= ~(1U << 9);
}

void gripper_init(void)
{
	/* Enable clock access to GPIOA */
	RCC->AHB1ENR |= (1U << 0);

	/* Set PA7 as output pin */
	GPIOA->MODER |= (1U << 14);
	GPIOA->MODER &= ~(1U << 15);
}

void light_on(void)
{
	/* Set PA5 output to high */
	GPIOA->ODR |= (1U << 5);
}

void light_off(void)
{
	/* Set PA5 output to low */
	GPIOA->ODR &= ~(1U << 5);
}

void valve_on(void)
{
	/* Set PA6 output to high */
	GPIOA->ODR |= (1U << 6);
}

void valve_off(void)
{
	/* Set PA6 output to low */
	GPIOA->ODR &= ~(1U << 6);
}

void pump_on(void)
{
	/* Set PA4 output to high */
	GPIOA->ODR |= (1U << 4);
}

void pump_off(void)
{
	/* Set PA4 output to low */
	GPIOA->ODR &= ~(1U << 4);
}

void gripper_on(void)
{
	/* Set PA7 output to high */
	GPIOA->ODR |= (1U << 7);
}

void gripper_off(void)
{
	/* Set PA7 output to low */
	GPIOA->ODR &= ~(1U << 7);
}
