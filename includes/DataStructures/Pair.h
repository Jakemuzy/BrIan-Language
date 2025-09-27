#ifndef _PAIR_H__
#define _PAIR_H__

typedef struct Pair{ 
    void* first;
    void* second;
} Pair;

void MakePair(void* first, void* second);

#endif
