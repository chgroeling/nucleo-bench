/* Application entry: semihosting output helpers and DWT-based benchmark loop. */

#include <stdint.h>
#include "algo_nop.hpp"
#include "algo_sprintf.hpp"
#include "compiler.hpp"

static constexpr uint64_t kCpuFreq{180000000ULL};

/* Per-algorithm repetition count — keep kBenchRuns × per-run time under the
   DWT wrap limit (2^32 cycles ≈ 23.8 s at 180 MHz). sprintf is orders of
   magnitude slower than a nop, so it runs 10× fewer repetitions. */
#if defined(USE_ALGO_SPRINTF)
static constexpr uint32_t kBenchRuns{100000U};
#else
static constexpr uint32_t kBenchRuns{3000000U};
#endif

extern "C" {
void _semihost_write0(const char *s);
void _dwt_init(void);
void _dwt_zero(void);
uint32_t _dwt_cyccnt(void);
}


/* Software 64-bit unsigned divide + modulo.

   The Cortex-M4 has a 32-bit hardware divider (UDIV) but no 64-bit one, so a
   plain `uint64_t / uint64_t` makes GCC emit a call to libgcc's
   __aeabi_uldivmod. This bare-metal image otherwise links no support library
   at all; implementing the divide here keeps libgcc out too.

   Classic shift-subtract long division. Every shift is by a *constant* (1 or
   63), so GCC inlines them — a variable 64-bit shift would itself pull in
   __aeabi_llsl / __aeabi_llsr helpers, defeating the purpose. */
static uint64_t udivmod64(uint64_t num, uint64_t den, uint64_t &rem)
{
    uint64_t q{0};
    uint64_t r{0};
    for (uint32_t i{0U}; i < 64U; i++) {
        r = (r << 1) | (num >> 63);
        num <<= 1;
        q <<= 1;
        if (r >= den) {
            r -= den;
            q |= 1U;
        }
    }
    rem = r;
    return q;
}


/* Print unsigned integer via semihosting. */
static void _semihost_write_uint(uint32_t val)
{
    char buf[11]{};
    char *end{buf + sizeof(buf)};
    char *p{end};
    *--p = '\0';
    do {
        *--p = '0' + (val % 10);
        val /= 10;
    } while (val > 0U);
    _semihost_write0(p);
}

/* Print average nanoseconds per run from total cycle count and run count. */
static void _semihost_write_avg_ns(uint64_t total_cycles, uint32_t runs)
{
    uint64_t divisor{(uint64_t)runs * kCpuFreq};
    uint64_t rem{};
    uint64_t ns{udivmod64(total_cycles * 1000000000ULL + divisor / 2U, divisor, rem)};
    _semihost_write_uint((uint32_t)ns);
    _semihost_write0(" ns");
}

/* Print seconds with 3-digit millisecond precision via semihosting. */
static void _semihost_write_seconds(uint64_t cycles)
{
    uint64_t frac{};
    uint64_t sec{udivmod64(cycles, kCpuFreq, frac)};
    uint64_t ms_rem{};
    uint32_t ms{(uint32_t)udivmod64(frac * 1000U + kCpuFreq / 2U, kCpuFreq, ms_rem)};

    _semihost_write_uint((uint32_t)sec);
    _semihost_write0(".");

    if (ms < 100U) _semihost_write0("0");
    if (ms < 10U)  _semihost_write0("0");
    _semihost_write_uint(ms);
    _semihost_write0(" s");
}

/* Benchmark runner — calls algo() N times in a tight loop. */
static class AlgoRunner {
    uint32_t m_runs{0U};
    void algo(void)
    {
#if defined(USE_ALGO_NOP)
        algo_nop();
#elif defined(USE_ALGO_SPRINTF)
        algo_sprintf();
#else
        /* No algorithm selected (ALGO=none): compiler_barrier() is a zero-instruction
           compiler barrier (see compiler.hpp) so -O3 cannot prove the loop body
           empty and delete the whole `for` loop. It adds no cycles, so the
           measured time reflects the pure loop overhead alone.

           For a real algorithm, call it here and guard its result with
           do_not_optimize() / clobber_memory() so -O3 cannot discard it. */
        compiler_barrier();
#endif
    }
public:
    AlgoRunner(uint32_t runs) : m_runs{runs} {}
    uint32_t runs(void) const { return m_runs; }
    void run(void)
    {
        for (uint32_t r{0U}; r < m_runs; r++) {
            algo();
        }
    }
} g_runner{kBenchRuns};

int main(void)
{
    uint64_t t0{}, t1{};

    _semihost_write0("--- start ---\n");
    _semihost_write0("wrap ");
    _semihost_write_seconds(0x100000000ULL);
    _semihost_write0("\nruns ");
    _semihost_write_uint(g_runner.runs());
    _semihost_write0("\n");

    _dwt_init();
    _dwt_zero();

    g_runner.run();

    t1 = _dwt_cyccnt();

    _semihost_write0("dt = ");
    _semihost_write_seconds(t1 - t0);
    _semihost_write0("  avg = ");
    _semihost_write_avg_ns(t1 - t0, g_runner.runs());
    _semihost_write0("\n");

    return 0;
}
