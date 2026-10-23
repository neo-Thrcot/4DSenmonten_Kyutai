#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>
#include <stm32f303_timer.h>
#include <stm32f303_usart.h>

/*USART*/
/*to sub mcu, USART2*/
#define USART2_SUB_TX		PB3
#define USART2_SUB_RX		PB4
#define USART2_SUB_BAUDRATE	115200

/*Motor*/
/*pinout*/
#define M1_1				PA1
#define M1_2				PA2
#define M2_1				PA3
#define M2_2				PA4
#define M3_1				PA5
#define M3_2				PA6
/*timer config*/
#define PWM_FREQ			25000
#define PWM_RESOLUTION		1023
/*PWM macro*/
#define M1_1_PWM(val)		(TIM2->CCR2 = (val))
#define M1_2_PWM(val)		(TIM2->CCR3 = (val))
#define M2_1_PWM(val)		(TIM2->CCR4 = (val))
#define M2_2_PWM(val)		(TIM3->CCR2 = (val))
#define M3_1_PWM(val)		(TIM2->CCR1 = (val))
#define M3_2_PWM(val)		(TIM3->CCR1 = (val))

/*LED*/
#define LED_B				PA11
#define LED_R				PA12

/*DIP SW*/
#define DIP1				PA7
#define DIP2				PB0
#define DIP3				PB1
#define DIP4				PA8

void GPIO_Init(void);
void Motor_Init(void);

USART SubSerial(USART2, USART2_SUB_RX, USART2_SUB_TX);

int main(void)
{
	RCC_Init();
	SysTick_Init();
	GPIO_Init();
	Motor_Init();

	SubSerial.init(USART2_SUB_BAUDRATE);

	while(1)
	{
		M1_1_PWM(0);
		for(uint16_t i = 0; i < 1024; i++) {
			M1_2_PWM(i);
			delay(5);
		}

		M1_1_PWM(1023);
		M1_2_PWM(1023);
		delay(2500);

		M1_2_PWM(0);
		for(uint16_t i = 0; i < 1024; i++) {
			M1_1_PWM(i);
			delay(5);
		}

		M1_1_PWM(1023);
		M1_2_PWM(1023);
		delay(2500);
	}

	return 0;
}

void Motor_Init(void)
{
	TIM_Init(TIM2, PWM_FREQ, PWM_RESOLUTION);
	TIM_Init(TIM3, PWM_FREQ, PWM_RESOLUTION);

	TIM_PWM_Init(TIM2, M1_1, 2);
	TIM_PWM_Init(TIM2, M1_2, 3);
	TIM_PWM_Init(TIM2, M2_1, 4);
	TIM_PWM_Init(TIM3, M2_2, 2);
	TIM_PWM_Init(TIM2, M3_1, 1);
	TIM_PWM_Init(TIM3, M3_2, 1);

	TIM_Start(TIM2);
	TIM_Start(TIM3);

	M1_1_PWM(0);
	M1_2_PWM(0);
	M2_1_PWM(0);
	M2_2_PWM(0);
	M3_1_PWM(0);
	M3_2_PWM(0);
}

void GPIO_Init(void)
{
	pinMode(LED_B, OUTPUT);		pinWrite(LED_B, HIGH);
	pinMode(LED_R, OUTPUT);		pinWrite(LED_R, HIGH);

	pinMode(DIP1, INPUT);		pinPullSelect(DIP1, PULLDOWN);
	pinMode(DIP2, INPUT);		pinPullSelect(DIP2, PULLDOWN);
	pinMode(DIP3, INPUT);		pinPullSelect(DIP3, PULLDOWN);
	pinMode(DIP4, INPUT);		pinPullSelect(DIP4, PULLDOWN);
}
