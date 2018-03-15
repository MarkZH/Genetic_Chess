#!/bin/python

import os, functools, subprocess, sys

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
final_targets = ["release", "debug", "optimized_debug"]
depends = dict()
depends['all'] = final_targets
depends['clean'] = []

options = dict()
obj_dest = dict()
operations = dict()
bins = dict()

for target in final_targets:
    options[target] = "$(CFLAGS_" + target.upper() + ") $(LDFLAGS_" + target.upper() + ")"
    operations[target] = []

    obj_dest[target]= "$(" + target.upper() + "_OBJ_DIR)"

    out_variable = "$(OUT_" + target.upper() + ")"
    all_objects = "$(OBJ_" + target.upper() + ")"
    operations[out_variable] = [' '.join(["$(LD)",
                                          "-o", out_variable,
                                          all_objects,
                                          "$(LDFLAGS)",
                                          "$(LDFLAGS_" + target.upper() + ")",
                                          "$(CFLAGS)",
                                          "$(CFLAGS_" + target.upper() + ")"])]
    bins[target] = os.path.join('bin', target, program_name)
    depends[target] = ['before_' + target, out_variable]
    depends['before_' + target] = []
    depends[out_variable] = [all_objects]
    depends['clean'].append('clean_' + target)
    depends['clean_' + target] = []
    operations['clean_' + target] = ["rm -rf " + obj_dest[target] + " " + out_variable]

depends['.PHONY'] = []
for target in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)):
    if target not in operations:
        operations[target] = []
    if target != '.PHONY' and not target.startswith('$'):
        depends['.PHONY'].append(target)

options_list = dict()
linker_options = dict()
if len(sys.argv) == 1:
    print('Specify a compiler ("gcc" or "clang")')
    sys.exit(1)
elif sys.argv[1] == 'gcc':
    compiler = 'g++'
    options_list['debug'] = ["-g", "-DDEBUG"]
    options_list['release'] = ["-s", "-Ofast", "-DNDEBUG"]
    options_list['optimized_debug'] = ["-Ofast", "-DDEBUG"]

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
            "-Iinclude"]
    base_linker_options = ["-pthread", "-fexceptions"]

    linker_options['debug'] = []
    linker_options['release'] = ['-flto', '-fuse-linker-plugin']
    linker_options['optimized_debug'] = ['-flto', '-fuse-linker-plugin']
elif sys.argv[1] == 'clang':
    compiler = 'clang++'
    options_list['debug'] = ["-g", "-Og", "-DDEBUG"]
    options_list['release'] = ["-Ofast", "-DNDEBUG"]
    options_list['optimized_debug'] = ["-Ofast", "-DDEBUG"]

    base_options = [
            "-std=c++14",
            "-Weverything",
            "-Wno-padded",
            "-Wno-c++98-compat",
            "-Wno-exit-time-destructors",
            "-Wno-global-constructors",
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
            "-Wmain", 
            "-pedantic", 
            "-Wextra", 
            "-Wall", 
            "-Iinclude"]
    base_linker_options = ["-pthread", "-fexceptions"]

    linker_options['debug'] = []
    linker_options['release'] = ['-flto']
    linker_options['optimized_debug'] = ['-flto']
else:
    print('Uknown compiler: ' + sys.argv[1])
    sys.exit(1)


obj_dir_written = []
for target in final_targets:
    bin_dir = os.path.dirname(bins[target])
    operations['before_' + target].append('test -d ' + bin_dir + ' || mkdir -p ' + bin_dir)
    for (dirpath, dirnames, filenames) in os.walk(os.getcwd()):
        dirpath = dirpath[len(os.getcwd()) + 1 :]
        for source_file in [os.path.join(dirpath, fn) for fn in filenames if fn.endswith('.cpp')]:
            ext_length = len(source_file.split('.')[-1])
            obj_file = os.path.join(obj_dest[target], source_file[:-ext_length] + "o")
            depends[obj_file] = [source_file]
            operations[obj_file] = [' '.join(['$(CXX)', "$(CFLAGS)", "$(LDFLAGS)", options[target], "-c", source_file, "-o", obj_file])]
            obj_dest_dir = os.path.dirname(obj_file)
            if obj_dest_dir not in obj_dir_written:
                operations['before_' + target].append('test -d ' + obj_dest_dir + ' || mkdir -p ' + obj_dest_dir)
                obj_dir_written.append(obj_dest_dir)

            compile_depends = subprocess.check_output([compiler] + base_options + options_list[target] + ['-MM', source_file]).decode('ascii').split()
            depends[obj_file].extend(sorted(list(set(d for d in compile_depends if d != '\\' and d != source_file and not d.endswith(':')))))

with open("Makefile", 'w') as make_file:
    # Variables
    make_file.write("CXX = " + compiler + "\n")
    make_file.write("LD = " + compiler + "\n")
    make_file.write("\n")
    make_file.write("CFLAGS = " + " ".join(base_options) + "\n")
    make_file.write("LDFLAGS = " + " ".join(base_linker_options) + "\n")
    make_file.write("\n")
    for target in final_targets:
        make_file.write("OUT_" + target.upper() + " = " + bins[target] + '\n')
        make_file.write(target.upper() + '_OBJ_DIR = obj/' + target + '\n')
        make_file.write('OBJ_' + target.upper() + ' = ')
        make_file.write(' '.join([x for x in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)) if x.endswith('.o') and x.startswith('$(' + target.upper())]))
        make_file.write('\n')
        make_file.write('CFLAGS_' + target.upper() + ' = ' + ' '.join(options_list[target]) + '\n')
        make_file.write('LDFLAGS_' + target.upper() + ' = ' + ' '.join(linker_options[target]) + '\n')
        make_file.write('\n\n')

    for target in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)):
        make_file.write(target + " : " + " ".join(depends[target]) + "\n")
        make_file.write("\n".join(['\t' + x for x in operations[target]]))
        make_file.write('\n\n')
