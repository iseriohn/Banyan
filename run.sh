#!/bin/bash

g++ src/main.cpp -o main

cat $1 | tr '\r' '\n' | ./main $2 $3 $4 $5 $6 $7

rm -f main
