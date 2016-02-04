#include <stdio.h>
#include <stdlib.h>

#include "miniat/miniat.h"
#include "miniat_priv_typedefs.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_file.h"
#include "miniat/miniat_error.h"

#include "CuTest.h"

void t_file___read_block_count(CuTest *tc) {

	FILE *fp, *f;
	m_uword cnt;
	const m_error *err;
	char *path = "vm/test/miniat_file/six_block.miniat";
	fp = fopen(path, "rb");
	CuAssert(tc, "Could not open \"miniat_file/six_block.miniat\"", fp != NULL);

	miniat *m;
	m = malloc(sizeof(miniat));

	m_file_read_block_count(m, fp, &cnt);
	CuAssert(tc, "Block count equals 6", cnt == 6);


	/*Intentionally throwing an error*/
	TRY {
		cnt = 32;
		m_file_read_block_count (m, NULL, &cnt);
		cnt = 42;
	}
	CATCHALL{
		CuAssert(tc, "Did not throw exception", cnt == 32);
	}
	FINALLY{}
	ETRY;

	if(fp) {
		fclose(fp);
	}
	if(m) {
		free(m);
	}

	return;
}

void t_file___write_block_count(CuTest *tc) {

	FILE *fp;
	m_uword cnt;
	const m_error *err;

	miniat *m;
	m = malloc(sizeof(miniat));

	fp = fopen("write_block_count.miniat", "w+");

	m_file_write_block_count (m, fp, 12);
	CuAssert(tc, "File pointer not equal to NULL!", fp != NULL);

	rewind(fp);
	m_file_read_block_count (m, fp, &cnt);
	CuAssert(tc, "Block count equals 12", cnt == 12);

	TRY {
		cnt = 0;
		m_file_write_block_count (m, NULL, cnt);
		cnt = 32;
	}
	CATCHALL {
		CuAssert(tc, "Passing NULL should cause an error", cnt == 0);
	}
	FINALLY {}
	ETRY;

	if(fp) {
		fclose(fp);
	}
	if(m) {
		free(m);
	}
	remove("write_block_count.miniat");
	return;
}

void t_file___read_block(CuTest *tc) {

	FILE *fp, *f;
	m_uword cnt;

	m_file_block block;
	m_uword i;

	miniat *m;
	m = malloc(sizeof(miniat));

	fp = fopen("vm/test/miniat_file/six_block.miniat", "rb");
	CuAssert(tc, "File pointer not equal to NULL", fp != NULL);

	m_file_read_block_count (m, fp, &cnt);

	for(i=0; i<cnt; i++) {
		m_file_read_block (m, fp, &block, i);
	}

	if(fp) {
		fclose(fp);
	}
	if(m) {
		free(m);
	}

	return;

}

/*
 * To verify the integrity of the output file produced
 * I hand walked through the complete file ensuring that
 * it held to the file schema--Bill
 */
void t_file___write_block(CuTest *tc) {

	FILE *fp, *f;
	m_uword cnt;
	m_uword tmp;
	int i, j;
	m_file_block block;

	miniat *m;
	m = malloc(sizeof(miniat));

	fp = fopen("write_block.miniat", "w+");
	CuAssert(tc, "File pointer not equal to NULL", fp != NULL);

	/*write the header*/
	cnt = (rand() % 20) + 10;
	m_file_write_block_count (m, fp, cnt);

	/*start building the body data to write*/
	for(i=0; i<cnt; i++) {

		block.number = i;
		block.id = (rand() % m_block_type_cnt);
		block.start_address = ((rand() << 16) | (rand()));
		block.length = rand()%32;
		block.data = (m_uword *)malloc(sizeof(m_uword) * block.length);
		CuAssert(tc, "Block data not equal to NULL", block.data != NULL);
		/*make some data*/
		for(j=0; j < block.length; j++) {
			block.data[j] = ((rand() << 16) | (rand()));
		}
		/*write the block*/
		m_file_write_block (m, fp, &block);

		/*free the block.data*/
		if(block.data) {
			for(j=0; j < block.length; j++) {
				//free(block.data[j]);
			}
			//free(block.data);
			block.data = NULL;
		}
	}

	/*rewind the file pointer and read the blocks to verify*/
	rewind(fp);

	/*check the counts*/
	m_file_read_block_count (m, fp, &tmp);
	CuAssert(tc, "Block count matches previous block count", cnt == tmp);

	if(fp) {
		fclose(fp);
	}
	if(m) {
		free(m);
	}
	remove("write_block.miniat");
	return;
}

