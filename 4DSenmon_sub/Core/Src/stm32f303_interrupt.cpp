/*
 * stm32f303_interrupt.cpp
 *
 *  Created on: Dec 13, 2025
 *      Author: neoki
 */

#include <stm32f303_sys.h>
#include <stm32f303_usart.h>

extern USART MainSerial;

extern "C" {

void SysTick_Handler(void)
{
	IncTick();
}

void USART2_IRQHandler(void)
{
	MainSerial.IRQ_Handler();
}

}



