#include <stdlib.h>
#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat_priv_memory.h"
#include "miniat_priv_pipeline.h"
#include "miniat_priv_fetch.h"
#include "miniat_priv_decode.h"
#include "miniat_priv_execute.h"
#include "miniat_priv_writeback.h"
#include "miniat/miniat_error.h"
#include "miniat_priv_hazards.h"
#include "miniat_priv_pc.h"
#include "miniat_priv_pipeline.h"

void m_pipeline_stalls_init(miniat *m);

void m_pipeline_new(miniat *m) {

	if(m) {
		m_hazards_init(m);
		m_pipeline_stalls_init(m);
	}
	else {
		THROW(m_error_num_null_pointer);
	}

	return;
}

void m_pipeline_clock(miniat *m) {

	if(m) {

		m_writeback_stage(m);

		/* 
		 * If I am not stalled and the stage ahead of me is not a hazard, then I can move forward. 
		 */
		if(m->pipeline.execute.state != m_pipeline_stage_state_stalled && m->pipeline.writeback.state == m_pipeline_stage_state_no_hazard) {
			m_execute_stage(m);
		}
		if(m->pipeline.decode.state != m_pipeline_stage_state_stalled && m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
			m_decode_stage(m);
		}
		if(m->pipeline.fetch.state != m_pipeline_stage_state_stalled && m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {
			m_fetch_stage(m);
		}

shuffle_latches:
		if(m->pipeline.writeback.state == m_pipeline_stage_state_no_hazard) {
			m->pipeline.writeback.in.opcode = m->pipeline.execute.out.opcode;
			m->pipeline.writeback.in.hint = m->pipeline.execute.out.hint;
			m->pipeline.writeback.in.D = m->pipeline.execute.out.D;
			m->pipeline.writeback.in.NPC = m->pipeline.execute.out.NPC;
			m->pipeline.writeback.in.rX = m->pipeline.execute.out.rX;
			m->pipeline.writeback.in.result = m->pipeline.execute.out.result;
		}

		if(m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
			m->pipeline.execute.in.A = m->pipeline.decode.out.A;
			m->pipeline.execute.in.B = m->pipeline.decode.out.B;
			m->pipeline.execute.in.D = m->pipeline.decode.out.D;
			m->pipeline.execute.in.NPC = m->pipeline.decode.out.NPC;
			m->pipeline.execute.in.opcode = m->pipeline.decode.out.opcode;
			m->pipeline.execute.in.hint = m->pipeline.decode.out.hint;
			m->pipeline.execute.in.rX = m->pipeline.decode.out.rX;
		}

		if(m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {
			m->pipeline.decode.in.NPC = m->pipeline.fetch.out.NPC;
			m->pipeline.decode.in.data[M_LOW] = m->pipeline.fetch.out.data[M_LOW];
			m->pipeline.decode.in.data[M_HIGH] = m->pipeline.fetch.out.data[M_HIGH];
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_pipeline_stalls_init(miniat *m) {

	if(m) {
		m->pipeline.fetch.state = m->pipeline.decode.state = m->pipeline.execute.state = m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
	}
	else {
		THROW(m_error_num_null_pointer);
	}

	return;
}

void m_pipeline_stalls_clear_all(miniat *m) {

	if(m) {
		m->pipeline.fetch.state = m->pipeline.decode.state = m->pipeline.execute.state = m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_pipeline_stalls_set(miniat *m, m_pipeline_stage stage, m_pipeline_stage_state state, m_uword duration)  {

	if(m) {
		switch(stage) {
			case m_pipeline_stage_writeback:
				if(stage == m_pipeline_stage_writeback) {
					m->pipeline.writeback.state = state;
					m->pipeline.writeback.duration = duration;
				}
				else {
					if(m->pipeline.writeback.state == m_pipeline_stage_state_no_hazard) {
						m->pipeline.writeback.state = m_pipeline_stage_state_stalled;
					}
					else {
						break;
					}
				}
				/* no break */
			case m_pipeline_stage_execute:
				if(stage == m_pipeline_stage_execute) {
					m->pipeline.execute.state = state;
					m->pipeline.execute.duration = duration;
				}
				else {
					if(m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
						m->pipeline.execute.state = m_pipeline_stage_state_stalled;
					}
					else {
						break;
					}
				}
				/* no break */
			case m_pipeline_stage_decode:
				if(stage == m_pipeline_stage_decode) {
					m->pipeline.decode.state = state;
					m->pipeline.decode.duration = duration;
				}
				else {
					if(m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {
						m->pipeline.decode.state = m_pipeline_stage_state_stalled;
					}
					else {
						break;
					}
				}
				/* no break */
			case m_pipeline_stage_fetch:
				if(stage == m_pipeline_stage_fetch) {
					m->pipeline.fetch.state = state;
					m->pipeline.fetch.duration = duration;
				}
				else {
					if(m->pipeline.fetch.state == m_pipeline_stage_state_no_hazard) {
						m->pipeline.fetch.state = m_pipeline_stage_state_stalled;
					}
					else {
						break;
					}
				}
				break;
			default:
				THROW(m_error_num_barn_fire);
				/* no break */
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_pipeline_stalls_clear(miniat *m, m_pipeline_stage stage) {

	if(m) {
		switch(stage) {
			case m_pipeline_stage_writeback:
				if(stage == m_pipeline_stage_writeback) {
					m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
					m->pipeline.writeback.duration = M_NO_STALL_DURATION;
				}
				else {
					if(m->pipeline.writeback.state == m_pipeline_stage_state_stalled) {
						m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
					}
					else {
						break;
					}
				}
				/* no break */
			case m_pipeline_stage_execute:
				if(stage == m_pipeline_stage_execute) {
					m->pipeline.execute.state = m_pipeline_stage_state_no_hazard;
					m->pipeline.execute.duration = M_NO_STALL_DURATION;
				}
				else {
					if(m->pipeline.execute.state == m_pipeline_stage_state_stalled) {
						m->pipeline.execute.state = m_pipeline_stage_state_no_hazard;
					}
					else {
						break;
					}
				}
				/* no break */
			case m_pipeline_stage_decode:
				if(stage == m_pipeline_stage_decode) {
					m->pipeline.decode.state = m_pipeline_stage_state_no_hazard;
					m->pipeline.decode.duration = M_NO_STALL_DURATION;
				}
				else {
					if(m->pipeline.decode.state == m_pipeline_stage_state_stalled) {
						m->pipeline.decode.state = m_pipeline_stage_state_no_hazard;
					}
					else {
						break;
					}
				}
				/* no break */
			case m_pipeline_stage_fetch:
				if(stage == m_pipeline_stage_fetch) {
					m->pipeline.fetch.state = m_pipeline_stage_state_no_hazard;
					m->pipeline.fetch.duration = M_NO_STALL_DURATION;
				}
				else {
					if(m->pipeline.fetch.state == m_pipeline_stage_state_stalled) {
						m->pipeline.fetch.state = m_pipeline_stage_state_no_hazard;
					}
					else {
						break;
					}
				}
				break;
			default:
				THROW(m_error_num_barn_fire);
				/* no break */
		}

	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

/*
 * Resets a given miniat pipeline state to 0's
 *
 *@param m - the miniat whose pipeline state to clear
 */
static void m_pipeline_clear(miniat *m) {
	if(m) {
		memset(&m->pipeline.fetch, 0, sizeof(m_pipeline_fetch));
		memset(&m->pipeline.decode, 0, sizeof(m_pipeline_decode));
		memset(&m->pipeline.execute, 0, sizeof(m_pipeline_execute));
		memset(&m->pipeline.writeback, 0, sizeof(m_pipeline_writeback));
	}
	else {
		THROW(m_error_num_null_pointer);
	}

	return;
}

void m_pipeline_set_latch_to_NOP(miniat *m, m_pipeline_stage stage) {

	if(m) {
		switch(stage) {
		case m_pipeline_stage_fetch: memset(&(m->pipeline.fetch.out), 0, sizeof (m_pipeline_fetch_latch_out));
			break;
		case m_pipeline_stage_decode: memset(&(m->pipeline.decode.out), 0, sizeof (m_pipeline_decode_latch_out));
			break;
		case m_pipeline_stage_execute: memset(&(m->pipeline.execute.out), 0, sizeof (m_pipeline_execute_latch_out));
			break;
		default:
			THROW(m_error_num_barn_fire);
			/* no break */
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_pipeline_flush(miniat *m) {

	if(m) {
		m_pc_clear(m);
		m_pipeline_stalls_clear_all(m);
		m_hazards_clear(m);
		m_pipeline_clear(m);
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_pipeline_morph_to_INT(miniat *m, m_pipeline_stage stage, m_uword interrupt) {

	if(m) {
		m_uword tmp = 0;
		m_pipeline_fetch_latch_out *fetch = &(m->pipeline.fetch.out);
		m_pipeline_decode_latch_out *decode = &(m->pipeline.decode.out);
		m_pipeline_execute_latch_out *execute = &(m->pipeline.execute.out);

		m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);

		switch(stage) {
			case m_pipeline_stage_fetch:
				fetch->NPC = 0;
				fetch->data[1] = interrupt;
				((m_low_word_layout *)&fetch->data[0])->opcode = M_INT;
				break;

			case m_pipeline_stage_decode:
				decode->NPC = 0;
				decode->opcode = M_INT;
				decode->A = 0;
				decode->B = 0;
				decode->D = interrupt;
				decode->hint = 0;
				decode->rX = 0;
				break;

			case m_pipeline_stage_execute:
				execute->NPC = 0;
				execute->D = interrupt;
				execute->hint = 0;
				execute->opcode = M_INT;
				execute->rX = 0;
				break;

			case m_pipeline_stage_writeback:
				m_memory_read(m, M_CSR_INT_FLAG_LOW, &tmp);
				tmp |= (1 << interrupt);
				m_memory_write(m, M_CSR_INT_FLAG_LOW, tmp);
				m_hazards_remove(m, M_CSR_INT_FLAG_LOW, false);
				break;
			default:
				THROW(m_error_num_barn_fire);
				/* no break */
		}
		m_pipeline_stalls_clear(m, stage);
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}
