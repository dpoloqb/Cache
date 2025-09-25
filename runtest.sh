#!/bin/bash

TEST=$1
EXECUTABLE=$2
ANS_PREFIX=$3

TEST_NAME=$(basename $TEST .dat)
ANS=${TEST%.*}.${ANS_PREFIX}.ans

echo "Running test: $TEST_NAME with $EXECUTABLE"
echo "Expected answers: $ANS"

$EXECUTABLE < $TEST > ${TEST_NAME}.log

DIFF=$(diff -w ${TEST_NAME}.log ${ANS})

if [ $? -ne 0 ]; then
  echo "" >> ${TEST_NAME}.log
  echo "Diff is:" >> ${TEST_NAME}.log
  echo "$DIFF" >> ${TEST_NAME}.log
  echo "Test ${TEST_NAME} for ${EXECUTABLE} failed, see ${TEST_NAME}.log"
  exit 1
else
  rm ${TEST_NAME}.log
  echo "Test ${TEST_NAME} for ${EXECUTABLE} passed"
fi