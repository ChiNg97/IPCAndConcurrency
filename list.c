#include "list.h"
#include <stdlib.h>

#define totalNodesSize 100
#define headListSize 10

static node nodes[totalNodesSize];
static LIST heads[headListSize];
static LIST* avaiHeadChain;
static node* avaiNodeChain;
static int init = 0;

void initChain() {
	if (init == 0) {
		for (int i = 0; i < headListSize; i++) {
			if (i == headListSize - 1) {
				heads[i].next = NULL;
			}
			else {
				heads[i].next = &heads[i + 1];
			}

		}
		for (int i = 0; i < totalNodesSize; i++) {
			if (i == totalNodesSize - 1) {
				nodes[i].next = NULL;
			}
			else {
				nodes[i].next = &nodes[i + 1];
			}
		}
		avaiHeadChain = &heads[0];
		avaiNodeChain = &nodes[0];
		init = 1;
	}
}

LIST* ListCreate() {
	initChain();
	if (avaiHeadChain != NULL) {
		//Head points to the next free node from node pool
		LIST* tempList = avaiHeadChain;
		tempList->count = 0;
		tempList->currentPtr = NULL;
		tempList->firstPtr = NULL;
		tempList->lastPtr = NULL;
		//Adjust to next free head and node
		avaiHeadChain = avaiHeadChain->next;
		return tempList;
	}
	else {
		return NULL;
	}
}

int ListCount(LIST* list) {
	return list->count;
}

void* ListFirst(LIST* list) {
	list->currentPtr = list->firstPtr;
	return list->firstPtr->item;
} 

void* ListLast(LIST* list) {
	list->currentPtr = list->lastPtr;
	return list->lastPtr->item;
}

// Private helper for next
void next(LIST* list)
{
	if (list->currentPtr != NULL)
	{
		if (list->currentPtr == list->lastPtr)
		{
			list->currentPtr = NULL;
			list->currState = AFTER;
		}
		else
		{
			list->currentPtr = list->currentPtr->next;
			list->currState = NORMAL;
		}
	}
}

void* ListNext(LIST* list) {
	next(list);
	if (list->currentPtr == NULL)
	{
		return NULL;
	}
	return list->currentPtr->item;
}

// Private helper for previous
void prev(LIST* list)
{
	if (list->currentPtr != NULL)
	{
		if (list->currentPtr == list->firstPtr)
		{
			list->currentPtr = NULL;
			list->currState = BEFORE;
		}
		else
		{
			list->currentPtr = list->currentPtr->prev;
			list->currState = NORMAL;
		}
	}
}

void* ListPrev(LIST* list) {
	prev(list);
	if (list->currentPtr == NULL)
	{
		return NULL;
	}
	return list->currentPtr->item;
}

void* ListCurr(LIST* list) {
	if (list->currentPtr == NULL) {
		return NULL;
	}
	return list->currentPtr->item;
}

int ListAdd(LIST* list, void* item) {
	if (avaiNodeChain != NULL)
	{
		node* newNode = avaiNodeChain;
		avaiNodeChain = avaiNodeChain->next;
		newNode->item = item;

		// Normal case
		// Add to after currentPtr
		if (list->currentPtr != NULL)
		{
			newNode->prev = list->currentPtr;
			newNode->next = list->currentPtr->next;
			list->currentPtr->next = newNode;
		}
		// List empty
		else if (list->firstPtr == NULL && list->lastPtr == NULL)
		{
			newNode->prev = NULL;
			newNode->next = NULL;
			list->firstPtr = newNode;
			list->lastPtr = newNode;
			list->currentPtr = newNode;
		}
		// current is before first item
		else if (list->currState == BEFORE)
		{
			newNode->prev = NULL;
			newNode->next = list->firstPtr;
			list->firstPtr->prev = newNode;
			list->firstPtr = newNode;
			list->currState = NORMAL;
		}
		else if (list->currState == AFTER)
		{
			newNode->next = NULL;
			newNode->prev = list->lastPtr;
			list->lastPtr->next = newNode;
			list->lastPtr = newNode;
			list->currState = NORMAL;
		}

		list->currentPtr = newNode;
		list->count++;
		return 0;
	}
	else
	{
		return -1;
	}
}

int ListInsert(LIST* list, void* item) {
	if (avaiNodeChain != NULL) 
	{
		node* newNode = avaiNodeChain;
		avaiNodeChain = avaiNodeChain->next;
		newNode->item = item;

		// Normal case
		// Add to after currentPtr
		if (list->currentPtr != NULL)
		{
			newNode->prev = list->currentPtr->prev;
			newNode->next = list->currentPtr;
			list->currentPtr->prev = newNode;
		}
		// List empty
		else if (list->firstPtr == NULL && list->lastPtr == NULL)
		{
			newNode->prev = NULL;
			newNode->next = NULL;
			list->firstPtr = newNode;
			list->lastPtr = newNode;
		} 
		// current is before first item
		else if (list->currState == BEFORE)
		{
			newNode->prev = NULL;
			newNode->next = list->firstPtr;
			list->firstPtr->prev = newNode;
			list->firstPtr = newNode;
			list->currState = NORMAL;
		} 
		else if (list->currState == AFTER)
		{
			newNode->next = NULL;
			newNode->prev = list->lastPtr;
			list->lastPtr->next = newNode;
			list->lastPtr = newNode;
			list->currState = NORMAL;
		}

		list->currentPtr = newNode;
		list->count++;
		return 0;
	}
	else 
	{
		return -1;
	}
}

int ListAppend(LIST* list, void* item) {
	if (avaiNodeChain != NULL) {
		node* newNode = avaiNodeChain;
		avaiNodeChain = avaiNodeChain->next;

		newNode->item = item;
		newNode->prev = list->lastPtr;
		if (list->lastPtr != NULL)
		{
			list->lastPtr->next = newNode;
			newNode->prev = list->lastPtr;
		}

		if (list->firstPtr == NULL)
		{
			list->firstPtr = newNode;
		}

		list->currentPtr = newNode;
		list->lastPtr = newNode;
		newNode->next = NULL;
		list->count++;

		return 0;
	}
	else {
		return -1;
	}
}

int ListPrepend(LIST* list, void* item) {
	if (avaiNodeChain != NULL) {
		node* newNode = avaiNodeChain;
		avaiNodeChain = avaiNodeChain->next;

		newNode->item = item;
		newNode->next = list->firstPtr;
		if (list->firstPtr != NULL)
		{
			list->firstPtr->prev = newNode;
			newNode->next = list->firstPtr;
		}

		if (list->lastPtr == NULL)
		{
			list->lastPtr = newNode;
		}

		list->currentPtr = newNode;
		list->firstPtr = newNode;
		newNode->prev = NULL;
		list->count++;
		return 0;
	}
	else {
		return -1;
	}
}

void* ListRemove(LIST* list) {
	node* tempNode = list->currentPtr;
	if (list->currentPtr != NULL) {
		if (list->currentPtr == list->lastPtr) {
			//if list only has one item
			if (list->currentPtr == list->firstPtr) {
				list->firstPtr = NULL;
				list->lastPtr = NULL;
				list->currentPtr = NULL;
			}
			else {
				//if current node at the end
				list->currentPtr = list->currentPtr->prev;
				list->currentPtr->next = NULL;
				list->lastPtr = list->currentPtr;
			}
		}
		else {
			//normal case
			list->currentPtr->prev->next = list->currentPtr->next;
			list->currentPtr->next->prev = list->currentPtr->prev;
			list->currentPtr = list->currentPtr->next;
		}
		//return node to free node pool
		tempNode->next = avaiNodeChain;
		avaiNodeChain->prev = tempNode;
		avaiNodeChain = tempNode;
		tempNode->prev = NULL;
		if (list->count > 0) {
			list->count--;
		}
		return tempNode->item;
	}
	else {
		return NULL;
	}
}

void ListConcat(LIST* list1, LIST* list2) {
	//if at least one list are empty
	if ( (list1->count == 0) || (list2->count == 0) ) {
		//list1 empty
		if ( (list1->count == 0) && (list2->count != 0)) {
			list1->firstPtr = list2->firstPtr;
			list1->lastPtr = list2->lastPtr;
			list1->currentPtr = list2->currentPtr;
			list1->count = list2->count;
		}
	}
	else {
		list1->lastPtr->next = list2->lastPtr;
		list2->firstPtr->prev = list1->lastPtr;
		list1->lastPtr = list2->lastPtr;

		list1->count += list2->count;

		list2->currentPtr = NULL;
		list2->firstPtr = NULL;
		list2->lastPtr = NULL;
		list2->count = 0;
	}
	list2->next = avaiHeadChain;
	avaiHeadChain = list2;
}

void ListFree(LIST* list, void (*itemFree)(void*)) {
	list->currentPtr = list->firstPtr;
	while (list->currentPtr != NULL)
	{
		// Free item
		(*itemFree)(list->currentPtr->item);
		// Store next in list
		node* nextInList = list->currentPtr->next;
		
		// Add node back to free node chain
		avaiNodeChain->prev = list->currentPtr;
		list->currentPtr->next = avaiNodeChain;
		avaiNodeChain = list->currentPtr;
		avaiNodeChain->prev = NULL;
		list->count--;
		// move curr to next
		list->currentPtr = nextInList;
	}

	list->firstPtr = NULL;
	list->lastPtr = NULL;
	list->currentPtr = NULL;
	list->count = 0;

	list->next = avaiHeadChain;
	avaiHeadChain = list;
}

void* ListTrim(LIST* list) {
	list->currentPtr = list->lastPtr;
	return ListRemove(list);
}

void* ListSearch(LIST* list, int(*compator) (void* item1, void* item2), void* comparisonArg) {
	node* currCheckingPtr = list->firstPtr;
	while (currCheckingPtr != NULL) {
		if (compator(currCheckingPtr->item, comparisonArg) == 1) {
			list->currentPtr = currCheckingPtr;
			return list->currentPtr->item;
		}
		else {
			currCheckingPtr = currCheckingPtr->next;
		}
	}
	return NULL;
}

int freeNodeCount()
{
	node* curr = avaiNodeChain;
	int count = 0;
	while (curr != NULL)
	{
		curr = curr->next;
		++count;
	}

	return count;
}

int freeHeadCount()
{
	LIST* curr = avaiHeadChain;
	int count = 0;
	while (curr != NULL)
	{
		curr = curr->next;
		++count;
	}

	return count;
}

