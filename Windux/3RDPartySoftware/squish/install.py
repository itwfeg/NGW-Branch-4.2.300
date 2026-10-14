# record_playback app install script
# This feature installs the record_playback app
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/record_playback

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "record_playback"
featureVersion = "6.0.1"

exitCode = 0

# install deb packages
packages = glob.glob('*.deb')

if (0 < len(packages)):
    cmd = []
    cmd.append('dpkg')
    cmd.append('-i')

    for package in packages:
        cmd.append(package)

    exitCode, stdout, stderr = shellCommand.execute(cmd)

# install feature
exitCode |= installUtils.addFeatureToInstalledList(featureName, featureVersion)

if os.path.exists(os.path.join("/","etc","firstBoot")):
    exitCode |= installUtils.reboot(sync=True)

# done!
sys.exit(exitCode)
