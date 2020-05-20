#!/usr/bin/env bash

make
./scheduler -f processes.txt -a rr -m v -d -s 256
