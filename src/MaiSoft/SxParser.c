#include "SxParser.h"

int SuixinParse__OnEndOfID(
	SuixinParseSpec const *pSpec, uint8_t const *pData,
	int iBegin, int iEnd, void **ppUserItem)
{
	uint8_t const *pID = &pData[iBegin];
	int const cbID = iEnd - iBegin;
	int usrRet = 0;
	if (cbID < 1) {
		return SuixinParseErr_IdExpected;
	}
	usrRet = pSpec->pfnOnID(pID, cbID, pSpec->user, ppUserItem);
	if ((usrRet & SuixinAbsRetValMask) == AbortSuixinParse) {
		return SuixinParseErr_UserAbort;
	}
	return SuixinParseErr_None;
}

int SuixinParse__OnEndOfContent(
	SuixinParseSpec const *pSpec, uint8_t const *pData,
	int iBegin, int iEnd, void *pUserItem)
{
	if (!pUserItem) {
		// Skip as user is not interested with this Content
	}
	else {
		uint8_t const *pCtnt = &pData[iBegin];
		int cbCtnt = iEnd - iBegin;
		int usrRet = 0;
		if (cbCtnt < 1) {
			pCtnt = NULL; cbCtnt = 0;
		}
		usrRet = pSpec->pfnOnContent(pCtnt, cbCtnt, pSpec->user, pUserItem);
		if ((usrRet & SuixinAbsRetValMask) == AbortSuixinParse) {
			return SuixinParseErr_UserAbort;
		}
	}
	return SuixinParseErr_None;
}

void SuixinParse(
	SuixinParseStat *pStat,
	SuixinParseSpec const *pSpec)
{
	enum { tokenMark = '`' };
	uint8_t const *pData = pSpec->pData;
	int const cbPerChar = pSpec->charIs16bit ? 2 : 1;
	bool expectID = true;
	bool lineCommentMode = false;
	uint16_t cCurr = 0, cLast = 0;
	int iCurr = 0, iBegin = 0, iEnd = 0;
	int iCol = 0, iRow = 0;
	void *pUserItem = NULL;
	pStat->error = SuixinParseErr_None;
	if (!(pSpec->pData && pSpec->pfnOnID && pSpec->pfnOnContent)) {
		pStat->error = SuixinParseErr_InvalidArg; goto eof;
	}
	for (iCurr = 0; iCurr < pSpec->cbData;)
	{
		cCurr = (pSpec->charIs16bit)
			? *(uint16_t*)(&pData[iCurr])
			: *(uint8_t*)(&pData[iCurr]);
		if (lineCommentMode) goto next;
		// expect ID to find relevant userItem
		if (expectID) {
			if (iCol == 0) switch (cCurr) {
			case '\n':
			case '\r':
			case tokenMark:
				goto next;
			default:
				pStat->error = SuixinParseErr_BadChar;
				goto eof;
			}
			else if (iCol > 0) switch (cCurr) {
			case '\n':
				if (cLast == '\r') goto next;
			case '\r':
				pStat->error = SuixinParse__OnEndOfID(
					pSpec, pData, iBegin, iCurr, &pUserItem);
				if (pStat->error) goto eof;
				// switch mode to expect Content
				expectID = false;
				iBegin = iEnd = 0;
				goto next;
			case ' ':
			case '\t':
				goto next;
			case tokenMark:
				if (cLast == tokenMark) {
					iBegin = iEnd = 0;
					lineCommentMode = true;
					goto next;
				}
			default:
				// mark the begin of ID
				if (!iBegin) {
					iBegin = iCurr;
				}
			}
			goto next;
		}
		// expect Content for current userItem
		else switch (cCurr) {
		case '\n':
			if (cLast == '\r') goto next;
		case '\r':
			// every EOL could be the end of Content
			// new EOL overrides old EOL for obvious reason
			if (iCol > 0) {
				iEnd = iCurr;
			}
			goto next;
		case tokenMark:
			// Content can have any char but if the first char of line
			// is tokenMark, consider end of Content
			if (iCol == 0) {
				pStat->error = SuixinParse__OnEndOfContent(
					pSpec, pData, iBegin, iEnd, pUserItem);
				if (pStat->error) goto eof;
				// switch mode to expect next ID
				expectID = true;
				iBegin = iEnd = 0;
				pUserItem = NULL;
			}
			goto next;
		default:
			// mark the begin of Content
			if (!iBegin) {
				iBegin = iCurr;
			}
			goto next;
		}
	next:
		switch (cCurr) {
		case '\n':
			if (cLast == '\r') break;
		case '\r':
			lineCommentMode = false;
			iCol = 0;
			++iRow;
			break;
		default:
			++iCol;
			break;
		}
		cLast = cCurr;
		iCurr += cbPerChar;
	}
	// end of file
	// unusual case: end of last ID
	if (expectID) {
		pStat->error = SuixinParse__OnEndOfID(
			pSpec, pData, iBegin, iCurr, &pUserItem);
		if (pStat->error || !pUserItem) goto eof;
		pStat->error = SuixinParse__OnEndOfContent(
			pSpec, pData, iCurr, iCurr, pUserItem);
	}
	else { // normal case: end of Content of the last ID
		pStat->error = SuixinParse__OnEndOfContent(
			pSpec, pData, iBegin, iCurr, pUserItem);
	}
eof:
	pStat->iRow = iRow;
	pStat->iCol = iCol;
	pStat->iByte = iCurr;
}
