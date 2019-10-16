#!/usr/bin/env bash

mkdir -p output/$1 graphs/$1
seq 1 $2 | parallel -j 16 bash -c "bin/$1 ${@:4} > output/$1/{}.out; python plot.py output/a/{}.out graphs/a {}"
