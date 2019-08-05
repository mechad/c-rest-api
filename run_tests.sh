#!/bin/bash

# Fail as soon as a test fails
set -e

# Test names
TESTS=(json server)

# Compile the debug library to make sure the changes are applied
make NAME=test MODE=debug

# Find the project objects so we can test them
# TODO: should we separate these for each test?
OBJECTS=`find build/debug/test/ ! -name "main.o" -type f`


CC=gcc
TEST_CFLAGS="-std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -lcheck -lsubunit -lrt -lm"

for T in "${TESTS[@]}"
do
    $CC -o check_$T tests/check_$T.c $OBJECTS $TEST_CFLAGS
    ./check_$T
    rm check_$T
done
