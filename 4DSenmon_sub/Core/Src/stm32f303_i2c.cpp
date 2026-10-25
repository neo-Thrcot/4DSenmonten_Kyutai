/*
 * stm32f745_i2c.cpp
 *
 *  Created on: Jan 8, 2026
 *      Author: neoki
 */

#include <stm32f303_i2c.h>

I2C::I2C(I2C_TypeDef* hi2c)
{
	ch		= hi2c;
}

I2C::I2C(I2C_TypeDef* hi2c, GPIOPin_t sda, GPIOPin_t scl)
{
	ch		= hi2c;
	sda_pin	= sda;
	scl_pin = scl;
}

SysError_t I2C::init(uint32_t i2c_clock)
{
	switch((uint32_t)ch) {
		case I2C1_BASE:
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
			I2Cx_ER_IRQn = I2C1_ER_IRQn;
			I2Cx_EV_IRQn = I2C1_EV_IRQn;
			break;

		default:
			ch = NULL;
			return SYS_ERROR;
			break;
	}

	ch->CR1 &= ~I2C_CR1_PE;
	while(ch->CR1 & I2C_CR1_PE);

	ch->CR1 &= ~(I2C_CR1_NOSTRETCH | I2C_CR1_ANFOFF | I2C_CR1_DNF);
	if(i2c_clock == 1000000) {
		fastplasEnable();
		ch->TIMINGR = 0x00100001;
	} else if(i2c_clock == 400000) {
		ch->CR1 |= 2 << I2C_CR1_DNF_Pos;
		ch->TIMINGR = 0x00100008;
	} else {
		ch->CR1 |= 7 << I2C_CR1_DNF_Pos;
		ch->TIMINGR = 0x00201624;
	}

	pinInit();

	NVIC_EnableIRQ(I2Cx_ER_IRQn);
	NVIC_EnableIRQ(I2Cx_EV_IRQn);

	ch->CR1 |= I2C_CR1_PE;

	return SYS_OK;
}

bool I2C::addressMatch(uint8_t devaddr)
{
	uint32_t isr;

	masterInit(devaddr, 0, false, false, false);

	ch->CR2 |= I2C_CR2_START;

    while(1) {
    	isr = ch->ISR;

    	if(isr & I2C_ISR_NACKF) {
    		ch->ICR |= I2C_ICR_NACKCF;

    		while(!(ch->ISR & I2C_ISR_STOPF));
    		ch->ICR = I2C_ICR_STOPCF;

    		return false;
        }

    	if(isr & I2C_ISR_TC) {
    		ch->CR2 |= I2C_CR2_STOP;

    		while(!(ch->ISR & I2C_ISR_STOPF));
    		ch->ICR = I2C_ICR_STOPCF;

    		return true;
    	}
    }
}

SysError_t I2C::masterTransmit(uint8_t devaddr,
							   uint8_t* databuf,
							   uint16_t datasize,
							   uint64_t timeout_ms)
{
	uint64_t start_ms = millis();
	uint8_t nbytes;
	SysError_t states;

	if(datasize > 255) {
		nbytes = 255;
		masterInit(devaddr, nbytes, true, true, false);
	} else {
		nbytes = datasize;
		masterInit(devaddr, nbytes, true, false, false);
	}
	ch->CR2 |= I2C_CR2_START;

	states = masterWriteBuf(databuf, datasize, nbytes, timeout_ms, start_ms);
	if(states != SYS_OK) {
		return states;
	}

	while(!(ch->ISR & I2C_ISR_STOPF));
	ch->ICR = I2C_ICR_STOPCF;

	return SYS_OK;
}

SysError_t I2C::masterReceive(uint8_t devaddr,
							  uint8_t* databuf,
							  uint16_t datasize,
							  uint64_t timeout_ms)
{
	uint64_t start_ms = millis();
	uint8_t nbytes;
	SysError_t states;

	if(datasize > 255) {
		nbytes = 255;
	} else {
		nbytes = datasize;
	}

	if(datasize > 255) {
		masterInit(devaddr, nbytes, true, true, true);
	} else {
		masterInit(devaddr, nbytes, true, false, true);
	}
	ch->CR2 |= I2C_CR2_START;

	states = masterReadBuf(databuf, datasize, nbytes, timeout_ms, start_ms);
	if(states != SYS_OK) {
		return states;
	}

	while(!(ch->ISR & I2C_ISR_STOPF));
	ch->ICR = I2C_ICR_STOPCF;

	return SYS_OK;
}

SysError_t I2C::memWrite(uint8_t devaddr,
						 uint16_t regaddr,
						 bool is_16bitaddr,
						 uint8_t* databuf,
						 uint16_t datasize,
						 uint32_t timeout_ms)
{
	uint64_t start_ms = millis();
	uint8_t nbytes, regaddr_size;
	uint8_t regaddr_buf[2];
	SysError_t states;

	ch->ICR = I2C_ICR_BERRCF;

	if(is_16bitaddr == true) {
		regaddr_buf[0] = regaddr >> 8;
		regaddr_buf[1] = regaddr & 0xFF;
		regaddr_size = 2;
	} else {
		regaddr_buf[0] = (uint8_t)regaddr;
		regaddr_size = 1;
	}

	masterInit(devaddr, regaddr_size, false, true, false);
	ch->CR2 |= I2C_CR2_START;

	states = masterWriteBuf(regaddr_buf, regaddr_size, regaddr_size, timeout_ms, start_ms);
	if(states != SYS_OK) {
		ch->CR2 |= I2C_CR2_STOP;
		while(!(ch->ISR & I2C_ISR_STOPF));
		ch->ICR = I2C_ICR_STOPCF;

		return states;
	}

	states = waitFlag(I2C_ISR_TCR, true, timeout_ms, start_ms);
	if(states != SYS_OK) {
		ch->CR2 |= I2C_CR2_STOP;
		while(!(ch->ISR & I2C_ISR_STOPF));
		ch->ICR = I2C_ICR_STOPCF;

		return states;
	}

	ch->CR2 |= I2C_CR2_AUTOEND;
	if(datasize > 255) {
		nbytes = 255;
		masterReload(nbytes, true);
	} else {
		nbytes = datasize;
		masterReload(nbytes, false);
	}

	states = masterWriteBuf(databuf, datasize, nbytes, timeout_ms, start_ms);
	if(states != SYS_OK) {
		return states;
	}

	while(!(ch->ISR & I2C_ISR_STOPF));
	ch->ICR = I2C_ICR_STOPCF;
	while(ch->ISR & I2C_ISR_STOPF);

	return SYS_OK;
}

SysError_t I2C::memRead(uint8_t devaddr,
						uint16_t regaddr,
						bool is_16bitaddr,
						uint8_t* databuf,
						uint16_t datasize,
						uint32_t timeout_ms)
{
	uint64_t start_ms = millis();
	uint8_t nbytes, regaddr_size;
	uint8_t regaddr_buf[2];
	SysError_t states;

	ch->ICR = I2C_ICR_BERRCF;

	if(is_16bitaddr == true) {
		regaddr_buf[0] = regaddr >> 8;
		regaddr_buf[1] = regaddr & 0xFF;
		regaddr_size = 2;
	} else {
		regaddr_buf[0] = (uint8_t)regaddr;
		regaddr_size = 1;
	}

	masterInit(devaddr, regaddr_size, false, false, false);
	ch->CR2 |= I2C_CR2_START;

	states = masterWriteBuf(regaddr_buf, regaddr_size, regaddr_size, timeout_ms, start_ms);
	if(states != SYS_OK) {
		ch->CR2 |= I2C_CR2_STOP;
		while(!(ch->ISR & I2C_ISR_STOPF));
		ch->ICR = I2C_ICR_STOPCF;

		return states;
	}

	states = waitFlag(I2C_ISR_TC, true, timeout_ms, start_ms);
	if(states != SYS_OK) {
		ch->CR2 |= I2C_CR2_STOP;
		while(!(ch->ISR & I2C_ISR_STOPF));
		ch->ICR = I2C_ICR_STOPCF;

		return states;
	}

	if(datasize > 255) {
		nbytes = 255;
		masterInit(devaddr, nbytes, true, true, true);
	} else {
		nbytes = datasize;
		masterInit(devaddr, nbytes, true, false, true);
	}
	ch->CR2 |= I2C_CR2_START;

	states = masterReadBuf(databuf, datasize, nbytes, timeout_ms, start_ms);
	if(states != SYS_OK) {
		return states;
	}

	while(!(ch->ISR & I2C_ISR_STOPF));
	ch->ICR = I2C_ICR_STOPCF;

	return SYS_OK;
}

void I2C::pinInit(void)
{
	pinMode(scl_pin, OTHER);		pinMode(sda_pin, OTHER);
	pinOutType(scl_pin, OPENDRAIN);	pinOutType(sda_pin, OPENDRAIN);
	pinPullSelect(scl_pin, PULLUP);	pinPullSelect(sda_pin, PULLUP);
	pinSpeed(scl_pin, HIGHSPEED);	pinSpeed(sda_pin, HIGHSPEED);
	pinAFSelect(scl_pin, AF4);		pinAFSelect(sda_pin, AF4);
}

void I2C::fastplasEnable(void)
{
}

SysError_t I2C::waitFlag(uint32_t isr_flag,
						 bool type,
						 uint64_t timeout_ms,
						 uint64_t start_ms)
{
	uint32_t isr;

	while(1) {
		isr = ch->ISR;

		if(isr & I2C_ISR_NACKF) {
			clearFlag(I2C_ICR_NACKCF);

			return SYS_ERROR;
		}
		if(((isr & isr_flag) != 0) == type) {
			return SYS_OK;
		}

		if((millis() - start_ms) > timeout_ms) {
			return SYS_TIMEOUT;
		}
	}
}

void I2C::clearFlag(uint32_t icr_flag)
{
	ch->ICR = icr_flag;
}

void I2C::masterInit(uint8_t devaddr,
				uint8_t size,
				bool autoend,
				bool reload,
				bool wr)
{
	ch->CR2 &= ~(I2C_CR2_AUTOEND |
				 I2C_CR2_RELOAD |
				 I2C_CR2_NBYTES |
				 I2C_CR2_ADD10 |
				 I2C_CR2_RD_WRN |
				 I2C_CR2_SADD);
	ch->CR2 |= (size << I2C_CR2_NBYTES_Pos | devaddr << 1);

	if(autoend == true) {
		ch->CR2 |= I2C_CR2_AUTOEND;
	}
	if(reload == true) {
		ch->CR2 |= I2C_CR2_RELOAD;
	}
	if(wr == true) {
		ch->CR2 |= I2C_CR2_RD_WRN;
	}
}

void I2C::masterReload(uint8_t size, bool reload)
{
	if(reload == false) {
		ch->CR2 &= ~(I2C_CR2_RELOAD | I2C_CR2_NBYTES);
	} else {
		ch->CR2 &= ~(I2C_CR2_NBYTES);
	}
	ch->CR2 |= (size << I2C_CR2_NBYTES_Pos);
}

SysError_t I2C::masterWriteBuf(uint8_t* databuf,
							   uint16_t datasize,
							   uint8_t nbytes,
							   uint64_t timeout_ms,
							   uint64_t start_ms)
{
	uint16_t index = 0;
	SysError_t states;

	if(datasize > 255) {
		while(1) {
			for(uint8_t i = 0; i < nbytes; i++) {
				states = waitFlag(I2C_ISR_TXIS, true, timeout_ms, start_ms);

				if(states == SYS_OK) {
					ch->TXDR = databuf[index++];
				} else {
					return states;
				}
			}

			while(!(ch->ISR & I2C_ISR_TCR));

			datasize -= 255;
			if(datasize <= 255) {
				nbytes = datasize;
				masterReload(nbytes, false);
				break;
			} else {
				nbytes = 255;
				masterReload(nbytes, true);
			}
		}
	}

	for(uint8_t i = 0; i < nbytes; i++) {
		states = waitFlag(I2C_ISR_TXIS, true, timeout_ms, start_ms);

		if(states == SYS_OK) {
			ch->TXDR = databuf[index++];
		} else {
			return states;
		}
	}

	return SYS_OK;
}

SysError_t I2C::masterReadBuf(uint8_t* databuf,
							  uint16_t datasize,
							  uint8_t nbytes,
							  uint64_t timeout_ms,
							  uint64_t start_ms)
{
	uint16_t index = 0;
	SysError_t states;

	if(datasize > 255) {
		while(1) {
			for(uint8_t i = 0; i < nbytes; i++) {
				states = waitFlag(I2C_ISR_RXNE, true, timeout_ms, start_ms);

				if(states == SYS_OK) {
					databuf[index++] = ch->RXDR;
				} else {
					return states;
				}
			}

			while(!(ch->ISR & I2C_ISR_TCR));

			datasize -= 255;
			if(datasize <= 255) {
				nbytes = datasize;
				masterReload(nbytes, false);

				break;
			} else {
				nbytes = 255;
				masterReload(nbytes, true);
			}
		}
	}

	for(uint8_t i = 0; i < nbytes; i++) {
		states = waitFlag(I2C_ISR_RXNE, true, timeout_ms, start_ms);

		if(states == SYS_OK) {
			databuf[index++] = ch->RXDR;
		} else {
			return states;
		}
	}

	return SYS_OK;
}
