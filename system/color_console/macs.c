#include <stdbool.h>
#include <curses.h>

#include "macs.h"


static int MACS_MAP[MACS_CNT];


bool is_macs(int ch) {

	return ch <= MACS_MIN && ch >= MACS_MAX;
}


void macs_cleanup() {

	return;
}


void macs_init() {

	MACS_MAP[-MACS_ULCORNER + MACS_MIN] = ACS_ULCORNER;
	MACS_MAP[-MACS_LLCORNER + MACS_MIN] = ACS_LLCORNER;
	MACS_MAP[-MACS_URCORNER + MACS_MIN] = ACS_URCORNER;
	MACS_MAP[-MACS_LRCORNER + MACS_MIN] = ACS_LRCORNER;
	MACS_MAP[-MACS_LTEE + MACS_MIN] = ACS_LTEE;
	MACS_MAP[-MACS_RTEE + MACS_MIN] = ACS_RTEE;
	MACS_MAP[-MACS_BTEE + MACS_MIN] = ACS_BTEE;
	MACS_MAP[-MACS_TTEE + MACS_MIN] = ACS_TTEE;
	MACS_MAP[-MACS_HLINE + MACS_MIN] = ACS_HLINE;
	MACS_MAP[-MACS_VLINE + MACS_MIN] = ACS_VLINE;
	MACS_MAP[-MACS_PLUS + MACS_MIN] = ACS_PLUS;
	MACS_MAP[-MACS_S1 + MACS_MIN] = ACS_S1;
	MACS_MAP[-MACS_S9 + MACS_MIN] = ACS_S9;
	MACS_MAP[-MACS_DIAMOND + MACS_MIN] = ACS_DIAMOND;
	MACS_MAP[-MACS_CKBOARD + MACS_MIN] = ACS_CKBOARD;
	MACS_MAP[-MACS_DEGREE + MACS_MIN] = ACS_DEGREE;
	MACS_MAP[-MACS_PLMINUS + MACS_MIN] = ACS_PLMINUS;
	MACS_MAP[-MACS_BULLET + MACS_MIN] = ACS_BULLET;
	MACS_MAP[-MACS_LARROW + MACS_MIN] = ACS_LARROW;
	MACS_MAP[-MACS_RARROW + MACS_MIN] = ACS_RARROW;
	MACS_MAP[-MACS_DARROW + MACS_MIN] = ACS_DARROW;
	MACS_MAP[-MACS_UARROW + MACS_MIN] = ACS_UARROW;
	MACS_MAP[-MACS_BOARD + MACS_MIN] = ACS_BOARD;
	MACS_MAP[-MACS_LANTERN + MACS_MIN] = ACS_LANTERN;
	MACS_MAP[-MACS_BLOCK + MACS_MIN] = ACS_BLOCK;
	MACS_MAP[-MACS_S3 + MACS_MIN] = ACS_S3;
	MACS_MAP[-MACS_S7 + MACS_MIN] = ACS_S7;
	MACS_MAP[-MACS_LEQUAL + MACS_MIN] = ACS_LEQUAL;
	MACS_MAP[-MACS_GEQUAL + MACS_MIN] = ACS_GEQUAL;
	MACS_MAP[-MACS_PI + MACS_MIN] = ACS_PI;
	MACS_MAP[-MACS_NEQUAL + MACS_MIN] = ACS_NEQUAL;
	MACS_MAP[-MACS_STERLING + MACS_MIN] = ACS_STERLING;

	return;
}


int macs_map(int ch) {

	if(is_macs(ch)) {
		return MACS_MAP[-ch + MACS_MIN];
	}
	else {
		return ERR;
	}
}
