import os
import sys

sys.path.append(os.path.join('..', 'Application', 'shared', 'sessionHandler', 'python'))

import argparse
import buildFeature
import file_io
import md5checksum
import re
import shellCommand
import shutil
import tarfile
from array import array

def byteSwapFile(inFilename,  outFilename):
    a = array('H',open(inFilename, "rb").read())
    a.byteswap()
    open(outFilename, "wb").write(a)

def extractApplicationSRecs(inFile,  outFile):
    appSentinel = re.compile('^S[0-9,A-F][0-9,A-F][0-9,A-F][0-9,A-F][0-9,A-F][0-9,A-F]3FFE8[0-9,A-F]+$')
    status,  data = file_io.readFromFile(inFile)
    output = []
    if status == 'success':
        for line in data:
            if appSentinel.match(line):
                output.append(line)
                break
            else:
                output.append(line)

        status,  message = file_io.writeToFile(outFile,  output)
        if status == 'success':
            return True

    return False

def main():
    parser = argparse.ArgumentParser(description="Build wrapper image files from node id s-records, named node 1-6")
    parser.add_argument("release_type",
                        action="store",
                        help="The type of node board release: aws3 or ngw or or ngw2p or ngs")
    parser.add_argument("release_version",
                        action="store",
                        help="The version of the entire node board release")
    parser.add_argument("node1_to_5_boards_version",
                        action="store",
                        help="The version of the node boards 1-5")
    parser.add_argument("node6_board_version",
                        action="store",
                        help="The version of the node 6 or camera board")
    parser.add_argument("tof_app_ver",
                        action="store",
                        help="The version of the TOF App")
    parser.add_argument("tof_boot_ver",
                        action="store",
                        help="The version of TOF Boot")

    parser.set_defaults(windows=False)
    args = parser.parse_args()

    if ('aws3' != args.release_type) and ('ngw' != args.release_type) and ('ngw2p' != args.release_type) and ('ngs' != args.release_type):
        print 'Invalid release type specified.'
        sys.exit(1)

    if  os.path.exists(os.path.join('.', 'release')):
        shutil.rmtree(os.path.join(".", 'release'))

    for current_node in xrange(1, 7):
        if not os.path.exists(os.path.join('.', 'release', 'node' + str(current_node))):
            os.makedirs(os.path.join(".", 'release', 'node' + str(current_node)))

        if current_node <= 5:
            srecordFilename = os.path.join('.', 'Node%d_APP.elf.p.S' % (current_node))
        else:
            srecordFilename = os.path.join('.', 'WrapperCameraSystem.elf.p.S')

        if extractApplicationSRecs(srecordFilename, os.path.join('.',  'release', 'node' + str(current_node), 'node' + str(current_node) + '.srec')):

            if os.name == "posix":
                cmd = []
                cmd.append("chmod")
                cmd.append("+x")
                cmd.append("./dsc-checksum")

                exitCode,  stdout,  stderr = shellCommand.executeShellCommand(cmd)

            cmd = []
            if os.name == "posix":
                cmd.append("./dsc-checksum")
            else:
                cmd.append("dsc-checksum.exe")

            cmd.append(srecordFilename)
            if current_node <= 5:
                cmd.append(args.node1_to_5_boards_version)
            else:
                cmd.append(args.node6_board_version)

            cmd.append(str(current_node))

            exitCode,  stdout,  stderr = shellCommand.executeShellCommand(cmd)

            if exitCode == 0:
                if os.path.exists(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.srec')):
                    os.remove(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.srec'))

                # move nodeN to nodeN release directory
                os.rename(os.path.join('.', 'node' + str(current_node) + '.bin'),
                                os.path.join('.', 'release', 'node' + str(current_node),  'node' + str(current_node) + '.bin'))

                #byteSwapFile(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.bin'),
                                    #os.path.join('.', 'release', 'node' + str(current_node), 'source.bin'))

                if os.path.exists(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.bin')):
                    os.rename(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.bin'),
                                    os.path.join('.', 'release', 'node' + str(current_node), 'source.bin'))

                if os.path.exists(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.bin')):
                    os.remove(os.path.join('.', 'release', 'node' + str(current_node), 'node' + str(current_node) + '.bin'))

                if current_node <= 5:
                    status,  message = file_io.writeToFile(os.path.join('.','release', 'node' + str(current_node),  'version.ini'),  [args.node1_to_5_boards_version])
                else:
                    status,  message = file_io.writeToFile(os.path.join('.','release', 'node' + str(current_node),  'version.ini'),  [args.node6_board_version])

                if status == 'success':
                    print 'Node number %d release data generated' % (current_node)
            else:
                    print 'Could not generate checksum on binary via dsc-checksum'
                    sys.exit(1)
        else:
            print 'Could not find file or extract application data from specfied s-record filename: %s' % ('Node%d.elf.S' % (current_node))
            sys.exit(1)

    # SOF-4509: Include TOP App and Boot firmware
    if args.release_type == 'ngw2p':
        if not os.path.exists(os.path.join('.', 'release', 'tof/app')):
            os.makedirs(os.path.join(".", 'release/tof/app'))
            shutil.move('sourceTOFApp.bin', 'release/tof/app/sourceTOFApp.bin')
            status,  message = file_io.writeToFile(os.path.join('.','release', 'tof/app',  'version.ini'),  [args.tof_app_ver])
            if status == 'success':
                print 'Created version.ini for TOF App'

        if not os.path.exists(os.path.join('.', 'release', 'tof/boot')):
            os.makedirs(os.path.join(".", 'release/tof/boot'))
            shutil.move('sourceTOFBoot.bin', 'release/tof/boot/sourceTOFBoot.bin')
            status,  message = file_io.writeToFile(os.path.join('.','release', 'tof/boot',  'version.ini'),  [args.tof_boot_ver])
            if status == 'success':
                print 'Created version.ini for TOF Boot'

    feature = 'htseries-any-%s-%s-release' % (args.release_type, args.release_version)

    # tar firmware
    archiveName = '%s.tgz' % (feature)

    with tarfile.open(archiveName, 'w:gz') as generic_archive:
        for current_node in xrange(1, 7):
            buildFeature.addFileToArchive(os.path.join('.', 'release', 'node' + str(current_node)),  os.path.join('.'), generic_archive)

        # SOF-4509: Include TOF App and Boot firmware - if 2nd Label Applier
        if args.release_type == 'ngw2p':
            buildFeature.addFileToArchive( 'release/tof', os.path.join('.'), generic_archive)
        
    # generate md5
    md5FilePath = md5checksum.md5PathFromFilename(archiveName)

    with open(md5FilePath, "wb") as md5_file:
        md5_file.write(md5checksum.md5_for_file(archiveName, block_size=256*128))
        md5_file.write('\n')

    # sign tarball
    working_directory = os.getcwd()
    os.chdir('../Application/certs/')
    if os.name == "posix":
        exitCode = os.system("./signFeatureInstaller.sh ../../buildWrapperImageTools/htseries-any-%s-%s-release.tgz" % (args.release_type, args.release_version))
    else:
        exitCode = os.system("signFeatureInstaller.cmd ../../buildWrapperImageTools/htseries-any-%s-%s-release.tgz" % (args.release_type, args.release_version))
    os.chdir(working_directory)

    # package up everything for deployment
    packageName = os.path.join('.', '%s.package.tgz' % (feature))

    with tarfile.open(packageName, 'w:gz') as generic_archive:
        buildFeature.addFileToArchive(os.path.join('.', '%s.tgz' % (feature)),  os.path.join('.'), generic_archive)
        buildFeature.addFileToArchive(os.path.join('.', '%s.md5' % (feature)), os.path.join('.'), generic_archive)
        buildFeature.addFileToArchive(os.path.join('.', '%s.tgz.crt' % (feature)),  os.path.join('.'), generic_archive)
        buildFeature.addFileToArchive(os.path.join('.', '%s.tgz.signed' % (feature)), os.path.join('.'), generic_archive)

    print 'The package file %s created successfully.' % (packageName)
    sys.exit(0)

if __name__ == '__main__':
    main()
