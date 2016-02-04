#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat_priv_pc.h"
#include "miniat/miniat_error.h"


void m_pc_init(miniat *m) {

	if(m) {
		m->pc.queue = &(m->reg[M_REGISTER_PC]);
		m->pc.size = 0;
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_pc_add_last(miniat *m, m_uword address) {

	if(m && m->pc.queue) {
		m_uword *pc_queue = m->pc.queue;
		m_ubyte *size = &(m->pc.size);

		if(*size < M_PC_QUEUE_SIZE) {
			pc_queue[*size] = address;
			(*size)++;
		}
		else {
			THROW(m_error_num_pc_queue_full);
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

m_uword m_pc_remove_first(miniat *m) {

	m_uword address;
	if(m && m->pc.queue) {
		m_uword *pc_queue = m->pc.queue;
		m_ubyte *size = &(m->pc.size);

		if(*size > 0) {
			/*Grab the current head of the queue*/
			address = pc_queue[0];

			/*Decrement the size of the PCQ, since an element has been removed*/
			(*size)--;

			/*Since the head has been removed, slide the data up in the queue towards the head */
			pc_queue[0] = pc_queue[1];
			pc_queue[1] = pc_queue[2];
			pc_queue[2] = 0;

		}
		else {
			THROW(m_error_num_pc_queue_empty);
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return address;
}

void m_pc_clear(miniat *m) {

	if(m && m->pc.queue) {
		if(m->pc.queue) {
			memset(m->pc.queue, 0, M_PC_QUEUE_SIZE);
			m->pc.size = 0;
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}
