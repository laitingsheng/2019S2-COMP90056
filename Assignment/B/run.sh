#!/usr/bin/env bash

mkdir -p output/$1 graphs/$1
seq 1 $2 | parallel -j 16 bash -c "bin/$1 ${@:3} > output/$1/{}.out; python $1_plot.py output/$1/{}.out graphs/$1 {}"
