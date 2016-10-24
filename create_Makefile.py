#!/bin/python

import os, functools

def make_sort(a, b):
    if a == b:
        return 0

    # all target is first
    if a == 'all':
        return -1
    if b == 'all':
        return 1

    # .PHONY is last
    if a == '.PHONY':
        return 1
    if b == '.PHONY':
        return -1

    # include files are next to last
    if a.startswith('include'):
        if not b.startswith('include'):
            return 1
    elif b.startswith('include'):
        return -1

    # Blank lines go at end
    if not a:
        return 1
    if not b:
        return -1

   # Sort cpp files before object files
    if a[0] == '$':
        if b[0] != '$':
            return 1
    elif b[0] == '$':
        return -1

    if a < b:
        return -1
    if a > b:
        return 1


program_name = 'genetic_chess'
final_targets = ["release", "debug"]
depends = dict()
depends['all'] = final_targets
depends['clean'] = []

options = dict()
obj_dest = dict()
operations = dict()
bins = dict()

for target in final_targets:
    options[target] = "$(CFLAGS_" + target.upper() + ")"
    operations[target] = []

    obj_dest[target]= "$(" + target.upper() + "_OBJ_DIR)"

    depends[target] = []
    for sub in ['before_', 'after_']:
        depends[sub + target] = []
        depends[target].append(sub + target)

    out_variable = "$(OUT_" + target.upper() + ")"
    all_objects = "$(OBJ_" + target.upper() + ")"
    operations[out_variable] = [' '.join(["$(LD)", "-o", out_variable, all_objects, "$(LDFLAGS)"])]
    bins[target] = os.path.join('bin', target, program_name)

    depends[out_variable] = ['before_' + target, all_objects]
    depends['clean'].append('clean_' + target)
    depends['clean_' + target] = []
    operations['clean_' + target] = ["rm " + all_objects + " " + out_variable]

depends['.PHONY'] = []
for target in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)):
    if target not in operations:
        operations[target] = []
    if target != '.PHONY' and not target.startswith('$'):
        depends['.PHONY'].append(target)

obj_dir_written = []
for target in final_targets:
    depends[target] = ['before_' + target, "$(OUT_" + target.upper() + ')', 'after_' + target]
    bin_dir = os.path.dirname(bins[target])
    operations['before_' + target].append('test -d ' + bin_dir + ' || mkdir -p ' + bin_dir)
    for (dirpath, dirnames, filenames) in os.walk(os.getcwd()):
        dirpath = dirpath[len(os.getcwd()) + 1 :]
        for source_file in [fn for fn in filenames if fn.endswith('.cpp')]:
            source_file = os.path.join(dirpath, source_file)
            obj_file = os.path.join(obj_dest[target], source_file[:-4] + ".o")
            depends[obj_file] = [source_file]
            operations[obj_file] = [' '.join(['$(CXX)', "$(CFLAGS)", options[target], "-c", source_file, "-o", obj_file])]
            obj_dest_dir = os.path.dirname(obj_file)
            if obj_dest_dir not in obj_dir_written:
                operations['before_' + target].append('test -d ' + obj_dest_dir + ' || mkdir -p ' + obj_dest_dir)
                obj_dir_written.append(obj_dest_dir)
            with open(source_file) as src:
                for line in src:
                    if line.find('#include "') != -1:
                        include_file_name = os.path.join('include', line.split('"')[1])
                        depends[obj_file].append(include_file_name)




compiler = 'g++'

base_options = [
	"-Wnon-virtual-dtor", 
	"-Wshadow", 
	"-Winit-self", 
	"-Wredundant-decls", 
	"-Wcast-align", 
	"-Wundef", 
	"-Wfloat-equal", 
	"-Wunreachable-code", 
	"-Wmissing-declarations", 
	"-Wmissing-include-dirs", 
	"-Wswitch-enum", 
	"-Wswitch-default", 
	"-Wzero-as-null-pointer-constant", 
	"-Wmain", 
	"-pedantic", 
	"-Wextra", 
	"-Wall", 
	"-std=c++11", 
	"-fexceptions", 
	"-Iinclude"]
linker_options = ["-pthread"]
options_list = dict()
options_list['debug'] = ["-g", "-DDEBUG"]
options_list['release'] = ["-s", "-fexpensive-optimizations", "-O3"]

with open("Makefile", 'w') as make_file:
    # Variables
    make_file.write("CXX = " + compiler + "\n")
    make_file.write("LD = " + compiler + "\n")
    make_file.write("\n")
    make_file.write("CFLAGS = " + " ".join(base_options) + "\n")
    make_file.write("LDFLAGS = " + " ".join(linker_options) + "\n")
    make_file.write("\n")
    for target in final_targets:
        make_file.write("OUT_" + target.upper() + " = " + bins[target] + '\n')
        make_file.write(target.upper() + '_OBJ_DIR = obj/' + target + '\n')
        make_file.write('OBJ_' + target.upper() + ' = ')
        for obj in [x for x in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)) if x.endswith('.o') and target.upper() in x]:
            make_file.write(obj + ' ')
        make_file.write('\n')
        make_file.write('CFLAGS_' + target.upper() + ' = ' + ' '.join(options_list[target]) + '\n')
        make_file.write('\n\n')

    for target in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)):
        make_file.write(target + " : " + " ".join(depends[target]) + "\n")
        make_file.write("\n".join(['\t' + x for x in operations[target]]))
        make_file.write('\n\n')
