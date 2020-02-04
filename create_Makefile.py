#!/bin/python

import os, functools, subprocess, sys

def target_sort(a, b):
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

    # Sort targets creating files after meta-targets
    if a[0] == '$':
        if b[0] != '$':
            return 1
    elif b[0] == '$':
        return -1

    if a < b:
        return -1
    if a > b:
        return 1

def all_targets_so_far(depends, operations):
    return sorted(list(set(list(depends.keys()) + list(operations.keys()))), key=functools.cmp_to_key(target_sort))

if len(sys.argv) == 1 or sys.argv[1] not in ['gcc', 'clang']:
    print('Specify a compiler ("gcc" or "clang")')
    sys.exit(1)

program_name = 'genetic_chess'
final_targets = ["release", "debug"]
depends = dict()
depends['all'] = final_targets + ["docs"]
depends['clean'] = (f"clean_{target}" for target in depends['all'])
depends["docs"] = ["user_manual", "code_docs"]
depends['clean_docs'] = (f"clean_{target}" for target in depends['docs'])

obj_dest = dict()
operations = dict()
bins = dict()
link_dirs = dict()

depends["code_docs"] = ['$(DOC_INDEX)']
operations['$(DOC_INDEX)'] = ['doxygen']
depends['$(DOC_INDEX)'] = ["Doxyfile", "$(ALL_SOURCES)"]

user_manual = 'doc/reference.pdf'
user_manual_tex = f'{os.path.splitext(user_manual)[0]}.tex'
user_manual_var = '$(USER_MANUAL)'
depends["user_manual"] = [user_manual_var]
depends[user_manual_var] = [
    user_manual_tex,
    'gene_pool_config_example.txt',
    'genetic_ai_example.txt',
    'doc/game-endings-log-plot.png',
    'doc/game_length_distribution.png',
    'doc/game_length_log_norm_distribution.png',
    'doc/pawn-crash-strength-plot.png',
    'doc/piece-strength-with-king-plot.png',
    'doc/win-lose-plot.png']
operations[user_manual_var] = [f'latexmk -synctex=1 -pdf -cd {user_manual_tex}']

for target in final_targets:
    out_variable = f"$(OUT_{target.upper()})"
    all_objects = f"$(OBJ_{target.upper()})"
    bin_dest = f"$({target.upper()}_BIN_DIR)"
    operations[out_variable] = [f"mkdir -p {bin_dest}",
                                ' '.join(["$(LD)",
                                          "-o", out_variable,
                                          all_objects,
                                          "$(LDFLAGS)",
                                          f"$(LDFLAGS_{target.upper()})",
                                          "$(CFLAGS)",
                                          f"$(CFLAGS_{target.upper()})"])]
    link_dirs[target] = os.path.join('bin', target)
    link_dir_variable = f"$(LINK_DIR_{target.upper()})"
    operations[os.path.join(link_dir_variable, '$(BIN)')] = [f'mkdir -p {link_dir_variable}',
                                                             f'ln -sf -t {link_dir_variable} `realpath {out_variable}`',
                                                             f'touch {out_variable}']
    depends[os.path.join(link_dir_variable, '$(BIN)')] = [out_variable]

    depends[target] = [out_variable, os.path.join(link_dir_variable, '$(BIN)')]
    depends[out_variable] = [all_objects]

    obj_dest[target] = f"$({target.upper()}_OBJ_DIR)"
    operations[f'clean_{target}'] = [f"rm -rf {obj_dest[target]} {bin_dest}"]

    bins[target] = os.path.join(bin_dest, '$(BIN)')
    depends[f'test_{target}'] = [target]
    operations[f'test_{target}'] = [f'{bins[target]} -{opt}' for opt in ['test', 'perft', 'speed']]

operations['clean_code_docs'] = ['rm -rf $(DOC_DIR)']
operations['clean_user_manual'] = [f'latexmk -C -cd {user_manual_tex}']
depends['test_all'] = [f'test_{x}' for x in final_targets]
depends['.PHONY'] = [t for t in all_targets_so_far(depends, operations) if not t.startswith('$')]

options_list = dict()
linker_options = dict()
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

system = sys.argv[1]
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
        "-Wunused-exception-parameter",
        "-Wswitch"])

all_sources = []
for target in final_targets:
    for (dirpath, dirnames, filenames) in os.walk(os.getcwd()):
        dirpath = os.path.relpath(dirpath)
        if dirpath == '.': dirpath = ''
        for source_file in [os.path.join(dirpath, fn) for fn in filenames]:
            if source_file.endswith('.h') or source_file.endswith('.cpp'):
                all_sources.append(source_file)
            if not source_file.endswith('.cpp'):
                continue
            obj_file = os.path.join(obj_dest[target], f"{os.path.splitext(source_file)[0]}.o")
            operations[obj_file] = [f"mkdir -p {os.path.dirname(obj_file)}",
                                    f"$(CXX) $(CFLAGS) $(LDFLAGS) $(CFLAGS_{target.upper()}) $(LDFLAGS_{target.upper()}) -c {source_file} -o {obj_file}"]

            compiler_command = [compiler] + base_options + options_list[target] + ['-MM', source_file]
            compile_depends = subprocess.check_output(compiler_command).decode('ascii').split(':', 1)[1].split()
            depends[obj_file] = list(set(d for d in compile_depends if d != '\\'))

with open("Makefile", 'w') as make_file:
    make_file.write(f"BIN = {program_name}\n")
    make_file.write(f"CXX = {compiler}\n")
    make_file.write(f"LD = {compiler}\n\n")
    make_file.write(f"CFLAGS = {' '.join(base_options)}\n")
    make_file.write(f"LDFLAGS = {' '.join(base_linker_options)}\n\n")
    make_file.write(f"DOC_DIR = {os.path.join('doc', 'doxygen', 'html')}\n")
    make_file.write(f"DOC_INDEX = {os.path.join('$(DOC_DIR)', 'index.html')}\n")
    make_file.write(f"ALL_SOURCES = {' '.join(all_sources)}\n\n")
    make_file.write(f"USER_MANUAL = {user_manual}\n\n")
    for target in final_targets:
        make_file.write(f"{target.upper()}_BIN_DIR = bin/{system}/{target}\n")
        make_file.write(f"OUT_{target.upper()} = {bins[target]}\n")
        make_file.write(f"LINK_DIR_{target.upper()} = {link_dirs[target]}\n")
        make_file.write(f"{target.upper()}_OBJ_DIR = obj/{system}/{target}\n")
        make_file.write(f"OBJ_{target.upper()} = ")
        make_file.write(' '.join([x for x in all_targets_so_far(depends, operations) if x.endswith('.o') and x.startswith(f"$({target.upper()}")]))
        make_file.write('\n')
        make_file.write(f"CFLAGS_{target.upper()} = {' '.join(options_list[target])}\n")
        make_file.write(f"LDFLAGS_{target.upper()} = {' '.join(linker_options[target])}\n\n")

    for target in all_targets_so_far(depends, operations):
        make_file.write(f"{target} : {' '.join(depends.get(target, []))}\n")
        make_file.write("\n".join([f"\t{x}" for x in operations.get(target, [])]))
        make_file.write('\n\n')
