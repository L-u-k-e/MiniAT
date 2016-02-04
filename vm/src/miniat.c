#include <stdlib.h>
#include <string.h>

#include "miniat/miniat.h"

#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat_priv_memory.h"
#include "miniat_priv_interrupts.h"
#include "miniat_priv_timers.h"
#include "miniat_priv_file.h"
#include "miniat_priv_pipeline.h"
#include "miniat_priv_util.h"
#include "miniat/miniat_error.h"
#include "miniat_priv_pc.h"
#include "miniat_priv_bus.h"

miniat *miniat_file_new(char *bin_file_path) {

	FILE *f = fopen(bin_file_path, "rb");
	return miniat_new(f);
}

miniat *miniat_new(FILE *binary_file) {
	
	miniat *m;

	if(!binary_file) {
		THROW(m_error_num_null_pointer);
	}

	m = (miniat *)malloc(sizeof(miniat));
	if(!m) {
		THROW(m_error_num_out_of_mem);
	}

	TRY {

		memset(m, 0, sizeof(miniat));

		m_interrupts_new(m);
		m_pipeline_new(m);
		m_load_file(m, binary_file);
		m_timers_new(m);
		m_memory_csr_new(m);
		m_pc_add_last(m, M_FLASH_START);
	}
	CATCHALL {
		miniat_dump_error(m);
		if(m) {
			free(m);
		}
		m = NULL;
		CTHROW(m_error_num_barn_fire);
	}
	FINALLY {}
	ETRY;

	return m;
}

void miniat_free(miniat *m) {

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(m->bin_file) {
		fclose(m->bin_file);
	}

	free(m);
	return;
}

void miniat_clock(miniat *m) {

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	TRY {
		m_timers_clock(m);
		m_interrupts_handle_interrupt(m);
		m_bus_shuffle(m);
		m_pipeline_clock(m);
		m_bus_shuffle(m);
		m_interrupts_handle_interrupt(m);

		m->interrupts.oifr_l = *(m->interrupts.ifr_l);
		m->interrupts.oifr_h = *(m->interrupts.ifr_h);
		m->ticks++;
	}
	CATCHALL {
		miniat_panic(_e_);
	}
	FINALLY {}
	ETRY;

	return;
}

void miniat_reset(miniat *m) {

	m_uword flash[M_FLASH_SIZE];

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	TRY {

		m_uword ticks = m->ticks;

		memcpy(flash, &(m->mem[M_FLASH_START]), sizeof(m_uword) * M_FLASH_SIZE);

		memset(m, 0, sizeof(miniat));

		m_interrupts_new(m);
		m_pipeline_new(m);
		m_pc_init(m);
		m_timers_new(m);
		m_memory_csr_new(m);
		m_pc_add_last(m, M_FLASH_START);

		m->ticks = ticks;
		memcpy(&(m->mem[M_FLASH_START]), flash, sizeof(m_uword) * M_FLASH_SIZE);
	}
	CATCHALL {
		miniat_panic(_e_);
	}
	FINALLY {}
	ETRY;

	return;
}
