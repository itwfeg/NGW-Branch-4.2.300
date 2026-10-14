# walmart app hook install script
# This feature installs the Walmart Application Hook
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/walmart_rfid_apphook

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "walmart_rfid_apphook"
featureVersion = "1.0.0"

# locate the deb packages
packages = glob.glob('*.deb')

# process packages
for package in packages:
    os.system('dpkg -i ' + package)

# create link to hook
if not os.path.exists(os.path.join("/", "usr", "lib", "libHobartCustomerHookWalmartRFID.so.1")):
    sys.exit(-1)

os.system('ln -sf ./libHobartCustomerHookWalmartRFID.so.1 /usr/lib/libHobartCustomerHook.so')

# update feature
exitCode = 0

exitCode |= installUtils.addFeatureToInstalledList(featureName, featureVersion)

if os.path.exists(os.path.join("/","etc","firstBoot")):
    exitCode |= installUtils.reboot(sync=True)

# done
sys.exit(exitCode)
