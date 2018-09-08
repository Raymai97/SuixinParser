#ifndef APP_H
#define APP_H

#include "test.h"
#include <stdbool.h>

#define APP_NAME "Suixin Parser Test"

#ifndef UNUSED
#define UNUSED(x)		(void)(x)
#endif

enum AppConstant {
	PlatformIntMax = ((unsigned)-1) >> 1,
	StdInputMax = 999,
	AppMinLoopCount = 10,
	AppMaxLoopCount = PlatformIntMax
};

enum AppErr {
	AppErr_None,
	AppErr_OutOfMem,
	AppErr_TestFileNotSet,
	AppErr_TestErr
};

enum AppTestMode {
	TestMode_RunOnce,
	TestMode_Benchmark
};

enum AppTestClient {
	TestCli_C_1,
	TestCli_Cpp_1
};

typedef struct AppSelf AppSelf;
struct AppSelf {
	enum AppErr appErr;
	char szStdInput[StdInputMax];
	char *pszTestFile;
	enum AppTestMode testMode;
	int testLoopCount;
	enum AppTestClient testClient;
	TestStat testStat;
};

#define APP_SELF AppSelf *pSelf

char const *AppTextTestFile(char const *);
char const *AppTextTestMode(enum AppTestMode);
char const *AppTextTestClient(enum AppTestClient);

int AppGetLine(APP_SELF);
char AppGetChar(APP_SELF);

void AppPutTitle(char const *);
void AppPutLastErr(APP_SELF, char const *);

bool AppSetTestFilePath(APP_SELF, char const *);

#endif//APP_H
