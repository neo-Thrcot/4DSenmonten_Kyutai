/*
 * stm32f302_gpio.cpp
 *
 *  Created on: Sep 22, 2025
 *      Author: neoki
 */

#include <stm32f303_gpio.h>

static uint8_t gpio_clk = 0x00;

void pinMode(GPIOPin_t pin, GPIOMode_t mode)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	if(!(gpio_clk & (1UL << port))) {
		gpio_clk |= 1UL << port;
		RCC->AHBENR |= 1UL << (port + 17);
	}

	GPIOx->MODER &= ~(0b11UL << (pin_num * 2));
	GPIOx->MODER |= (mode << (pin_num * 2));
}

void pinType(GPIOPin_t pin, GPIOType_t type)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	GPIOx->OTYPER &= ~(1UL << pin_num);
	GPIOx->OTYPER |= (type << pin_num);
}

void pinSpeed(GPIOPin_t pin, GPIOSpeed_t speed)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	GPIOx->OTYPER &= ~(0b11UL << (pin_num * 2));
	GPIOx->OTYPER |= (speed << (pin_num * 2));
}

void pinAFSelect(GPIOPin_t pin, GPIOAF_t af)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	if(pin_num < 8) {
		GPIOx->AFR[0] &= ~(0b1111UL << (pin_num * 4));
		GPIOx->AFR[0] |= (af << (pin_num * 4));
	} else {
		pin_num -= 8;
		GPIOx->AFR[1] &= ~(0b1111UL << (pin_num * 4));
		GPIOx->AFR[1] |= (af << (pin_num * 4));
	}
}

void pinWrite(GPIOPin_t pin, bool states)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	if(states == LOW) {
		GPIOx->ODR |= (1UL << pin_num);
	} else {
		GPIOx->ODR &= ~(1UL << pin_num);
	}
}

void pinToggle(GPIOPin_t pin)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	GPIOx->ODR ^= (1UL << pin_num);
}

void portWrite(GPIOPort_t port, uint16_t states)
{
	GPIOx_TypeDef(port)->ODR = states;
}

bool pinRead(GPIOPin_t pin)
{
	uint8_t port = PORT_NUM(pin);
	uint8_t pin_num = PIN_NUM(pin);
	GPIO_TypeDef* GPIOx = GPIOx_TypeDef(port);

	if(GPIOx->IDR & (1UL << pin_num)) {
		return HIGH;
	} else {
		return LOW;
	}
}
