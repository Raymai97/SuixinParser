#include "app.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <helper/astrcpy.h>
#include <helper/TimeTake.h>

static bool MyDoTest(AppSelf *pSelf);
static void MyDoBenchmark(AppSelf *pSelf);
static void MyAskSetTestCli(AppSelf *pSelf);
static void MyAskSetTestMode(AppSelf *pSelf);

int main(int argc, char **argv)
{
	AppSelf self = { 0 }, *pSelf = &self;
	pSelf->testLoopCount = AppMinLoopCount;
	fprintf(stdout, "Welcome to %s" "\n\n", APP_NAME);
	if (argc > 1) {
		if (!AppSetTestFilePath(pSelf, argv[1])) goto eof;
	}
	while (true) {
		AppPutTitle(APP_NAME);
		fprintf(stdout,
			"[1]    Client: %s" "\n",
			AppTextTestClient(pSelf->testClient)
		);
		fprintf(stdout,
			"[2] Test File: %s" "\n",
			AppTextTestFile(pSelf->pszTestFile)
		);
		fprintf(stdout,
			"[3] Test Mode: %s",
			AppTextTestMode(pSelf->testMode)
		);
		if (pSelf->testMode == TestMode_Benchmark) {
			fprintf(stdout, " for %i iteration", pSelf->testLoopCount);
		}
		fprintf(stdout, "\n");
		fprintf(stdout,
			"~ [b] Begin test   [q] Quit" "\n"
		);
		switch (AppGetChar(pSelf)) {
		case '1':
			MyAskSetTestCli(pSelf);
			continue;
		case '2':
			fprintf(stdout,
				"Enter new Test File path" "\n");
			if (AppGetLine(pSelf) > 0) {
				if (!AppSetTestFilePath(pSelf, pSelf->szStdInput)) goto eof;
			}
			continue;
		case '3':
			MyAskSetTestMode(pSelf);
			continue;
		case 'b': case 'B':
			switch (pSelf->testMode) {
			case TestMode_RunOnce:
				MyDoTest(pSelf);
				continue;
			case TestMode_Benchmark:
				MyDoBenchmark(pSelf);
				continue;
			}
			continue;
		case 'q': case 'Q':
			fprintf(stdout, "Bye." "\n");
			goto eof;
		}
	}
eof:
	return pSelf->appErr ? 1 : 0;
}

void TestUser_OnItem(
	void *pUser, char const *pszKey, char const *pszVal)
{
	AppSelf *pSelf = pUser;
	switch (pSelf->testMode) {
	case TestMode_RunOnce:
		fprintf(stdout, "[%s]" "\n", pszKey);
		fprintf(stdout, "%s" "\n\n", pszVal);
		break;
	}
}

static bool MyDoTest(AppSelf *pSelf)
{
	TestStat *pStat = &(pSelf->testStat);
	TestSpec testSpec = { 0 };
	if (!pSelf->pszTestFile) {
		pSelf->appErr = AppErr_TestFileNotSet;
		goto gg;
	}
	testSpec.pszFile = pSelf->pszTestFile;
	testSpec.pUser = pSelf;
	switch (pSelf->testClient) {
	case TestCli_C_1:
		Test_C_1(pStat, testSpec);
		break;
	case TestCli_Cpp_1:
		Test_Cpp_1(pStat, testSpec);
		break;
	}
	if (pStat->testErr) {
		pSelf->appErr = AppErr_TestErr;
		goto gg;
	}
	return true;
gg:
	AppPutLastErr(pSelf, NULL);
	return false;
}

static void MyDoBenchmark(AppSelf *pSelf)
{
	int i; TTtime t1, t2;
	t1 = TimeTakeNow();
	for (i = 0; i < pSelf->testLoopCount; ++i) {
		if (!MyDoTest(pSelf)) { return; }
	}
	t2 = TimeTakeNow();
	AppPutTitle("Benchmark result");
	fprintf(stdout,
		"Time taken: %i milliseconds" "\n\n",
		(int)TimeTakenInMs(t1, t2));
}

static void MyAskSetTestCli(AppSelf *pSelf)
{
	enum AppTestClient const clients[] = {
		TestCli_C_1, TestCli_Cpp_1
	}; char c;
	fprintf(stdout,
		"Set new Test Client" "\n"
		"[1] %s" "\n"
		"[2] %s" "\n",
		AppTextTestClient(clients[0]),
		AppTextTestClient(clients[1]));
	switch (c = AppGetChar(pSelf), c) {
	case '1':
	case '2':
		pSelf->testClient = clients[c - '1'];
		break;
	}
}

static void MyAskSetTestMode(AppSelf *pSelf)
{
	enum AppTestMode const modes[] = {
		TestMode_RunOnce, TestMode_Benchmark
	}; char c;
	fprintf(stdout,
		"Set new Test Mode" "\n"
		"[1] %s" "\n"
		"[2] %s" "\n",
		AppTextTestMode(modes[0]),
		AppTextTestMode(modes[1])
	);
	switch (c = AppGetChar(pSelf), c) {
	case '1':
	case '2':
		pSelf->testMode = modes[c - '1'];
		switch (pSelf->testMode) {
		case TestMode_Benchmark:
			fprintf(stdout,
				"Iteration count? now %i, min %i, max %i\n",
				pSelf->testLoopCount, AppMinLoopCount, AppMaxLoopCount
			);
			if (AppGetLine(pSelf) > 0) {
				int val = (int)strtol(pSelf->szStdInput, NULL, 10);
				if (val < AppMinLoopCount) {
					val = AppMinLoopCount;
				}
				else if (val > AppMaxLoopCount) {
					val = AppMaxLoopCount;
				}
				pSelf->testLoopCount = val;
			}
			break;
		}
		break;
	}
}

