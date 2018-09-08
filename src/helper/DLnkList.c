#include "DLnkList.h"
#include <stddef.h>

DLnkList_Item **
DLnkList_Append(
	DLnkList_Item **ppHead,
	DLnkList_Item *pItem)
{
	DLnkList_Item *pHead = *ppHead;
	// Append to the end of linked list
	if (pHead) {
		while (pHead->pNext) { pHead = pHead->pNext; }
		pItem->pPrev = pHead;
		pHead->pNext = pItem;
	}
	// else be the head of the linked list
	else {
		*ppHead = pItem;
	}
	return ppHead;
}

DLnkList_Item *
DLnkList_Remove(
	DLnkList_Item **ppHead,
	DLnkList_Item *pItem)
{
	// The best part about Double Linked List is that
	// we don't need the head to remove the item
	(void)ppHead;
	// Link the prev item with next item
	if (pItem->pPrev) {
		pItem->pPrev->pNext = pItem->pNext;
		pItem->pPrev = NULL;
	}
	// Link the next item with prev item
	if (pItem->pNext) {
		pItem->pNext->pPrev = pItem->pPrev;
		pItem->pNext = NULL;
	}
	return pItem;
}
