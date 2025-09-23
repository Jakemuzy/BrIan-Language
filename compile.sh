#!/bin/bash


GCC_FLAGS="-std=c89 -Werror -Wall -Wextra"
declare -a INCLUDES
declare -a EXE_INCLUDES


INCLUDES[0]="TokenizerFunctions.h"

INCLUDE_COUNTER=0
for i in "${INCLUDES[@]}"
do
	gcc -c $i
	${EXE_INCLUDES[$INCLUDE_COUNTER]=$i}
	INCLUDE_COUNTER=$((INCLUDE_COUNTER++))
done
echo ${EXE_INCLUDES[0]}

#gcc $GCC_FLAGS 
