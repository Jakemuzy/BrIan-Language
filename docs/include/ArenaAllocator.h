#ifndef _BRIAN_ARENA_H_
#define _BRIAN_ARENA_H_

#include "ErrorHandler.h"

// Probably should be system dependent
// ie.) check storage capacity, 32/64bit system, etc
#define ARENA_MAX_CAPACITY_BYTES 1000000
#define ARENA_CAPACITY_MULTIPLIER_FROM_FILESIZE 5

/*      BrIan Arena Allocator
    ----------------------------
      Arena allocator used for 
       multiple phases of the 
          BrIan pipeline. 
     Also tied to error handler 
        that way compilation
     state isn't required to be 
        passed to each phaes 

*/

typedef struct Arena {
   void* base;
   size_t offset;
   size_t capacity;

   struct Arena* next;
} Arena;

Arena* CreateArena(size_t capacity);
void* AllocateArena(Arena* arena, size_t size);
void ResetArena(Arena* arena);
void DestroyArena(Arena* arena);

// Separate functions for permanent / temporary arenas

#endif
