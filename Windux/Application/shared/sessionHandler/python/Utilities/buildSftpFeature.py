#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

# NOTE:
# Must have python version 2.7 or greater to run this script
# engdev1 has version 2.6 so this will be issue until updated

import buildFeature
import argparse

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="build sftpUpdate tarball")
    parser.add_argument("reltag",
                        action="store",
                        help="SVN release tag 99.99.99-MMDDYYYY_HHMMSS to use for feature files")

    args = parser.parse_args()

    featureName = "sftp_update"
    featureFilename = '%s.tgz' % (featureName)
    featureInstallerFilename = 'installSftpFeature.py'
    device = 'hti'
    osSupported = 'linux'

    if buildFeature.buildGenericFeature(args.reltag, featureName, device, osSupported, featureInstallerFilename) == 'success':
        sys.exit(0)
    else:
        sys.exit(1)

