extern void _semihost_write_asm(const char *buf, unsigned int len);
extern void _dwt_init(void);
extern unsigned int _dwt_cyccnt(void);

static void _semihost_write_uint(unsigned int val)
{
    char buf[11];
    char *end = buf + sizeof(buf);
    char *p = end;
    *--p = '\0';
    do {
        *--p = '0' + (val % 10);
        val /= 10;
    } while (val > 0);
    _semihost_write_asm(p, (unsigned int)(end - p - 1));
}

#define CPU_FREQ 180000000

static void _semihost_write_seconds(unsigned int cycles)
{
    unsigned int sec = cycles / CPU_FREQ;
    unsigned int frac = cycles % CPU_FREQ;
    unsigned int ms = (frac * 1000 + CPU_FREQ / 2) / CPU_FREQ;

    _semihost_write_uint(sec);
    _semihost_write_asm(".", 1);

    if (ms < 100) _semihost_write_asm("0", 1);
    if (ms < 10)  _semihost_write_asm("0", 1);
    _semihost_write_uint(ms);
    _semihost_write_asm(" s", 2);
}

static void algo_under_test(void)
{
    /* TODO: replace with actual algorithm — this is test code */
    for (int i = 0; i < 200000000; i++) {
        __asm volatile ("nop");
    }
}

int main(void)
{
    unsigned int t0, t1;

    _semihost_write_asm("--- start ---\n", 14);

    _dwt_init();
    t0 = _dwt_cyccnt();

    algo_under_test();

    t1 = _dwt_cyccnt();

    _semihost_write_asm("dt = ", 5);
    _semihost_write_seconds(t1 - t0);
    _semihost_write_asm("\n", 1);

    return 0;
}
