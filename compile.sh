#!/bin/bash

# Preprocessing
if [ "$#" -lt 1 ]
then
    echo "Please enter output name"
    exit
elif [ "$#" -lt 2 ]
then
    echo "Please enter main folder"
    exit
fi

EXEC_NAME=$1
MAIN_LOCATION=$2

# General Settings
GCC_FLAGS="-std=c99 -g -Werror" # -Wall -Wextra"

INCLUDE_PATHS="./includes:./includes/DataStructures:./includes/Concurrency"
#INCLUDES=$(echo "$INCLUDE_PATH/"*.h)
SRC_PATH="src/*.c TestCases/$MAIN_LOCATION/$EXEC_NAME.c"
#SOURCES=$(echo "$SRC_PATH/"*.c)
BUILD_PATH="builds"


OLD_IFS=$IFS
IFS=":"
for INC_PATH in $INCLUDE_PATHS
do
    GCC_FLAGS="$GCC_FLAGS -I$INC_PATH"
done
IFS=$OLD_IFS

# Compile each SRC file separately LATER
mkdir -p "$BUILD_PATH"
CUR_EXEC="$GCC_FLAGS $SRC_PATH -o $BUILD_PATH/$EXEC_NAME"

printf "COMPILING WITH FLAGS:\n\t%s\n" "$CUR_EXEC"
gcc $CUR_EXEC
