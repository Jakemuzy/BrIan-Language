#!/bin/bash

# Preprocessing
if [ "$#" -lt 1 ]
then
    echo "Please enter output name"
    exit
fi

# General Settings
GCC_FLAGS="-std=c89 -g -Werror" # -Wall -Wextra"

INCLUDE_PATHS="./includes:./includes/DataStructures:./includes/Concurrency"
#INCLUDES=$(echo "$INCLUDE_PATH/"*.h)
SRC_PATH="src"
SRC_PATH="src/*.c TestCases"
#SOURCES=$(echo "$SRC_PATH/"*.c)
BUILD_PATH="builds"

EXEC_NAME=$1

OLD_IFS=$IFS
IFS=":"
for INC_PATH in $INCLUDE_PATHS
do
    GCC_FLAGS="$GCC_FLAGS -I$INC_PATH"
done
IFS=$OLD_IFS

# Compile each SRC file separately LATER
CUR_EXEC="$GCC_FLAGS $SRC_PATH/*.c -o $BUILD_PATH/$EXEC_NAME"

printf "COMPILING WITH FLAGS:\n\t$CUR_EXEC\n" 
gcc $CUR_EXEC
