# -*- coding: utf-8 -*-

# not tested in python3

import imp
import os
import sys

from modulefinder import ModuleFinder


PYTHON_INSTALL_PATH = sys.exec_prefix


class ModuleHandler(object):
    def __init__(self, filename, pythonPath=None):
        mf = ModuleFinder(pythonPath)

        # Run the main script
        mf.run_script(filename)
        self.modules = []

        for i in list(mf.modules.keys()):
            try:
                path = None
                f, path, desc = imp.find_module(i)

                if (desc[2] == imp.C_BUILTIN):
                    #print 'module %s is a builtin' % (i)
                    pass

                elif (not self.isBuiltInModule(path)):
                    #print 'module %s is not buitin' % (i)
                    self.modules.append(path)

            except:
                if path != None and not self.isBuiltInModule(path):
                    #print '%s is not a builtin module' % (i)
                    self.modules.append(path)
#
        had_errors = False
        missing_modules = []

        for module_name, referring_items in list(mf.badmodules.items()):
            try:
                module_file, module_path, module_description = imp.find_module(module_name)
            except ImportError:
                if module_name in sys.modules:
                    continue
#
                for referring_item in referring_items:
                    if not referring_item or referring_item == '-':
                        continue

                    try:
                        module_file, module_path, module_description = imp.find_module(referring_item.split('.')[0])
                    except ImportError:
                        # Print customized message when modules cannot be loaded.
                        print(('Module {0} referring to {1} could not be imported.'.format(referring_item, module_name)))
                        missing_modules.append(module_name)
                        had_errors = True
                        continue
#
                    if PYTHON_INSTALL_PATH in module_path:
                        continue
                    elif self.isBuiltInModule(module_path):
                        continue
#
                    try:
                        print(('Attempting to import module {0}, ignore any output that follows:'.format(referring_item)))
                        __import__(referring_item)
                    except ImportError:
                        # Print customized message when modules cannot be loaded.
                        print(('Module {0} referring to {1} could not be imported.'.format(referring_item, module_name)))
                        missing_modules.append(module_name)
                        had_errors = True
#
            if module_description[2] == imp.C_BUILTIN:
                # Ignore builtin modules
                continue
#
            elif PYTHON_INSTALL_PATH in module_path:
                # Ignore modules in python install path.
                continue
#
            elif self.isBuiltInModule(module_path):
                # Ignore builtin modules
                continue
#
        if had_errors:
            for module_name in missing_modules:
                print(('Module {0} could not be imported'.format(module_name)))

            raise ImportError('Some modules could not be loaded, please check your PYTHONPATH environment variable, build cancelled.')
#
    def getModules(self):
        return self.modules
#
    def isBuiltInModule(self, m):
        #print os.path.dirname(m)
        if ('nt' == os.name):
            if (os.path.join("C:\\", "Python27") not in os.path.dirname(m)):
                return False
#
        else:
            if ((os.path.join("/", "usr", "lib", "python") not in os.path.dirname(m)) and
                (os.path.join("/", "usr", "local", "lib", "python") not in os.path.dirname(m)) and
                (os.path.join(os.path.expanduser("~"), ".local", "lib", "python") not in os.path.dirname(m))):
                return False
#
        return True
#
    def __str__(self):
        print(("\n".join(self.modules)))
#
