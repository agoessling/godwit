#include "clock.h"

#include <assert.h>
#include <avr32/io.h>
#include <stdint.h>

#define PM_UNLOCK(addr) AVR32_PM.unlock = (0xAA << AVR32_PM_UNLOCK_KEY_OFFSET) \
    | ((AVR32_PM_UNLOCK_ADDR_MASK & (int)addr) << AVR32_PM_UNLOCK_ADDR_OFFSET);

int32_t ClockInit(int32_t freq) {
  // Frequency must be below 30MHz for normal-speed flash read.
  assert(0 < freq && freq <= 30000000);

  // Turn on and wait for RC120M.
  AVR32_SCIF.RC120MCR.en = 1;
  while (!AVR32_SCIF.RC120MCR.en) {}

  // Find cpusel = log2(120e6/freq) - 1.
  int32_t arg = 120000000 / freq;
  int32_t cpusel = -1;
  while (arg >>= 1)
    cpusel++;

  // Check that CPUSEL is in bounds.
  if (cpusel < 0)
    cpusel = 0;
  if (cpusel >= (1 << AVR32_PM_CPUSEL_CPUSEL_SIZE))
    cpusel = (1 << AVR32_PM_CPUSEL_CPUSEL_SIZE) - 1;

  // Set CPU and all peripheral clocks to same frequency.
  // PBASEL
  avr32_pm_pbasel_t pba_reg = {.pbdiv = 1, .pbsel = cpusel};
  PM_UNLOCK(&AVR32_PM.PBASEL);
  AVR32_PM.PBASEL = pba_reg;
  while (!AVR32_PM.SR.ckrdy) {}
  // PBBSEL
  avr32_pm_pbbsel_t pbb_reg = {.pbdiv = 1, .pbsel = cpusel};
  PM_UNLOCK(&AVR32_PM.PBBSEL);
  AVR32_PM.PBBSEL = pbb_reg;
  while (!AVR32_PM.SR.ckrdy) {}
  // CPUSEL
  avr32_pm_cpusel_t cpu_reg = {.cpudiv = 1, .cpusel = cpusel};
  PM_UNLOCK(&AVR32_PM.CPUSEL);
  AVR32_PM.CPUSEL = cpu_reg;
  while (!AVR32_PM.SR.ckrdy) {}

  // Set main clock to RC120M.
  PM_UNLOCK(&AVR32_PM.MCCTRL);
  AVR32_PM.mcctrl = 0x03 << AVR32_PM_MCCTRL_MCSEL_OFFSET;

  // Return actual frequency used.
  return 120000000 >> (cpusel + 1);
}
