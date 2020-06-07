#!/usr/bin/env bash

make s=1
./scheduler -f rr_memoryoverlap.txt -a rr -s 20 -m p -q 10
