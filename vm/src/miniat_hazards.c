#include <stdlib.h>
#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat/miniat_error.h"

void m_hazards_init(miniat *m) {

        if(m) {
                m->pc.queue = &(m->reg[M_REGISTER_PC_ONE]);
                m->pc.size = 0;
        }
        else {
    		THROW(m_error_num_null_pointer);
        }
        return;
}

void m_hazards_clear(miniat *m) {
        if(m) {
                memset(&(m->pipeline.register_hazards), 0, sizeof(m_pipeline_register_hazards));
                memset(&(m->pipeline.memory_hazards), 0, sizeof(m_pipeline_memory_hazards));
        }
        else {
    		THROW(m_error_num_null_pointer);
        }
        return;
}

void m_hazards_add(miniat *m, m_uword address, bool is_reg) {

	int i;
	m_uword *q;
	m_ubyte *size;

	if(m) {
		if(is_reg) {
			q = m->pipeline.register_hazards.queue;
			size = &(m->pipeline.register_hazards.size);
		}
		else {
			q = m->pipeline.memory_hazards.queue;
			size = &(m->pipeline.memory_hazards.size);
		}
		if((address != M_REGISTER_ZERO && address != M_REGISTER_PC_ONE && is_reg) || !is_reg) {
			for(i = 0; (i < M_HAZARD_QUEUE_SIZE); i++) {
				if(q[i] == 0) {
					q[i] = address;
					(*size)++;
					break;
				}
			}

		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_hazards_remove(miniat *m, m_uword address, bool is_reg) {

	m_uword *q;
	m_ubyte *size;
	int i;

	if(m) {
		if(is_reg) {
			q = m->pipeline.register_hazards.queue;
			size = &(m->pipeline.register_hazards.size);
		}
		else {
			q = m->pipeline.memory_hazards.queue;
			size = &(m->pipeline.memory_hazards.size);
		}
		if((address != 0 && address != M_REGISTER_PC_ONE && is_reg) || !is_reg) {
			for(i = 0; i < M_HAZARD_QUEUE_SIZE; i++) {
				if(q[i] == address) {
					q[i] = 0;
					(*size)--;
					break;
				}
			}
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

bool m_hazards_check(miniat *m, m_uword address,bool is_reg) {

	m_uword *q;
	int i;
	if(m) {
		if(is_reg) {
			if(address == M_REGISTER_ZERO  || address == M_REGISTER_PC_ONE) {
					return false;
			}
			q = m->pipeline.register_hazards.queue;
		}
		else {
			q = m->pipeline.memory_hazards.queue;
		}

		for(i = 0; i < M_HAZARD_QUEUE_SIZE; i++) {
			if(q[i] == address) {
				return true;
			}
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}

	return false;
}
