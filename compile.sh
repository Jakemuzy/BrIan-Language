#!/bin/bash

# General Settings
GCC_FLAGS="-std=c89 -Werror -Wall -Wextra"

INCLUDE_PATH="includes"
INCLUDES=$(echo "$INCLUDE_PATH/"*.h)
SRC_PATH="src"
SOURCES=$(echo "$SRC_PATH/"*.c)

BUILD_PATH="builds"
PROG_NAME=$1
if [ $# -lt 2 ]
then
	PROG_NAME="Tokenizer"
fi

GCC_FLAGS="$GCC_FLAGS -I$INCLUDE_PATH"
GCC_FLAGS="$GCC_FLAGS $PROG_NAME.c -o $BUILD_PATH/$PROG_NAME"

OLD_IFS=$IFS
IFS=' '

for header in $INCLUDES
do
	echo $header
done

echo $GCC_FLAGS
#gcc $GCC_FLAGS 
