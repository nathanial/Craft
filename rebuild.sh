#!/usr/bin/env bash
set -e
git submodule init
git submodule update
cd actor-framework
./configure
make
sudo make install
cd ..
cmake .
make