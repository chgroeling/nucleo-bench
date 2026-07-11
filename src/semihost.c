/* ARM semihosting stub (SYS_WRITE0). */

void _semihost_write0(const char *s)
{
    __asm volatile (
        "mov r0, #4\n\t"
        "mov r1, %[str]\n\t"
        "bkpt #0xAB"
        :
        : [str] "r" (s)
        : "r0", "r1", "memory"
    );
}

