/* System clock bring-up: HSI -> PLL -> 180 MHz with over-drive mode. */
/* DWT cycle-counter helpers for benchmarking. */

#include <stdint.h>

#define RCC_BASE      0x40023800UL
#define RCC_CR        (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR   (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR      (*(volatile uint32_t *)(RCC_BASE + 0x08))
#define RCC_APB1ENR   (*(volatile uint32_t *)(RCC_BASE + 0x40))

#define PWR_BASE      0x40007000UL
#define PWR_CR        (*(volatile uint32_t *)(PWR_BASE + 0x00))
#define PWR_CSR       (*(volatile uint32_t *)(PWR_BASE + 0x04))

#define FLASH_BASE    0x40023C00UL
#define FLASH_ACR     (*(volatile uint32_t *)(FLASH_BASE + 0x00))

void _sysclk_180mhz(void)
{
    // 1. Enable Power Interface (PWR) Clock
    RCC_APB1ENR |= (1 << 28);

    // 2. Set Flash Latency immediately (5 wait states needed for 180 MHz)
    // Plus Prefetch, Instruction Cache, and Data Cache
    FLASH_ACR = 5 | (1 << 8) | (1 << 9) | (1 << 10);

    // 3. Configure Bus Prescalers (AHB = /1, APB1 = /4, APB2 = /2)
    RCC_CFGR = (RCC_CFGR & ~((0xF << 4) | (0x7 << 8) | (0x7 << 11)))
             | (0 << 4) | (5 << 8) | (4 << 11);

    // 4. Configure PLL Parameters (Source = HSI 16MHz)
    // M = 16 (1MHz input to PLL)
    // N = 360 (360MHz VCO)
    // P = 2 (360 / 2 = 180MHz SYSCLK)
    // Q = 7
    RCC_PLLCFGR = (16 << 0) | (360 << 6) | (0 << 16) | (7 << 24);

    // 5. Activate Over-Drive Mode
    PWR_CR |= (1 << 16);            // Set ODEN (Bit 16)
    while (!(PWR_CSR & (1 << 16))); // Wait for ODRDY (Bit 16)
    PWR_CR |= (1 << 17);            // Set ODSWEN (Bit 17)
    while (!(PWR_CSR & (1 << 17))); // Wait for ODSWRDY (Bit 17)

    // 6. Turn on Main PLL and wait for lock
    RCC_CR |= (1 << 24);           // Set PLLON
    while (!(RCC_CR & (1 << 25))); // Wait for PLLRDY

    // 7. Switch System Clock Source over to the stable PLL
    RCC_CFGR = (RCC_CFGR & ~(3 << 0)) | (2 << 0); // SW = 10b (PLL)
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));    // Wait for SWS to confirm PLL
}

#define DWT_CTRL   (*(volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT (*(volatile uint32_t *)0xE0001004)
#define DEMCR      (*(volatile uint32_t *)0xE000EDFC)

void _dwt_init(void)
{
    DEMCR |= 0x01000000;
    DWT_CYCCNT = 0;
    DWT_CTRL |= 1;
}

uint32_t _dwt_cyccnt(void)
{
    return DWT_CYCCNT;
}

void _dwt_zero(void)
{
    DWT_CYCCNT = 0;
}
