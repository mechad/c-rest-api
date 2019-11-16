#!/bin/bash

# all|unit|integration defines what test are going to be run
# if not assigned, it defaults to all
RUN=$1
if [[ $RUN != "unit" && $RUN != "integration" ]]
then
    RUN="all"
fi

# Fail as soon as a test fails
set -e

# Terminal colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

function error() {
    echo ""
    printf "${RED}Not all the tests passed${NC}\n"
    kill -SIGTERM $U_PID &> /dev/null
}
trap error ERR

# Test names
TESTS=(json server)
# Integration tests
I_TESTS=(staticfiles simpleapi)

# Compile the library to make sure the changes are applied
mkdir -p build
cd build
cmake ..
make -j4
cd ..

# Find the project objects so we can test them
OBJECTS=build/libcrestapi.a

CC=gcc
TEST_CFLAGS="-std=c99 -pthread -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-function -lcheck -lsubunit -lrt -lm"

if [[ $RUN == "unit" || $RUN == "all" ]]
then
    echo "------------- Running unit tests -------------"
    for T in "${TESTS[@]}"
    do
        $CC -o check_$T tests/check_$T.c $OBJECTS $TEST_CFLAGS
        ./check_$T
        rm check_$T
    done
    echo "------------- Done running unit tests -------------"
    echo ""
fi

if [[ $RUN == "integration" || $RUN == "all" ]]
then
    echo "------------- Running integration tests -------------"
    for U in "${I_TESTS[@]}"
    do
        $CC -o i_$U tests/integration/i_$U.c $OBJECTS $TEST_CFLAGS
        ./i_$U &> /dev/null &
        U_PID=$!
        python3 tests/integration/i_$U.py
        kill -SIGTERM $U_PID
        rm i_$U
    done
    echo "------------- Done running integration tests -------------"
    echo ""
fi

printf "${GREEN}All the tests passed${NC}\n"

