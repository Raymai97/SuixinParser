#ifndef TEST_H
#define TEST_H

#include <MaiSoft/SxParser.h>

enum TestErr {
	TestErr_None = 0,
	TestErr_OutOfMem,
	TestErr_OpenFile,
	TestErr_FileTooBig,
	TestErr_ReadFileCbMismatch,
	TestErr_SxParseError
};

typedef struct TestSpec TestSpec;
struct TestSpec {
	char const *pszFile;
	void *pUser;
};

typedef struct TestStat TestStat;
struct TestStat {
	SuixinParseStat sxParseStat;
	enum TestErr testErr;
};

typedef void(TestFuncType)(
	TestStat * pStat, TestSpec const spec
);
TestFuncType Test_C_1, Test_Cpp_1;

void TestUser_OnItem(
	void *pUser, char const *pszKey, char const *pszVal
);

#endif//TEST_H
