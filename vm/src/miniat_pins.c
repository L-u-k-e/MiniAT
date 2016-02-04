#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat_priv_memory.h"
#include "miniat_priv_bus.h"
#include "miniat/miniat_error.h"

/*
 * Since this file is public, all definitions are in miniat.h
 */

void miniat_pins_bus_set(miniat *m, m_bus bus){

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	memcpy(&m->bus.pub, &bus, sizeof(bus));
	/*
	 * An immediate call to "get" should
	 * return the new data from private.
	 */
	m_bus_shuffle(m);

	return;
}

m_bus miniat_pins_bus_get(miniat *m){

	m_bus bus;

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	memcpy(&bus, &m->bus.priv, sizeof(bus));
	return bus;
}

void miniat_pins_set_xint(miniat *m, m_xint xint, m_byte data){

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(xint >= m_xint_id_cnt) {
		THROW(m_error_num_out_of_range);
	}

	(*m->interrupts.ifr_h) = ((*m->interrupts.ifr_h) & ~(1<<xint)) | (data.bits.bit0 << xint);

	return;
}

m_byte miniat_pins_get_xint(miniat *m, m_xint xint){

	m_byte data = {
			.u = 0
	};

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(xint >= m_xint_id_cnt) {
		THROW(m_error_num_out_of_range);
	}

	data.u = ((m_word)((*m->interrupts.ifr_h) >> xint)).bits.bit0;

	return data;
}

void miniat_pins_set_timers(miniat *m, m_timer_id timer, m_timers_io pins){

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(timer >= m_timer_id_cnt) {
		THROW(m_error_num_out_of_range);
	}

	m->timers.general_purpose_timer[timer].io->line.input = pins.line.input;

	return;
}

m_timers_io miniat_pins_get_timers(miniat *m, m_timer_id timer){

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(timer >= m_timer_id_cnt) {
		THROW(m_error_num_out_of_range);
	}

	return *(m->timers.general_purpose_timer[timer].io);
}

void miniat_pins_set_gpio_port(miniat *m, m_gpio_id port, m_wyde pins){

	m_uword address = M_GPIO_PORT_A;

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(port >= m_gpio_id_cnt) {
		THROW(m_error_num_out_of_range);
	}

	address += port;
	m_memory_write(m, address, pins.u);
	return;
}

m_wyde miniat_pins_get_gpio_port(miniat *m, m_gpio_id port){

	m_uword pins = 0;
	m_uword address = M_GPIO_PORT_A;

	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	if(port >= m_gpio_id_cnt) {
		THROW(m_error_num_out_of_range);
	}

	address += port;
	m_memory_read(m, address, &pins);
	return *((m_wyde *)(&pins));
}

m_bus *miniat_conector_bus_get(miniat *m) {
	if(!m) {
		THROW(m_error_num_null_pointer);
	}

	return &m->bus.pub;
}
