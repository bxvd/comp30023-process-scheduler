#!/usr/bin/env bash

make
./scheduler -f processes.txt -a ff -m u -d > out.txt
