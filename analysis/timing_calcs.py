#!/usr/bin/python

import sys

def main(file_name):
    timings = dict()
    with open(file_name) as f:
        for line in f:
            try:
                name, time = line.split('|')
            except ValueError:
                print(line)
                return
            try:
                count, time_sum = timings[name]
            except KeyError:
                count = 0
                time_sum = 0
            timings[name] = (count + 1, time_sum + float(time))
    
    print('Average time (sec)|Total time (sec)|Count|Name') # Column Headings
    for name in timings:
        count, time_sum = timings[name]
        print(str(time_sum/count) + '|' + str(time_sum) + '|' + str(count) + '|' + name)

if __name__ == "__main__":
    main(sys.argv[1])
