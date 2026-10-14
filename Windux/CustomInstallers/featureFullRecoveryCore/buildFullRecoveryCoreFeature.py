import os
import tarfile
import sys
import argparse
import shutil

sys.path.append("./../../Application/shared/SessionHandler/python/")
import buildFeature


def buildFullRecoveryCoreFeature(featureFilename, relTag, recoveryPackage):
    featurePath = "/"
    recoveryTarball = os.path.basename(recoveryPackage).replace(".package", "")

    with tarfile.open(recoveryPackage, "r:gz") as recoveryPackageArchive:
        recoveryPackageArchive.extract(recoveryTarball)

    shutil.copyfile(recoveryTarball, 'recovery.tgz')

    if buildFeature.buildGenericFeature(args.reltag, featureName, device, osSupported, featureInstallerFilename, *filesToAdd) == 'success':
        try:
            os.remove("recovery.tgz")
            os.remove(recoveryTarball)
        except Exception as err:
            print "Could not clean up temporary tarballs: {0}".format(err)
            sys.exit(0)
    else:
        sys.exit(1)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="build monolithic core tarball")
    parser.add_argument("reltag",
                        action="store",
                        help="SVN release tag 99.99.99-MMDDYYYY_HHMMSS to use for feature files")


    args = parser.parse_args()
    featureName = "full_recovery_core"
    featureFilename = '%s.tgz' % (featureName)
    featureInstallerFilename = '%s_install.py' % (featureName)
    device = 'hti'
    osSupported = 'linux'
    filesToAdd = ['recovery.tgz']
    recoveryTarballPackage=os.path.join('hti-linux-recovery_core-{0}.package.tgz'.format(args.reltag))

    buildFullRecoveryCoreFeature(featureFilename, args.reltag, recoveryTarballPackage)
