/* ARM semihosting stubs (SYS_WRITE). */

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

