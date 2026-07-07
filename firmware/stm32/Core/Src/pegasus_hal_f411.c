/*
 * PEGASUS real hardware adapter for the STM32F411CEU6 (WeAct Black Pill).
 *
 * Register-level, no STM32Cube HAL dependency. Overrides the weak stubs in
 * pegasus_hal.c. Build with PEGASUS_TARGET_F411 defined so only one adapter
 * is linked (see how pegasus_hal.c is guarded).
 *
 * Clock : HSE 25 MHz -> PLL -> 96 MHz (fallback HSI 16 MHz -> 96 MHz).
 * UART  : USART1 PA9(TX)/PA10(RX) AF7, 115200 8N1, RX by IRQ + ring buffer.
 * ADC   : ADC1 scan of 10 channels (PA0..PA7 = IN0..IN7, PB0=IN8, PB1=IN9)
 *         with circular DMA2 Stream0 into a shared buffer.
 * Motors: TB6612 PWMA=PB8 (TIM4_CH3), PWMB=PB9 (TIM4_CH4), AF2, ~19.2 kHz.
 *         Direction pins AIN1=PB10, AIN2=PB11, BIN1=PB12, BIN2=PB13,
 *         STBY=PB14 as plain GPIO outputs.
 * Fan   : FAN_PWM=PA8 (TIM1_CH1) AF1, ~19.2 kHz, duty 0..1000.
 * Time  : SysTick 1 kHz -> millis(); control tick flag every 2 ms (500 Hz).
 * WDG   : IWDG ~100 ms, refreshed from the control loop.
 */

#if defined(PEGASUS_TARGET_F411)

#include "pegasus_hal.h"
#include "board_pins.h"
#include "stm32f4xx.h"

#define HSE_HZ            25000000u
#define SYSCLK_HZ         96000000u
#define PWM_ARR           999u          /* 0..1000 duty resolution */
#define PWM_PSC           4u            /* 96MHz/(4+1)/1000 = 19.2 kHz */
#define UART_BAUD         115200u
#define CONTROL_TICK_MS   2u            /* 500 Hz control loop */

/* ---- Clock: HSE 25 MHz -> PLL -> 96 MHz (fallback HSI) ---------------- */

static void clock_init(void) {
    /* Voltage scale 1 for operation up to 100 MHz. */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;              /* VOS = 0b11 -> Scale 1 */

    /* Try HSE; fall back to HSI if the Black Pill crystal does not start. */
    uint32_t pll_m;
    RCC->CR |= RCC_CR_HSEON;
    uint32_t timeout = 0x8000u;
    while (!(RCC->CR & RCC_CR_HSERDY) && --timeout) { }
    const bool use_hse = (RCC->CR & RCC_CR_HSERDY) != 0u;
    if (use_hse) {
        pll_m = 25u;                   /* 25 MHz / 25 = 1 MHz VCO input */
    } else {
        RCC->CR |= RCC_CR_HSION;
        while (!(RCC->CR & RCC_CR_HSIRDY)) { }
        pll_m = 16u;                   /* 16 MHz / 16 = 1 MHz VCO input */
    }

    /* VCO = 1 MHz * 192 = 192 MHz; P=2 -> 96 MHz SYSCLK; Q=4 -> 48 MHz USB. */
    RCC->PLLCFGR = pll_m
                 | (192u << RCC_PLLCFGR_PLLN_Pos)
                 | (0u   << RCC_PLLCFGR_PLLP_Pos)     /* P = 2 */
                 | (4u   << RCC_PLLCFGR_PLLQ_Pos)
                 | (use_hse ? RCC_PLLCFGR_PLLSRC_HSE : 0u);

    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) { }

    /* Flash: 3 wait states at 96 MHz / 3.3 V, prefetch + caches on. */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN
               | FLASH_ACR_LATENCY_3WS;

    /* AHB /1 (96 MHz), APB1 /2 (48 MHz), APB2 /1 (96 MHz). */
    RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2))
              | RCC_CFGR_PPRE1_DIV2;

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { }

    SystemCoreClock = SYSCLK_HZ;
}

/* ---- GPIO ------------------------------------------------------------- */

/* MODER: 00 input, 01 output, 10 alternate, 11 analog. */
static void set_moder(GPIO_TypeDef *port, uint32_t pin, uint32_t mode) {
    port->MODER = (port->MODER & ~(3u << (pin * 2u))) | (mode << (pin * 2u));
}

static void set_af(GPIO_TypeDef *port, uint32_t pin, uint32_t af) {
    const uint32_t idx = pin >> 3u;           /* AFR[0] pins 0-7, AFR[1] pins 8-15 */
    const uint32_t shift = (pin & 7u) * 4u;
    port->AFR[idx] = (port->AFR[idx] & ~(0xFu << shift)) | (af << shift);
}

static void gpio_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    /* PA0..PA7 analog (ADC IN0..IN7). */
    for (uint32_t p = 0; p <= 7u; ++p) set_moder(GPIOA, p, 3u);
    /* PB0, PB1 analog (ADC IN8, IN9). */
    set_moder(GPIOB, 0u, 3u);
    set_moder(GPIOB, 1u, 3u);

    /* PA8 -> TIM1_CH1 (fan), AF1. */
    set_moder(GPIOA, 8u, 2u);  set_af(GPIOA, 8u, 1u);
    /* PA9 -> USART1_TX, PA10 -> USART1_RX, AF7. */
    set_moder(GPIOA, 9u, 2u);  set_af(GPIOA, 9u, 7u);
    set_moder(GPIOA, 10u, 2u); set_af(GPIOA, 10u, 7u);

    /* PB8 -> TIM4_CH3 (PWMA), PB9 -> TIM4_CH4 (PWMB), AF2. */
    set_moder(GPIOB, 8u, 2u);  set_af(GPIOB, 8u, 2u);
    set_moder(GPIOB, 9u, 2u);  set_af(GPIOB, 9u, 2u);

    /* PB10..PB14 push-pull outputs: AIN1, AIN2, BIN1, BIN2, STBY. */
    for (uint32_t p = 10u; p <= 14u; ++p) {
        set_moder(GPIOB, p, 1u);
        GPIOB->BSRR = (1u << (p + 16u));      /* start low (coast, driver off) */
    }
}

/* ---- USART1 (115200 8N1, RX IRQ) -------------------------------------- */

static void uart_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    /* PCLK2 = 96 MHz, OVER8=0: BRR = Fck / baud. */
    USART1->BRR = (uint16_t)(SYSCLK_HZ / UART_BAUD);
    USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE;
    USART1->CR1 |= USART_CR1_UE;
    NVIC_SetPriority(USART1_IRQn, 2u);
    NVIC_EnableIRQ(USART1_IRQn);
}

/* ---- Time base -------------------------------------------------------- */

static volatile uint32_t s_millis = 0;
static volatile uint32_t s_last_control_ms = 0;
static volatile bool s_control_due = false;

void SysTick_Handler(void) {
    s_millis++;
    if ((uint32_t)(s_millis - s_last_control_ms) >= CONTROL_TICK_MS) {
        s_last_control_ms = s_millis;
        s_control_due = true;
    }
}

uint32_t pegasus_hal_millis(void) {
    return s_millis;
}

bool pegasus_hal_control_tick_due(void) {
    if (s_control_due) {
        s_control_due = false;
        return true;
    }
    return false;
}

/* ---- UART ring buffer ------------------------------------------------- */

#define RX_RING_SIZE 256u  /* power of two */
static volatile uint8_t s_rx_ring[RX_RING_SIZE];
static volatile uint16_t s_rx_head = 0;
static volatile uint16_t s_rx_tail = 0;

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) {
        const uint8_t byte = (uint8_t)USART1->DR;  /* reading DR clears RXNE */
        const uint16_t next = (uint16_t)((s_rx_head + 1u) & (RX_RING_SIZE - 1u));
        if (next != s_rx_tail) {          /* drop byte on overflow, never block */
            s_rx_ring[s_rx_head] = byte;
            s_rx_head = next;
        }
    }
    if (USART1->SR & USART_SR_ORE) {
        (void)USART1->DR;                 /* clear overrun */
    }
}

bool pegasus_hal_uart_read_byte(uint8_t *byte) {
    if (s_rx_tail == s_rx_head) {
        return false;
    }
    *byte = s_rx_ring[s_rx_tail];
    s_rx_tail = (uint16_t)((s_rx_tail + 1u) & (RX_RING_SIZE - 1u));
    return true;
}

void pegasus_hal_uart_write(const uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; ++i) {
        while (!(USART1->SR & USART_SR_TXE)) { }
        USART1->DR = data[i];
    }
}

/* ---- ADC snapshot ----------------------------------------------------- */

#define ADC_CHANNELS 10u
static volatile uint16_t s_adc_dma[ADC_CHANNELS];

void pegasus_hal_get_adc_snapshot(uint16_t *values, uint8_t count) {
    for (uint8_t i = 0; i < count && i < ADC_CHANNELS; ++i) {
        values[i] = s_adc_dma[i];
    }
    for (uint8_t i = ADC_CHANNELS; i < count; ++i) {
        values[i] = 0;
    }
}

/* ---- ADC1 + DMA2 setup ------------------------------------------------ */

static void adc_set_sq(uint8_t rank, uint8_t channel) {
    /* rank 1..10 -> SQR3 (1..6), SQR2 (7..12). 5 bits per slot. */
    if (rank <= 6u) {
        ADC1->SQR3 |= ((uint32_t)channel & 0x1Fu) << ((rank - 1u) * 5u);
    } else {
        ADC1->SQR2 |= ((uint32_t)channel & 0x1Fu) << ((rank - 7u) * 5u);
    }
}

static void adc_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    /* ADC clock = PCLK2 / 4 = 24 MHz (<= 36 MHz limit). */
    ADC->CCR = (ADC->CCR & ~ADC_CCR_ADCPRE) | (1u << ADC_CCR_ADCPRE_Pos);

    /* Long sample time (0b110 = 144 cycles) on all channels for high-Z IR. */
    ADC1->SMPR2 = 0x36DB6DB6u & 0x3FFFFFFFu; /* IN0..IN9 ~ 0b110 each */
    ADC1->SMPR1 = 0x00000036u;               /* IN8, IN9 */

    /* Sequence: rank r -> channel (r-1), i.e. IN0..IN9 in order. */
    ADC1->SQR1 = ((ADC_CHANNELS - 1u) << ADC_SQR1_L_Pos);
    ADC1->SQR2 = 0;
    ADC1->SQR3 = 0;
    for (uint8_t r = 1u; r <= ADC_CHANNELS; ++r) {
        adc_set_sq(r, (uint8_t)(r - 1u));
    }

    ADC1->CR1 = ADC_CR1_SCAN;
    ADC1->CR2 = ADC_CR2_DMA | ADC_CR2_DDS | ADC_CR2_CONT;

    /* DMA2 Stream0 Channel0 = ADC1: periph->mem, 16-bit, circular, MINC. */
    DMA2_Stream0->CR = 0;
    while (DMA2_Stream0->CR & DMA_SxCR_EN) { }
    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)s_adc_dma;
    DMA2_Stream0->NDTR = ADC_CHANNELS;
    DMA2_Stream0->CR = (0u << DMA_SxCR_CHSEL_Pos)
                     | DMA_SxCR_MINC
                     | (1u << DMA_SxCR_PSIZE_Pos)   /* 16-bit periph */
                     | (1u << DMA_SxCR_MSIZE_Pos)   /* 16-bit mem */
                     | DMA_SxCR_CIRC;
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->CR2 |= ADC_CR2_SWSTART;    /* free-running scan into DMA buffer */
}

/* ---- Timers: TIM4 (motors), TIM1 (fan) -------------------------------- */

static void timers_init(void) {
    /* TIM4 CH3/CH4 PWM for motors. */
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->PSC = PWM_PSC;
    TIM4->ARR = PWM_ARR;
    TIM4->CCMR2 = (6u << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE
                | (6u << TIM_CCMR2_OC4M_Pos) | TIM_CCMR2_OC4PE;
    TIM4->CCER = TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM4->CCR3 = 0;
    TIM4->CCR4 = 0;
    TIM4->CR1 = TIM_CR1_ARPE;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->CR1 |= TIM_CR1_CEN;

    /* TIM1 CH1 PWM for fan (advanced timer needs MOE). */
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->PSC = PWM_PSC;
    TIM1->ARR = PWM_ARR;
    TIM1->CCMR1 = (6u << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
    TIM1->CCER = TIM_CCER_CC1E;
    TIM1->CCR1 = 0;
    TIM1->BDTR = TIM_BDTR_MOE;
    TIM1->CR1 = TIM_CR1_ARPE;
    TIM1->EGR = TIM_EGR_UG;
    TIM1->CR1 |= TIM_CR1_CEN;
}

/* ---- IWDG (~100 ms) --------------------------------------------------- */

static void iwdg_init(void) {
    IWDG->KR = 0x5555u;              /* enable write access to PR/RLR */
    IWDG->PR = 3u;                   /* LSI/32 ~= 1 kHz */
    IWDG->RLR = 100u;                /* 100 counts ~= 100 ms */
    IWDG->KR = 0xCCCCu;              /* start watchdog */
}

/* ---- Public init ------------------------------------------------------ */

void pegasus_hal_init(void) {
    clock_init();
    gpio_init();
    uart_init();
    adc_init();
    timers_init();

    /* SysTick at 1 kHz. */
    SysTick_Config(SYSCLK_HZ / 1000u);
    NVIC_SetPriority(SysTick_IRQn, 0u);   /* time base is top priority */

    iwdg_init();                          /* last: loop must now feed it */
}

/* ---- Motors (TB6612) -------------------------------------------------- */
/*
 * Direction truth table per channel (datasheet TB6612FNG):
 *   IN1=1 IN2=0 -> forward (CW),  IN1=0 IN2=1 -> reverse,
 *   IN1=1 IN2=1 -> short brake,   IN1=0 IN2=0 -> coast/stop.
 * PWM duty is applied on PWMx; sign of the argument selects direction.
 */

#define AIN1_PIN 10u
#define AIN2_PIN 11u
#define BIN1_PIN 12u
#define BIN2_PIN 13u
#define STBY_PIN 14u

static uint16_t clamp_duty(int16_t pwm) {
    int32_t mag = pwm < 0 ? -(int32_t)pwm : (int32_t)pwm;
    if (mag > (int32_t)PWM_ARR + 1) mag = (int32_t)PWM_ARR + 1;
    return (uint16_t)mag;
}

static void set_pin(uint32_t pin, bool high) {
    GPIOB->BSRR = high ? (1u << pin) : (1u << (pin + 16u));
}

void pegasus_hal_motor_left(int16_t pwm) {
    set_pin(AIN1_PIN, pwm > 0);
    set_pin(AIN2_PIN, pwm < 0);
    TIM4->CCR3 = clamp_duty(pwm);   /* PB8 = TIM4_CH3 = PWMA (left) */
}

void pegasus_hal_motor_right(int16_t pwm) {
    set_pin(BIN1_PIN, pwm > 0);
    set_pin(BIN2_PIN, pwm < 0);
    TIM4->CCR4 = clamp_duty(pwm);   /* PB9 = TIM4_CH4 = PWMB (right) */
}

void pegasus_hal_motor_brake(void) {
    set_pin(AIN1_PIN, true);  set_pin(AIN2_PIN, true);
    set_pin(BIN1_PIN, true);  set_pin(BIN2_PIN, true);
    TIM4->CCR3 = 0;
    TIM4->CCR4 = 0;
}

void pegasus_hal_motor_coast(void) {
    set_pin(AIN1_PIN, false); set_pin(AIN2_PIN, false);
    set_pin(BIN1_PIN, false); set_pin(BIN2_PIN, false);
    TIM4->CCR3 = 0;
    TIM4->CCR4 = 0;
}

void pegasus_hal_tb6612_stby(bool enable) {
    /* STBY high = driver active. Firmware calls with true to enable. */
    set_pin(STBY_PIN, enable);
}

/* ---- Fan (PA8 = TIM1_CH1) --------------------------------------------- */

void pegasus_hal_fan_pwm(uint16_t pwm) {
    if (pwm > (uint16_t)PWM_ARR + 1u) pwm = (uint16_t)PWM_ARR + 1u;
    TIM1->CCR1 = pwm;
}

/* ---- Watchdog (IWDG ~100 ms) ------------------------------------------ */

void pegasus_hal_watchdog_refresh(void) {
    IWDG->KR = 0xAAAAu;   /* reload counter */
}

#endif /* PEGASUS_TARGET_F411 */
