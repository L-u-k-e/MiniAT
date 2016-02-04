#include "miniat/miniat.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat_priv_pipeline.h"
#include "miniat_priv_memory.h"
#include "miniat_priv_registers.h"
#include "miniat_priv_interrupts.h"
#include "miniat_priv_fetch.h"
#include "miniat_priv_pc.h"

/*
 * This function implements the fetch stage of the MiniAT
 * Interrupts: Illegal Instruction
 */
void m_fetch_stage(miniat *m) {
	
	m_uword pc;
	m_uword npc;
	m_memory_characteristics addr_info;
	m_uword data[M_INSTRUCTION_SIZE_WORDS];
	m_uword int_tripped = 0;

	pc = m_pc_remove_first(m);
	npc = pc + M_INSTRUCTION_SIZE_WORDS;

	/*Check for a bad address*/
	addr_info = m_memory_address_lookup(npc);

	/*
	 * If the PC queue is empty, no deeper stages are trying to branch so its ok to add the NPC to the PC queue. Else, someone else is
	 * branching and my NPC doesn't matter becuase it is past the delay slot (your the tail instruction in the delay slot)
	 */
	if(m->pc.size == 0) {
		m_pc_add_last(m, npc);
	}

	/*Complete the fetch so long as the NPC is in RAM or Flash*/
	if((addr_info.type == m_memory_type_flash || addr_info.type == m_memory_type_ram)) {
		m_memory_read(m, pc, &data[M_LOW]);
		m_memory_read(m, pc+1, &data[M_HIGH]);
	}
	else {
		/*Trip the ifr for illegal address */
		int_tripped = M_INT_ILLEGAL_ADDRESS;
	}

	/*
	 * Fetch the low and high words from memory and pass it through to the output latches, including NPC
	 */
	if(!int_tripped) {
		m->pipeline.fetch.out.data[M_LOW] = data[M_LOW];
		m->pipeline.fetch.out.data[M_HIGH] = data[M_HIGH];
	}
	else {
		m_pipeline_morph_to_INT(m, m_pipeline_stage_fetch, int_tripped);
	}
	m->pipeline.fetch.out.NPC = npc;

	return;
}
