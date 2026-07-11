#include <stdint.h>

#define RCC_BASE      0x40023800UL
#define RCC_CR        (*(volatile uint32_t *)(RCC_BASE + 0x00))
#define RCC_PLLCFGR   (*(volatile uint32_t *)(RCC_BASE + 0x04))
#define RCC_CFGR      (*(volatile uint32_t *)(RCC_BASE + 0x08))

#define PWR_BASE      0x40007000UL
#define PWR_CR        (*(volatile uint32_t *)(PWR_BASE + 0x00))
#define PWR_CSR       (*(volatile uint32_t *)(PWR_BASE + 0x04))

#define FLASH_BASE    0x40023C00UL
#define FLASH_ACR     (*(volatile uint32_t *)(FLASH_BASE + 0x00))

void _sysclk_180mhz(void)
{
    while (!(RCC_CR & (1 << 1)));

    FLASH_ACR = 2 | (1 << 8) | (1 << 9) | (1 << 10);

    RCC_CFGR = (RCC_CFGR & ~(0x7 << 8 | 0x7 << 11))
             | (4 << 8) | (0 << 11);

    RCC_PLLCFGR = (8 << 0) | (84 << 6) | (0 << 16) | (7 << 24);

    RCC_CR |= (1 << 24);
    while (!(RCC_CR & (1 << 25)));

    RCC_CFGR = (RCC_CFGR & ~(3 << 0)) | (2 << 0);
    while ((RCC_CFGR & (3 << 2)) != (2 << 2));
}
