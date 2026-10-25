/*
 * stm32f745_i2c.h
 *
 *  Created on: Jan 8, 2026
 *      Author: neoki
 */

#ifndef INC_STM32F303_I2C_H_
#define INC_STM32F303_I2C_H_

#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>

typedef enum
{
	HOGEHOGE
} I2CCallbackType_t;

class I2C
{
	public:
		/*setting*/
		I2C(I2C_TypeDef* hi2c);
		I2C(I2C_TypeDef* hi2c, GPIOPin_t sda, GPIOPin_t scl);
		SysError_t	init(uint32_t i2c_clock);
		SysError_t	init(uint32_t i2c_clock, GPIOPin_t sda, GPIOPin_t scl);
		void		setCallback(I2CCallbackType_t type, CallbackFunc_t func);
		bool		addressMatch(uint8_t devaddr);

		/*transmit*/
		SysError_t 	masterTransmit(uint8_t devaddr, uint8_t* databuf, uint16_t datasize, uint64_t timeout_ms);
		SysError_t 	masterTransmitIT(uint8_t devaddr, uint8_t* databuf, uint16_t datasize);
		SysError_t 	masterTransmitDMA(uint8_t devaddr, uint8_t* databuf, uint16_t datasize);

		/*receive*/
		SysError_t masterReceive(uint8_t devaddr, uint8_t* databuf, uint16_t datasize, uint64_t timeout_ms);
		SysError_t masterReceiveIT(uint8_t devaddr, uint8_t* databuf, uint16_t datasize);
		SysError_t masterReceiveDMA(uint8_t devaddr, uint8_t* databuf, uint16_t datasize);

		/*memory access*/
		SysError_t memWrite(uint8_t devaddr, uint16_t regaddr, bool is_16bitaddr, uint8_t* databuf, uint16_t datasize, uint32_t timeout_ms);
		SysError_t memRead(uint8_t devaddr, uint16_t regaddr, bool is_16bitaddr, uint8_t* databuf, uint16_t datasize, uint32_t timeout_ms);

	private:
		I2C_TypeDef* 	ch;
		IRQn_Type		I2Cx_ER_IRQn;
		IRQn_Type		I2Cx_EV_IRQn;
		uint32_t 		APBxClk;
		GPIOPin_t 		sda_pin, scl_pin;

		/*GPIO config function*/
		void pinInit(void);

		void fastplasEnable(void);

		/*Hardware control function*/
		bool checkError(void);
		SysError_t waitFlag(uint32_t isr_flag, bool type, uint64_t timeout_ms, uint64_t start_ms);
		void clearFlag(uint32_t icr_flag);

		void masterInit(uint8_t devaddr, uint8_t size, bool autoend, bool reload, bool wr);
		void masterReload(uint8_t size, bool reload);
		SysError_t masterWriteBuf(uint8_t* databuf, uint16_t datasize, uint8_t nbytes, uint64_t timeout_ms, uint64_t start_ms);
		SysError_t masterReadBuf(uint8_t* databuf, uint16_t datasize, uint8_t nbytes, uint64_t timeout_ms, uint64_t start_ms);
};

#endif /* INC_STM32F303_I2C_H_ */
