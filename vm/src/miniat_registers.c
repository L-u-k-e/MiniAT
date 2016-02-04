#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat/miniat_error.h"
#include "miniat_priv_pipeline.h"
#include "miniat_priv_registers.h"
#include "miniat_priv_pc.h"

void m_registers_read(miniat *m, m_ubyte address, m_uword *data) {

	if(m && data) {
		*data = m->reg[address];
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_registers_write(miniat *m, m_ubyte address, m_uword data) {

	if(m) {
		if(address == M_REGISTER_PC) {
			m_pc_add_last(m, data);
		}
		else if(address != M_REGISTER_ZERO) {
			m->reg[address] = data;
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}
