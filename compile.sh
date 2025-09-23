#!/bin/bash

# General Settings
GCC_FLAGS="-std=c89 -Werror -Wall -Wextra"

INCLUDE_PATH="includes"
INCLUDES=$(echo "$INCLUDE_PATH/"*.h)
SRC_PATH="src"
SOURCES=$(echo "$SRC_PATH/"*.c)
BUILD_PATH="builds"

GCC_FLAGS="$GCC_FLAGS -I$INCLUDE_PATH"

# Compile each SRC file separately
IFS=' '
for SRC in $SOURCES
do
	EXEC="${SRC//$SRC_PATH/}"
	EXEC="${EXEC//.c/}"	
	CUR_EXEC="$GCC_FLAGS $SRC -o $BUILD_PATH$EXEC"

	printf "COMPILING WITH FLAGS:\n\t$CUR_EXEC\n" 
	gcc "$CUR_EXEC"
done
