#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat/miniat_error.h"
#include "miniat_priv_memory.h"

inline void m_memory_write(miniat *m, m_uword address, m_uword data) {

	m_memory_characteristics mem_info;
	if(m) {
		mem_info = m_memory_address_lookup(address);
		if(mem_info.permissions == m_memory_permissions_write_access ||
				mem_info.permissions == m_memory_permissions_full_access) {
			m->mem[address] = data;
		}
		else {
			/* writes to invalid addresses go to the bit bucket*/
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

inline void m_memory_read(miniat *m, m_uword address, m_uword *data) {
	m_memory_characteristics mem_info;
	if(m) {
		mem_info = m_memory_address_lookup(address);
		if(mem_info.permissions == m_memory_permissions_read_access || mem_info.permissions == m_memory_permissions_full_access) {
			*data = m->mem[address];
		}
		else {
			/*Reads at invalid addresses return all Fs*/
			*data = ~0;
		}
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}

m_memory_characteristics m_memory_address_lookup(m_uword address) {

	m_memory_characteristics identifier;

	if(address >= M_RAM_START && address <= M_RAM_END) {
		identifier.type = m_memory_type_ram;
		identifier.permissions = m_memory_permissions_full_access;
		identifier.duration = M_RAM_STALL_DURATION;
	}
	else if(address >= M_IVT_START && address <= M_IVT_END) {
		if(address == M_IVT_INT_RESET) {
			identifier.permissions = m_memory_permissions_read_access;
		}
		else {
			identifier.permissions = m_memory_permissions_full_access;
		}
		identifier.type = m_memory_type_ivt;
		identifier.duration = M_IVT_STALL_DURATION;
	}
	else if(address >= M_GPIO_START && address <= M_GPIO_END) {

		identifier.type = m_memory_type_gpio;
		identifier.permissions = m_memory_permissions_full_access;
		identifier.duration = M_GPIO_STALL_DURATION;
	}
	else if(address >= M_CSR_START && address <= M_CSR_END) {

		identifier.type = m_memory_type_csr;
		identifier.permissions = m_memory_permissions_full_access;
		identifier.duration = M_CSR_STALL_DURATION;
	}
	else if(address >= M_ID_START && address <= M_ID_END) {

		identifier.type = m_memory_type_id;
		identifier.permissions = m_memory_permissions_read_access;
		identifier.duration = M_ID_STALL_DURATION;
	}
	else if(address >= M_FLASH_START && address <= M_FLASH_END) {

		identifier.type = m_memory_type_flash;
		identifier.permissions = m_memory_permissions_full_access;
		identifier.duration = M_FLASH_STALL_DURATION;
	}
	else if(address >= M_PERIPHERAL_START && address <= M_PERIPHERAL_END) {

		identifier.type = m_memory_type_peripheral;
		identifier.permissions = m_memory_permissions_full_access;
		/*stall duration is irrelevant, bus transactions take an unknown time to complete*/
		identifier.duration = 0;
	}
	else {
		identifier.type = m_memory_type_invalid;
		identifier.permissions = m_memory_permissions_no_access;
		/*stall duration is irrelevant, memory is invalid*/
		identifier.duration = ~0;

	}
	return identifier;
}

void m_memory_csr_new(miniat *m) {
	if(m) {
		m->csr = (m_csr *)&(m->mem[M_CSR_SYSTEM_REGISTER]);
	}
	else {
		THROW(m_error_num_null_pointer);
	}
	return;
}
