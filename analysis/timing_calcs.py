#!/usr/bin/python

# For use with Scoped_Stopwatch result files

import sys

def main(args):
    file_names = [name for name in args if not name.startswith('-')]
    time_unit = 'sec'
    if len(file_names) < len(args):
        time_unit = [unit for unit in args if unit not in file_names][0][1:]
    timings = dict()
    for index, file_name in enumerate(file_names, 1):
        with open(file_name) as f:
            print('Reading file ' + str(index) + ' of ' + str(len(file_names)) + ': ' + file_name, flush=True, file=sys.stderr)
            for line_number, line in enumerate(f):
                try:
                    name, time = line.split('|')
                    count, time_sum = timings[name]
                except ValueError as e:
                    print('ERROR:', line_number, line)
                    print(e)
                    return
                except KeyError:
                    count = 0
                    time_sum = 0
                timings[name] = (count + 1, time_sum + float(time))
    
    if time_unit == 'sec':
        multiplier = 1
    elif time_unit == 'msec':
        multiplier = 1e3
    elif time_unit == 'usec':
        multiplier = 1e6
    elif time_unit == 'nsec':
        multiplier = 1e9
    else:
        print('Invalid time unit:', time_unit)
        sys.exit(1)

    print('Average time (' + time_unit + ')|Total time (sec)|Count|Percent Time|Name') # Column Headings
    total_time = sum(x[1] for x in timings.values())
    for name in timings:
        count, time_sum = timings[name]
        print(str(multiplier*time_sum/count) + '|' +
              str(time_sum) + '|' +
              str(count) + '|' +
              str(100*time_sum/total_time) + '|' +
              name)

if __name__ == "__main__":
    main(sys.argv[1:])
