#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Office Viewer install script
# This script is currently utilized as part of the makeSD process, and the office_viewer
# feature is installed at that time, rather than at first boot from the /preinstall/ dir
#
# installToTarget would be the ideal time to do this, however ENGDEV1 currently is operating
# as a version of Ubuntu that is too old and incompatible with the necessary dpkg commands.
#
# At such time as a build server is running Ubuntu v > 15.04 this script will work as intended,
# and this functionality should be moved from makeSD.sh (local VM) to installToTarget.sh (build server)
#
# CRW
#

import os
import getopt

# Must add reference to python tree so import such as 'import shellCommand' will work
import sys
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import glob
import shellCommand
import installUtils

featureName = "office_viewer"
featureVersion = "4.4.7.2"


def main():

    commands = []
    total = len(sys.argv)
    print(( "total args: %d" % (total) ))
    if(total >= 3):
        target = sys.argv[total-2]
        liveRfs = sys.argv[total-1]

    print(( "Target: %s" % (target) ))
    print(( "LiveRfs: %s" % (liveRfs) ))

    targetPath = os.path.join(target, liveRfs)

    os.system("echo %s" % (targetPath))

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
        # tar xvzf pkg.tar.gz -C path-to-target/tmp
        cmd = []
        cmd.append("tar")
        cmd.append("xvzf")
        cmd.append(str(targetPath)+"/usr/local/hobart/features/office_viewer/"+pkg+".tar.gz")
        cmd.append("-C")
        cmd.append("/tmp")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(cmd)

        # install
		#/tmp/LibreOfficeDev_4.4.5.2_Linux_deb/DEBS; /usr/bin/dpkg -i --force-architecture *.deb
        os.system("cd %s; sudo dpkg -i --force-architecture --root=%s   *.deb" % (os.path.join("/", "tmp", pkg, "DEBS"), targetPath))
        # cleanup
        os.system("rm -rf %s" % (os.path.join("/","tmp", pkg)))

    # fix ownership
    os.system("chown -R root:root %s" % (os.path.join(targetPath,"opt","libreofficedev4.4")))

    # update feature
    installUtils.addFeatureToInstalledList(featureName, featureVersion)

    # done!
    sys.exit(0)

if __name__ == "__main__":
    main()

