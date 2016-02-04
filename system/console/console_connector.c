#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "miniat/miniat.h"
#include "simple_term.h"
#include "simple_kb.h"

#define MINIAT_LOG_FILENAME "miniat.log"

#define M_SIMPLE_KB_ADDRESS   0x4002
#define M_SIMPLE_TERM_ADDRESS 0x4000

miniat *m = NULL;
p_simple_term *t = NULL;
p_simple_kb *k = NULL;

static void miniat_start(int argc, char *argv[]) {

	m_bus *bus; /* bus state from miniat, when we use connectors it must be pointer! */

	/* Boiler plate code to just make sure you gave me a bin file to run */
	if (argc != 2) {
		fprintf(stderr, "You must specify a bin file name to run");
		THROW(m_error_num_illegal_argument);
	}

	/*
	 * We need instances of the peripherals and a miniat that we want to use. Since this
	 * system uses a keyboard and mouse, we create those objects to use in our system.
	 *
	 * This version of simple echo terminal uses miniat_file_new, which means you don't
	 * need to write the code that opens a file.
	 */
	m = miniat_file_new(argv[1]);

	bus = miniat_conector_bus_get(m);
	if (!bus) {
		fprintf(stderr, "Could not get miniat bus connector!\n");
		miniat_free(m);
		THROW(m_error_num_null_pointer);
	}

	t = p_simple_term_new(M_SIMPLE_TERM_ADDRESS);
	k = p_simple_kb_new(M_SIMPLE_KB_ADDRESS);

	/*
	 * This example uses connectors. Connectors make your life easier. Once you
	 * connect a peripheral to a miniat bus connector, you don't need to manually
	 * transfer the data back and forth.
	 */
	p_simple_kb_bus_connector_set(k, bus);
	p_simple_term_bus_connector_set(t, bus);

	/* Just do it for ever, the miniat handles the ctr^c keyboard interrupt, so you can always
	   stop it by pressing ctr^c on the command line */
	while(1) {

		/* Give the miniat a clock cycle */
		miniat_clock(m);

		/*
		 * Give the terminal and kb a clock so it can do something with the data we got
		 * from the miniat and set to the peripherals.
		 */
		p_simple_term_clock(t);
		p_simple_kb_clock(k);

	}

	/* Free the data we use before exiting */
	p_simple_term_free(t);
	p_simple_kb_free(k);
	miniat_free(m);
}

static void cleanup(int signum) {
	(void)signum;

	if(m) {
		miniat_free(m);
		m = NULL;
	}
	if(t) {
		p_simple_term_free(t);
		t = NULL;
	}
	if(k) {
		p_simple_kb_free(k);
		k = NULL;
	}
	exit(1);
}

int main(int argc, char *argv[]) {
	TRY {
		signal(SIGINT, cleanup);
		miniat_start(argc, argv);
	}
	CATCHALL {
		miniat_dump_error();
	}
	FINALLY {
		cleanup(0);
	}
	ETRY;
	return 0;
}
