#!/usr/bin/python
# -*- coding: utf-8 -*-
from optparse import OptionParser
import re
import os
import shutil


class GenTest(object):

  def __init__(self, driver, src, function_prefix):
    self._src = src
    self._function_prefix = function_prefix.replace('\\', r'\\\\')
    self._driver_contents = [line.rstrip() for line in open('vm/test/CuTest/test.c')]
    self._out = open(driver, 'w')

  # Open the file with the provided name and parse it for lines that start with
  # the identified prefix. Use these lines to figure out the actual names of 
  # all the test functions in the file and aggregate the names into a list. 
  def _gen_tests_from_file(self, file_name):
    f = open(file_name, 'r').readlines()
    funcs = [l.split('(')[0].split()[1] for l in f if re.search(self._function_prefix, l)]
    return funcs




  # dynamically insert the suite additions and the function signatures for each function
  # in the input list at the marked locations in out's copy of the test driver.
  def _write_output(self, tests):
    replacements =  {
      'function_signatures': (lambda func_name: 'extern void ' + func_name + '(CuTest *);'),
      'suite_additions': (lambda func_name: '\tSUITE_ADD_TEST(suite, ' + func_name + ');')
    }

    for keyword, mapping_func in replacements.items():
      insertion_point = re.compile('{{{_'+ keyword + '_}}}', re.I)
      i = next(i + 1 for i, string in enumerate(self._driver_contents) if insertion_point.search(string))
      self._driver_contents[i:i] = [mapping_func(func_name) for func_name in tests]
      
    self._out.write('\n'.join(self._driver_contents))
    self._out.close();




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
   
    self._write_output(tests) 





if __name__ == '__main__':
  parser = OptionParser()
  parser.add_option(
    '-t',
    '--test-driver',
    dest='testdriver',
    help='The C file running the tests. (this is the output file)',
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
  gt = GenTest(options.testdriver, args, options.function_prefix)
  gt.generate()  


