/* 1000‑nop throughput test — measures effective CPI to verify CPU execution
   frequency.  Each nop is 1 cycle on Cortex‑M4, so total_cycles / nop_count
   gives cycles per instruction.  Use the result to confirm the core is
   really running at 180 MHz.

   Example run with 3 000 000 repetitions ran for 17.567 s:
     3 000 000 000 nops  /  17.567 s  =  170.8 MHz effective.
     The ~5 % gap to 180 MHz comes from -O0 loop overhead and the function
     call / return around each invocation of algo_nop().  The nops themselves
     are 1 cycle each — the rest is measurement tax.
     With -O3 the loop and call overhead are largely eliminated, reaching
     178 MHz effective — within 1 % of the core clock.

   Keep this file as a reference — add new .cpp/hpp pairs for real algorithms. */

#include "algo_nop.hpp"

void algo_nop(void)
{
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");

  // -----------------------------------------
  // # 100 nops
  // -----------------------------------------
  //
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  // ## 10 nops
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
  __asm volatile("nop");
}
