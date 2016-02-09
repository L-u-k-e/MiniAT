/* Auto-generated code! Modify at your own peril. */
#include "CuTest.h"
#include "test_suites.h"
void add_all_tests(CuSuite* suite) {
	SUITE_ADD_TEST(suite, t_exec___branch_delay);
	SUITE_ADD_TEST(suite, t_error___exec_bus_read);
	SUITE_ADD_TEST(suite, t_error___exec_bus_write);
	SUITE_ADD_TEST(suite, t_error___pins_interface_test);
	SUITE_ADD_TEST(suite, t_error___exec_store_load);
	SUITE_ADD_TEST(suite, t_error___exec_timers_time);
	SUITE_ADD_TEST(suite, t_error___exec_timers_count);
	SUITE_ADD_TEST(suite, t_error___exec_timers_toggle);
	SUITE_ADD_TEST(suite, t_error___exec_timers_pwm);
	SUITE_ADD_TEST(suite, t_error___exec_watchdog);
	SUITE_ADD_TEST(suite, t_file___read_block_count);
	SUITE_ADD_TEST(suite, t_file___write_block_count);
	SUITE_ADD_TEST(suite, t_file___read_block);
	SUITE_ADD_TEST(suite, t_file___write_block);
	SUITE_ADD_TEST(suite, t_pc___remove_first);
	SUITE_ADD_TEST(suite, t_pc___add_last);
	SUITE_ADD_TEST(suite, t_pc___clear);
	SUITE_ADD_TEST(suite, t_registers___read);
	SUITE_ADD_TEST(suite, t_registers___write);
	SUITE_ADD_TEST(suite, t_registers___pc);
	SUITE_ADD_TEST(suite, t_util___str_duplicate_simple);
	SUITE_ADD_TEST(suite, t_util___str_duplicate_null);
}
