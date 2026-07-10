    .syntax unified
    .cpu cortex-m4
    .thumb
    .section .text._semihost_write_asm,"ax",%progbits

    .global _semihost_write_asm
    .type _semihost_write_asm, %function

@ void _semihost_write_asm(const char *buf, unsigned int len)
@ r0 = buf, r1 = len
_semihost_write_asm:
    push {r4, r5, lr}
    sub sp, sp, #12          @ allocate 3 words on stack for the write block

    mov r2, #1               @ block[0] = handle (1 = stdout)
    str r2, [sp, #0]
    str r0, [sp, #4]         @ block[1] = buffer pointer
    str r1, [sp, #8]         @ block[2] = length

    mov r0, #5               @ SYS_WRITE
    mov r1, sp               @ pointer to write block
    bkpt #0xAB               @ semihosting call

    add sp, sp, #12          @ free stack
    pop {r4, r5, pc}

    .global _semihost_exit_asm
    .type _semihost_exit_asm, %function

@ void _semihost_exit_asm(int code)
@ r0 = exit code
_semihost_exit_asm:
    push {r4, r5, lr}
    sub sp, sp, #8           @ allocate 2 words on stack for the exit block

    ldr r2, =0x20026         @ block[0] = ADP_Stopped_ApplicationExit
    str r2, [sp, #0]
    str r0, [sp, #4]         @ block[1] = exit code

    mov r0, #0x18            @ SYS_EXIT
    mov r1, sp               @ pointer to exit block
    bkpt #0xAB               @ semihosting call

    add sp, sp, #8           @ free stack
    pop {r4, r5, pc}

    .ltorg
