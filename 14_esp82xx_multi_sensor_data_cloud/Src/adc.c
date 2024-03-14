#include "stm32f4xx.h"
#include "adc.h"

#define GPIOAEN               (1U << 0)
#define ADC1EN                (1U << 8)
#define ADC_CH1               (1U << 0)
#define ADC_SEQ_LEN_1         0x00
#define CR2_ADCON             (1U << 0)
#define CR2_CONT              (1U << 1)
#define CR2_SWSTART           (1U << 30)
#define SR_EOC                (1U << 1)

void pa1_adc_init(void)
{
	/* configure the ADC gpio pin */

	/* enable clock access to gpioa */
	RCC->AHB1ENR |= GPIOAEN;

	/* set PA1 as analog pin (to use for ADC Datasheet page 39) */
	GPIOA->MODER |= (1U << 2);
	GPIOA->MODER |= (1U << 3);

	/* configure the ADC module */

	/* enable clock access to the ADC */
	RCC->APB2ENR |= ADC1EN;

	/* converstion sequence start */
	ADC1->SQR3 = ADC_CH1;

	/* set length of sequence */
	ADC1->SQR1 = ADC_SEQ_LEN_1;

	/* enable ADC module */
	ADC1->CR2 = CR2_ADCON;

}

void start_conversion(void)
{
	/* enable continuous conversion RF page: 233 */
	ADC1->CR2 |= CR2_CONT;

	/* start ADC conversion */
	ADC1->CR2 |= CR2_SWSTART;
}

uint32_t adc_read(void)
{
	/* wait for conversion to be complete [read end of conversion flag EOC RF page 228] */
	while (!(ADC1->SR & SR_EOC));

	/* read converted result */
	return (ADC1->DR);
}
