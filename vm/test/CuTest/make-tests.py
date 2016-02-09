#!/usr/bin/python
# -*- coding: utf-8 -*-

from optparse import OptionParser
import re
import sys


class GenTest(object):

    def __init__(
        self,
        outfile,
        src,
        function_prefix,
        ):

        self._src = src
        self._outfile = outfile
        self._function_prefix = function_prefix.replace('\\', r'\\\\')

    def generate(self):

        funcs = list()
        self._outfile.write('/* This is auto-generated code. Edit at your own peril. */\n'
                             + '#include <stdio.h>\n'
                            + '#include "CuTest.h"\n')

        for fn in self._src:
            f = open(fn, 'r')
            for line in f:
                m = re.search(self._function_prefix, line)
                if m != None:
                    line = re.sub(r'\(CuTest \*.*', r'', line)
                    funcs.append(line.strip())

        for func in funcs:
            self._outfile.write('extern ' + func + '(CuTest *);\n')

        self._outfile.write('\n')
        self._outfile.write('void RunAllTests(void)\n' + '{\n'
                            + '    int i;\n' + '    FILE *fp;\n'
                            + '    CuSuite* suite = CuSuiteNew();\n')

        for func in funcs:
            self._outfile.write('    SUITE_ADD_TEST(suite, '
                                + func.split()[1] + ');\n')

        self._outfile.write('\n')
        self._outfile.write('''    CuSuiteRun(suite);

'''
                            + '''    fp = fopen(UNITTESTLOG, "a+");
    if(!fp) {
'''
                            + '        return;\n' + '''    }

'''
                            + '    for(i = 0; i < suite->count; i++) {\n'

                            + '        if(suite->list[i]->assertsFailedCount) {\n'
                             + '            fprintf(fp, "Failed\\t");\n'
                             + '        }\n' + '        else {\n'
                            + '            fprintf(fp, "Passed\\t");\n'
                            + '        }\n'
                            + '        fprintf(fp, "%s\\t", suite->list[i]->name);\n'

                            + '        fprintf(fp, "%d\\t", suite->list[i]->assertsPassedCount);\n'

                            + '        fprintf(fp, "%d\\t", suite->list[i]->assertsFailedCount);\n'

                            + '        fprintf(fp, "%d\\t", (suite->list[i]->assertsPassedCount + suite->list[i]->assertsFailedCount));\n'

                            + '        fprintf(fp, "%3.3f\\n", (((float)(suite->list[i]->assertsPassedCount) / (suite->list[i]->assertsPassedCount + suite->list[i]->assertsFailedCount)) * 100));\n'
                             + '''    }

    fflush(fp);
'''
                            + '''    fclose(fp);

'''
                            + '''    fp = fopen(SUITELOG, "a+");

'''
                            + '    if(suite->testFailedCount) {\n'
                            + '        fprintf(fp, "Failed\\t");\n'
                            + '    }\n' + '    else {\n'
                            + '        fprintf(fp, "Passed\\t");\n'
                            + '    }\n'
                            + '    fprintf(fp, "%d\\t", suite->testPassedCount);\n'

                            + '    fprintf(fp, "%d\\t", suite->testFailedCount);\n'

                            + '    fprintf(fp, "%d\\t", suite->count);\n'

                            + '    fprintf(fp, "%3.3f\\n", ((float)(suite->testPassedCount) / suite->count) * 100);\n'
                             + '    fflush(fp);\n'
                            + '''    fclose(fp);
}

'''
                            + 'int main(int argc, char *argv[])\n' + '{\n'
                            + '    RunAllTests();\n' + '    return 0;\n'
                             + '}\n')
        self._outfile.close()


if __name__ == '__main__':

    parser = OptionParser()
    parser.add_option(
        '-o',
        '--output',
        dest='outputfile',
        help='Write func main to file, default is stdout',
        metavar='OUTPUTFILE',
        default='stdout',
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

    f = sys.stdout
    if options.outputfile != 'stdout':
        f = open(options.outputfile, 'w+')

    gt = GenTest(f, args, options.function_prefix)
    gt.generate()