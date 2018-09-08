#include "base.h"
#include <helper/astrcpy.h>
#include <helper/DLnkList.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// ----- Private declare -----

enum MyError {
	MyError_None,
	MyError_OutOfMem
};

typedef struct MyData MyData;
typedef struct MyItem MyItem;

struct MyData {
	MyItem *pListHead;
	enum MyError error;
};

struct MyItem {
	DLnkList_Item base;
	char *pszKey, *pszVal;
};

// ----- Specialize Double Linked List -----

static MyItem * MyAppendItem(MyItem **ppHead, MyItem *pItem)
{
	return (MyItem*)DLnkList_Append(
		(DLnkList_Item**)ppHead,
		(DLnkList_Item*)pItem
	);
}

static MyItem * MyRemoveItem(MyItem **ppHead, MyItem *pItem)
{
	return (MyItem*)DLnkList_Remove(
		(DLnkList_Item**)ppHead,
		(DLnkList_Item*)pItem
	);
}

static MyItem * MyNewItem(char const *pKey, int cbKey)
{
	MyItem *pItem = calloc(sizeof(MyItem), 1);
	if (!pItem) {
		return NULL;
	}
	pItem->pszKey = Astrlcpy(pKey, cbKey);
	if (!pItem->pszKey) {
		free(pItem);
		return NULL;
	}
	return pItem;
}

static void MyDeleteItem(MyItem *pItem)
{
	if (pItem->pszKey) {
		free(pItem->pszKey);
	}
	if (pItem->pszVal) {
		free(pItem->pszVal);
	}
	free(pItem);
}

// ----- Private implementation -----

static enum SuixinUserReturn MySx_OnID(
	uint8_t const *pID, int cbID, void *user, void **ppItem)
{
	// This callback is called when an ID is encountered.
	// 
	// Value of *ppItem, if set to non-NULL, will make the parser to
	// call the OnContent callback with its value.
	// Otherwise, it will skip the Content of current ID.
	// 
	// In this example, we allocate an Item with ID here,
	// pass the pointer of Item to OnContent callback via *ppItem.
	//
	MyData *pMy = user;
	MyItem **ppHead = &(pMy->pListHead);
	MyItem *pItem = MyNewItem((char const *)pID, cbID);
	if (!pItem) {
		pMy->error = MyError_OutOfMem;
		return AbortSuixinParse;
	}
	*ppItem = pItem;
	MyAppendItem(ppHead, pItem);
	return ContinueSuixinParse;
}

static enum SuixinUserReturn MySx_OnContent(
	uint8_t const *pCtnt, int cbCtnt, void *user, void *pItemRaw)
{
	// This callback is called when the Content of ID is encountered.
	// 
	// In this example, we simply refer back the Item of ID via *ppItem
	// that we set in our OnID callback. We will complete the Item
	// with the Content we got here.
	//
	MyData *pMy = user;
	MyItem **ppHead = &(pMy->pListHead);
	MyItem *pItem = pItemRaw;
	char *pszVal = Astrlcpy((char const *)pCtnt, cbCtnt);
	// If messed up, remove and delete the incomplete Item.
	if (!pszVal) {
		pMy->error = MyError_OutOfMem;
		MyDeleteItem(MyRemoveItem(ppHead, pItem));
		return AbortSuixinParse;
	}
	pItem->pszVal = pszVal;
	return ContinueSuixinParse;
}

static void MySxParseBuffer(
	uint8_t const * pData, int cbData,
	SuixinParseStat * pSxStat, void *pSxUser)
{
	SuixinParseSpec spec = { 0 };
	spec.pData = pData;
	spec.cbData = cbData;
	spec.charIs16bit = false;
	spec.user = pSxUser;
	spec.pfnOnID = MySx_OnID;
	spec.pfnOnContent = MySx_OnContent;
	SuixinParse(pSxStat, &spec);
}

static void MyEnumAndFreeItems(
	MyItem **ppHead, void *pTestUser)
{
	MyItem *pItem = *ppHead;
	while (pItem)
	{
		void *pNext = pItem->base.pNext;
		TestUser_OnItem(pTestUser,
			pItem->pszKey,
			pItem->pszVal);
		MyDeleteItem(MyRemoveItem(ppHead, pItem));
		pItem = pNext;
	}
	*ppHead = NULL;
}

extern void Test_C_1(
	TestStat * pStat, TestSpec const test)
{
	enum TestErr err = TestErr_None;
	FILE *pFile = NULL;
	long cbFile = 0;
	int cbData = 0;
	uint8_t *pData = NULL;
	int cbRead = 0;
	MyData my = { 0 };
	
	// Open the test file
	pFile = fopen(test.pszFile, "rb");
	if (!pFile) {
		err = TestErr_OpenFile;
		goto eof;
	}
	
	// Check the file size
	fseek(pFile, 0L, 2);
	cbFile = ftell(pFile);
	if (cbFile > 0xFFFF) {
		err = TestErr_FileTooBig;
		goto eof;
	}
	
	// Allocate buffer to store the file data
	cbData = (int)cbFile;
	pData = malloc(cbData);
	if (!pData) {
		err = TestErr_OutOfMem;
		goto eof;
	}
	
	// Read all file data to the buffer
	fseek(pFile, 0L, 0);
	cbRead = (int)fread(pData, 1, cbData, pFile);
	if (cbRead != cbData) {
		err = TestErr_ReadFileCbMismatch;
		goto eof;
	}

	// Parse and enum the Suixin items
	MySxParseBuffer(pData, cbData, &(pStat->sxParseStat), &my);
	if (my.error == MyError_OutOfMem) {
		err = TestErr_OutOfMem;
		goto eof;
	}
	if (pStat->sxParseStat.error) {
		err = TestErr_SxParseError;
		goto eof;
	}
	MyEnumAndFreeItems(&(my.pListHead), test.pUser);
eof:
	pStat->testErr = err;
	if (pData) {
		free(pData), pData = NULL;
	}
	if (pFile) {
		fclose(pFile), pFile = NULL;
	}
}
