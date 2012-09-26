#!/bin/bash

test -d build || mkdir build/
cd build && cmake -DCONFIG_PATH:PATH=../cfg/test.cfg ../ && make && cd ..
