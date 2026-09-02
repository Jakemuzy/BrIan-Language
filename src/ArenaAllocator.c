#include "ArenaAllocator.h"

/*
Arena works better for name scopes, since they are truly
temporary, however bump allocator works best for AST 
*/

/* 
    Estimate original capacity based on file size?
    or maybe some other metric?
*/

Arena* CreateArena(size_t capacity)
{
    Arena* arena = malloc(sizeof(Arena));
    if (!arena) ERROR(ERR_FLAG_ABORT, COMPILER_ERR, "Arena allocation failed\n");

    arena->offset = 0;
    arena->capacity = capacity;
    arena->base = malloc(capacity);
    if (!arena->base) ERROR(ERR_FLAG_ABORT, COMPILER_ERR, "Arena allocation failed\n");

    arena->next = NULL;

    return arena;
}

void* AllocateArena(Arena* arena, size_t size)
{
    size_t alignedOffset = (arena->offset + 7) & ~7;
    if (alignedOffset + size > arena->capacity) {

        size_t newCapacity = arena->capacity ? arena->capacity * 2 : 64;

        if (newCapacity < size) 
            newCapacity = size;

        if (!arena->next) 
            arena->next = CreateArena(newCapacity);

        if (!arena->next)
            arena->next = CreateArena(arena->capacity); 
        return AllocateArena(arena->next, size);
    }

    void* ptr = (char*)arena->base + alignedOffset;
    arena->offset = alignedOffset + size;
    return ptr;
}

void ResetArena(Arena* arena)
{
    if (arena->next) ResetArena(arena->next);
    arena->offset = 0;
}

void DestroyArena(Arena* arena)
{
    if (arena->next) DestroyArena(arena->next);
    free(arena->base);
    free(arena);
}
