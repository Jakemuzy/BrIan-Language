#!/bin/bash

# Preprocessing
if [ "$#" -lt 1 ]
then
    echo "Please enter output name"
    exit
fi

# General Settings
GCC_FLAGS="-std=c89 -Werror -Wall -Wextra"

INCLUDE_PATH="includes"
INCLUDES=$(echo "$INCLUDE_PATH/"*.h)
SRC_PATH="src"
SOURCES=$(echo "$SRC_PATH/"*.c)
BUILD_PATH="builds"

EXEC_NAME=$1
GCC_FLAGS="$GCC_FLAGS -I$INCLUDE_PATH"

# Compile each SRC file separately LATER
CUR_EXEC="$GCC_FLAGS $SRC_PATH/*.c -o $BUILD_PATH/$EXEC_NAME"

printf "COMPILING WITH FLAGS:\n\t$CUR_EXEC\n" 
gcc $CUR_EXEC
