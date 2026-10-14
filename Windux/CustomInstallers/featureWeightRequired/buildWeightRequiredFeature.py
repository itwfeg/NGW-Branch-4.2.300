import sys

# NOTE:
# Must have python version 2.7 or greater to run this script

import buildFeature
import argparse

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="build WeightRequired tarball")
    parser.add_argument("reltag",
                        action="store",
                        help="SVN release tag 99.99.99-MMDDYYYY_HHMMSS to use for feature files")

    args = parser.parse_args()

    featureName = "weight-required-update"
    featureFilename = '%s.tgz' % (featureName)
    featureInstallerFilename = 'installWeightRequiredFeature.py'
    device = 'hti'
    osSupported = 'linux'

    if buildFeature.buildGenericFeature(args.reltag, featureName, device, osSupported, featureInstallerFilename) == 'success':
        sys.exit(0)
    else:
        sys.exit(1)

