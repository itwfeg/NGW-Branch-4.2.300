try:
    import environment_manager
    environment_manager.setup()
except ImportError:
    pass

import argparse
import os
import sys
import shutil

import buildFeature

from sftpProtocol import SFTPClient

featureName = 'monolithic_core_preinstall'
featureFilename = "/".join(["..", "ScaleManagement", "smBackend", "smBackend.py"])
# No installer file required, extraction of tarball is all that is needed
featureInstallerFilename = 'monolithic_core_preinstall.py'

parser = argparse.ArgumentParser(description="build monolithic core tarball")
parser.add_argument("reltag",
                    action="store",
                    help="SVN release tag 99.99.99-MMDDYYYY_HHMMSS to use for feature files")

args = parser.parse_args()
device = 'hti'
osTypes = ['linux']

filenames = ('/'.join(('.', 'Application', 'shared', 'sessionHandler', 'python', 'installUpgrade.py')),
             '/'.join(('.', 'Application', 'certs', 'HobartRootCA.crt')),
             '/'.join(('.', 'Application', 'certs', 'verifyFeatureInstaller.sh')),
             '/'.join(('.', 'Application', 'certs', 'ssh', 'hobart_ssh.pub')),
             '/'.join(('.', 'mono-upgrade-files', 'eGTouch0.param')),
            )

for osSupported in osTypes:
    if buildFeature.buildGenericFeature(args.reltag, featureName, device, osSupported, featureInstallerFilename, *filenames) != 'success':
        sys.exit(1)

tarBallName = buildFeature.buildTarballName(args.reltag, device, osSupported, featureName)
base_name = os.path.splitext(tarBallName)[0]
md5_name = base_name + '.md5'
reltag_version = '-'.join(args.reltag.split('-')[1:])

shutil.copyfile(tarBallName, 'hte-feature_monolithic_core_preinstall-{0}.tgz'.format(reltag_version))
shutil.copyfile(md5_name, 'hte-feature_monolithic_core_preinstall-{0}.md5'.format(reltag_version))

sys.exit(0)
