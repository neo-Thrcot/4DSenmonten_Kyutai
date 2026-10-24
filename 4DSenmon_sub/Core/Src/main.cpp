#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>
#include <stm32f303_usart.h>
#include <stm32f303_timer.h>

#include <stdio.h>

/*USART*/
/*to sub mcu, USART2*/
#define USART2_MAIN_TX			PB3
#define USART2_MAIN_RX			PB4
#define USART2_MAIN_BAUDRATE	115200

/*Motor*/
/*pinout*/
#define M1_1					PA0
#define M1_2					PA1
#define M2_1					PA2
#define M2_2					PA3
/*timer config*/
#define PWM_FREQ				25000
#define PWM_RESOLUTION			1023
/*PWM macro*/
#define M1_1_PWM(val)			(TIM2->CCR1 = (val))
#define M1_2_PWM(val)			(TIM2->CCR2 = (val))
#define M2_1_PWM(val)			(TIM2->CCR3 = (val))
#define M2_2_PWM(val)			(TIM2->CCR4 = (val))

/*LED*/
#define LED_B					PA11
#define LED_R					PA12

/*DIP SW*/
#define DIP1					PA4
#define DIP2					PA5
#define DIP3					PA6
#define DIP4					PA7
#define DIP_READ()				((GPIOA->IDR & (0b1111UL << 4)) >> 4)

void GPIO_Init(void);
void Motor_Init(void);

/*Callback function*/
void USART2_Receive_CmpltCallback(void);

USART MainSerial(USART2, USART2_MAIN_RX, USART2_MAIN_TX);
static bool is_main_received = false;

volatile uint8_t dip_state = 0;

int main(void)
{
	RCC_Init();
	SysTick_Init();
	GPIO_Init();
	Motor_Init();

	Serial.init(115200);

	MainSerial.init(USART2_MAIN_BAUDRATE);
	MainSerial.setCallback(USART_RX_COMPLETE, USART2_Receive_CmpltCallback);

	while(1)
	{
		dip_state = DIP_READ();
		delay(500);

//		static bool is_main_rxset = false;
//		static uint8_t mainrx = 0x00;
//
//		if(is_main_received == false && is_main_rxset == false) {
//			is_main_rxset = true;
//
//			MainSerial.receiveIT(&mainrx, 1);
//		} else if(is_main_received == true) {
//			is_main_received = is_main_rxset = false;
//
//			pinToggle(LED_B);
//			pinToggle(LED_R);
//		}
	}

	return 0;
}

void GPIO_Init(void)
{
	pinMode(LED_B, OUTPUT);
	pinMode(LED_R, OUTPUT);

	pinMode(DIP1, INPUT);		pinPullSelect(DIP1, PULLDOWN);
	pinMode(DIP2, INPUT);		pinPullSelect(DIP2, PULLDOWN);
	pinMode(DIP3, INPUT);		pinPullSelect(DIP3, PULLDOWN);
	pinMode(DIP4, INPUT);		pinPullSelect(DIP4, PULLDOWN);

	pinWrite(LED_B, LOW);
	pinWrite(LED_R, HIGH);
}

void Motor_Init(void)
{
	TIM_Init(TIM2, PWM_FREQ, PWM_RESOLUTION);

	TIM_PWM_Init(TIM2, M1_1, 1);
	TIM_PWM_Init(TIM2, M1_2, 2);
	TIM_PWM_Init(TIM2, M2_1, 3);
	TIM_PWM_Init(TIM3, M2_2, 4);

	TIM_Start(TIM2);

	M1_1_PWM(0);
	M1_2_PWM(0);
	M2_1_PWM(0);
	M2_2_PWM(0);
}

void USART2_Receive_CmpltCallback(void)
{
	is_main_received = true;
}
