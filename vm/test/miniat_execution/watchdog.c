#include <stdio.h>
#include <stdlib.h>

#include "miniat/miniat.h"
#include "miniat_priv_registers.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_hazards.h"

#include "CuTest.h"

void t_error___exec_watchdog(CuTest *tc) {

	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword ticks;
	const char *log_name = __func__;

	bin_file = fopen(OUTDIR"/bin/watchdog.bin", "rb");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for(i=0; i <= 1000; i++) {

		miniat_clock(miniat);

		/*
		 * Since we're checking writeback in, the code were checking
		 * for still hasn't executed so we need to add 1 more to the ticks
		 * to account for that code actually being executed or check for NPC + 2
		 * for the instructions whose affect we are looking for.
		 */
		if(miniat->pipeline.writeback.in.NPC == 0x2016) {
			CuAssert(tc, "Count should be 0", miniat->timers.watchdog.control->count == 0);
			CuAssert(tc, "WD should be enabled", miniat->timers.watchdog.control->enable == 1);
			ticks = miniat->ticks;
		}
		else if(miniat->pipeline.writeback.in.NPC == 0x2022) {
			CuAssert(tc, "Count should be 0", miniat->timers.watchdog.control->count == 0);
			CuAssert(tc, "WD should be enabled", miniat->timers.watchdog.control->enable == 1);
			ticks = miniat->ticks;
		}
		else if(miniat->reg[M_REGISTER_PC] == 0x2018) {
			CuAssert(tc, "Count should be 40", miniat->timers.watchdog.control->count == 10);
			CuAssert(tc, "Timer should occur every 40 ticks", ((miniat->ticks - ticks) == 40));
		}
	}
	miniat_free(miniat);
	return;
}

