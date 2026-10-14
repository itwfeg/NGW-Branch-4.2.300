# Bixolon install script
# This feature installs Bixolon printer driver support
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/bixolon

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "bixolon"
featureVersion = "1.2.3"

exitCode = 0

# extract software
cmd = []
cmd.append('/usr/bin/tar')
cmd.append('-axvf')
cmd.append('Software_BxlLabelCupsDrv_Linux_v1.2.3P01.tgz')

exitCode, stdout, stderr = shellCommand.execute(cmd)

# change working dir
os.chdir('Software_BxlLabelCupsDrv_Linux_v1.2.3P01/Software_BxlLabelCupsDrv_Linux_v1.2.3P01_RaspberryPi_x86')

# fixup installation script (scale does not have sudo command)
cmd = []
cmd.append('/usr/bin/sed')
cmd.append('-e')
cmd.append('s/sudo//g')
cmd.append('-i')
cmd.append('setup_v1.2.3P01.sh')

exitCode, stdout, stderr = shellCommand.execute(cmd)

# install driver(s)
cmd = []
cmd.append('./setup_v1.2.3P01.sh')

exitCode, stdout, stderr = shellCommand.execute(cmd)

# change working dir
os.chdir('../..')

# cleanup
cmd = []
cmd.append('/bin/rm')
cmd.append('-rf')
cmd.append('Software_BxlLabelCupsDrv_Linux_v1.2.3P01/')

exitCode, stdout, stderr = shellCommand.execute(cmd)

# install feature
exitCode |= installUtils.addFeatureToInstalledList(featureName, featureVersion)

# done!
sys.exit(exitCode)
