#include "mm_symbol.h"

FreeBlock g_blocks[2];

void init()
{
   g_blocks[0].size = 1;
   g_blocks[0].prev = nullptr;
   g_blocks[0].next = &g_blocks[1];
   g_blocks[1].size = 2;
   g_blocks[1].prev = &g_blocks[0];
   g_blocks[1].next = nullptr;
}