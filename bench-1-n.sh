#!/bin/bash

BIND=bind.init.txt
THREADS=(1 2 4 8 16 32 64 128 256)
MAIN=$1
DIR=$2

for ((i=0;i<${#THREADS[@]};i++))
do
  $MAIN $DIR/config.${THREADS[$i]}.$BIND
done
