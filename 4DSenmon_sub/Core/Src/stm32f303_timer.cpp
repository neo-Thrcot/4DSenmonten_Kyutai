/*
 * stm32f303_timer.cpp
 *
 *  Created on: Oct 23, 2026
 *      Author: neoki
 */

#include <stm32f303_timer.h>

void TIM_Init(TIM_TypeDef* htim, uint32_t freq, uint32_t resolution)
{
	switch ((uint32_t)htim) {
		case TIM2_BASE:
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			break;
		case TIM3_BASE:
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			break;
	}

	htim->PSC = APB1TIMCLK / (freq * (resolution + 1));
	htim->ARR = resolution;

	htim->CR1 |= TIM_CR1_ARPE;
}

void TIM_PWM_Init(TIM_TypeDef* htim, GPIOPin_t pin, uint8_t ch)
{
	pinMode(pin, OTHER);
	switch ((uint32_t)htim) {
		case TIM2_BASE:
			pinAFSelect(pin, AF1);
			break;
		case TIM3_BASE:
			pinAFSelect(pin, AF2);
			break;
	}

	switch(ch) {
		case 1:
			htim->CCMR1 &= ~TIM_CCMR1_OC1M;
			htim->CCMR1 |= (0b0110UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
			htim->CCER |= TIM_CCER_CC1E;
			htim->CCR1 = 0;
			break;

		case 2:
			htim->CCMR1 &= ~TIM_CCMR1_OC2M;
			htim->CCMR1 |= (0b0110UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE;
			htim->CCER |= TIM_CCER_CC2E;
			htim->CCR2 = 0;
			break;

		case 3:
			htim->CCMR2 &= ~TIM_CCMR2_OC3M;
			htim->CCMR2 |= (0b0110UL << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE;
			htim->CCER |= TIM_CCER_CC3E;
			htim->CCR3 = 0;
			break;

		case 4:
			htim->CCMR2 &= ~TIM_CCMR2_OC4M;
			htim->CCMR2 |= (0b0110UL << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE;
			htim->CCER |= TIM_CCER_CC4E;
			htim->CCR4 = 0;
			break;
	}
}

void TIM_Start(TIM_TypeDef* htim)
{
	htim->EGR |= TIM_EGR_UG;
	htim->CR1 |= TIM_CR1_CEN;
}
