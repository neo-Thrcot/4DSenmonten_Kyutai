/*
 * stm32f303_gpio.h
 *
 *  Created on: Dec 13, 2025
 *      Author: neoki
 */

#ifndef INC_STM32F303_GPIO_H_
#define INC_STM32F303_GPIO_H_

#include <stm32f303_sys.h>

#define HIGH		true
#define LOW			false

#define GPIOx_BASE(port)	(AHB2PERIPH_BASE + ((port) * 0x00000400UL))
#define GPIOx_TypeDef(port)	((GPIO_TypeDef*)GPIOx_BASE(port))

#define PORT_NUM(pin)		(((pin) & 0xF0) >> 4)
#define PIN_NUM(pin)		((pin) & 0x0F)

typedef enum
{
	PORTA		= 0UL,
	PORTB,
	PORTF		= 5UL
} GPIOPort_t;

typedef enum
{
	PA0			= 0x00UL,
	PA1,
	PA2,
	PA3,
	PA4,
	PA5,
	PA6,
	PA7,
	PA8,
	PA9,
	PA10,
	PA11,
	PA12,
	PA13,
	PA14,
	PA15,
	PB0,
	PB1,
	PB3			= 0x13,
	PB4,
	PB5,
	PB6,
	PB7,
	PF0			= 0x50UL,
	PF1
} GPIOPin_t;

typedef enum
{
	INPUT		= 0UL,
	OUTPUT,
	OTHER,
	ANALOG
} GPIOMode_t;

typedef enum
{
	AF0			= 0UL,
	AF1,
	AF2,
	AF3,
	AF4,
	AF5,
	AF6,
	AF7,
	AF8,
	AF9,
	AF10,
	AF11,
	AF12,
	AF13,
	AF14,
	AF15
} GPIOAF_t;

typedef enum
{
	PUSHPULL	= 0UL,
	OPENDRAIN
} GPIOType_t;

typedef enum
{
	LOWSPEED	= 0UL,
	MEDSPEED,
	HIGHSPEED	= 3UL
} GPIOSpeed_t;

void pinMode(GPIOPin_t pin, GPIOMode_t mode);
void pinType(GPIOPin_t pin, GPIOType_t type);
void pinSpeed(GPIOPin_t pin, GPIOSpeed_t speed);
void pinAFSelect(GPIOPin_t pin, GPIOAF_t af);

void pinWrite(GPIOPin_t pin, bool states);
void pinToggle(GPIOPin_t pin);
void portWrite(GPIOPort_t port, uint16_t states);

bool pinRead(GPIOPin_t pin);
uint16_t portRead(GPIOPort_t port);


#endif /* INC_STM32F303_GPIO_H_ */
