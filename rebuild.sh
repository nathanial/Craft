#!/usr/bin/env bash
git submodule init
git submodule update
cd actor-framework
./configure
make
cd ..
cmake .
make