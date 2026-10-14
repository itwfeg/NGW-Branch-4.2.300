# coding=utf-8
""" Compile all PyQt5 ui and Resource files in a directory, or the current working directory. """
import argparse
import os

from PyQt5 import uic


def compile_forms(directory=None):
    """ Compile all QT UI forms in a given directory, recursively.

        This follows the naming scheme that Eric IDE uses, namely:
            {filename}.ui -> Ui_{filename}.py

        Usage:
            import compile_forms
            # Compile current working directory
            compile_forms.compile_forms()
            # Compile specific forms directory
            compile_forms.compile_forms("/path/to/forms")

        :param directory: The directory to compile forms in, defaults to the current working directory.
        :return: None
    """
    if not directory:
        directory = os.getcwd()

    uic.compileUiDir(directory, recurse=True, map=get_ui_directory)


def compile_resource(resource_name):
    """ Compile a given QT UI resource file.

        This follows the naming scheme the Eric IDE uses, namely:
            {filename}.qrc -> {filename}_rc.py

        NOTE: The compiled resource module will be placed in the same directory as the qrc file.

        Usage:
        import compile_forms
        compile_forms.compile_resource("/path/to/resource.qrc")

        :param resource_name: The path to the resource file to compile.
        :return: None
    """
    print("Compile {0} to {1}".format(resource_name + '.qrc', resource_name + '_rc.py'))
    os.system("pyrcc5 {0}.qrc -o {0}_rc.py".format(resource_name))


def get_ui_directory(directory_name, file_name):
    """ Get the qualified ui filename for a given ui filename. """
    print("Compiling {0} to {1}".format(file_name, 'Ui_' + file_name))
    file_name = 'Ui_' + file_name
    return directory_name, file_name


if __name__ == '__main__':
    """ Run the script compiler via command line.
    
        Usage examples:
        # Compile forms in the cwd.
        python compile_forms.py
        # Compile forms at a specific path.
        python compile_forms.py --directory /path/to/forms/
        # Compile a resource as well as forms.
        python compile_forms.py --directory /path/to/forms/ --resource /path/to/resource.qrc
    """
    parser = argparse.ArgumentParser(description="Recursive PyQt5 UI and resource compiler.")
    parser.add_argument('--directory', default="cwd")
    parser.add_argument('--resource', default=None)
    args = parser.parse_args()

    if args.directory == "cwd":
        directory_arg = os.getcwd()
    else:
        directory_arg = args.directory

    if args.resource:
        compile_resource(args.resource)

    compile_forms(directory_arg)
