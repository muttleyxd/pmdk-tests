#!/usr/bin/env python3
#
# Copyright 2017, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


import sys
import argparse
from os import linesep, walk
from os.path import isdir, join, relpath, abspath
from difflib import unified_diff
from pathlib import PurePath
from subprocess import run, PIPE, DEVNULL, STDOUT

DOXY_START = '/**'
DOXY_END = '*/'
DOXY_EXTENSIONS = ('.cc',)
LICENCE_EXTENSIONS = ('.cc', '.cpp', '.h', '.sh',
                      '.py', '.tpp', '.txt', '.cmake')
CPP_EXTENSIONS = ('.c', '.cc', '.cpp', '.h', '.tpp')

CLANG_STYLE = ['BasedOnStyle: Google',
               'AllowShortFunctionsOnASingleLine: None',
               'AllowShortIfStatementsOnASingleLine: false',
               'AllowShortCaseLabelsOnASingleLine: false']
CLANG_STYLE = '"{' + ', '.join(CLANG_STYLE) + '}"'

missing_licence_files = []
clang_format_bin = ''


def format_file(file):
    with open(file, 'r', encoding='utf-8', newline=linesep) as f:
        lines = f.readlines()
        formatted = lines[:]

    if has_extension(file, CPP_EXTENSIONS):
        formatted = clang_format(file)
    if has_extension(file, DOXY_EXTENSIONS):
        formatted = format_doxy(formatted)
    if has_extension(file, LICENCE_EXTENSIONS):
        formatted = format_licence(formatted, file)

    return lines, formatted


def write_to_file(file, formatted):
    with open(file, 'w', encoding='utf-8') as f:
        f.write(''.join([line.replace('\r\n', '\n') for line in formatted]))


def has_extension(file, extensions):
    return any(file.endswith(ext) for ext in extensions)


def show_diff_if_exists(file, lines, formatted):
    diff = list(unified_diff(lines, formatted))
    if diff:
        print(f'{file} diff:')
        sys.stdout.writelines(diff)
        print()
        return True
    else:
        return False


def format_doxy(lines):
    indices = get_doxy_line_indices(lines)
    if indices:
        return [format_doxy_line(
            line) if i in indices else line for i, line in enumerate(lines)]
    else:
        return lines


def get_doxy_line_indices(lines):
    iterations = range(len(lines))

    def end_found(i):
        return lines[i].strip() == DOXY_END and i + 1 in iterations \
            and ('TEST_F' in lines[i + 1] or 'TEST_P' in lines[i + 1])

    start_found = False
    indices = []
    region_indices = []
    for i in iterations:
        if lines[i].strip() == DOXY_START:
            region_indices = [i]
            start_found = True
            continue
        if start_found and end_found(i):
            indices.extend(region_indices + [i])
            region_indices = []
            start_found = False
            continue
        if start_found and lines[i].strip().startswith('*'):
            region_indices.append(i)
        else:
            start_found = False

    return indices


def format_doxy_line(line):
    if line.strip() == DOXY_START and not line.startswith(DOXY_START):
        return line.strip() + linesep
    if line.strip() == DOXY_END and not line.startswith(f' {DOXY_END}'):
        return f' {DOXY_END}' + line.split(DOXY_END)[1]
    if '*' in line and r'\li' in line and not line.startswith(
            r' *          \li'):
        return r' *          \li' + line.split(r'\li')[1]
    if r'\test' in line and not line.startswith(r' * \test'):
        return r' * \test' + line.split(r'\test')[1]
    if line.strip().startswith('*') and not line.startswith(' *'):
        return ' *' + line.split('*')[1]

    return line


def format_licence(lines, filename):
    shebang = '#!'
    comments = ['/*', ' *', '#', ' */']
    if has_extension(filename, CPP_EXTENSIONS):
        comments.remove('#')

    def get_comment_in(line):
        return next(
            (c for c in comments if line.lstrip().startswith(c.strip())), None)

    licence_found = False
    formatted = lines[:]
    for i, line in enumerate(formatted):
        if i == 0 and line.startswith(shebang):
            continue
        if not get_comment_in(line) and not line.strip():
            break
        used_comment = get_comment_in(line)
        if used_comment:
            licence_found = True
            formatted[i] = used_comment + \
                line.split(used_comment.strip(), 1)[1]

    if not licence_found:
        missing_licence_files.append(filename)

    return formatted


def check_prerequisites():
    if not find_clang_format():
        sys.exit('No clang-format in version 3.9 found.')


def find_clang_format():
    global clang_format_bin
    found = False

    proc = run('clang-format --version', stdout=PIPE, stderr=DEVNULL,
               shell=True)
    if proc.returncode == 0 and b'version 3.9' in proc.stdout:
        clang_format_bin = 'clang-format'
        found = True
    else:
        proc = run('clang-format-3.9 --version', stdout=DEVNULL, stderr=STDOUT,
                   shell=True)
        if proc.returncode == 0:
            clang_format_bin = 'clang-format-3.9'
            found = True
    return found


def clang_format(file):
    cmd = clang_format_bin + ' ' + file + f' -style={CLANG_STYLE}'
    proc = run(cmd, stdout=PIPE, shell=True)
    return proc.stdout.decode('utf-8').splitlines(keepends=True)


def get_files_to_format(root_dir, ignored):
    def is_ignored(file):
        if any(ignored == directory for directory in
               PurePath(relpath(abspath(file), root_dir)).parts[:-1]):
            return True
        return False

    to_format = []
    checked_extensions = DOXY_EXTENSIONS + LICENCE_EXTENSIONS + CPP_EXTENSIONS
    for root, _, files in walk(root_dir):
        to_format.extend(join(root, file) for file in files if has_extension(
            file, checked_extensions) and not is_ignored(join(root, file)))

    return to_format


if __name__ == '__main__':
    class CheckPrerequisitesAction(argparse.Action):
        def __call__(self, parser, namespace, values, option_string=None):
            check_prerequisites()
            parser.exit()

    parser = argparse.ArgumentParser(
        description='Autoformat doxy-style test designs'
                    ' in comments and licences.')
    parser.add_argument('-p', '--path',
                        help='Path to the file to be formatted or directory'
                             ' if in recursive mode.', required=True)
    parser.add_argument(
        '-r', '--recursive', action='store_true', default=False,
        help='Recursive mode with root directory provided by'
             ' -p/--path argument')
    parser.add_argument('-i', '--in-place', action='store_true', default=False,
                        help="Format files in place,"
                             " don't end with failure in case of diffs.")
    parser.add_argument('--ignore-dir', default='build',
                        help='In recursive mode ignore elements under '
                             'directory with given name. Default: build')
    parser.add_argument('--check-prerequisites',  nargs=0,
                        help='Check prerequisites only.',
                        action=CheckPrerequisitesAction)
    parser.add_argument(
        '-q', '--quiet', action='store_true',
        default=False, help='Do not print diff to output.')
    args = parser.parse_args()
    check_prerequisites()

    diffs_after_formatting = False
    files_to_format = []

    if args.recursive:
        if not isdir(args.path):
            sys.exit(f'{args.path} is not a directory')
        files_to_format = get_files_to_format(args.path, args.ignore_dir)
    else:
        files_to_format.append(args.path)

    for file in files_to_format:
        original, formatted = format_file(file)
        diff = list(unified_diff(original, formatted))
        if diff:
            diffs_after_formatting = True
            if not args.quiet:
                print(f'{file} diff:')
                sys.stdout.writelines(diff)
                print()
            if args.in_place:
                write_to_file(file, formatted)

    if missing_licence_files:
        print('No licences found in:')
        for file in missing_licence_files:
            print(file)

    if not args.in_place and diffs_after_formatting or missing_licence_files:
        sys.exit(1)
