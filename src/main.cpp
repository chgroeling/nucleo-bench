/* Application entry: semihosting output helpers and DWT-based benchmark loop. */

#include <stdint.h>
#include "algo_nop.hpp"

extern "C" {
void _semihost_write_asm(const char *buf, uint32_t len);
void _dwt_init(void);
void _dwt_zero(void);
uint32_t _dwt_cyccnt(void);
}

static void _semihost_write_uint(uint32_t val)
{
    char buf[11];
    char *end = buf + sizeof(buf);
    char *p = end;
    *--p = '\0';
    do {
        *--p = '0' + (val % 10);
        val /= 10;
    } while (val > 0);
    _semihost_write_asm(p, (uint32_t)(end - p - 1));
}

#define CPU_FREQ 180000000

static void _semihost_write_avg_ns(uint64_t total_cycles, uint32_t runs)
{
    uint64_t divisor = (uint64_t)runs * CPU_FREQ;
    uint64_t ns = (total_cycles * 1000000000ULL + divisor / 2) / divisor;
    _semihost_write_uint((uint32_t)ns);
    _semihost_write_asm(" ns", 3);
}

static void _semihost_write_seconds(uint64_t cycles)
{
    uint64_t sec = cycles / CPU_FREQ;
    uint64_t frac = cycles % CPU_FREQ;
    uint32_t ms = (uint32_t)((frac * 1000 + CPU_FREQ / 2) / CPU_FREQ);

    _semihost_write_uint((uint32_t)sec);
    _semihost_write_asm(".", 1);

    if (ms < 100) _semihost_write_asm("0", 1);
    if (ms < 10)  _semihost_write_asm("0", 1);
    _semihost_write_uint(ms);
    _semihost_write_asm(" s", 2);
}

static class AlgoRunner {
    uint32_t m_runs;
    void algo(void)
    {
        algo_nop();
    }
public:
    AlgoRunner(uint32_t runs) : m_runs(runs) {}
    uint32_t runs(void) const { return m_runs; }
    void run(void)
    {
        for (uint32_t r = 0; r < m_runs; r++) {
            algo();
        }
    }
} g_runner(3000000);

int main(void)
{
    uint64_t t0, t1;

    _semihost_write_asm("--- start ---\n", 14);
    _semihost_write_asm("wrap ", 5);
    _semihost_write_seconds(0x100000000ULL);
    _semihost_write_asm("\nruns ", 6);
    _semihost_write_uint(g_runner.runs());
    _semihost_write_asm("\n", 1);

    _dwt_init();

    _dwt_zero();
    t0 = 0;

    g_runner.run();

    t1 = _dwt_cyccnt();

    _semihost_write_asm("dt = ", 5);
    _semihost_write_seconds(t1 - t0);
    _semihost_write_asm("  avg = ", 8);
    _semihost_write_avg_ns(t1 - t0, g_runner.runs());
    _semihost_write_asm("\n", 1);

    return 0;
}
