#!/bin/python

import sys

if len(sys.argv) != 2:
    sys.exit(1)

gene_pool_file_name = sys.argv[1]
origin_file_name = gene_pool_file_name + '_ancestry.txt'
with open(gene_pool_file_name) as input, \
        open(origin_file_name, 'w') as output:
    for line in (x.strip() for x in input):
        if line.startswith('ID:'):
            current_id = line.split(':')[1].strip()
            continue

        if not line.startswith('Ancestry:'):
            continue

        data = [x.split('->')[1].strip() for x in line.split(':')[1].split('/') if x.strip()]
        output.write(','.join([current_id] + data) + '\n')
