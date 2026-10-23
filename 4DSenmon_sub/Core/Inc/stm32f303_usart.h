#ifndef INC_STM32F303_USART_H_
#define INC_STM32F303_USART_H_

#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>

typedef enum
{
	USART_TX_COMPLETE,
	USART_RX_COMPLETE,
	USART_RX_OVERRUN
} USARTCallbackType_t;

class USART {
	public:
		/*USART init function*/
		USART(USART_TypeDef* usart_ch);
		USART(USART_TypeDef* usart_ch, GPIOPin_t rx, GPIOPin_t tx);
		SysError_t init(uint32_t baudrate);
		SysError_t init(uint32_t baudrate, GPIOPin_t rx, GPIOPin_t tx);
		void setCallback(USARTCallbackType_t type, CallbackFunc_t func);

		/*USART transmit function*/
		SysError_t __send(uint8_t data);
		SysError_t transmit(uint8_t* buf, uint16_t size, uint32_t timeout);
		SysError_t transmitIT(uint8_t* buf, uint16_t size);

		/*USART receive function*/
		SysError_t receive(uint8_t* buf, uint16_t size, uint32_t timeout);
		SysError_t receiveIT(uint8_t* buf, uint16_t size);

		/*alternate interrupt function*/
		void IRQ_Handler(void);

	private:
		USART_TypeDef* 		ch;
		IRQn_Type			USARTx_IRQn;
		uint32_t 			APBxCLK;
		GPIOPin_t			rx_pin, tx_pin;

		/*transmit*/
		volatile uint8_t	*txdata_buf;
		volatile uint16_t 	txdata_num;
		volatile uint16_t	txdata_cnt;
		volatile SysError_t	tx_states;
		CallbackFunc_t		Transmit_CmpltCallback;

		/*receive*/
		volatile uint8_t	*rxdata_buf;
		volatile uint16_t 	rxdata_num;
		volatile uint16_t	rxdata_cnt;
		volatile SysError_t	rx_states;
		CallbackFunc_t		Receive_CmpltCallback;
		CallbackFunc_t		Receive_OvrunCallback;

		/*GPIO config function*/
		void pinInit(void);
};

extern "C" int __io_putchar(int ch);

extern USART Serial;

#endif /* INC_STM32F303_USART_H_ */
