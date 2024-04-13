#ifndef MALLOC_LIB_H
#define MALLOC_LIB_H
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/memlayout.h"

#define FALSE 0
#define TRUE 1
#define NULL 0
#define ROOT 0
#define INRANGE 1
#define OUTRANGE 0
#define DEFAULTALLOCSZ 4096
#define MAXMALLOCNODESZ (PHYSTOP -KERNBASE)/sizeof(node)
typedef union node
{
  // actual node in free list
  struct
  {
    union node *prev;
    union node *next;
    // free blocks size.
    // sizeof(freelist)/sizeof(node), round up.
    int size;
  } n;
  uint64 Align;
} node;

void* _malloc(int size);
void _free(void *ptr);


#endif
