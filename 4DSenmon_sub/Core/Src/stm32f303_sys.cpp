#include <stm32f303_sys.h>

static volatile uint32_t msTick = 0;

void RCC_Init(void)
{
	/*PLL config*/
	RCC->CFGR &= ~(RCC_CFGR_PPRE1_Msk | RCC_CFGR_PLLSRC_Msk | RCC_CFGR_PLLMUL_Msk);
	RCC->CFGR |= (RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL9);

	/*Flash latency*/
	FLASH->ACR &= ~FLASH_ACR_LATENCY_Msk;
	FLASH->ACR |= FLASH_ACR_LATENCY_1;

	/*PLL enable & sysclock to PLL*/
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL);

	/*System timer config*/
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->CR1 &= (~(1 << 0));
}

void SysTick_Init(void)
{
	SysTick->LOAD = (AHBCLK / 1000) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void IncTick(void)
{
	msTick++;
}

uint32_t GetTick(void)
{
	return msTick;
}

void delay(uint32_t ms)
{
	uint32_t start = msTick;
	while((msTick - start) < ms);
}
