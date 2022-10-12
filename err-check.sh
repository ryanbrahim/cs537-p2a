#!/bin/bash
TEST_DIR="/home/cs537-1/tests/p2a/tests"
RESULT_DIR="tests-out"

echo ""
echo ""
echo "-------------Input-------------"
cat ${TEST_DIR}/${1}.in
echo ""
echo ""
echo ""
echo "--------Expected Output--------"
cat ${TEST_DIR}/${1}.out
echo ""
echo ""
echo "---------Actual Output---------"
cat ${RESULT_DIR}/${1}.out
echo ""
echo ""
echo "--------Expected Error---------"
cat ${TEST_DIR}/${1}.err
echo ""
echo ""
echo "---------Actual Error----------"
cat ${RESULT_DIR}/${1}.err
echo ""
echo ""



