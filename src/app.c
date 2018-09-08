#include "app.h"
#include <helper/astrcpy.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SELF (*(pSelf))

char const *AppTextTestFile(char const *pszFile)
{
	enum {
		MaxStrlen = 70,
		EllipsisCount = 6,
		MaxHalfLen = (MaxStrlen - EllipsisCount) / 2
	};
	static char szDisp[MaxStrlen + 1];
	if (pszFile)
	{
		char *p = szDisp;
		int i, len = (int)strlen(pszFile);
		if (len <= MaxStrlen) {
			return strcpy(szDisp, pszFile);
		}
		for (i = 0; i < MaxHalfLen; ++i) {
			*p++ = pszFile[i];
		}
		for (i = 0; i < EllipsisCount; ++i) {
			*p++ = '.';
		}
		for (i = (len - MaxHalfLen); i < len; ++i) {
			*p++ = pszFile[i];
		}
		return szDisp;
	}
	return "<not set>";
}

char const *AppTextTestMode(enum AppTestMode mode)
{
	if (mode == TestMode_RunOnce) {
		return "Run once, show output";
	}
	else if (mode == TestMode_Benchmark) {
		return "Benchmark time taken";
	}
	return "";
}

char const *AppTextTestClient(enum AppTestClient cli)
{
	if (cli == TestCli_C_1) {
		return "Sample 1 in C";
	}
	else if (cli == TestCli_Cpp_1) {
		return "Sample 1 in C++";
	}
	return "";
}



// Return byte count of character that user entered.
// Return -2 if CTRL+Z/CTRL+D, -1 if input too long.
int AppGetLine(APP_SELF)
{
	char * const pszInput = (SELF.szStdInput);
	int ret = -2;
	fprintf(stdout, "> ");
	if (fgets(pszInput, StdInputMax, stdin))
	{
		char *pNL = strchr(pszInput, '\n');
		if (pNL) {
			*pNL = 0;
			ret = (int)(pNL - pszInput);
			goto eof;
		}
		if (fgetc(stdin) == '\n') {
			ret = StdInputMax - 1;
			goto eof;
		}
		fprintf(stdout,
			"Input must be less than %i chars.\n",
			StdInputMax);
		while (fgetc(stdin) != '\n');
		ret = -1;
	}
eof:
	fprintf(stdout, "\n");
	return ret;
}

char AppGetChar(APP_SELF)
{
	return AppGetLine(&SELF) == 1 ? SELF.szStdInput[0] : 0;
}



void AppPutTitle(char const *pszTitle)
{
	size_t len = strlen(pszTitle);
	fprintf(stdout, "%s\n", pszTitle);
	for (; len-- > 0;) {
		fputc('-', stdout);
	}
	fprintf(stdout, "-\n");
}

void AppPutLastErr(APP_SELF, char const * pszHeader)
{
	enum AppErr const appErr = SELF.appErr;
	fprintf(stderr, "Error: ");
	if (pszHeader) {
		fprintf(stderr, "%s\n", pszHeader);
	}
	if (appErr == AppErr_TestErr) {
		TestStat const *pTestStat = &(SELF.testStat);
		enum TestErr const testErr = pTestStat->testErr;
		if (testErr == TestErr_SxParseError) {
			SuixinParseStat const *pStat = &(pTestStat->sxParseStat);
			enum SuixinParseErr const sxErr = pStat->error;
			fprintf(stderr, "Suixin Parser %s\n",
				sxErr == SuixinParseErr_InvalidArg ?
				"complained about invalid argument." :
				sxErr == SuixinParseErr_Unexpected ?
				"encountered an unexpected error" :
				sxErr == SuixinParseErr_UserAbort ?
				"stopped because user abort" :
				sxErr == SuixinParseErr_BadChar ?
				"encountered an illegal char" :
				sxErr == SuixinParseErr_IdExpected ?
				"was expecting a missing ID" :
				"failed for unknown reason."
			);
			switch (sxErr) {
			case SuixinParseErr_Unexpected:
			case SuixinParseErr_UserAbort:
			case SuixinParseErr_BadChar:
			case SuixinParseErr_IdExpected:
				fprintf(stderr, "when processing {\n"
					"    iRow: %i, iCol: %i, iByte: %i \n"
					"}.",
					pStat->iRow, pStat->iCol, pStat->iByte
				);
				break;
			}
		} // ... testErr == TestErr_SxParseError
		else {
			fprintf(stderr, "Test client %s.",
				testErr == TestErr_OutOfMem ?
				"failed to allocate memory" :
				testErr == TestErr_OpenFile ?
				"failed to open file" :
				testErr == TestErr_FileTooBig ?
				"think the file is too big" :
				testErr == TestErr_ReadFileCbMismatch ?
				"failed to read all desired file bytes" :
				"failed for unknown reason"
			);
		}
	} // ... appErr == AppErr_TestErr
	else {
		fprintf(stderr, "%s.",
			appErr == AppErr_OutOfMem ?
			"Failed to allocate memory" :
			appErr == AppErr_TestFileNotSet ?
			"Test File not set" :
			"Failed for unknown reason"
		);
	}
	fprintf(stderr, "\n\n");
}



bool AppSetTestFilePath(APP_SELF, char const * pszVal)
{
	char *psz = Astrcpy(pszVal);
	if (!psz) {
		SELF.appErr = AppErr_OutOfMem;
		AppPutLastErr(&SELF, "Failed to set test file path.");
		return false;
	}
	if (SELF.pszTestFile) {
		free(SELF.pszTestFile);
	}
	SELF.pszTestFile = psz;
	return true;
}


