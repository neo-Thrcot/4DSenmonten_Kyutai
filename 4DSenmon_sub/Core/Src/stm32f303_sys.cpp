#include <stm32f303_sys.h>

static volatile uint32_t preTick = 0;
static volatile uint32_t tickCarry = 0;

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

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	preTick = DWT->CYCCNT;

	SysTick->LOAD = (AHBCLK / 1000) - 1;
	SysTick->VAL = 0;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk |
				     SysTick_CTRL_TICKINT_Msk |
					 SysTick_CTRL_ENABLE_Msk;
}

void IncTick(void)
{
	if(DWT->CYCCNT < preTick) {
		tickCarry++;
	}
	preTick = DWT->CYCCNT;
}

uint64_t GetTick(void)
{
	uint32_t tick;
	uint32_t prev = preTick;
	uint32_t carry = tickCarry;

	tick = DWT->CYCCNT;
	if(tick < prev) {
		carry++;
	}

	return FULLTICK(carry, tick);
}

uint64_t millis(void)
{
	return GetTick() / 72000;
}

uint64_t micros(void)
{
	return GetTick() / 72;
}

void delay_ms(uint64_t ms)
{
	uint64_t start = millis();
	while((millis() - start) < ms);
}

void delay_us(uint64_t us)
{
	uint64_t start = micros();
	while((micros() - start) < us);
}
