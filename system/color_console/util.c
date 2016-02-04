#include <ctype.h>
#include <stdarg.h>

#include "util.h"

void util_print(util_print_level lvl, FILE *out, const char *filename, int lineno, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);
	fflush(NULL);
	switch(lvl) {
	case util_print_level_error:
		fprintf(out, "Error: ");
		break;
	case util_print_level_impossible:
		fprintf(out, "Impossible, %s:%d - This should never happen...\n\n", filename, lineno);
		break;
	}
	vfprintf(out, fmt, args);
	va_end(args);
	fprintf(out, "\n");
	fflush(out);

	return;
}

int strcmp_caseless(char *a, char *b) {

	int result;

	if(!a || !b) {
		/* Really this is undefined.  We're just letting things keep going */
		return -1;
	}

	result = 0;
	while(!result && *a != '\0' && *b != '\0') {
		int ca = *a;
		int cb = *b;

		result += tolower(ca) - tolower(cb);

		a++;
		b++;
	}

	if(result == 0) {
		result = *a - *b;
	}

	return result;
}
