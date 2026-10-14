#!/usr/bin/python
import argparse
import os
import tarfile
import sys
import logging
from hashlib import md5

log_filepath = 'packaging_log.txt'

if os.path.exists(log_filepath):
    os.remove(log_filepath)

logging.basicConfig(filename=log_filepath,
                    level=logging.DEBUG,
                    format='[%(levelname)s-%(asctime)s]: %(message)s',
                    datefmt="%Y-%m-%d %H:%M:%S")


def main():
    """ Main entry-point for script
    """
    logging.debug('Started script')
    parser = argparse.ArgumentParser(description='Automated sign and packaging process for HTSeries Features.')
    parser.add_argument('feature_filename', 
                        action='store', 
                        help='The name of the feature to package.')
    args = parser.parse_args()
    feature_filename = args.feature_filename
    
    #base_directory = '/usr/local/hobart/svn/branch-timesys-3.10-walmart/Windux/Application/certs/'
    #os.chdir(base_directory)
    
    logging.debug('Running signFeatureInstaller')
    
    if sys.platform == 'win32':
        os.system('signFeatureInstallerPIT.cmd {0}'.format(feature_filename))
    else:
        os.system('./signFeatureInstallerPIT.sh {0}'.format(feature_filename))

    feature_name = os.path.splitext(feature_filename)[0]

    logging.debug('Creating MD5')
    logging.debug(feature_name + '.md5')

    # Generate an md5 checksum of the tarball
    try:
        f = open(feature_filename, "rb")
        data = f.read()
        f.close()

        Hash = md5(data).hexdigest()
        out = open(feature_name + str(".md5"), "wb")
        out.write(Hash)
        out.close()
    except:
        logging.debug('Failed to create MD5')

    logging.debug('Creating package.tgz tarball')
    logging.debug(feature_name + '.package.tgz')
    
    # Create the upgrade package
    try:
        with tarfile.open(feature_name + '.package.tgz', 'w:gz') as archive:
            for root, dirs, files in os.walk('.'):                
                for filename in files:
                    if filename.endswith('.package.tgz'):
                        continue

                    if feature_name in filename:
                        logging.debug('archiving and removing {0}'.format(filename))
                        archive.add(filename)
                        os.remove(filename)
    except:
        logging.exception('Could not open or walk through tarfile')


if __name__ == '__main__':
    main()

