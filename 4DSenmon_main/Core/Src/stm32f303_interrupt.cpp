/*
 * stm32f303_interrupt.cpp
 *
 *  Created on: Dec 13, 2025
 *      Author: neoki
 */

#include <stm32f303_sys.h>



extern "C" {

void SysTick_Handler(void)
{
	IncTick();
}

}



