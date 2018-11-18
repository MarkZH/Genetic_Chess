#!/usr/bin/bash

grep "\[" "$1" |
    grep "White\|Black" |
    cut -d\" -f2 |
    cut -d' ' -f3 |
    sort -n |
    uniq -c |
    sed "s/^ *//" |
    cut -d" " -f1 |
    sort -n |
    uniq -c |
    column -t -o\; -O2,1 -N"Frequency,Child count"
