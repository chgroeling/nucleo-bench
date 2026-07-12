/* Vector table, Reset_Handler, data/bss init, weak ISR aliases. */

#include <stddef.h>

extern unsigned int _estack;
extern int main(void);
extern void _sysclk_180mhz(void);

/* Hand-rolled copy / fill for startup.

   These are deliberately NOT named memcpy / memset. At -O3 GCC's loop-idiom
   pass rewrites the .data copy and .bss zero loops in Reset_Handler into calls
   to memcpy / memset, which the linker then satisfies from newlib-nano (libc),
   dragging C-library objects into an otherwise libc-free image. By giving our
   own routines private names and calling them explicitly, startup pulls in no
   libc at all. The standard memcpy / memset symbols are only ever linked (from
   libc) if the user's own algorithm actually references them — so the C library
   stays out unless you opt into it.

   The `no-tree-loop-distribute-patterns` attribute is essential: without it,
   GCC would recognize the byte loop inside each function as a memcpy / memset
   idiom and "optimize" it into a call to itself — infinite recursion. */
__attribute__((optimize("no-tree-loop-distribute-patterns")))
static void _memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n--) {
        *d++ = *s++;
    }
}

__attribute__((optimize("no-tree-loop-distribute-patterns")))
static void _memset(void *dst, int c, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    while (n--) {
        *d++ = (unsigned char)c;
    }
}

typedef void (*init_fn)(void);
extern init_fn __preinit_array_start;
extern init_fn __preinit_array_end;
extern init_fn __init_array_start;
extern init_fn __init_array_end;

void Reset_Handler(void);

void Default_Handler(void)
{
    while (1) {}
}

static void _exit_breakpoint(void)
{
    __asm volatile ("bkpt #0");
}

void NMI_Handler(void)                    __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)              __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)              __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)               __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void)             __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)                    __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)               __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)                 __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)                __attribute__((weak, alias("Default_Handler")));

void WWDG_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));
void Reserved1_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved2_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved3_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved4_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void)  __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void Reserved5_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler(void)         __attribute__((weak, alias("Default_Handler")));
void Reserved6_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved7_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved8_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved9_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void Reserved10_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved11_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved12_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler(void)        __attribute__((weak, alias("Default_Handler")));
void Reserved13_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void Reserved14_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved15_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved16_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved17_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved18_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved19_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved20_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved21_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved22_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void Reserved23_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void)                 __attribute__((weak, alias("Default_Handler")));

__attribute__((section(".isr_vector"), used))
void (* const isr_vector[98])(void) = {
    (void (*)(void))&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0,
    0,
    0,
    0,
    SVC_Handler,
    DebugMon_Handler,
    0,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_IRQHandler,
    TAMP_STAMP_IRQHandler,
    RTC_WKUP_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Stream0_IRQHandler,
    DMA1_Stream1_IRQHandler,
    DMA1_Stream2_IRQHandler,
    DMA1_Stream3_IRQHandler,
    DMA1_Stream4_IRQHandler,
    DMA1_Stream5_IRQHandler,
    DMA1_Stream6_IRQHandler,
    ADC_IRQHandler,
    Reserved1_IRQHandler,
    Reserved2_IRQHandler,
    Reserved3_IRQHandler,
    Reserved4_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_TIM9_IRQHandler,
    TIM1_UP_TIM10_IRQHandler,
    TIM1_TRG_COM_TIM11_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    TIM3_IRQHandler,
    TIM4_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C2_EV_IRQHandler,
    I2C2_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    USART2_IRQHandler,
    Reserved5_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    OTG_FS_WKUP_IRQHandler,
    Reserved6_IRQHandler,
    Reserved7_IRQHandler,
    Reserved8_IRQHandler,
    Reserved9_IRQHandler,
    Reserved10_IRQHandler,
    Reserved11_IRQHandler,
    Reserved12_IRQHandler,
    DMA1_Stream7_IRQHandler,
    Reserved13_IRQHandler,
    SDIO_IRQHandler,
    TIM5_IRQHandler,
    SPI3_IRQHandler,
    Reserved14_IRQHandler,
    Reserved15_IRQHandler,
    Reserved16_IRQHandler,
    Reserved17_IRQHandler,
    Reserved18_IRQHandler,
    Reserved19_IRQHandler,
    Reserved20_IRQHandler,
    Reserved21_IRQHandler,
    Reserved22_IRQHandler,
    Reserved23_IRQHandler,
    FPU_IRQHandler,
};

void Reset_Handler(void)
{
    extern unsigned int _sidata, _sdata, _edata;
    extern unsigned int _sbss, _ebss;

    _memcpy(&_sdata, &_sidata, (size_t)((char *)&_edata - (char *)&_sdata));
    _memset(&_sbss, 0, (size_t)((char *)&_ebss - (char *)&_sbss));

    _sysclk_180mhz();

    {
        init_fn *p;
        for (p = &__preinit_array_start; p < &__preinit_array_end; p++)
            (*p)();
        for (p = &__init_array_start; p < &__init_array_end; p++)
            (*p)();
    }

    main();

    _exit_breakpoint();
    while (1) {}
}
