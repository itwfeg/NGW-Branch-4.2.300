# Walmart OS install script
# This feature installs Walmart specific OS functionality
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/walmart

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "walmart"
featureVersion = "1.1.0"

# copy proxy settings
cmd = []
cmd.append("/bin/cp")
cmd.append("proxySettings")
cmd.append("/usr/local/hobart")
exitCode, stdout, stderr = shellCommand.execute(cmd)

# update ssh keys [hobart]
cmd = []
cmd.append("/bin/mkdir")
cmd.append("-p")
cmd.append("/home/hobart/.ssh")
exitCode, stdout, stderr = shellCommand.execute(cmd)

cmd = []
cmd.append("/bin/touch")
cmd.append("/home/hobart/.ssh/authorized_keys")
exitCode, stdout, stderr = shellCommand.execute(cmd)

os.system("/bin/cat walmart_ssh.pub >> /home/hobart/.ssh/authorized_keys")

# update root certificates
os.system("rename .cer .crt *.cer")
os.system("/bin/cp *.crt /usr/local/share/ca-certificates")
os.system("/usr/sbin/update-ca-certificates")

# update feature
installUtils.addFeatureToInstalledList(featureName, featureVersion)

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
