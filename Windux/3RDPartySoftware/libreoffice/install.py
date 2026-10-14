# Office Viewer install script
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/office_viewer

import os

# Must add reference to python tree so import such as 'import shellCommand' will work
import sys
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import shellCommand
import installUtils

featureName = "office_viewer"
featureVersion = "4.4.7.2"

# need to install these packages
pkgs = ['LibreOfficeDev_4.4.7.2_Linux_deb',
        'LibreOfficeDev_4.4.7.2_Linux_deb_langpack_en-US',
        'LibreOfficeDev_4.4.7.2_Linux_deb_langpack_es',
        'LibreOfficeDev_4.4.7.2_Linux_deb_langpack_fr',
        'LibreOfficeDev_4.4.7.2_Linux_deb_helppack_en-US',
        'LibreOfficeDev_4.4.7.2_Linux_deb_helppack_es',
        'LibreOfficeDev_4.4.7.2_Linux_deb_helppack_fr']

# install each package
for pkg in pkgs:
    # tar xvzf pkg.tar.gz
    cmd = []
    cmd.append("/bin/tar")
    cmd.append("xvzf")
    cmd.append(pkg+".tar.gz")
    cmd.append("-C")
    cmd.append("/tmp")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # install
    os.system("cd %s; /usr/bin/dpkg -i --force-architecture *.deb" % (os.path.join("/","tmp",pkg,"DEBS")))

    # cleanup
    os.system("/bin/rm -rf %s" % (os.path.join("/","tmp",pkg)))

# fix ownership
os.system("/bin/chown -R root:root %s" % (os.path.join("/","opt","libreofficedev4.4")))

# update feature
installUtils.addFeatureToInstalledList(featureName, featureVersion)

# done!
sys.exit(0)
