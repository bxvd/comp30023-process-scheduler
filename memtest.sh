#!/usr/bin/env bash

make s=1
echo
echo
echo ./scheduler $(< tests/testcase1.in)
./scheduler $(< tests/testcase1.in)
echo
echo
echo ./scheduler $(< tests/testcase2.in)
./scheduler $(< tests/testcase2.in)
echo
echo
echo ./scheduler $(< tests/testcase3.in)
./scheduler $(< tests/testcase3.in)
echo
echo
echo ./scheduler $(< tests/testcase4.in)
./scheduler $(< tests/testcase4.in)
echo
echo
echo ./scheduler $(< tests/testcase5.in)
./scheduler $(< tests/testcase5.in)
echo
echo
echo ./scheduler $(< tests/testcase6.in)
./scheduler $(< tests/testcase6.in)
