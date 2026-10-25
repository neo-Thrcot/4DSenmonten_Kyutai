#include <stm32f303_sys.h>
#include <stm32f303_gpio.h>
#include <stm32f303_i2c.h>
#include <stm32f303_usart.h>
#include <stm32f303_timer.h>

#include <stdio.h>

/*I2C*/
/*peripheral*/
#define I2C1_SCL				PB6
#define I2C1_SDA				PB7
#define I2C1_CLOCK				100000
/*6axis*/
#define LSM6_ADDR			0x6A
#define REG_WHO_AM_I 		0x0F
#define REG_CTRL1			0x10
#define REG_CTRL2 			0x11
#define REG_CTRL6 			0x15
#define REG_CTRL8		 	0x17
#define REG_OUTX_L_G		0x22

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

SysError_t writeReg(uint8_t reg, uint8_t val);
SysError_t readRegs(uint8_t reg, uint8_t* databuf, uint8_t datasize);

bool lsm6_init(void);
SysError_t readSensor(float* gx, float* gy, float* gz,
					  float* ax, float* ay, float* az);

/*Callback function*/
void USART2_Receive_CmpltCallback(void);

I2C i2c1(I2C1, I2C1_SCL, I2C1_SDA);

USART MainSerial(USART2, USART2_MAIN_RX, USART2_MAIN_TX);
static bool is_main_received = false;

volatile uint8_t dip_state = 0;

int main(void)
{
	uint8_t id_cnt = 0;

	RCC_Init();
	GPIO_Init();
	Motor_Init();

	Serial.init(115200);

	MainSerial.init(USART2_MAIN_BAUDRATE);
	MainSerial.setCallback(USART_RX_COMPLETE, USART2_Receive_CmpltCallback);

	i2c1.init(100000);

	for(uint8_t id = 0x00; id <= 0x7F; id++) {
		if(i2c1.addressMatch(id) == true) {
			id_cnt++;
			printf("ID:0x%2x\n", id);
		}
	}
	if(id_cnt == 0) {
		printf("Not device\n");
	}

	if (!lsm6_init()) {
		printf("LSM6DSV16X init error!\n");
	    while (1);
	}
	printf("LSM6DSV16X initialized.\n");

	uint8_t ctrl2 = 2;

	readRegs(REG_CTRL2, &ctrl2, 1);

	printf("CTRL2 = 0x%02X\n", ctrl2);

	while(1)
	{
		float ax, ay, az, gx, gy, gz;
		SysError_t err;

		err = readSensor(&gx, &gy, &gz, &ax, &ay, &az);

		printf("err:%d ", (int)err);
		printf("ax:%3.3f ay:%3.3f az:%3.3f gx:%3.3f gy:%3.3f gz:%3.3f\n", ax, ay, az, gx, gy, gz);

		delay_ms(10);
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

SysError_t writeReg(uint8_t reg, uint8_t val)
{
	return i2c1.memWrite(LSM6_ADDR, reg, false, &val, 1, 500);
}

SysError_t readRegs(uint8_t reg, uint8_t* databuf, uint8_t datasize)
{
	return i2c1.memRead(LSM6_ADDR, reg, false, databuf, datasize, 500);
}

bool lsm6_init(void)
{
  uint8_t who;
  SysError_t err;

  readRegs(REG_WHO_AM_I, &who, 1);
  printf("WHO_AM_I = 0x%x\n", who);
  if (who != 0x70) {
	  return false;
  }

//  err = writeReg(REG_CTRL1, 0x09);
//  printf("CTRL1 write: %d\n", (int)err);
//
//  err = writeReg(REG_CTRL2, 0x09);
//  printf("CTRL2 write: %d\n", (int)err);
//
//  err = writeReg(REG_CTRL6, 0x04);
//  printf("CTRL6 write: %d\n", (int)err);
//
//  err = writeReg(REG_CTRL8, 0x82);
//  printf("CTRL8 write: %d\n", (int)err);

  writeReg(REG_CTRL1, 0b00001001);		delay_ms(1);
  writeReg(REG_CTRL2, 0b00001001);		delay_ms(1);
  writeReg(REG_CTRL6, 0b00000100);		delay_ms(1);
  writeReg(REG_CTRL8, 0b10000010);		delay_ms(1);

  return true;
}

SysError_t readSensor(float* gx, float* gy, float* gz,
					  float* ax, float* ay, float* az)
{
  uint8_t rawdata[12];
  int16_t convdata[6];
  SysError_t err;

  err = readRegs(REG_OUTX_L_G, rawdata, 12);
  for(uint8_t i = 0; i < 6; i++) {
	  convdata[i] = (rawdata[i * 2 + 1] << 8 | rawdata[i * 2]);
  }

  *gx = (float)convdata[0] * 0.07f;
  *gy = (float)convdata[1] * 0.07f;
  *gz = (float)convdata[2] * 0.07f;
  *ax = (float)convdata[3] * 0.000244f;
  *ay = (float)convdata[4] * 0.000244f;
  *az = (float)convdata[5] * 0.000244f;

  return err;
}

void USART2_Receive_CmpltCallback(void)
{
	is_main_received = true;
}
