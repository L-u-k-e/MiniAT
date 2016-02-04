#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat_priv_pipeline.h"
#include "miniat/miniat_error.h"
#include "miniat_priv_bus.h"

bool m_bus_read(miniat *m, m_uword address, m_uword *data, m_uword *int_tripped) {

	bool is_read_complete = false;
	if(m && data && int_tripped) {
		switch(m->bus.state) {

			/*
			 * The initial state of the bus. If a read starts
			 * and the bus us not in the proper state, a bus
			 * error is thrown.
			 */
			case m_bus_state_initial:
				if(m->bus.priv.req == M_LOW &&
						m->bus.priv.ack == M_LOW) {

					/*
					 * All bus transactions have this time constraint for
					 * the peripheral to bring ack high. If they don't then
					 * a transition from the transfer state to the final state
					 * occurs without the ack
					 */
					m->bus.strobe_duration = M_BUS_STROBE_DURATION;
					m->bus.priv.address = address;
					m->bus.priv.rW = M_LOW;
					m->bus.priv.req = M_HIGH;
					m->bus.state = m_bus_state_transfer;
				}
				else {
					*int_tripped = M_INT_BUS_ERROR;
					m_bus_reset(m);
				}
				break;

			/*
			 * Check to see if they have acknowledged are
			 * request. If they have, the data is "valid".
			 */
			case m_bus_state_transfer:

				if(m->bus.priv.req == M_HIGH &&
						m->bus.priv.ack == M_HIGH) {
					m->bus.storage = m->bus.priv.data;
					m->bus.priv.req = M_LOW;
					m->bus.state = m_bus_state_final;
				}
				else {
					m->bus.strobe_duration--;

					if(m->bus.strobe_duration == 0) {
						m->bus.state = m_bus_state_final;
					}
				}
				break;

			/*
			 * The final state, transition back to initial state,
			 * but wait for the peripheral to bring it's ack line low
			 * or if the strobe duration is exhausted, just move on.
			 */
			case m_bus_state_final:

				if((m->bus.priv.req == M_LOW
						&& m->bus.priv.ack == M_LOW) ||
						(m->bus.strobe_duration == 0)) {
					m->bus.state = m_bus_state_initial;

					/* The data is transfered back to the caller of this function on the last
					 * call. */
					is_read_complete = true;
					*data = m->bus.storage;
					m->bus.storage = 0;
					m->bus.strobe_duration = 0;
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

	return is_read_complete;
}

bool m_bus_write(miniat *m, m_uword address, m_uword data, m_uword *int_tripped) {

	bool is_write_complete = false;

	if(m && int_tripped) {

		switch(m->bus.state) {
			/*
			 * The initial state of the bus. If a read starts
			 * and the bus us not in the proper state, a bus
			 * error is thrown. Since were writing, we put data
			 * on the line as well.
			 */
			case m_bus_state_initial:
				if(m->bus.priv.req == M_LOW &&
						m->bus.priv.ack == M_LOW) {
					/*
					 * All bus transactions have this time constraint for
					 * the peripheral to bring ack high. If they don't then
					 * a transition from the transfer state to the final state
					 * occurs without the ack
					 */
					m->bus.strobe_duration = M_BUS_STROBE_DURATION;
					m->bus.priv.address = address;
					m->bus.priv.data = data;
					m->bus.priv.rW = M_HIGH;
					m->bus.priv.req = M_HIGH;
					m->bus.state = m_bus_state_transfer;
				}
				else {
					*int_tripped = M_INT_BUS_ERROR;
					m_bus_reset(m);
				}
				break;

			case m_bus_state_transfer:

				/*
				 * Check to see if they have acknowledged are
				 * request. If they have, the data is "valid".
				 */
				if(m->bus.priv.req == M_HIGH &&
						m->bus.priv.ack == M_HIGH) {
					m->bus.priv.req = M_LOW;
					m->bus.state = m_bus_state_final;
				}
				else {
					m->bus.strobe_duration--;

					if(m->bus.strobe_duration == 0) {
						m->bus.state = m_bus_state_final;
					}
				}
				break;

			/*
			 * Just wait for ack to go low or if the strobe
			 * duration is exhausted, then transition back to
			 * initial state.
			 */
			case m_bus_state_final:
				if((m->bus.priv.req == M_LOW
						&& m->bus.priv.ack == M_LOW) ||
						(m->bus.strobe_duration == 0)) {
					m->bus.state = m_bus_state_initial;
					is_write_complete = true;
					m->bus.strobe_duration = 0;
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

	return is_write_complete;

}

void m_bus_reset(miniat *m) {

	if(m) {
		m->bus.priv.req = M_LOW;
		m->bus.state = m_bus_state_initial;
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

void m_bus_shuffle(miniat *m) {

	if(m) {

		/* Peripheral control lines go from public to private
		 * (from outside world to internal) */
		m->bus.priv.ack = m->bus.pub.ack;

		/* MiniAT internal lines need to be sent to public */
		m->bus.pub.address = m->bus.priv.address;
		m->bus.pub.rW = m->bus.priv.rW;
		m->bus.pub.req = m->bus.priv.req;

		/* Data is dependent on read versus write */
		if(!m->bus.priv.rW) {
			/* On reads the data flow is from external to internal so pub to priv */
			m->bus.priv.data = m->bus.pub.data;
		}
		else {
			/* On writes the data flow is from internal to external so priv to pub */
			m->bus.pub.data = m->bus.priv.data;
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}
