import buildFeature
import os
import tarfile
import sys
import argparse
import shutil


def buildMonolithicCoreFeature(featureFilename, relTag, liveTarball, recoveryPackage, ubootPackage):
    featurePath = "/"
    recoveryTarball = os.path.basename(recoveryPackage).replace(".package", "")
    ubootTarball = os.path.basename(ubootPackage).replace(".package", "")

    with tarfile.open(recoveryPackage, "r:gz") as recoveryPackageArchive:
        recoveryPackageArchive.extract(recoveryTarball)

    with tarfile.open(ubootPackage, "r:gz") as ubootPackageArchive:
        ubootPackageArchive.extract(ubootTarball)

    shutil.copyfile(liveTarball, 'live.tgz')
    shutil.copyfile(recoveryTarball, 'recovery.tgz')
    shutil.copyfile(ubootTarball, 'uboot.tgz')

    shutil.copyfile("/".join(["..", 'Os', 'embedded', 'rfsOverlays', 'recovery', 'etc', 'init.d', 'recovery.sh']), "recovery.sh")
    shutil.copyfile("/".join(["..", 'Os', 'embedded', 'rfsOverlays', 'recovery', 'etc', 'init.d', 'rc.local']), "rc.local")
    shutil.copyfile("/".join(["..", 'Release', 'rootfs', 'recovery', 'etc',  'fstab']), 'fstab')
    shutil.copyfile('/'.join(["..", 'Windux', 'Application', 'shared', 'sessionHandler', 'python', 'startupSequence.py']), 'startupSequence.py')

    if buildFeature.buildGenericFeature(args.reltag, featureName, device, osSupported, featureInstallerFilename, *filesToAdd) == 'success':
        try:
            os.remove("live.tgz")
            os.remove("recovery.tgz")
            os.remove("uboot.tgz")
            os.remove(recoveryTarball)
            os.remove(ubootTarball)
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
    parser.add_argument("live_tarball",
                        action="store",
                        help="live tarball name")
    parser.add_argument("recovery_tarball",
                        action="store",
                        help="recovery tarball name")
    parser.add_argument("uboot_tarball",
                        action="store",
                        help="uboot tarball name")

    args = parser.parse_args()
    featureName = "monolithic_core"
    featureFilename = '%s.tgz' % (featureName)
    featureInstallerFilename = '%s_install.py' % (featureName)
    device = 'hti'
    osSupported = 'linux'
    filesToAdd = ["live.tgz", 'recovery.tgz', 'uboot.tgz', 'recovery.sh', 'rc.local', '/'.join(('.', 'mono-upgrade-files', 'eGTouch0.param')), 'fstab']

    buildMonolithicCoreFeature(featureFilename, args.reltag, args.live_tarball, args.recovery_tarball, args.uboot_tarball)
