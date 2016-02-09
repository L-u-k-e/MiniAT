#include <stdio.h>
#include "CuTest.h"
#include "test_suites.h"

void RunAllTests(void) {
	CuSuite* suite = CuSuiteNew();
	add_all_tests(suite);
	CuSuiteRun(suite);
}

int main(int argc, char *argv[]) {
	RunAllTests();
	return 0;
}
