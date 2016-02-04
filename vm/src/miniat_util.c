#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "miniat/miniat.h"
#include "miniat_priv_util.h"

/*
 * Provides a deep copy of the buffer passed in, NULL buffer gives NULL back.
 */
char *m_util_str_duplicate(miniat *m, char *s) {
	char *result = NULL;
	if(s && m) {
		result = malloc(sizeof(char) * (strlen(s) + 1));
		if(!result) {
			THROW(m_error_num_out_of_mem);
		}
		else {
			strcpy(result, s);
		}
	}
	return result;
}
