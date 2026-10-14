#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import tarfile
import argparse
import re
import file_io
import sys
import shutil
import os
import md5checksum
import moduleFinder
import glob
import osUtils
import installUtils
import datetime

def __setFileAttributes(tarinfo):
    tarinfo.uid = tarinfo.gid = 0
    tarinfo.uname = tarinfo.gname = "root"
    tarinfo.mode = 0o644
    tarinfo.type = tarfile.REGTYPE
    return tarinfo

def __setBinaryAttributes(tarinfo):
    tarinfo.uid = tarinfo.gid = 0
    tarinfo.uname = tarinfo.gname = "root"
    tarinfo.mode = 0o755
    tarinfo.type = tarfile.REGTYPE
    return tarinfo

def __setDirAttributes(tarinfo):
    tarinfo.uid = tarinfo.gid = 0
    tarinfo.uname = tarinfo.gname = "root"
    tarinfo.mode = 0o755
    tarinfo.type = tarfile.DIRTYPE
    return tarinfo

def updateFeatureVersion(reltag, featureFilename, featureName):
    relTagTokens = reltag.split('-')
    version = relTagTokens[0]
    status, details = osUtils.assignVariableInFile(featureFilename, '^%s_VERSION\s*=\s*\"[0-9]+.[0-9]+.[0-9]+\"$' % featureName.upper()), '%s_VERSION="%s"' % (featureName.upper(), version)
    return status, details

def getImportedModules(filename, pythonPath=None):
    # get all modules imported by python filename
    mh = moduleFinder.ModuleHandler(filename, pythonPath)
    modules = mh.getModules()
    moduleList = []
    moduleLookup = []

    for curModule in modules:
        moduleLookup.append(os.path.basename(curModule))
        moduleList.append({'name' : os.path.basename(curModule),  'path' : os.path.dirname(curModule)})
#
    return (moduleList, moduleLookup)
#
def addDirectoryToArchive(dirname, destPath, archive):
    dirName = os.path.dirname(dirname)
    if dirName != '' and dirName != os.getcwd():
        osUtils.copytree(dirname, os.path.basename(dirname), symlinks=False, ignore=None)
        archive.add(name=os.path.basename(dirname),
                    arcname="%s/%s" % (destPath, os.path.basename(dirname)))
        osUtils.rmDestPath(os.path.basename(dirname))
    else:
        archive.add(name=os.path.basename(dirname),
                    arcname="%s/%s" % (destPath, os.path.basename(dirname)))

def addEmptyDirectoryToArchive(dirname, archive):
    t = tarfile.TarInfo(dirname)
    t = __setDirAttributes(t)
    archive.addfile(t)

def addFileToArchive(filename, destPath, archive):
    dirName = os.path.realpath(os.path.dirname(filename)).replace('/', '').replace('\\', '')
    cwd = os.getcwd().replace('/', '').replace('\\', '')
#
    makeLocalCopy=False
    if dirName != '' and dirName != cwd:
        makeLocalCopy=True
#
    if os.path.isdir(filename):
        if makeLocalCopy:
            shutil.copytree(filename, os.path.basename(filename))
#
        if destPath == '.':
            if os.path.dirname(filename) == '.':
                archive.add(name=os.path.basename(filename),
                        arcname="./")
            else:
                archive.add(name=os.path.basename(filename),
                        arcname="./%s" % (os.path.basename(filename)))
        else:
            archive.add(name=os.path.basename(filename),
                    arcname="%s/%s" % (destPath, os.path.basename(filename)))
#
        if makeLocalCopy:
            shutil.rmtree(os.path.basename(filename))
    else:
        if makeLocalCopy:
            shutil.copyfile(filename, os.path.basename(filename))
#
        attr = __setFileAttributes
        if destPath == "etc/init.d":
            attr = __setBinaryAttributes
        elif installUtils.isbinary(filename):
            attr = __setBinaryAttributes
#
        archive.add(name=os.path.basename(filename),
                    arcname="%s/%s" % (destPath, os.path.basename(filename)),
                    filter=attr)
#
        if makeLocalCopy:
            os.remove(os.path.basename(filename))
#
def insertModulesIntoArchive(featureFilename, archive, destPath, pythonPath=None, checkLookup=[], installModule=False):
    modules, curModuleLookup = getImportedModules(featureFilename, pythonPath)

    for curModule in modules:
        if curModule['name'] not in checkLookup:
            addFileToArchive(os.path.join(curModule['path'], curModule['name']), destPath, archive)
            checkLookup.append(curModule['name'])
#
    if installModule:
        try:
            os.remove('install.py')
        except:
            pass
#
        shutil.copyfile(featureFilename, 'install.py')
        featureFilename = 'install.py'
#
    addFileToArchive(featureFilename, destPath, archive)
    if installModule:
        try:
            os.remove('install.py')
        except:
            pass
#
    moduleLookup = checkLookup + [os.path.basename(featureFilename)]

    return moduleLookup
#
def cleanBuildDirectory():
    for filename in glob.glob("*.tgz"):
        os.remove(filename)

    for filename in glob.glob("*.md5"):
        os.remove(filename)

def commandLineRelTag():
    parser = argparse.ArgumentParser(description="build scale feature that includes an install.py to install package and either a\n main python file that is a pure python feature or a tar gz file")
    parser.add_argument("reltag",
                        action="store",
                        help="SVN release tag 99.99.99-MMDDYYYY_HHMMSS to use for feature files")
    return parser.parse_args()

def commandLineParse():
    parser = argparse.ArgumentParser(description="build scale feature that includes an install.py to install package and either a\n main python file that is a pure python feature or a tar gz file")
    parser.add_argument("reltag",
                        action="store",
                        help="SVN release tag 99.99.99-MMDDYYYY_HHMMSS to use for feature files")

    parser.add_argument("featurename",
                        action="store",
                        help="feature name with underscores representing space")

    parser.add_argument("featurefilename",
                        action="store",
                        help="full path to feature filename (*.py or *.tgz)")

    parser.add_argument("installscript",
                        action="store",
                        help="full path to install.py filename")

    parser.add_argument("devicetype",
                        action="store",
                        choices=['hts', 'hti', 'htx', 'htseries', 'ptrhtque', 'dshtacc'],
                        help="specify device type")

    parser.add_argument("ossupported",
                        action="store",
                        choices=['linux','win', 'any'],
                        help="specify operating system supported")

    return parser.parse_args()

def buildFeatureTarball(featureFilename, relTag, containerName, destPath, featureName, installScript, osSupported, additionalFiles=[]):
    # open the tarball
    archive = tarfile.open(containerName, 'w:gz')

    moduleLookup = []

    # if the install main filename is a python file
    if os.path.splitext(os.path.basename(featureFilename))[1] == '.py':

        # update the feature version
        status, data = updateFeatureVersion(relTag, featureFilename, featureName)

        # if feature python main file version updated
        if status == 'success':
            if osSupported == 'win':
                archivePath = r'\Program Files\hobart\python'
            elif osSupported == 'linux':
                archivePath = 'usr/local/hobart/python'
            elif osSupported == 'any':
                archivePath = 'hobart/python'
            else:
                print('error os supported type not valid')
                return 'error'

            # insert all python modules imported by main python script into archive
            moduleLookup = insertModulesIntoArchive(featureFilename, archive, archivePath)

        else:
            # error
            print('error can not update version number in python feature module')
            return 'error'

    # main filename is a tarball so no pure python feature module to be installed
    elif os.path.splitext(os.path.basename(featureFilename))[1] == '.tgz':

        # add main tarball to archive
        addFileToArchive(featureFilename, destPath, archive)

    if installScript != None:
        # insert all python modules imported by install script that are not already in archive into archive
        moduleLookup = insertModulesIntoArchive(installScript, archive, destPath, checkLookup=[], installModule=True)

    if 0 < len(additionalFiles):
        for filename in additionalFiles:
            # add main tarball to archive
            addFileToArchive(filename, destPath, archive)

    # close the archive
    archive.close()

    # compute checksum
    md5FilePath = md5checksum.md5PathFromFilename(containerName)

    status = 'error'
    # write checksum file
    with open(md5FilePath,  "wb") as out_file:
        out_file.write(md5checksum.md5_for_file(containerName, block_size=256*128))
        out_file.write('\n')
        status = 'success'

    return status

def setApplicationSoftwareVersionInfo(relTag):

    tokens = relTag.split("-")
    relTag = tokens[0]
    today = datetime.date.today()
    today = today.strftime("%m/%d/%Y")

    time = datetime.datetime.now()
    time = time.strftime("%H:%M:%S")

    lines = []
    lines.append("Release ID: %s" % relTag)
    lines.append("Creation Date: %s" % today)
    lines.append("Creation Time: %s" % time)

    status, result = file_io.writeToFile('SOFTWARE_VERSION_INFO', lines)
    return status

def buildTarballName(relTag, deviceType, osSupported, featureName):
    relTagTokens = relTag.split("-")
    containerPrefix = '%s-%s-feature-%s' % (deviceType, osSupported, featureName)
    return "%s-%s.tgz" % (containerPrefix, "-".join(relTagTokens[-2:]))

def buildFeature(relTag, featureName, featureFilename, installScript, deviceType, osSupported, additionalFiles=[]):
    '''
    Builds a feature that includes either a pure python based module (all imported modules included as well)
    or anything that can be put in a .tgz file and a python install script with all the modules it imports.
    An installScript must be supplied.

    relTag - release tag in the form generated by installToTarget.sh
    featureName - name of the feature with underscores in name instead of whitespace
                  such as sm_backend
    featureFilename - tarball (.tgz) or .py file indicating the main filename to process for the feature
    installScript - main python module filename to use to install the feature on the device
    deviceType - hti, hts, htx, htseries, dshtacc, prthtque
    osSupported - linux or win
    '''
    containerName = buildTarballName(relTag, deviceType, osSupported, featureName)

    if osSupported == 'linux':
        destPath = 'usr/local/hobart/features/%s' % (featureName)
    elif osSupported == 'win':
        # TODO:
        # Needs checked for windows
        destPath = r'\Program Files\hobart\features\%s' % (featureName)
    elif osSupported == 'any':
        destPath = 'hobart/features/%s' % (featureName)
    else:
        print('OS support type not valid')
        return 'error'

    return buildFeatureTarball(featureFilename, relTag, containerName, destPath, featureName, installScript, osSupported, additionalFiles)
#
#
def buildGenericTarball(archiveName, baseArchiveDirectory, installScript, osSupported, *filenames):
    moduleLookup = []

    with tarfile.open(archiveName, 'w:gz') as generic_archive:
        # Add all additional required files to archive.
        for filename in filenames:
            if filename.endswith('.py'):
                oldBaseArchiveDirectory = baseArchiveDirectory

                if osSupported == 'win':
                    archiveDirectory = r'\Program Files\hobart\python'
                elif osSupported == 'linux':
                    archiveDirectory = 'usr/local/hobart/python'
                elif osSupported == 'any':
                    archiveDirectory = 'hobart/python'
                else:
                    print('error os supported type not valid')
                    return 'error'
                moduleLookup = insertModulesIntoArchive(filename, generic_archive, archiveDirectory, checkLookup=moduleLookup, installModule=False)
                baseArchiveDirectory = oldBaseArchiveDirectory
            else:
                addFileToArchive(filename, baseArchiveDirectory, generic_archive)

        # Add installer script to archive.
        insertModulesIntoArchive(installScript, generic_archive, baseArchiveDirectory, checkLookup=[], installModule=True)

    # compute checksum
    md5FilePath = md5checksum.md5PathFromFilename(archiveName)

    # write checksum file
    with open(md5FilePath, "wb") as md5_file:
        md5_file.write(md5checksum.md5_for_file(archiveName, block_size=256*128))
        md5_file.write('\n')

    return 'success'
#
#
def buildGenericFeature(releaseTag, featureName, deviceType, systemSupported, installScript, *filenames):
    """ Build a generic feature that includes a pure python based module and any number of supporting files necessary.

        :param releaseTag: The SVN Release Tag generated by installToTarget.sh - ###.###.###-MMDDYYYY-HHMMSS
        :param featureName: The name of this feature.
        :param deviceType: The type of device this is installed for (hti, hts, htx, htseries, dshtacc, prthtque)
        :param systemSupported: The type of system this feature supports (Linux or Win or Any).
        :param installScript: The filename of the main python installation module for this feature.
        :param filenames: var-args compiled into a list of external dependencies that this feature requires.
    """
    archiveName = buildTarballName(releaseTag, deviceType.lower(), systemSupported.lower(), featureName)

    if systemSupported.lower() == 'linux':
        baseArchiveDirectory = 'usr/local/hobart/features/{0}'
    elif systemSupported.lower() == 'win':
        baseArchiveDirectory = r'\Program Files\hobart\features\{0}'
    elif systemSupported.lower() == 'any':
        baseArchiveDirectory = 'hobart/features/{0}'
    else:
        print(('Supported system type {0} unknown'.format(systemSupported.lower())))
        return 'error'

    baseArchiveDirectory = baseArchiveDirectory.format(featureName)
    return buildGenericTarball(archiveName, baseArchiveDirectory, installScript, systemSupported, *filenames)
#
#
def buildAgnosticFeature(releaseTag, featureName, agnosticFeature):
    """ Build an OS Agnostic feature that grabs code from two isolated branches and combines them into one feature
        that will be conditionally installed based on the target system.

        :param releaseTag: The SVN Release Tag generated by installToTarget.sh - ###.###.###-MMDDYYYY-HHMMSS
        :param featureName: The name of this feature.
        :param agnosticFeatureConfig: AgnosticFeature describing the paths to used to build this feature.
            See: AgnosticFeature
    """
    archiveName = buildTarballName(releaseTag, 'htseries', 'any', featureName)
    agnosticFeature.buildArchive(archiveName, featureName)
#
#
class HobartPythonEnvironment(object):
    def __init__(self, branchPath):
        self.branchPath = branchPath
        self.originalPath = [path for path in sys.path]
        self.subDirectories = ["ScaleManagement",
                              "ScaleManagement\\source",
                              "ScaleManagement\\source\\payload",
                              "ScaleManagement\\smBackend",
                              "Windux\\Application\\shared\\sessionHandler\\python",
                              "Windux\\Application\\shared\\util\\licensing",
                              "Windux\\Application\\shared\\util\\dongleKey",
                              ]
#
    def list(self):
        pythonPath = []

        for subDirectory in self.subDirectories:
            pythonPath.append(os.path.join(self.branchPath, subDirectory))
#
        return pythonPath
#
    def __enter__(self):
        pass
#
    def __exit__(self, type, value, traceback):
        sys.path = self.originalPath
#
#
class AgnosticFeature(object):
    """ Data Container for all the data necessary for the buildAgnosticFeature method. """
    def __init__(self, **kwargs):
        """ AgnosticFeatureConfiguration initializer.

            :param linuxBranchPath: The path (absolute) to the branch (directory) that contains the code for a linux os.
            :param windowsBranchPath: The path (absolute) to the branch (directory) that contains the code for a windows os.
            :param linuxInstallerPath: The path (relative to linuxBranchPath) to the install script for a linux os.
            :param windowsInstallerPath: The path (relative to windowsBranchPath) to the install script for a windows os.
            :param linuxFilenames: The paths (relative to linuxBranchPath) to the files to install for a linux os.
            :param windowsFilenames: The paths (relative to windowsBranchPath) to the files to install for a windows os.
        """
        self.linuxBranchPath = kwargs.get('linuxBranchPath', './')
        self.windowsBranchPath = kwargs.get('windowsBranchPath', './')
        self.linuxInstallerPath = os.path.join(self.linuxBranchPath, kwargs.get('linuxInstallerPath', ''))
        self.windowsInstallerPath = os.path.join(self.windowsBranchPath, kwargs.get('windowsInstallerPath', ''))
        self.linuxFilenames = kwargs.get('linuxFilenames', [])
        self.windowsFilenames = kwargs.get('windowsFilenames', [])
#
    def buildArchive(self, archivePath, featureName):
        with tarfile.open(archivePath, 'w:gz') as archive:
            self.buildWindowsArchive(archive, featureName)
            self.buildLinuxArchive(archive, featureName)
#
        md5FilePath = md5checksum.md5PathFromFilename(archivePath)

        with open(md5FilePath, "wb") as md5_file:
            md5_file.write(md5checksum.md5_for_file(archivePath, block_size=256*128))
            md5_file.write('\n')
#
    def insertPythonScript(self, archive, moduleLookup, pythonBaseDirectory, absoluteFilePath, pythonPath=None):
        return insertModulesIntoArchive(absoluteFilePath,
                                        archive,
                                        pythonBaseDirectory,
                                        pythonPath,
                                        checkLookup=moduleLookup,
                                        installModule=False)
#
    def addFileNames(self, archive, branchPath, installerPath, filenames, featureBaseDirectory, pythonBaseDirectory, pythonPath=None):
        moduleLookup = []

        for relativeFilePath in filenames:
            absoluteFilePath = os.path.join(branchPath, relativeFilePath)

            if absoluteFilePath.endswith('.py'):
                moduleLookup = self.insertPythonScript(archive, moduleLookup, pythonBaseDirectory, absoluteFilePath, pythonPath)
            else:
                addFileToArchive(absoluteFilePath, featureBaseDirectory, archive)

        insertModulesIntoArchive(installerPath, archive, featureBaseDirectory, pythonPath, checkLookup=[], installModule=True)
#
    def buildWindowsArchive(self, archive, featureName):
        featureBaseDirectory = 'windows/hobart/features/{0}'.format(featureName)
        pythonBaseDirectory = 'windows/hobart/python'
        pythonPath = self.windowsPythonEnvironment()

        with ChangeWorkingDirectory(os.path.dirname(self.windowsInstallerPath)):
            self.addFileNames(archive,
                              self.windowsBranchPath,
                              self.windowsInstallerPath,
                              self.windowsFilenames,
                              featureBaseDirectory,
                              pythonBaseDirectory,
                              pythonPath)
#
    def buildLinuxArchive(self, archive, featureName):
        featureBaseDirectory = 'linux/hobart/features/{0}'.format(featureName)
        pythonBaseDirectory = 'linux/hobart/python'
        pythonPath = self.linuxPythonEnvironment()

        with ChangeWorkingDirectory(os.path.dirname(self.linuxInstallerPath)):
            self.addFileNames(archive,
                              self.linuxBranchPath,
                              self.windowsInstallerPath,
                              self.windowsFilenames,
                              featureBaseDirectory,
                              pythonBaseDirectory,
                              pythonPath)
#
    def windowsPythonEnvironment(self):
        return HobartPythonEnvironment(self.windowsBranchPath).list()
#
    def linuxPythonEnvironment(self):
        return HobartPythonEnvironment(self.linuxBranchPath).list()
#
#
class ChangeWorkingDirectory(object):
    def __init__(self, newDirectory):
        self.newDirectory = newDirectory
        self.oldDirectory = os.getcwd()
#
    def __enter__(self):
        os.chdir(self.newDirectory)
#
    def __exit__(self, type, value, traceback):
        os.chdir(self.oldDirectory)
#
#
if __name__ == '__main__':
    args = commandLineParse()
    if buildFeature(args.reltag, args.featurename, args.featurefilename, args.installscript, args.scaletype, args.osSupported) == 'success':
        sys.exit(0)
    else:
        sys.exit(1)
