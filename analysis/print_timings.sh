#!/usr/bin/bash

python timing_calcs.py timings.txt | sort -g | column -s \| -t
