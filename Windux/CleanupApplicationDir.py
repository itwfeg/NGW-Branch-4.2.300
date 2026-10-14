import os
import filecmp
import argparse
import tarfile
import shutil
import logging

# ----------------------------------------------------------------------------------------------------------------------
#   Author: Eric Landes
#   Company: ITWFEG
#   Creation Date: 11/14/2018
#
#   Purpose: This script is to be called after populating the Release/application directory, and before packaging the
#            directory into an application upgrade. This script walks through the application directory and compares the
#            files to the extracted files from the given core upgrade  tarball. If the file in the application directory
#            is identical to the file from the tarball, the application file is removed. This effectively reduces the
#            application directory to only the files that have changed since the given core tarball
# ----------------------------------------------------------------------------------------------------------------------



basePath = os.path.join(os.getcwd(), '..', 'Base')
applicationPath = os.path.join(os.getcwd(), '..', 'Release', 'application')
# applicationPath = os.path.join(os.getcwd(), '..', 'application')
# basePath = os.path.join('C:\\Users\\eric.landes\\Desktop', 'Base')
# applicationPath = os.path.join('C:\\Users\\eric.landes\\Desktop', 'application')
quietMode = False
logging.basicConfig(filename=os.path.join(os.getcwd(), 'CleanupApplication.log'), level=logging.DEBUG)

def __quietPrint(str):
    """
    Prints if quiet flag is not set
    :param str: the string to print
    :return: None
    """
    if not quietMode:
        print(str)
    logging.debug(str)


def __extractTarball(tarball):
    """
    Extracts the given tarball to the Base directory

    :param tarball: network client stream
    :returns True if the extraction was successful false if it failed

    """
    try:
        __quietPrint('Using {}'.format(tarball))
        if os.path.isdir(basePath):
            __quietPrint('Removing Base Directory')
            shutil.rmtree(basePath)
        os.mkdir(basePath)

        os.system('tar -xzf {} -C {}'.format(tarball, basePath))
        #with tarfile.open(tarball, 'r:gz') as intarball:
        #    memeberNames = intarball.getnames()
        #    for root, dirs, files in os.walk(applicationPath):
        #        for file in files:
        #            relpath = './' + os.path.relpath(os.path.join(root, file), applicationPath)
        #            relpath = relpath.replace('\\', '/')
        #            if (relpath in memeberNames):
        #                intarball.extract(relpath, basePath)
        #                __quietPrint('Extracting {}'.format(relpath))
            #intarball.extractall(path=basePath)

    except Exception as e:
        __quietPrint(e)
        return False

def __CleanupApplication():
    """
    Compares files in
    :return: None
    """
    with open("ApllicationPackingList", 'w') as list:
        if os.path.isdir(basePath) and os.path.isdir(applicationPath):
            for root, dirs, files in os.walk(applicationPath):
                for file in files:
                    try:
                        appFile = os.path.join(root, file)
                        baseFile = os.path.join(basePath, os.path.relpath(os.path.join(root, file), applicationPath))
                        if os.path.isfile(appFile):
                            if os.path.isfile(baseFile):
                                # __quietPrint("Comparing {} to {}".format(appFile, baseFile))
                                if filecmp.cmp(appFile, baseFile, False):
                                    __quietPrint("Removing {}".format(appFile))
                                    os.system("rm -f \"{}\"".format(appFile))
                                else:
                                    list.write("{}\n".format(os.path.relpath(os.path.join(root, file))))
                            else:
                                __quietPrint("File Not in Base {} from {}".format(baseFile, appFile))
                                list.write("{}\n".format(os.path.relpath(os.path.join(root, file))))
                        else:
                            __quietPrint("File Not in app?? {}".format(appFile))
                    except Exception as e:
                        __quietPrint(e)
                        pass
        else:
            __quietPrint("{} or {} not found try running without -k".format(basePath, applicationPath))
            return False

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Compares /Release/application to the given tarball and removes "
                                                 "identical files")
    parser.add_argument('-t', dest='tarball', help='A path to the tarball to compare with the application directory',
                        required=True)
    parser.add_argument('-k', dest='keep', action='store_true', help='Use the currently extracted tarball')
    parser.add_argument('-q', dest='quiet', action='store_true', help='Shut up and don\'t let me know if you\'re broken')
    args = parser.parse_args()
    quietMode = args.quiet
    if not args.keep or not os.path.isdir(basePath):
        __extractTarball(args.tarball)
    __CleanupApplication()
