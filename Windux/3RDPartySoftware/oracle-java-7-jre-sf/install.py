# Oracle Java install script
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/java_jre

import os

# Must add reference to python tree so import such as 'import shellCommand' will work
import sys
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import shellCommand

featureName = "java_jre"

# extract jre and create link to this version
os.system("/usr/bin/tar zxvf ejre-7u4-fcs-b20-linux-arm-vfp-client_headful-12_apr_2012.gz -C /opt")

os.system("/bin/rm /opt/java")
os.system("/bin/ln -s /opt/ejre1.7.0_04/ /opt/java")

# install plugin
os.system("/bin/mkdir -p /root/.mozilla/plugins")

os.system("/bin/rm /root/.mozilla/plugins/libnpjp2.so")
os.system("/bin/ln -s /opt/java/lib/arm/libnpjp2.so /root/.mozilla/plugins/libnpjp2.so")

# TODO:
# Need to check all exitCodes and reflect status below
sys.exit(0)
