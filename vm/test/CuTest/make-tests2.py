#!/usr/bin/python
# -*- coding: utf-8 -*-
from optparse import OptionParser
import re
import os
import shutil


class GenTest(object):

	def __init__(self, out_template, src, function_prefix):
		self._src = src
		self._function_prefix = function_prefix.replace('\\', r'\\\\')
		self._header = open(out_template + '.h', 'w')
		self._calls  = open(out_template + '.c', 'w')
		self._setup_files();


	# Open the file with the provided name and parse it for lines that start with
	# the identified prefix. Use these lines to figure out the actual names of 
	# all the test functions in the file and aggregate the names into a list. 
	def _gen_tests_from_file(self, file_name):
		f = open(file_name, 'r').readlines()
		funcs = [l.split('(')[0].split()[1] for l in f if re.search(self._function_prefix, l)]
		return funcs



	# Skeletonize the C and header files. 
	def _setup_files(self):
		lines = [
			'/* Auto-generated code! Modify at your own peril. */',
			'#include "CuTest.h"',
			'#include "test_suites.h"'
			'',
			'void add_all_tests(CuSuite* suite) {', ''
		]
		self._calls.write('\n'.join(lines))
		lines = [
			'/* Auto-generated code! Modify at your own peril. */',
			'',
			'extern void add_all_tests(CuSuite *);', ''
		]
		self._header.write('\n'.join(lines))



	# Write a test function's signature to the header file, provided its name. 
	def _gen_declaration(self, func):
		self._header.write('extern void ' + func + '(CuTest *);\n')



	# Write the function call to add a test to the suite, provided the name of the test function. 
	def _gen_call(self, func):
		self._calls.write('\tSUITE_ADD_TEST(suite, ' + func + ');\n')



	# Go through every file in the _src list and parse it for test function definitions. 
	# Aggregate the definitions into a list, then use the list to generate a .h file 
	# containing the function signatures and a .c file defining a function named:
	# add_all_tests(), which adds all the tests in the aggregated list to the suite.
	def generate(self):
		tests = []
		for file_name in self._src:
			tests_in_this_file = self._gen_tests_from_file(file_name)
			if tests_in_this_file:
				tests += tests_in_this_file 
		
		for test in tests:
			self._gen_declaration(test)
			self._gen_call(test)
			
		self._calls.write('}\n')
		self._calls.close()
		self._header.close()





if __name__ == '__main__':
	parser = OptionParser()
	parser.add_option(
		'-t',
		'--test-driver',
		dest='testdriver',
		help='The C file running the tests.',
		metavar='TESTDRIVER'
	)
	parser.add_option(
		'-p',
		'--prefix',
		dest='function_prefix',
		help='Use prefix to find test functions in src files',
		metavar='FUNCPREFIX',
		default='^void t_.*',
	)
	(options, args) = parser.parse_args()
	dir  = os.path.dirname(options.testdriver)
	path = os.path.join(dir, 'test_suites')
	gt   = GenTest(path, args, options.function_prefix)
	gt.generate()   

	#lastly, we need to copy the driver over
	shutil.copyfile('vm/test/CuTest/test.c', options.testdriver) 

