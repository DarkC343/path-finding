#!/bin/bash

if [[ $2 == "dfs" ]];
then
    for i in $(seq 1 $1); do /usr/bin/time -f %E -o $4 -a ./main -i $3 -D run -p; done
elif [[ $2 == "bfs" ]];
then
    for i in $(seq 1 $1); do /usr/bin/time -f %E -o $4 -a ./main -i $3 -B run -p; done
fi