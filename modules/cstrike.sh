#!/bin/sh
g++ -fPIC -Iinclude -c cstrike/cstrike.cpp
g++ -shared -o cstrike.so cstrike.o
rm cstrike.o
