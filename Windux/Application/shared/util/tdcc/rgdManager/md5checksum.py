# -*- Mode: Python; tab-width: 4 -*-

import os
import sys
import string
import hashlib
import file_io


def generate_md5_checksum_file(file_to_process):
    md5_output_filename = md5_path_from_filename(file_to_process)
    md5_checksum = md5_for_file(file_to_process)
    try:
        with open(md5_output_filename, "w") as f:
            f.write(md5_checksum)
    except:
        return False
    return True


def filename_without_extension(file_path):
    try:
        file_basename = os.path.basename(file_path)
        file_without_extension = os.path.splitext(file_basename)[0]
        return file_without_extension
    except:
        return None


def md5_for_file(path, block_size=256*128):
    md5 = hashlib.md5()
    with open(path,'rb') as f:
        for chunk in iter(lambda: f.read(block_size), b''):
            md5.update(chunk)
    return md5.hexdigest()


def md5_path_from_filename(filename):
    filename_no_extension = filename_without_extension(filename)
    if filename_no_extension is not None:
        return os.path.join(os.path.dirname(filename), filename_no_extension + '.md5')
    else:
        return None


def md5_filename_from_installer_filename_exists(filename):
    md5_path = md5_path_from_filename(filename)
    if md5_path is not None:
        if os.path.exists(md5_path):
            return True
        else:
            return False
    else:
        return False


def md5_valid_based_on_checksum_file(filename):
    md5_path = md5_path_from_filename(filename)
    if md5_path is not None:
        status, data = file_io.read_from_file(md5_path)
        if status == 'success':
            for i in range(0, len(data)):
                if data[i].strip() != '':
                    md5checksum = md5_for_file(filename)
                    if md5checksum.strip() == data[i].strip():
                        return True
        return False
    else:
        return False


if __name__ == "__main__":
    print(md5_for_file(sys.argv[1]))

