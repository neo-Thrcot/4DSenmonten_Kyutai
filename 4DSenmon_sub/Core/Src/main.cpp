#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>
#include <stm32f303_usart.h>

/*USART*/
/*to sub mcu, USART2*/
#define USART2_MAIN_TX		PB3
#define USART2_MAIN_RX		PB4
#define USART2_MAIN_BAUDRATE	115200

/*LED*/
#define LED_B				PA11
#define LED_R				PA12

/*Callback function*/
void USART2_Receive_CmpltCallback(void);

USART MainSerial(USART2, USART2_MAIN_RX, USART2_MAIN_TX);
static bool is_main_received = false;

int main(void)
{
	RCC_Init();
	SysTick_Init();

	MainSerial.init(USART2_MAIN_BAUDRATE);
	MainSerial.setCallback(USART_RX_COMPLETE, USART2_Receive_CmpltCallback);

	pinMode(LED_B, OUTPUT);
	pinMode(LED_R, OUTPUT);

	pinWrite(LED_B, HIGH);
	pinWrite(LED_R, LOW);

	while(1)
	{
		static bool is_main_rxset = false;
		static uint8_t mainrx = 0x00;

		if(is_main_received == false && is_main_rxset == false) {
			is_main_rxset = true;

			MainSerial.receiveIT(&mainrx, 1);
		} else if(is_main_received == true) {
			is_main_received = is_main_rxset = false;

			pinToggle(LED_B);
			pinToggle(LED_R);
		}
	}

	return 0;
}

void USART2_Receive_CmpltCallback(void)
{
	is_main_received = true;
}
