/*
 * Minimal CMSIS system file for PEGASUS (STM32F411CEU6).
 * Clock tree is configured later by pegasus_hal_f411.c; here we only
 * guarantee a sane reset state and enable the FPU.
 */

#include "stm32f4xx.h"

/* Updated by pegasus_hal_f411.c after the PLL is locked. */
uint32_t SystemCoreClock = 16000000u; /* HSI after reset */

void SystemInit(void)
{
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    /* Full access to CP10/CP11 (FPU) before any float instruction runs. */
    SCB->CPACR |= ((3UL << 20u) | (3UL << 22u));
#endif

    /* Reset RCC to HSI defaults so a soft reset always boots the same way. */
    RCC->CR |= RCC_CR_HSION;
    RCC->CFGR = 0x00000000u;
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_CSSON | RCC_CR_PLLON);
    RCC->PLLCFGR = 0x24003010u;
    RCC->CR &= ~RCC_CR_HSEBYP;
    RCC->CIR = 0x00000000u;

    /* Vector table at start of flash. */
    SCB->VTOR = FLASH_BASE;
}

void SystemCoreClockUpdate(void)
{
    /* PEGASUS runs a fixed 96 MHz configuration; see pegasus_hal_f411.c. */
}
