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

    out_variable = "$(OUT_" + target.upper() + ")"
    all_objects = "$(OBJ_" + target.upper() + ")"
    operations[out_variable] = [' '.join(["$(LD)", "-o", out_variable, all_objects, "$(LDFLAGS)"])]
    bins[target] = os.path.join('bin', target, program_name)
    depends[target] = []
    for sub in ['before_', 'after_']:
        depends[sub + target] = []
        depends[target].append(sub + target)
    depends[target].insert(1, out_variable)

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
    bin_dir = os.path.dirname(bins[target])
    operations['before_' + target].append('test -d ' + bin_dir + ' || mkdir -p ' + bin_dir)
    for (dirpath, dirnames, filenames) in os.walk(os.getcwd()):
        dirpath = dirpath[len(os.getcwd()) + 1 :]
        for source_file in [os.path.join(dirpath, fn) for fn in filenames if fn.endswith('.cpp') or fn.endswith('.h')]:
            ext_length = len(source_file.split('.')[-1])
            obj_file = os.path.join(obj_dest[target], source_file[:-ext_length] + "o")
            if 'include' in obj_file:
                inc_ind = obj_file.find('include')
                inc_len = len('include')
                obj_file = obj_file[:inc_ind] + 'src' + obj_file[inc_ind + inc_len:]
            try:
                if source_file not in depends[obj_file]:
                    depends[obj_file].append(source_file)
            except KeyError:
                depends[obj_file] = [source_file]
            if source_file.endswith('.cpp'):
                operations[obj_file] = [' '.join(['$(CXX)', "$(CFLAGS)", options[target], "-c", source_file, "-o", obj_file])]
            obj_dest_dir = os.path.dirname(obj_file)
            if obj_dest_dir not in obj_dir_written:
                operations['before_' + target].append('test -d ' + obj_dest_dir + ' || mkdir -p ' + obj_dest_dir)
                obj_dir_written.append(obj_dest_dir)
            with open(source_file) as src:
                skip = False
                for line in src:
                    if line.find('#ifdef') != -1:
                        skip = (line.split()[1] != target.upper())
                    elif line.find("#else") != -1:
                        skip = not skip
                    elif line.find("#endif") != -1:
                        skip = False
                    elif not skip and line.find('#include "') != -1:
                        file_name = line.split('"')[1]
                        include_file_name = os.path.join('include', file_name)
                        if not os.path.isfile(include_file_name):
                            include_file_name = os.path.join(os.path.dirname(source_file), file_name)
                        if os.path.isfile(include_file_name) and include_file_name not in depends[obj_file]:
                            depends[obj_file].append(include_file_name)

to_delete = []
for obj_file in depends:
    if obj_file.endswith('.o') and obj_file not in operations:
        to_delete.append(obj_file)

for thing in to_delete:
    del depends[thing]

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
	"-fexceptions", 
	"-Iinclude",
        "-pthread"]
linker_options = ["-pthread"]
options_list = dict()
options_list['debug'] = ["-g", "-DDEBUG"]
options_list['release'] = ["-s", "-O2", "-DNDEBUG"]

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
