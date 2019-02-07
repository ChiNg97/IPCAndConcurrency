#ifndef LIST_HEADER
#define LIST_HEADER

typedef struct node node;

struct node {
	void* item;
	node* next;
	node* prev;
};

typedef struct LIST LIST;

typedef enum
{
	BEFORE,
	NORMAL,
	AFTER
} ListCurrState;


struct LIST {
	LIST* next;
	node* currentPtr;
	node* firstPtr;
	node* lastPtr;
	int count;

	// Private state helper
	ListCurrState currState;
};

LIST* ListCreate();

int ListCount(LIST* list); 

void* ListFirst(LIST* list);

void* ListLast(LIST* list);

void* ListNext(LIST* list);

void* ListPrev(LIST* list);

void* ListCurr(LIST* list);

int ListAdd(LIST* list, void* item);

int ListInsert(LIST* list, void* item);

int ListAppend(LIST* list, void* item);

int ListPrepend(LIST* list, void* item);

void* ListRemove(LIST* list);

void ListConcat(LIST* list1, LIST* list2);

void ListFree(LIST* list, void(*itemFree)(void*));

void* ListTrim(LIST* list);

void* ListSearch(LIST* list, int(*compator)(void* item1, void* item2), void* comparisonArg);


int freeHeadCount();
int freeNodeCount();
void initChain();
#endif // LIST_HEADER


