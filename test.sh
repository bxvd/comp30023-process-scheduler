#!/usr/bin/env bash

make
./scheduler -f rr_memoryoverlap.txt -a ff -s 20 -m p
