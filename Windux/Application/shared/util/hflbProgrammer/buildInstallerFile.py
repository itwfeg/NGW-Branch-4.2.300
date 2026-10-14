import tarfile
import argparse
import sys
import md5checksum


def build_installer_tarball(container_filename, tarball_filename):

    if md5checksum.generate_md5_checksum_file(container_filename):
        with tarfile.open(tarball_filename, "w:gz") as tar:
            tar.add(container_filename)
            tar.add(md5checksum.md5PathFromFilename(container_filename))


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Build gzipped installer tarball")
    parser.add_argument("container_filename", type=str, help='Container filename to insert into installer tarball')
    parser.add_argument("tarball_filename", type=str, help='installer tarball filename to output')
    args = parser.parse_args()
    sys.exit(build_installer_tarball(args.container_filename, args.tarball_filename))
