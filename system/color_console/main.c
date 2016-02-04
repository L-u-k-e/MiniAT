#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <signal.h>
#include <curses.h>

#include "miniat/miniat.h"
#include "util.h"
#include "term.h"
#include "peripherals.h"
#include "keyb.h"
#include "macs.h"
#include "ports.h"


#define MINIAT_LOG_FILENAME "miniat.log"

#define MAX_CYCLES (~0ULL)


miniat *m = NULL;
char *input_filename = NULL;
FILE *infile = NULL;

unsigned long long cycles = 0;


/*
 * Command line option flags
 */
bool option_print_cycles = false; /* print cycles executed by the application */


static void cleanup();
static void init();
static void miniat_start(int argc, char *argv[]);
static void parse_options(int argc, char *argv[]);
static void print_usage();
static void signal_handler(int sig);


int main(int argc, char *argv[]) {

	TRY {
		miniat_start(argc, argv);
	}
        CATCHALL {
		miniat_dump_error();
	}
	FINALLY {}
	ETRY;

	return EXIT_SUCCESS;
}


static void miniat_start(int argc, char *argv[]) {

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	atexit(cleanup);

	parse_options(argc, argv);
	if(!(infile = fopen(input_filename, "rb"))) {
		ERROR("Couldn't open \"%s\".  %s", input_filename, strerror(errno));
	}

	m = miniat_new(infile);

	init();

	for(;;) {
		terminal_should_be_big_enough();
		if(cycles < MAX_CYCLES) {
			cycles++;
		}

		miniat_clock(m);
		peripherals_clock(m);
		ports_clock(m);
	}

	return;
}


static void signal_handler(int sig) {

	if(sig == SIGINT || sig == SIGTERM) {
		exit(EXIT_SUCCESS);
	}

	return;
}

static void init() {

	terminal_setup();
	terminal_init();
	keyb_init();
	macs_init();

	return;
}

static void cleanup() {

	if(m) {
		/* MiniAT also closes the binary file it was passed on miniat_new */
		miniat_free(m);
	}

	/*
	 * Call all the other cleanup functions
	 */
	peripherals_cleanup();
	terminal_cleanup();
	keyb_cleanup();
	macs_cleanup();

	if(option_print_cycles && cycles > 0) {
		if(cycles < MAX_CYCLES) {
			printf("\n%llu cycles executed\n", cycles);
		}
		else {
			printf("Runtime exceeded %llu cycles!\n", ~0ULL);
		}
	}

	return;
}


/*
 * print_usage()
 *
 * Display command and option usage
 */
static void print_usage() {

	fprintf(stderr, "\n");
	fprintf(stderr, "Usage:  miniat_color_console [-c] bin_file\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "    -c    On exit, display the number of cycles executed by the binary\n");
	fprintf(stderr, "\n");

	return;
}


static void parse_options(int argc, char *argv[]) {

	extern char *optarg;
	extern int optind;

	int option;
	int c_cnt = 0;

	if(input_filename) {
		IMPOSSIBLE("Barry had a little ham");
	}

	while((option = getopt(argc, argv, "c")) != -1) {
		switch((char)option) {
		case 'c':
			option_print_cycles = true;
			c_cnt++;
			break;
		default:
			print_usage();
			exit(EXIT_FAILURE);
			break;
		}
	}

	if(optind == (argc - 1)) {
		input_filename = argv[optind];
		if(strcmp_caseless(strrchr(input_filename, '.'), ".bin") != 0) {
			print_usage();
			ERROR("Expected \".bin\" file as input");
		}
	}
	else{
		print_usage();
		exit(EXIT_FAILURE);
	}

	if(c_cnt > 1) {
		print_usage();
		ERROR("Not sure what is intended by repeat flag usage...");
	}

	return;
}

