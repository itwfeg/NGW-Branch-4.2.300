# HTe-Enterprise app hook install script
# This feature installs the application Hook
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/hte_apphook

import os
import sys

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import installUtils
import shellCommand

featureName = "hte_apphook"
featureVersion = "2.0.0"

print ( "forgetting hobart/db/hte ..." )
if os.path.exists(os.path.join("/", "var", "lib", "dpkg", "info" "hobart-hte-apphook.list")):
    os.system("sed -i.bak '/db\/hte/d' /var/lib/dpkg/info/hobart-hte-apphook.list")
if os.path.exists(os.path.join("/", "var", "lib", "dpkg", "info" "hobart-hte-apphook.md5sums")):
    os.system("sed -i.bak '/db\/hte/d' /var/lib/dpkg/info/hobart-hte-apphook.md5sums")

# locate the deb packages
packages = glob.glob('*.deb')

# process packages
for package in packages:
    os.system('dpkg -i ' + package)

print ( "forgetting hobart/db/hte ..." )
if os.path.exists(os.path.join("/", "var", "lib", "dpkg", "info" "hobart-hte-apphook.list")):
    os.system("sed -i.bak '/db\/hte/d' /var/lib/dpkg/info/hobart-hte-apphook.list")
if os.path.exists(os.path.join("/", "var", "lib", "dpkg", "info" "hobart-hte-apphook.md5sums")):
    os.system("sed -i.bak '/db\/hte/d' /var/lib/dpkg/info/hobart-hte-apphook.md5sums")

# create link to hook
if not os.path.exists(os.path.join("/", "usr", "lib", "libHobartCustomerHookHTe.so.2")):
    sys.exit(-1)

os.system('ln -sf ./libHobartCustomerHookHTe.so.2 /usr/lib/libHobartCustomerHook.so')

# update feature
exitCode = 0

exitCode |= installUtils.addFeatureToInstalledList(featureName, featureVersion)

if os.path.exists(os.path.join("/","etc","firstBoot")):
    exitCode |= installUtils.reboot(sync=True)

# done
sys.exit(exitCode)
