#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "CuTest.h"

/*-------------------------------------------------------------------------*
 * CuStr
 *-------------------------------------------------------------------------*/

char* CuStrAlloc(int size)
{
	char* newStr = (char*) malloc( sizeof(char) * (size) );
	return newStr;
}

char* CuStrCopy(const char* old)
{
	int len = strlen(old);
	char* newStr = CuStrAlloc(len + 1);
	strcpy(newStr, old);
	return newStr;
}

/*-------------------------------------------------------------------------*
 * CuString
 *-------------------------------------------------------------------------*/

void CuStringInit(CuString* str)
{
	str->length = 0;
	str->size = STRING_MAX;
	str->buffer = (char*) malloc(sizeof(char) * str->size);
	str->buffer[0] = '\0';
}

CuString* CuStringNew(void)
{
	CuString* str = (CuString*) malloc(sizeof(CuString));
	str->length = 0;
	str->size = STRING_MAX;
	str->buffer = (char*) malloc(sizeof(char) * str->size);
	str->buffer[0] = '\0';
	return str;
}

void CuStringResize(CuString* str, int newSize)
{
	str->buffer = (char*) realloc(str->buffer, sizeof(char) * newSize);
	str->size = newSize;
}

void CuStringAppend(CuString* str, const char* text)
{
	int length;

	if (text == NULL) {
		text = "NULL";
	}

	length = strlen(text);
	if (str->length + length + 1 >= str->size)
		CuStringResize(str, str->length + length + 1 + STRING_INC);
	str->length += length;
	strcat(str->buffer, text);
}

void CuStringAppendChar(CuString* str, char ch)
{
	char text[2];
	text[0] = ch;
	text[1] = '\0';
	CuStringAppend(str, text);
}

void CuStringAppendFormat(CuString* str, const char* format, ...)
{
	va_list argp;
	char buf[HUGE_STRING_LEN];
	va_start(argp, format);
	vsprintf(buf, format, argp);
	va_end(argp);
	CuStringAppend(str, buf);
}

void CuStringInsert(CuString* str, const char* text, int pos)
{
	int length = strlen(text);
	if (pos > str->length)
		pos = str->length;
	if (str->length + length + 1 >= str->size)
		CuStringResize(str, str->length + length + 1 + STRING_INC);
	memmove(str->buffer + pos + length, str->buffer + pos, (str->length - pos) + 1);
	str->length += length;
	memcpy(str->buffer + pos, text, length);
}

void CuStringPrint(const char* result, const char* name, const char* file, int line, const char* string) {

	FILE *fp;
	fp = fopen(ASSERTIONLOG, "a+");
	if(fp) {
		fprintf(fp, "%s\t%s\t%s\t%d\t%s\n", result, name, file, line, string);
		fflush(fp);
		fclose(fp);
	}
	return;
}

/*-------------------------------------------------------------------------*
 * CuTest
 *-------------------------------------------------------------------------*/

void CuTestInit(CuTest* t, const char* name, TestFunction function)
{
	t->name = CuStrCopy(name);
	t->assertsFailedCount = 0;
	t->assertsPassedCount = 0;
	t->ran = 0;
	t->function = function;
	t->jumpBuf = NULL;
}

CuTest* CuTestNew(const char* name, TestFunction function)
{
	CuTest* tc = CU_ALLOC(CuTest);
	CuTestInit(tc, name, function);
	return tc;
}

void CuTestRun(CuTest* tc)
{
	jmp_buf buf;
	tc->jumpBuf = &buf;
	if (setjmp(buf) == 0)
	{
		tc->ran = 1;
		(tc->function)(tc);
	}
	tc->jumpBuf = 0;
}

static void CuFailInternal(CuTest* tc, const char* file, int line, const char* string)
{
	CuStringPrint("Failed", tc->name, file, line, string);

	tc->assertsFailedCount++;
	if (tc->jumpBuf != 0) longjmp(*(tc->jumpBuf), 0);
}

static void CuPassInternal(CuTest* tc, const char* file, int line, const char* string)
{
	tc->assertsPassedCount++;
	CuStringPrint("Passed", tc->name, file, line, string);
	return;
}

void CuAssert_LineMsg(CuTest* tc, const char* file, int line, const char* message, int condition)
{
	if(condition) {
		CuPassInternal(tc, file, line, message);
	}
	else {
		CuFailInternal(tc, file, line, message);
	}
	return;
}

void CuAssertStrEquals_LineMsg(CuTest* tc, const char* file, int line, const char* message,
	const char* expected, const char* actual)
{
	if((expected == NULL && actual == NULL) || (expected != NULL && actual != NULL &&
	  strcmp(expected, actual) == 0)) {
		CuPassInternal(tc, file, line, message);
	}
	else {
		CuFailInternal(tc, file, line, message);
	}
}

void CuAssertIntEquals_LineMsg(CuTest* tc, const char* file, int line, const char* message,
	int expected, int actual)
{
	if (expected == actual) {
		CuPassInternal(tc, file, line, message);
	}
	else {
		CuFailInternal(tc, file, line, message);
	}
	return;
}

void CuAssertDblEquals_LineMsg(CuTest* tc, const char* file, int line, const char* message,
	double expected, double actual)
{
	if(expected == actual) {
		CuPassInternal(tc, file, line, message);
	}
	else {
		CuFailInternal(tc, file, line, message);
	}
}

void CuAssertPtrEquals_LineMsg(CuTest* tc, const char* file, int line, const char* message,
	void* expected, void* actual)
{
	if (expected == actual) {
		CuPassInternal(tc, file, line, message);
	}
	else {
		CuFailInternal(tc, file, line, message);
	}
}


/*-------------------------------------------------------------------------*
 * CuSuite
 *-------------------------------------------------------------------------*/

void CuSuiteInit(CuSuite* testSuite)
{
	testSuite->count = 0;
	testSuite->testFailedCount = 0;
	testSuite->testPassedCount = 0;
}

CuSuite* CuSuiteNew(void)
{
	CuSuite* testSuite = CU_ALLOC(CuSuite);
	CuSuiteInit(testSuite);
	return testSuite;
}

void CuSuiteAdd(CuSuite* testSuite, CuTest *testCase)
{
	assert(testSuite->count < MAX_TEST_CASES);
	testSuite->list[testSuite->count] = testCase;
	testSuite->count++;
}

void CuSuiteAddSuite(CuSuite* testSuite, CuSuite* testSuite2)
{
	int i;
	for (i = 0 ; i < testSuite2->count ; ++i)
	{
		CuTest* testCase = testSuite2->list[i];
		CuSuiteAdd(testSuite, testCase);
	}
}

void CuSuiteRun(CuSuite* testSuite)
{
	int i;
	for (i = 0 ; i < testSuite->count ; ++i)
	{
		CuTest* testCase = testSuite->list[i];
		CuTestRun(testCase);
		if (testCase->assertsFailedCount) { testSuite->testFailedCount += 1; }
		else { testSuite->testPassedCount += 1; }
	}
}
