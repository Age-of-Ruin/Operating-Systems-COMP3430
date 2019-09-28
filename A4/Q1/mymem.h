typedef struct fl {
  unsigned long	size;
  void	        *next;
} freeListNode, *freeListNodePtr;

#define MINALLOCSIZE (sizeof(freeListNode))

// prototypes
void myinit(unsigned long areasz);
void *mymalloc(unsigned long sz);
void myfree(void * addr, unsigned long sz);

int numNodesOnFreeList();
