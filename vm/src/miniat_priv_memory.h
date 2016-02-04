#ifndef _MINIAT_PRIV_MEMORY_H_
#define _MINIAT_PRIV_MEMORY_H_

#include "miniat/miniat.h"

/**
 * Get the memory characteristics for a memory address. used to help
 * determine what permissions are given to a particular memory address.
 * @param address
 *  The address to check
 * @return
 *  The characteristics of the memory
 */
extern m_memory_characteristics m_memory_address_lookup(m_uword address);

/**
 * Write to memory and verifies the validity of the memory address before
 * performing the write.
 *
 * @param m
 *  The miniat whose memory to write
 * @param address
 *  The address of the memory to write
 * @param data
 *  The data to write
 */
extern void m_memory_write(miniat *m, m_uword address, m_uword data);

/**
 * Read from memroy
 * @param m
 *  The miniat whose memory to read
 * @param address
 *  The address to read
 * @param data
 *  The value of the read is stored here
 */
extern void m_memory_read(miniat *m, m_uword address, m_uword *data);

/**
 * Initialize the CSR's. This function maps the CSR pointers back into the proper spot in mem
 * @param m
 *  The miniat whose CSR's to map
 */
extern void m_memory_csr_new(miniat *m);

#endif
