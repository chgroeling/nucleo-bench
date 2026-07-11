#include <stdint.h>

void _semihost_write_asm(const char *buf, unsigned int len)
{
    uintptr_t block[3];
    block[0] = 1;
    block[1] = (uintptr_t)buf;
    block[2] = len;

    __asm volatile (
        "mov r0, #5\n\t"
        "mov r1, %[blk]\n\t"
        "bkpt #0xAB"
        :
        : [blk] "r" (block)
        : "r0", "r1", "memory"
    );
}

unsigned int _semihost_clock(void)
{
    unsigned int cs;
    __asm volatile (
        "mov r0, #0x10\n\t"
        "bkpt #0xAB\n\t"
        "mov %0, r0"
        : "=r" (cs)
        :
        : "r0", "memory"
    );
    return cs;
}

#define DWT_CTRL   (*(volatile unsigned int *)0xE0001000)
#define DWT_CYCCNT (*(volatile unsigned int *)0xE0001004)
#define DEMCR      (*(volatile unsigned int *)0xE000EDFC)

void _dwt_init(void)
{
    DEMCR |= 0x01000000;
    DWT_CYCCNT = 0;
    DWT_CTRL |= 1;
}

unsigned int _dwt_cyccnt(void)
{
    return DWT_CYCCNT;
}

