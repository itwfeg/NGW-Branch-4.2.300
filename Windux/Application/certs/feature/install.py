# Hobart Root CA install script
# This feature installs the Hobart Root CA and Public SSH key
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/hobart_root_ca

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "hobart_root_ca"
featureVersion = "1.0.0"

# update ssh keys [root]
cmd = []
cmd.append("/bin/mkdir")
cmd.append("-p")
cmd.append("/root/.ssh")
exitCode, stdout, stderr = shellCommand.execute(cmd)

cmd = []
cmd.append("/bin/touch")
cmd.append("/root/.ssh/authorized_keys")
exitCode, stdout, stderr = shellCommand.execute(cmd)

os.system("/bin/cat hobart_ssh.pub >> /root/.ssh/authorized_keys")

# update add root ca
os.system("/bin/cp *.crt /usr/local/share/ca-certificates")
os.system("/usr/sbin/update-ca-certificates")

# update feature
installUtils.addFeatureToInstalledList(featureName, featureVersion)

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
