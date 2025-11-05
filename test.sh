#!/bin/bash

# Test all cases

COMPILER="./compile.sh"
TEST_CASES_PATH="TestCases/Tokenizer"
PROG_NAME="Tokenizer"

"$COMPILER" "$PROG_NAME"
printf "\n"

IFS=" "
for TestCase in "$TEST_CASES_PATH"/*.b
do
    printf "Testing $TestCase\n";
    ./builds/"$PROG_NAME" "$TestCase"

    [[ $? -eq 0 ]] && printf "\tPASS\n" || printf "\tFAIL\n" 
done
