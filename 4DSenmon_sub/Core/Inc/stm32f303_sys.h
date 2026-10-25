/*
 * stm32f303_sys.h
 *
 *  Created on: Dec 13, 2025
 *      Author: neoki
 */

#ifndef INC_STM32F303_SYS_H_
#define INC_STM32F303_SYS_H_

#include <stm32f3xx.h>

#define SYSCLK			72000000
#define AHBCLK			72000000
#define APB1CLK			36000000
#define APB1TIMCLK		72000000
#define APB2CLK			72000000
#define APB1TIMCLK		72000000

#define ABS_DIFF(a, b)			((a > b) ? a - b : b - a)
#define FULLTICK(carry, tick)	(((uint64_t)(carry) << 32) | (tick))

#define DISABLE					false
#define ENABLE					true

typedef enum
{
	SYS_OK		= 0UL,
	SYS_ERROR,
	SYS_BUSY,
	SYS_TIMEOUT
}SysError_t;

typedef void (*CallbackFunc_t)(void);

void RCC_Init(void);

void IncTick(void);
uint64_t GetTick(void);

uint64_t millis(void);
uint64_t micros(void);
void delay_ms(uint64_t ms);
void delay_us(uint64_t us);

#endif /* INC_STM32F303_SYS_H_ */
