# telit install script
# This feature installs Telit Device Wise binaries.
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/telit

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "telit"
featureVersion = "16.1.21-003"

exitCode = 0

# install deb packages
packages = glob.glob('*.deb')

if (1 == len(packages)):
    os.system('dpkg -i ' + packages[0])

elif (1 < len(packages)):
    cmd = []
    cmd.append('dpkg')
    cmd.append('-i')

    for package in packages:
        cmd.append(package)

    exitCode, stdout, stderr = shellCommand.execute(cmd)

# remove feature installation
exitCode = installUtils.removeInstallerFiles(featureName)

# done!
sys.exit(exitCode)
