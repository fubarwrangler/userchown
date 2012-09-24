#!/bin/bash

test -d build || mkdir build/
cd build && cmake ../ && make && cd ..
