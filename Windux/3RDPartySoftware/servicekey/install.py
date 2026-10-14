# service key install script
# This feature installs a service key
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/servicekey

import os
import sys

featureName = "servicekey"

# create key
os.system("/bin/mkdir /etc/hobart/service")
os.system("/bin/head -c 42 /dev/urandom > /etc/hobart/service/key.dat")

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
