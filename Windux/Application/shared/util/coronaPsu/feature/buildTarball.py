import argparse
import os
import sys
import tarfile
import time
import glob

sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))
import buildFeature

FEATURE_NAME = 'pillar_sync'
FEATURE_INSTALLER_NAME = 'install.py'
DEVICE = 'blackbox'
SUPPORTED_OS = 'linux'
RELEASE_NUMBER = '1.2.0'
FEATURE_FILENAME = 'blackbox-%s-feature-%s-%s.tgz' % (SUPPORTED_OS, FEATURE_NAME, RELEASE_NUMBER)
WILDCARD_EXCLUSIONS = ['.idea', '.DS_Store', 'httpserver.log', '.pyc']


def get_time_stamp():
    return time.strftime("-%m%d20%y_%I%M")


def filter_files(tarinfo):
    for exclude_item in WILDCARD_EXCLUSIONS:
        if exclude_item in tarinfo.name:
            return None

    tarinfo.uid = 0
    tarinfo.gid = 0
    tarinfo.uname = 'root'
    tarinfo.gname = 'root'
    return tarinfo


def build_pillar_sync_tarball(tarball_filename, tarball_type='runtime'):

    exit_code = 0

    where_am_i = os.path.dirname(os.path.realpath(__file__))

    if tarball_type == 'feature':
        remove_files = glob.glob('%s-%s-feature-%s-%s-*' % (DEVICE, SUPPORTED_OS, FEATURE_NAME, RELEASE_NUMBER))
        for filename in remove_files:
            os.remove(filename)

    elif tarball_type == 'runtime':
        if os.path.exists(tarball_filename):
            os.remove(tarball_filename)

    with tarfile.open(tarball_filename, "w:gz") as tar:

        print 'Inserting the web server runtime'
        tar.add(os.path.join(where_am_i, '..', '..', 'webServer', 'web2py'),
                filter=filter_files,
                recursive=True, arcname='web2py')

        print 'Inserting the web server routes'
        tar.add(os.path.join(where_am_i, '..', 'PillarSync', 'pillar_sync_web_routes.py'),
                filter=filter_files,
                arcname='web2py/routes.py')

        print 'Inserting the PillarSync runtime'
        tar.add(os.path.join(where_am_i, '..', 'PillarSync'),
                filter=filter_files, recursive=True,
                arcname='web2py/applications/PillarSync')

        print 'Creating empty databases folder'
        # add empty databases folder
        databases_folder = tarfile.TarInfo('web2py/applications/PillarSync/databases')
        databases_folder.type = tarfile.DIRTYPE
        databases_folder.uid = 0
        databases_folder.gid = 0
        databases_folder.uname = 'root'
        databases_folder.gname = 'root'
        tar.addfile(databases_folder)

        if tarball_type == 'feature':

            print 'Adding feature files'
            # add the python installer file
            tar.add(os.path.join(where_am_i, FEATURE_INSTALLER_NAME),
                    filter=filter_files,
                    arcname='install.py')

            python_packages = ['pyserial-2.7.tar.gz',
                               'paho-mqtt-1.2.tar.gz',
                               'web2py-2.1.1.tar.gz',
                               'pygal-2.4.0-py2.py3-none-any.whl',
                               'pydal-20190915.2.tar.gz',
                               'functools32-3.2.3-2.tar.gz',
                               'protobuf-3.14.0-py2.py3-none-any.whl']

            for package in python_packages:
                # add the sparkplug google protobufs package
                tar.add(os.path.join(where_am_i, '..', '..', '..', 'sessionHandler',
                                     'python', package),
                        filter=filter_files,
                        arcname=package)

            sm_backend_path = os.path.join(where_am_i, '..', '..', '..', '..', '..', '..', '..', 'HTe', 'trunk',
                                           'smBackend')

            # add the autoconfig file update
            if os.path.exists(sm_backend_path):
                tar.add(os.path.join(sm_backend_path, 'autoconfig.py'),
                        filter=filter_files,
                        arcname='autconfig.py')

            else:
                print "ERROR: Can not build feature file without HTe SM Backend source" \
                      "checked out at the location: \n%s" % sm_backend_path
                exit_code = 1

    return exit_code


if __name__ == '__main__':

    exit_code = 1
    parser = argparse.ArgumentParser(prog='buildTarball',
                                     description="Build gzipped feature or runtime tarball file")
    options_group = parser.add_mutually_exclusive_group(required=True)
    options_group.add_argument('-f', '--feature', help="build a feature file", action='store_true')
    options_group.add_argument('-r', '--runtime', help='runtime tarball name to create')
    args = parser.parse_args()

    if args.feature:

        release_tag = RELEASE_NUMBER + get_time_stamp()
        tarball_code = build_pillar_sync_tarball(FEATURE_FILENAME, tarball_type='feature')
        if tarball_code == 0:
            if buildFeature.buildFeature(release_tag,
                                         FEATURE_NAME,
                                         FEATURE_FILENAME,
                                         None,
                                         DEVICE,
                                         SUPPORTED_OS) == 'success':
                try:
                    os.remove(FEATURE_FILENAME)
                except:
                    pass
                exit_code = 0

    elif args.runtime:

        exit_code = build_pillar_sync_tarball(args.runtime)

    else:
        print "Invalid Option specified"

    sys.exit(exit_code)
