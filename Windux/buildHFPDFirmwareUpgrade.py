import argparse
import subprocess
import sys
import os

sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), "nrfutil-0.5.3"))
from nordicsemi.dfu.package import Package


def build(version="" , source="",destination="" ):
    versionInt = (int) (version.replace('.', '0'))
    package = Package(Package.DEFAULT_DEV_TYPE,
                      Package.DEFAULT_DEV_REV,
                      versionInt,
                      Package.DEFAULT_SD_REQ,
                      source,
                      None,
                      None,
                      Package.DEFAULT_DFU_VER,
                      None)

    package.generate_package(destination)


if __name__ == "__main__":
    print("Start")
    parser = argparse.ArgumentParser()
    parser.add_argument('--version', dest='version', action='store', help="The version of the firmware file",
                        default="", required=True, type=str)
    parser.add_argument('--source', dest='source', action='store', help="The path to the HFPD source binary file",
                        required=True, type=str)
    parser.add_argument('--destination', dest='destination', action='store', help="The path to the HFPD source binary file",
                        required=True, type=str)
    args = parser.parse_args()
    build(args.version, args.source, args.destination)