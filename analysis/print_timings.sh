#!/usr/bin/bash

python $(dirname "$0")/timing_calcs.py "$1" | sort -g | column -s \| -t
