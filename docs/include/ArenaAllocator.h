#ifndef _BRIAN_COMPILER_H_
#define _BRIAN_COMPILER_H_

#include "ErrorHandler.h"

// Probably should be system dependent
// ie.) check storage capacity, 32/64bit system, etc
#define ARENA_MAX__CAPACITY_BYTES 1000000

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

typedef struct {
    void* base;
    size_t offset;
    size_t capacity;

    struct Arena* next;
} Arena;

Arena* CreateArena(size_t capacity);
void* AllocateArena(Arena* arena, size_t size);
void ResetArena(Arena* arena);

unsigned int EstimateArenaSize(FILE* fptr);

// Separate functions for permanent / temporary arenas