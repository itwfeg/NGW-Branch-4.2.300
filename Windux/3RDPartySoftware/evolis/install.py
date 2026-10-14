# EVOLIS install script
# This feature installs Evolis printer driver support
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/evolis

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "evolis"
featureVersion = "6.6.1"

exitCode = 0

# install deb packages
packages = glob.glob('*.deb')

if (0 < len(packages)):
    cmd = []
    cmd.append('dpkg')
    cmd.append('-i')
    cmd.append('--force-architecture')

    for package in packages:
        cmd.append(package)

    exitCode, stdout, stderr = shellCommand.execute(cmd)

# install feature
exitCode |= installUtils.addFeatureToInstalledList(featureName, featureVersion)

# done!
sys.exit(exitCode)
