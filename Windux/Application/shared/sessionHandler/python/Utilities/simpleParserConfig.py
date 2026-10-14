# -*- coding: utf-8 -*-

import file_io

class Dictionary:

    def __init__(self, filename, delimiter='=', iniFormat=False):
        self.filename = filename
        self.delimiter = delimiter
        self.iniFormat = iniFormat

    def read(self):
        values = {}
        lineNo = 0
        status, lines = file_io.readFromFile(self.filename)
        if status == 'success':
            for line in lines:
                lineNo += 1
                line = line.strip()
                if line != '':
                    if self.iniFormat and line[0] == '[':
                        # section marker so ignore
                        continue
                    else:
                        tokens = line.split(self.delimiter)
                        if len(tokens) == 1:
                            values[tokens[0]] = None
                        elif len(tokens) == 2:
                            values[tokens[0]] = tokens[1]
                        else:
                            return ('error', 'Invalid name=value pair on line %d' % (lineNo))

            return ('success',values)
        else:
            return ('error', 'File named %s not found' % self.filename)

    def write(self, nameValues):
        lines = []
        for name in list(nameValues.keys()):
            if nameValues[name] == None:
                name = name.strip()
                if self.iniFormat and name[0] == '[':
                    lines.append(name)
                    continue
                else:
                    value = ''
            else:
                value = nameValues[name]
            lines.append("%s=%s" % (name, value))
        status,  message = file_io.writeToFile(self.filename, lines)
        if status == 'success':
            return ('success', None)
        else:
            return ('error', message)
