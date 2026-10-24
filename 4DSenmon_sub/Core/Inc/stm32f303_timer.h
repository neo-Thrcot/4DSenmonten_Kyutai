/*
 * stm32f303_timer.h
 *
 *  Created on: Oct 23, 2026
 *      Author: neoki
 */

#ifndef INC_STM32F303_TIMER_H_
#define INC_STM32F303_TIMER_H_

#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>

void TIM_Init(TIM_TypeDef* htim, uint32_t freq, uint32_t resolution);
void TIM_PWM_Init(TIM_TypeDef* htim, GPIOPin_t pin, uint8_t ch);
void TIM_Start(TIM_TypeDef* htim);

#endif /* INC_STM32F303_TIMER_H_ */
