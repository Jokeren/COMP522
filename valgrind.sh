#!/bin/bash

THREADS=(1 2 4 8 16 32)
MAIN=$1
DIR=$2

for ((i=0;i<${#THREADS[@]};i++))
do
  valgrind --tool=massif $MAIN $DIR/config.${THREADS[$i]}.bind.init.txt
done
