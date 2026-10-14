import os
import string
import file_io
import re
import sys


def file_match_and_replace(filename, regex_pattern_match_value, replacement_value):
    regex_pattern = re.compile(regex_pattern_match_value)
    status, data = file_io.read_from_file(filename)
    if status == 'success':
        newLines = []
        for i in range(0, len(data)):
            if regex_pattern.match(data[i]):
                # dos bat file is crazy as far as passing arguments to scripts such as this one
                # ultimately you have to put three quotes to get two and then we have
                # to strip one set off.  Faster to handle issue in python the escape
                # command in dos bat file
                newLines.append(replacement_value.replace('""', '"'))
            else:
                newLines.append(data[i])

        status, data = file_io.write_to_file(filename, newLines)
        if status != 'success':
            return 'error', None
        else:
            return 'success', None
    else:
        return 'error', None


if __name__ == '__main__':
    # print(sys.argv[1])
    # print(sys.argv[2])
    # print(sys.argv[3])

    status, data = file_match_and_replace(sys.argv[1], sys.argv[2], sys.argv[3])
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
