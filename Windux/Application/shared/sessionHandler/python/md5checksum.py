# -*- Mode: Python; tab-width: 4 -*-

import os
import sys
import string
import hashlib
import osUtils
import file_io


def generate_md5_checksum_file(file_to_process):
    md5_output_filename = md5PathFromFilename(file_to_process)
    md5_checksum = md5_for_file(file_to_process)
    try:
        with open(md5_output_filename, "w") as f:
            f.write(md5_checksum)
    except:
        return False
    return True


def md5_for_file(path, block_size=256*128):
    '''
    Block size directly depends on the block size of your filesystem
    to avoid performances issues
    Here I have blocks of 4096 octets (Default NTFS)
    '''
    md5 = hashlib.md5()
    with open(path,'rb') as f:
        for chunk in iter(lambda: f.read(block_size), b''):
            md5.update(chunk)
    return md5.hexdigest()


def md5PathFromFilename(filename):
    filenameNoExtension = osUtils.filenameWithoutExtension(filename)
    if filenameNoExtension != None:
        return os.path.join(os.path.dirname(filename), filenameNoExtension  + '.md5')
    else:
        return None


def md5FilenameFromInstallerFilenameExists(filename):
    md5Path = md5PathFromFilename(filename)
    if md5Path != None:
        if os.path.exists(md5Path):
            return True
        else:
            return False
    else:
        return False


def md5ValidBasedOnChecksumFile(filename):
    md5Path = md5PathFromFilename(filename)
    if md5Path != None:
        status, data = file_io.readFromFile(md5Path)
        if status == 'success':
            for i in xrange(0, len(data)):
                if string.strip(data[i]) != '':
                    md5checksum = md5_for_file(filename)
                    if string.strip(md5checksum) == string.strip(data[i]):
                        return True
        return False
    else:
        return False


if __name__ == "__main__":
    # python 3
    print(md5_for_file(sys.argv[1]))
    
    # python 2 - I needed this for my VM to generate certs
    # print md5_for_file(sys.argv[1])

