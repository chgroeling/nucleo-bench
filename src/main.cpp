/* Application entry: semihosting output helpers and DWT-based benchmark loop. */

#include <stdint.h>
#include "algo_nop.hpp"
#include "compiler.hpp"

static constexpr uint64_t kCpuFreq{180000000ULL};
static constexpr uint32_t kBenchRuns{3000000U};

extern "C" {
void _semihost_write0(const char *s);
void _dwt_init(void);
void _dwt_zero(void);
uint32_t _dwt_cyccnt(void);
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
    uint64_t ns{(total_cycles * 1000000000ULL + divisor / 2U) / divisor};
    _semihost_write_uint((uint32_t)ns);
    _semihost_write0(" ns");
}

/* Print seconds with 3-digit millisecond precision via semihosting. */
static void _semihost_write_seconds(uint64_t cycles)
{
    uint64_t sec{cycles / kCpuFreq};
    uint64_t frac{cycles % kCpuFreq};
    uint32_t ms{(uint32_t)((frac * 1000U + kCpuFreq / 2U) / kCpuFreq)};

    _semihost_write_uint((uint32_t)sec);
    _semihost_write0(".");

    if (ms < 100U) _semihost_write0("0");
    if (ms < 10U)  _semihost_write0("0");
    _semihost_write_uint(ms);
    _semihost_write0(" s");
}

/* Benchmark runner — calls algo() N times in a tight loop. */
static class AlgoRunner {
    uint32_t m_runs;
    void algo(void)
    {
#ifdef USE_TEST_ALGO
        algo_nop();
#else
        /* No algorithm selected: compiler_barrier() is a zero-instruction
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
