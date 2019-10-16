#!/usr/bin/env bash

mkdir -p $3/$1
seq 1 $2 | xargs -i bash -c "bin/$1 ${@:4} > $3/$1/{}.out"
