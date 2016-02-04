#include <stdlib.h>

#include "miniat/miniat.h"
#include "miniat_priv_structures.h"
#include "miniat_priv_util.h"

#include "CuTest.h"

void t_util___str_duplicate_simple(CuTest *tc) {
	char *s1 = "Testing, 1, 2, 3...";
	char *s2 = NULL;

	miniat *m;
	m = malloc(sizeof(miniat));

	s2 = m_util_str_duplicate(m, s1);

	CuAssertStrEquals(tc, "Strings are equal", s1, s2);

	if(s2) {
		free(s2);
	}
	if(m) {
		free(m);
	}
}

void t_util___str_duplicate_null(CuTest *tc) {
	char *s2 = NULL;

	miniat *m;
	m = malloc(sizeof(miniat));

	s2 = m_util_str_duplicate(m, NULL);

	CuAssertStrEquals(tc, "String equals NULL", NULL, s2);

	if(s2) {
		free(s2);
	}
	if(m) {
		free(m);
	}
}
