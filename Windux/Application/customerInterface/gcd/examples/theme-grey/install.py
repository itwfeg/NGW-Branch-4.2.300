# GCD Theme install script
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/gcd_theme_grey

import os

# Must add reference to python tree so import such as 'import shellCommand' will work
import sys
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import shellCommand

featureName = "gcd_theme_grey"

os.system("/bin/cp grey.png greyFS.png TLXML81.xml /usr/local/hobart/layouts")

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
