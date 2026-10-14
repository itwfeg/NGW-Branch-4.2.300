# Text To Speech install script
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/text_to_speech

import os

# Must add reference to python tree so import such as 'import shellCommand' will work
import sys
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import shellCommand

featureName = "text_to_speech"

# need to execute following commands
# tar xvzf espeak.tar.gz
cmd = []
cmd.append("/bin/tar")
cmd.append("xvzf")
cmd.append("espeakhf.tar.gz")
cmd.append("-C")
cmd.append("/")
exitCode, stdout, stderr = shellCommand.execute(cmd)

os.system("/bin/chown -R root:root %s" % (os.path.join("/","usr","share","espeak-data")))
os.system("/bin/chown -R root:root %s" % (os.path.join("/","usr","lib","libespeak*")))
os.system("/bin/chown -R root:root %s" % (os.path.join("/","usr","bin","speak")))

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
