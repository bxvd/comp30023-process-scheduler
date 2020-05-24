#!/usr/bin/env bash

make
./scheduler -f processes.txt -a rr -m u -q 10
