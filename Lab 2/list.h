// the type definition for a linked list node
typedef struct ln {
	long		key;
	long		data;
	struct ln	*next;
} listNode, *listNodePtr;

// prototype definitions for the list operations
void listInsert(listNodePtr *listPtr, long key, long value);

void listDelete(listNodePtr *listPtr, long key);

long listSearch(listNodePtr listPtr, long key);
