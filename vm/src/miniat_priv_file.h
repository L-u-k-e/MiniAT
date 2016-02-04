#ifndef _MINIAT_PRIV_FILE_H_
#define _MINIAT_PRIV_FILE_H_

#include <stdio.h>
#include <stdbool.h>

#include "miniat/miniat.h"

#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"

/*!
 * @file
 *
 * file block format
 *
 * 0xBADCC0DE
 * Number Of Blocks
 * <-- Repeat The Section Below To Add Blocks -->
 * 0xDEADBEEF
 * 0xDEADBEEF
 * Block Number
 * I.D. (m_block_type)
 * Start Address
 * Length (amount of data)
 * 0xDEADBEEF
 * Data (m_uword(s))
 */

extern void m_load_file(miniat *m, FILE *file_stream);

/*!
 * @file
 *
 * file block format
 *
 * 0xBADCC0DE
 * Number Of Blocks
 * <-- Repeat The Section Below To Add Blocks -->
 * 0xDEADBEEF
 * 0xDEADBEEF
 * Block Number
 * I.D. (m_block_type)
 * Start Address
 * Length (amount of data)
 * 0xDEADBEEF
 * Data (m_uword(s))
 */

extern void m_load_file(miniat *m, FILE *file_stream);

/*!
 * @param file_stream is a open handle of the file to be read
 * @param cnt returns the number of blocks that the file contains
 *
 * read the file id and the block count
 */
extern void m_file_read_block_count (miniat *m, FILE *file_stream, m_uword *cnt);

/*!
 * @param file_stream is the open handle of the file to be written
 * @para cnt is the number of blocks to be written
 *
 * write the file id and the block count
 */
extern void m_file_write_block_count (miniat *m, FILE *file_stream, int cnt);

/*!
 * @param file_stream is a open handle of the file to be read
 * @param m_file_block is a pointer to the block that will be loaded
 * @param number is the block number to be read
 *
 * read the block number, id, start address,
 * length and data into the m_file_block
 */
extern void m_file_read_block (miniat *m, FILE *file_stream, m_file_block *b, m_uword number);

/*!
 * @param file_stream is the open handle of the file ot be written
 * @param m_file_block is a pointer to the block what will be written to the file
 *
 * write the block number, id, start address,
 * length and data into the m_file_block
 */
extern void m_file_write_block (miniat *m, FILE *file_stream, m_file_block *b);


/*!
 * check to see if the current system is big_endian
 */
extern bool is_big_endian ();

/*!
 * @param the m_uword that needs to be converted to the opposite endianess
 *
 * this function converts a m_uword from whatever endianess it is currently in to the opposite
 */
extern void endian_convert (miniat *m, m_uword *word);

#endif /* _MINIAT_PRIV_FILE_H_ */

