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


if len(sys.argv) == 1 or sys.argv[1] not in ['gcc', 'clang']:
    print('Specify a compiler ("gcc" or "clang")')
    sys.exit(1)

program_name = 'genetic_chess'
final_targets = ["release", "debug"]
depends = dict()
depends['all'] = final_targets
depends['clean'] = []

options = dict()
obj_dest = dict()
bin_dest = dict()
operations = dict()
bins = dict()
link_dirs = dict()

for target in final_targets:
    options[target] = f"$(CFLAGS_{target.upper()}) $(LDFLAGS_{target.upper()})"
    operations[target] = []

    obj_dest[target]= f"$({target.upper()}_OBJ_DIR)"
    bin_dest[target]= f"$({target.upper()}_BIN_DIR)"

    out_variable = f"$(OUT_{target.upper()})"
    link_dir_variable = f"$(LINK_DIR_{target.upper()})"
    all_objects = f"$(OBJ_{target.upper()})"
    operations[out_variable] = [' '.join(["$(LD)",
                                          "-o", out_variable,
                                          all_objects,
                                          "$(LDFLAGS)",
                                          f"$(LDFLAGS_{target.upper()})",
                                          "$(CFLAGS)",
                                          f"$(CFLAGS_{target.upper()})"])]
    link_dirs[target] = os.path.join('bin', target)
    operations[os.path.join(link_dir_variable, '$(BIN)')] = [f'ln -sf -t {link_dir_variable} `realpath {out_variable}`']
    depends[os.path.join(link_dir_variable, '$(BIN)')] = [out_variable, 'LINK']
    bins[target] = os.path.join(bin_dest[target], '$(BIN)')
    depends[target] = [f'before_{target}', out_variable, f'after_{target}']
    depends[f'before_{target}'] = []
    depends[out_variable] = [all_objects]
    depends[f'after_{target}'] = [os.path.join(link_dir_variable, '$(BIN)')]
    depends['clean'].append(f'clean_{target}')
    depends[f'clean_{target}'] = []
    operations[f'clean_{target}'] = [f"rm -rf {obj_dest[target]} {bin_dest[target]}"]

depends['LINK'] = []
depends['.PHONY'] = []
for target in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)):
    if target not in operations:
        operations[target] = []
    if target != '.PHONY' and not target.startswith('$'):
        depends['.PHONY'].append(target)

options_list = dict()
linker_options = dict()
system = sys.argv[1]
base_options = [
        "-std=c++17",
        "-lstdc++fs",
        "-fno-rtti",
        "-Wshadow",
        "-Wcast-align",
        "-Wundef",
        "-Wfloat-equal",
        "-Wunreachable-code",
        "-pedantic",
        "-Wextra",
        "-Wall",
        "-Werror",
        "-Iinclude"]
base_linker_options = ["-pthread"]
linker_options['debug'] = []
linker_options['release'] = ['-flto']
options_list['debug'] = ["-g"]
options_list['release'] = ["-O3", "-DNDEBUG"]

if system == 'gcc':
    compiler = 'g++'
    base_options.extend([
        "-Wzero-as-null-pointer-constant",
        "-Wmain"])

elif system == 'clang':
    compiler = 'clang++'
    linker_options['debug'] = ["-fsanitize=undefined", "-fsanitize=integer"]
    options_list['debug'].extend(["-Og"] + linker_options['debug'])
    base_options.extend([
        "-Wno-unused-command-line-argument",
        "-Wnon-virtual-dtor",
        "-Wredundant-decls",
        "-Wmissing-declarations",
        "-Wmissing-include-dirs",
        "-Wswitch"])

obj_dir_written = []
for target in final_targets:
    operations[f'before_{target}'].append(f'mkdir -p {bin_dest[target]}')
    operations[f'before_{target}'].append(f'mkdir -p $(LINK_DIR_{target.upper()})')
    for (dirpath, dirnames, filenames) in os.walk(os.getcwd()):
        dirpath = os.path.relpath(dirpath)
        if dirpath == '.': dirpath = ''
        for source_file in [os.path.join(dirpath, fn) for fn in filenames if fn.endswith('.cpp')]:
            without_extension = '.'.join(source_file.split('.')[:-1])
            obj_file = os.path.join(obj_dest[target], f"{without_extension}.o")
            depends[obj_file] = [source_file]
            operations[obj_file] = [f"$(CXX) $(CFLAGS) $(LDFLAGS) {options[target]} -c {source_file} -o {obj_file}"]
            obj_dest_dir = os.path.dirname(obj_file)
            if obj_dest_dir not in obj_dir_written:
                operations[f'before_{target}'].append(f'mkdir -p {obj_dest_dir}')
                obj_dir_written.append(obj_dest_dir)

            compile_depends = subprocess.check_output([compiler] + base_options + options_list[target] + ['-MM', source_file]).decode('ascii').split()
            depends[obj_file].extend(sorted(list(set(d for d in compile_depends if d != '\\' and d != source_file and not d.endswith(':')))))

with open("Makefile", 'w') as make_file:
    # Variables
    make_file.write(f"BIN = {program_name}\n")
    make_file.write(f"CXX = {compiler}\n")
    make_file.write(f"LD = {compiler}\n\n")
    make_file.write(f"CFLAGS = {' '.join(sorted(base_options, key=lambda s : s.lower()))}\n")
    make_file.write(f"LDFLAGS = {' '.join(base_linker_options)}\n\n")
    for target in final_targets:
        make_file.write(f"{target.upper()}_BIN_DIR = bin/{system}/{target}\n")
        make_file.write(f"OUT_{target.upper()} = {bins[target]}\n")
        make_file.write(f"LINK_DIR_{target.upper()} = {link_dirs[target]}\n")
        make_file.write(f"{target.upper()}_OBJ_DIR = obj/{system}/{target}\n")
        make_file.write(f"OBJ_{target.upper()} = ")
        make_file.write(' '.join([x for x in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)) if x.endswith('.o') and x.startswith(f"$({target.upper()}")]))
        make_file.write('\n')
        make_file.write(f"CFLAGS_{target.upper()} = {' '.join(options_list[target])}\n")
        make_file.write(f"LDFLAGS_{target.upper()} = {' '.join(linker_options[target])}\n\n\n")

    for target in sorted(depends.keys(), key=functools.cmp_to_key(make_sort)):
        make_file.write(f"{target} : {' '.join(depends[target])}\n")
        make_file.write("\n".join([f"\t{x}" for x in operations[target]]))
        make_file.write('\n\n')
