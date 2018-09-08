#ifndef DLNKLIST_H
#define DLNKLIST_H

// Simple Double Linked List for C

typedef struct DLnkList_Item DLnkList_Item;
struct DLnkList_Item {
	DLnkList_Item *pPrev, *pNext;
};

typedef int (*DLnkList_Comparer)(
	DLnkList_Item const *p1, DLnkList_Item const *p2
);

// Always return ppHead
DLnkList_Item **
DLnkList_Append(
	DLnkList_Item **ppHead,
	DLnkList_Item *pItem
);

// Return the item after removing it from the list
DLnkList_Item *
DLnkList_Remove(
	DLnkList_Item **ppHead,
	DLnkList_Item *pItem
);

#endif//DLNKLIST_H
