#!/usr/bin/env python3
#
# AUTHOR  : Randy Blankley
# COMPANY : ITW Hobart
# DATE    : November 13, 2017
#
# OVERVIEW
# This script erases and re-initializes mtd sections.
#
###################################################################################################

import argparse
import os
import shellCommand
import sys

import updateSplashDiagnostics
import updateSplashImage
import updateSplashVersion

###################################################################################################
#
# flash erase
#
###################################################################################################
def flasherase(section, offset, size):

    print(('erase ' + section + '...'))

    # block size
    blockSize = 0x1000

    # perform flash erase
    cmd = []
    cmd.append(os.path.join('/','usr','sbin','flash_erase'))
    cmd.append(section)
    cmd.append(str(offset / blockSize))
    cmd.append(str(size / blockSize))

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # check result
    if exitCode != 0:
        print(('Error erasing MTD ' + section))
        print(stdout)
        print(stderr)

###################################################################################################
#
# main
#
###################################################################################################
def main(actions):

    # mtd2
    # scale/wrapper data   0x10000
    # uversion bmp         0x10000
    if 'mtd2' in actions:

        # ARK
        # uversion bmp in its own partion
        if os.path.exists(os.path.join("/","dev", "mtd9" )):

            # erase
            flasherase(os.path.join('/','dev','mtd2'), 0, 0x10000)

        else:

            # erase
            flasherase(os.path.join('/','dev','mtd2'), 0, 0x20000)

            # refresh image
            updateSplashVersion.main()

    # mtd3
    # mfg section
    if 'mtd3' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd3'), 0, 0x10000)

    # mtd4
    # hw config
    if 'mtd4' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd4'), 0, 0x10000)

    # mtd5
    # diag bmp             0x10000
    # missing bmp          0x10000
    # sw config            0x20000
    if 'mtd5' in actions:

        # ARK
        # diag and missing bmps in its own partion
        if os.path.exists(os.path.join("/","dev", "mtd10" )) or os.path.exists(os.path.join("/","dev", "mtd11" )):

            # erase
            flasherase(os.path.join('/','dev','mtd5'), 0, 0x10000)

        else:

            # erase
            flasherase(os.path.join('/','dev','mtd5'), 0, 0x40000)

            # refresh image
            updateSplashDiagnostics.main()

    # mtd6
    # keys
    if 'mtd6' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd6'), 0, 0x80000)

    # mtd7
    # usplash bmp
    if 'mtd7' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd7'), 0, 0x20000)

        # refresh image
        updateSplashImage.main()

    # mtd8 (ARK only)
    # usplash alt bmp
    if 'mtd8' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd8'), 0, 0x20000)

        #TODO
        # load alt boot splash

    # mtd9 (ARK only)
    # uversion bmp
    if 'mtd9' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd9'), 0, 0x10000)

        # refresh image
        updateSplashVersion.main()

    # mtd10 (ARK only)
    # diag bmp
    if 'mtd10' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd10'), 0, 0x10000)

        # refresh image
        updateSplashDiagnostics.updateDiagBitmap()

    # mtd11 (ARK only)
    # missing bmp
    if 'mtd11' in actions:
        # erase
        flasherase(os.path.join('/','dev','mtd11'), 0, 0x10000)

        # refresh image
        updateSplashDiagnostics.updateMissingBitmap()


if __name__ == "__main__":

    if 'posix' != os.name:
        print('This OS is not supported.')
        sys.exit(1)

    # setup parser
    parser = argparse.ArgumentParser(description='Initialize MTD Sections.')

    parser.add_argument('--all', dest='allmtd', action='store_true', help='Initialize all MTD sections.')
    parser.add_argument('--mtd2', dest='mtd2', action='store_true', help='Initialize only MTD2.')
    parser.add_argument('--mtd3', dest='mtd3', action='store_true', help='Initialize only MTD3.')
    parser.add_argument('--mtd4', dest='mtd4', action='store_true', help='Initialize only MTD4.')
    parser.add_argument('--mtd5', dest='mtd5', action='store_true', help='Initialize only MTD5.')
    parser.add_argument('--mtd6', dest='mtd6', action='store_true', help='Initialize only MTD6.')
    parser.add_argument('--mtd7', dest='mtd7', action='store_true', help='Initialize only MTD7.')

    parser.set_defaults(allmtd=False)
    parser.set_defaults(mtd2=False)
    parser.set_defaults(mtd3=False)
    parser.set_defaults(mtd4=False)
    parser.set_defaults(mtd5=False)
    parser.set_defaults(mtd6=False)
    parser.set_defaults(mtd7=False)

    if os.path.exists(os.path.join("/","dev", "mtd8" )):
        parser.add_argument('--mtd8', dest='mtd8', action='store_true', help='Initialize only MTD8.')
        parser.set_defaults(mtd8=False)

    if os.path.exists(os.path.join("/","dev", "mtd9" )):
        parser.add_argument('--mtd9', dest='mtd9', action='store_true', help='Initialize only MTD9.')
        parser.set_defaults(mtd9=False)

    if os.path.exists(os.path.join("/","dev", "mtd10" )):
        parser.add_argument('--mtd10', dest='mtd10', action='store_true', help='Initialize only MTD10.')
        parser.set_defaults(mtd10=False)

    if os.path.exists(os.path.join("/","dev", "mtd11" )):
        parser.add_argument('--mtd11', dest='mtd11', action='store_true', help='Initialize only MTD11.')
        parser.set_defaults(mtd11=False)

    args = parser.parse_args()

    # prepare actions
    actions = []

    if args.allmtd or args.mtd2:
        actions.append('mtd2')
    if args.allmtd or args.mtd3:
        actions.append('mtd3')
    if args.allmtd or args.mtd4:
        actions.append('mtd4')
    if args.allmtd or args.mtd5:
        actions.append('mtd5')
    if args.allmtd or args.mtd6:
        actions.append('mtd6')
    if args.allmtd or args.mtd7:
        actions.append('mtd7')

    if os.path.exists(os.path.join("/","dev", "mtd8" )):
        if args.allmtd or args.mtd8:
            actions.append('mtd8')

    if os.path.exists(os.path.join("/","dev", "mtd9" )):
        if args.allmtd or args.mtd9:
            actions.append('mtd9')

    if os.path.exists(os.path.join("/","dev", "mtd10" )):
        if args.allmtd or args.mtd10:
            actions.append('mtd10')

    if os.path.exists(os.path.join("/","dev", "mtd11" )):
        if args.allmtd or args.mtd11:
            actions.append('mtd11')

    if not actions:
        print('Please specify MTD to initialize!')
        sys.exit(1)

    # handle actions
    main(actions)
    sys.exit(0)


