#ifndef MAISOFT_SUIXIN_PARSER_H
#define MAISOFT_SUIXIN_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum SuixinParseErr {
	SuixinParseErr_None = 0,
	SuixinParseErr_InvalidArg,
	SuixinParseErr_Unexpected,
	SuixinParseErr_UserAbort,
	SuixinParseErr_BadChar,
	SuixinParseErr_IdExpected
} error;

typedef struct SuixinParseStat SuixinParseStat;
struct SuixinParseStat {
	enum SuixinParseErr error;
	int iRow, iCol, iByte;
};

enum SuixinUserReturn {
	// Absolute return value:
	AbortSuixinParse = 0,
	ContinueSuixinParse,
	// Modifier flags of return value:
	// (none at the moment)
	// Mask to get absolute return value:
	SuixinAbsRetValMask = (1 << 5) - 1
};

typedef struct SuixinParseSpec SuixinParseSpec;
struct SuixinParseSpec {
	uint8_t const *pData;
	int cbData;
	bool charIs16bit;
	void *user;
	enum SuixinUserReturn(*pfnOnID)(
		uint8_t const *pID, int cbID, void *user, void **ppItem);
	enum SuixinUserReturn(*pfnOnContent)(
		uint8_t const *pCtnt, int cbCtnt, void *user, void *pItem);
};

void SuixinParse(SuixinParseStat *, SuixinParseSpec const *);

#endif//MAISOFT_SUIXIN_PARSER_H
