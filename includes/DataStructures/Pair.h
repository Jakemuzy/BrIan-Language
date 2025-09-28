#ifndef _PAIR_H__
#define _PAIR_H__

#include <Any.h>

/* Generic Typing (not implemented yet */
typedef struct { 
    B_ANY first;
    B_ANY second;
} Pair;

Pair MakePair(B_ANY first, B_ANY second);

#endif
