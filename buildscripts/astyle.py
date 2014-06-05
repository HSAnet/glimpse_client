#!/usr/bin/env python3

import re
import sys


def format_initializer_list(content):
    new_content = []
    is_func_header = False
    re_func_header = re.compile(r'[^:,].+\(.*\)$')
    re_func_block = re.compile(r'{$')
    re_init_list = re.compile(r'[:,] .+')

    for line in content:
        if re_func_header.match(line.strip()):
            is_func_header = True
            # count indentation of function header
            indentation = 0
            for c in line:
                if c == ' ':
                    indentation += 1
                else:
                    break
            new_content.append(line)
            continue

        if is_func_header and re_func_block.match(line.strip()):
            is_func_header = False
            new_content.append(line)
            continue

        if is_func_header and re_init_list.match(line.strip()):
            # apply same indentation to initializer list
            new_content.append('{}{}'.format(' ' * indentation, line.lstrip()))
            continue

        new_content.append(line)

    return new_content


def main(*args, **kwargs):
    for file_ in args:
        with open(file_) as f:
            content = f.readlines()

        content = format_initializer_list(content)

        with open(file_, 'w') as f:
            print(''.join(content), end='', file=f)


if __name__ == '__main__':
    main(*sys.argv[1:])
