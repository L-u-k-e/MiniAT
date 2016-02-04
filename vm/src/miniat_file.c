#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "miniat/miniat.h"
#include "miniat_priv_file.h"
#include "miniat_priv_defines.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_unions.h"
#include "miniat_priv_enums.h"
#include "miniat/miniat_error.h"

bool is_big_endian () {

	bool result = false;
	uint32_t test_word;
	uint8_t *test_byte;

	test_word = 0xFF000000;
	test_byte = (uint8_t *)(&test_word);

	if(test_byte [0] == 0xFF)
	{
		result = true;
	}

	return result;
}

void endian_convert (miniat *m, m_uword * word) {

	uint8_t * byte;
	m_uword temp_word;
	uint8_t * temp_byte;

	if (!(word || m)) {
		THROW(m_error_num_null_pointer);
	}

	byte = (uint8_t *) word;
	temp_word = *word;
	temp_byte = (uint8_t *) &temp_word;

	byte [0] = temp_byte [3];
	byte [1] = temp_byte [2];
	byte [2] = temp_byte [1];
	byte [3] = temp_byte [0];

	return;
}

void m_load_file(miniat *m, FILE *file_stream) {

	int i;
	int errno_cnt;

	m_uword cnt;
	m_file_block b;

	if(!(m && file_stream)) {
		THROW(m_error_num_null_pointer);
	}

	m->bin_file = file_stream;

	m_file_read_block_count(m, file_stream, &cnt);


	for(i = 0; i < cnt; i++) {

		m_file_read_block (m, file_stream, &b, i);

		memcpy(&(m->mem[b.start_address]), b.data, sizeof(m_uword) * b.length);

		if(b.data) {
			free(b.data);
		}
	}
	return;
}



void m_file_read_block_count (miniat *m, FILE *file_stream, m_uword *cnt) {

	bool big_endian;
	m_uword word;

	if (!file_stream || !cnt) {
		THROW(m_error_num_null_pointer);
	}

	big_endian = is_big_endian ();

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);

	if (word != M_FILE_ID) {
		THROW(m_error_num_invalid_binary);
	}

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);

	*cnt = word;

	return;
}
void m_file_write_block_count (miniat *m, FILE *file_stream, int cnt) {

	bool big_endian;
	int file_identifier;

	if (!file_stream) {
		THROW(m_error_num_null_pointer);
	}

	file_identifier = M_FILE_ID;
	big_endian = is_big_endian ();

	if (!fwrite (&file_identifier, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	if (big_endian) {
		endian_convert (m, (m_uword *) &cnt);
	}

	if (!fwrite (&cnt, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	return;
}

void m_file_read_block (miniat *m, FILE *file_stream, m_file_block *b, m_uword block_number) {

	bool big_endian;
	m_uword word;
	int i = 0;

	if (!file_stream || !b) {
		THROW(m_error_num_null_pointer);
	}

	big_endian = is_big_endian ();

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
		if (big_endian) endian_convert (m, &word);
	if (word != M_BLOCK_SEPERATOR) {
		THROW(m_error_num_invalid_binary);
	}

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);
	if (word != M_BLOCK_SEPERATOR) {
		THROW(m_error_num_invalid_binary);
	}

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);
	if (block_number != word) {
		THROW(m_error_num_invalid_binary);
	}
	b->number = word;

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);

	b->id = word;

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);
	b->start_address = word;

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);
	b->length = word;

	if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}
	if (big_endian) endian_convert (m, &word);
	if (word != M_BLOCK_SEPERATOR) {
		THROW(m_error_num_invalid_binary);
	}

	b->data = (m_uword *)malloc (sizeof (m_uword) * b->length);

	for (i = 0; i < b->length; i++) {
		if (!fread (&word, sizeof (m_uword), 1, file_stream)) {
			THROW(m_error_num_invalid_binary);
		}

		if (big_endian) endian_convert (m, &word);
		b->data [i] = word;
	}

	return;

}

void m_file_write_block (miniat *m, FILE *file_stream, m_file_block *b) {

	bool big_endian;
	int i = 0;
	//int endianness = -1;
	int seperator = M_BLOCK_SEPERATOR;

	if (!(file_stream || b || m)) {
		THROW(m_error_num_null_pointer);
	}

	big_endian = is_big_endian ();

	if (big_endian) endian_convert (m, (m_uword *) &seperator);
	if (!fwrite (&seperator, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	if (big_endian) endian_convert (m, (m_uword *) &seperator);
	if (!fwrite (&seperator, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	if (big_endian) endian_convert (m, (m_uword *) &b->number);
	if (!fwrite (&b->number, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	if (big_endian) endian_convert (m, (m_uword *) &b->id);
	if (!fwrite (&b->id, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
		return;
	}

	if (big_endian) endian_convert (m, &b->start_address);
	if (!fwrite (&b->start_address, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	if (big_endian) endian_convert (m,(m_uword *) &b->length);
	if (!fwrite (&b->length, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	if (big_endian) endian_convert (m, (m_uword *) &seperator);
	if (!fwrite (&seperator, sizeof (m_uword), 1, file_stream)) {
		THROW(m_error_num_invalid_binary);
	}

	for(i = 0; i < b->length; i++) {

		if (big_endian) endian_convert (m, &b->data [i]);
		if (!fwrite (&b->data [i], sizeof (m_uword), 1, file_stream)) {
			THROW(m_error_num_invalid_binary);
		}
	}

	return;
}
