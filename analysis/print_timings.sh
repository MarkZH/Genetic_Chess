#!/usr/bin/bash

# For use with Scoped_Stopwatch result files

python $(dirname "$0")/timing_calcs.py "$1" | sort -g | column -s \| -t
