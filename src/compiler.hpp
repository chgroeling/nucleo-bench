/* Compiler optimization barriers — control what the optimizer may delete or
   reorder, without emitting any instructions.

   When timing a real algorithm, the optimizer may notice that its output is
   never observed and either delete the computation as dead code or hoist a
   loop-invariant call out of the `for` loop entirely. Feed inputs and/or
   results through these helpers to force the work to actually happen.

   Modelled on Google Benchmark's DoNotOptimize / ClobberMemory and the Linux
   kernel's barrier(). Zero code is emitted — they are pure compiler fences. */

#pragma once

/* Force `value` to be materialized in a register / memory as a real side
   effect, so the compiler cannot discard the code that produced it.
   Use on an algorithm's result (or a mutated input) inside the loop. */
template <typename T>
static inline void do_not_optimize(T &&value)
{
    __asm volatile("" : : "r,m"(value) : "memory");
}

/* Full memory-clobber barrier: the compiler must assume all memory may have
   been read and written here, so it cannot reorder loads/stores across this
   point or keep stale values cached in registers. Use when an algorithm's
   effect is a write to memory the harness never reads back. */
static inline void clobber_memory(void)
{
    __asm volatile("" : : : "memory");
}

/* Lightweight compiler barrier: an observable side effect the optimizer must
   preserve, without the memory clobber of clobber_memory(). It emits zero
   instructions and forces no register reloads, so it costs nothing at runtime.
   Placing it in a code path stops the compiler from proving that path has no
   effect and deleting it — e.g. an empty benchmark loop body. */
static inline void compiler_barrier(void)
{
    __asm volatile("");
}
