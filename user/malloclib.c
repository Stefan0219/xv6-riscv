#include "user/malloclib.h"

static uint64 call_sbrk(uint64);
static uint64 getValbyAddr(node *);
static node* getPrev(node *);
static node* getNext(node *);
static void init_freelist();
static void setPrevNextToMe(node *);
static uint64 find_free_mem(int size);
static int getSize(node *);
static void evict(node *);
static int checkRange(node* checker, node* checkee);
static void mergeBlocks();
// static void print_list()


static node root;
static node *lastTimeLeft;
static int is_created = FALSE;


void *
_malloc(int nbytes)
{
  if (nbytes <= 0)
    return NULL;
  uint64 retAddr;
  int alloc_sz = (nbytes + sizeof(node) - 1) / sizeof(node) + 1;
  if (is_created == FALSE)
    init_freelist();
  retAddr = find_free_mem(alloc_sz);
  return (void *)retAddr;
}

void 
_free(void *ptr)
{
  if(ptr == NULL)
    return;
  node *s = &root;
  // list is empty
  node *insertNode = (node *)ptr - 1;
  if (s == getNext(s))
  {
    insertNode->n.next = root.n.next;
    insertNode->n.prev = &root;
    setPrevNextToMe(insertNode);
    return;
  }
  // list is not empty
  int havefound = FALSE;
  int insertVal = getValbyAddr(insertNode);
  node *i = getNext(&root);
  while (i != &root)
  {
    int cmpVal = getValbyAddr(i);
    if (insertVal > cmpVal)
    {
      // insert to the right postion.
      i = getNext(i);
    }
    else
    {
      if (checkRange(i, insertNode) == INRANGE)
        return;
      havefound = TRUE;
      break;
    }
  }
  if (havefound)
  {
    insertNode->n.next = i;
    insertNode->n.prev = i->n.prev;
    setPrevNextToMe(insertNode);
  }
  else
  {
    insertNode->n.next = &root;
    insertNode->n.prev = root.n.prev;
    setPrevNextToMe(insertNode);
  }
  mergeBlocks();
}

static void
init_freelist()
{
  lastTimeLeft = &root;
  is_created = TRUE;
  root.n.next = &root;
  root.n.prev = &root;
  // identify this node is root
  root.n.size = ROOT;
}


static uint64
find_free_mem(int nblocks)
{
  node *i = lastTimeLeft;
  node *ret = 0;
  node *temp = NULL;
  // int found = FALSE;
  while (TRUE)
  {
    if (getSize(i) >= nblocks)
    {
      if (getSize(i) == nblocks)
      { // alloc the whole space
        // found = TRUE;
        // kick out this node.
        evict(i);
        // return addr with out head block
        ret = i + 1;
        temp = getNext(i);
      }
      else if (getSize(i) > nblocks)
      {
        i->n.size -= nblocks;
        temp = i;
        // found = TRUE;
        i += i->n.size;
        i->n.size = nblocks;
        ret = i + 1;
      }
      lastTimeLeft = temp;
      return (uint64)ret;
    }
    i = getNext(i);
    // find no fit node
    if (i == lastTimeLeft)
    {
      uint64 check;
      check = call_sbrk(nblocks);
      // printf("here %d\n", getSize(getNext(&root)));
      // run out of memory
      if (check == NULL)
      {
        return NULL;
      }
    }
  }
}

static void 
evict(node *i)
{
  node *prev = getPrev(i);
  node *next = getNext(i);
  prev->n.next = next;
  next->n.prev = prev;
}

static uint64
call_sbrk(uint64 nblocks)
{
  if (nblocks < DEFAULTALLOCSZ)
    nblocks = DEFAULTALLOCSZ;
   if(nblocks > MAXMALLOCNODESZ)
     return NULL;
  long ret;
  ret = (long)sbrk(nblocks * sizeof(node));
  if (ret == -1)
  {
    return NULL;
  }
  node *i = (node *)ret;
  i->n.size = nblocks;
  _free((void *)(i + 1));
  return (uint64)i;
}

static node *
getNext(node *i)
{
  return i->n.next;
}

static node *
getPrev(node *i)
{
  return i->n.prev;
}

static int 
getSize(node *i)
{
  return i->n.size;
}

static uint64
getValbyAddr(node *n)
{
  return (uint64)n;
}

static void 
setPrevNextToMe(node *i)
{
  node *prev = getPrev(i);
  node *next = getNext(i);
  next->n.prev = i;
  prev->n.next = i;
}

// static void print_list()
// {
//   node *i = root.n.next;
//   printf("--------------------------------------------------\n");
//   printf("current free list is\n");
//   while (i != &root)
//   {
//     int size = getSize(i);
//     int addr = getValbyAddr(i);
//     printf("This node's size is %d, and it's start address is %d\n", size, addr);
//     int nextAddr = addr + size * sizeof(node);
//     printf("the range of this block is %d to %d\n\n", addr, nextAddr);
//     i = getNext(i);
//   }
//   printf("\n");
// }

static int 
checkRange(node *checker, node *checkee)
{
  uint64 startAddr = (uint64)checker;
  uint64 endAddr = checker->n.size * sizeof(node) + startAddr;
  uint64 check = (uint64)checkee;
  if (check >= startAddr && check <= endAddr)
  {
    return INRANGE;
  }
  else
    return OUTRANGE;
}

static void 
mergeBlocks()
{
  node *i = root.n.next;
  while (i != &root)
  {
    node *next = i->n.next;
    uint64 curend = (uint64)i + i->n.size * sizeof(node);
    uint64 nextStart = (uint64)next;
    if (curend == nextStart)
    {
      i->n.size += next->n.size;
      evict(next);
      continue;
    }
    else
    {
      i = getNext(i);
    }
  }
    lastTimeLeft = root.n.next;
}
