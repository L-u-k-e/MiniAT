/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include "CuTest.h"

void RunAllTests(void)
{
    int i;
    FILE *fp;
    CuSuite* suite = CuSuiteNew();

    CuSuiteRun(suite);

    fp = fopen(UNITTESTLOG, "a+");
    if(!fp) {
        return;
    }

    for(i = 0; i < suite->count; i++) {
        if(suite->list[i]->assertsFailedCount) {
            fprintf(fp, "Failed\t");
        }
        else {
            fprintf(fp, "Passed\t");
        }
        fprintf(fp, "%s\t", suite->list[i]->name);
        fprintf(fp, "%d\t", suite->list[i]->assertsPassedCount);
        fprintf(fp, "%d\t", suite->list[i]->assertsFailedCount);
        fprintf(fp, "%d\t", (suite->list[i]->assertsPassedCount + suite->list[i]->assertsFailedCount));
        fprintf(fp, "%3.3f\n", (((float)(suite->list[i]->assertsPassedCount) / (suite->list[i]->assertsPassedCount + suite->list[i]->assertsFailedCount)) * 100));
    }

    fflush(fp);
    fclose(fp);

    fp = fopen(SUITELOG, "a+");

    if(suite->testFailedCount) {
        fprintf(fp, "Failed\t");
    }
    else {
        fprintf(fp, "Passed\t");
    }
    fprintf(fp, "%d\t", suite->testPassedCount);
    fprintf(fp, "%d\t", suite->testFailedCount);
    fprintf(fp, "%d\t", suite->count);
    fprintf(fp, "%3.3f\n", ((float)(suite->testPassedCount) / suite->count) * 100);
    fflush(fp);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    RunAllTests();
    return 0;
}
