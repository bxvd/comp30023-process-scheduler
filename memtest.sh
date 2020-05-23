#!/usr/bin/env bash

make s=1
./scheduler -f processes.txt -a ff -m u -d
