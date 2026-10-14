# HTe apphook coinfig app install script
# This feature installs the config app
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/hteapphookconfig

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "hteapphookconfig"
featureVersion = "1.0.0"

# locate the deb packages
packages = glob.glob('*.deb')

# process packages
for package in packages:
    os.system('dpkg -i ' + package)

# update feature
installUtils.addFeatureToInstalledList(featureName, featureVersion)

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
