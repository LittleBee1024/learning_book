#pragma once

#include <stdint.h>

struct FreeBlock
{
   int size;
   struct FreeBlock *next;
   struct FreeBlock *prev;
};

extern FreeBlock g_blocks[2];
void init();
