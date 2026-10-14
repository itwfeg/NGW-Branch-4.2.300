import os
import sys
import tarfile
import shellCommand
import glob
import py_compile
import string
import file_io
import socket
import shutil
import datetime
import xml.etree.ElementTree as ET
import sys
import md5checksum

PY3 = sys.version_info[0] == 3

int2byte = (lambda x: bytes((x,))) if PY3 else chr

_text_characters = (
        b''.join(int2byte(i) for i in range(32, 127)) +
        b'\n\r\t\f\b')

# text_characters = "".join(map(chr, range(32, 127)) + list("\n\r\t\b"))
# _null_trans = string.maketrans("", "")


# def __istext(s):
#     '''
#     This routine should not be called directly
#     '''
#     if "\0" in s:
#         return 0
#
#     if not s:  # Empty files are considered text
#         return 1
#
#     # Get the non-text characters (maps a character to itself then
#     # use the 'remove' option to get rid of the text characters.)
#     t = s.translate(_null_trans, text_characters)
#
#     # If more than 30% non-text characters, then
#     # this is considered a binary file
#     if float(len(t)/len(s)) > 0.30:
#         return 0
#     return 1


def is_text_file(filename, blocksize=512):
    """ Uses heuristics to guess whether the given file is text or binary,
        by reading a single block of bytes from the file.
        If more than 30% of the chars in the block are non-text, or there
        are NUL ('\x00') bytes in the block, assume this is a binary file.
    """
    block = open(filename, 'rb').read(blocksize)
    if b'\x00' in block:
        # Files with null bytes are binary
        return False
    elif not block:
        # An empty file is considered a valid text file
        return True

    # Use translate's 'deletechars' argument to efficiently remove all
    # occurrences of _text_characters from the block
    not_text = block.translate(None, _text_characters)
    return float(len(not_text)) / len(block) <= 0.30

# def istextfile(filename, blocksize = 512):
#     '''
#     Determine if filename is a text based file
#     '''
#     return __istext(open(filename).read(blocksize))


def is_binary(filename):
    """
    Determine if filename is a binary file
    """
    return not is_text_file(filename)


def mk_dest_path(path):
    if not os.path.exists(path):
        os.makedirs(path)


def remove_installer_fragments(installer_tarball_name):

    archive = tarfile.open(installer_tarball_name)

    for tarinfo in archive.getmembers():

        if os.path.exists(os.path.join(get_feature_root(), tarinfo.name)):
            if os.path.isdir(os.path.join(get_feature_root(), tarinfo.name)):
                shutil.rmtree(os.path.join(get_feature_root(), tarinfo.name))
            else:
                os.remove(os.path.join(get_feature_root(), tarinfo.name))

    os.system("/bin/sync")


def remove_feature(feature_name):

    if os.path.exists(os.path.join(get_feature_root(), feature_name)):
        # shutil.rmtree(os.path.join(get_feature_root(), feature_name))
        os.system("/usr/bin/sudo /bin/rm -rf %s" % os.path.join(get_feature_root(), feature_name))


def cleanup_feature(filepath, destination="/"):
    """ Before installing a new feature upgrade, iterate through the feature archive and clear out any
        potential remnants left over by
        previous versions of the same feature to ensure that they do not break the newest install.
    """
    try:
        archive = tarfile.open(filepath)

        for tarinfo in archive.getmembers():
            # directories = os.path.dirname(tarinfo.name).split(os.sep)
            directory_name = os.path.join(destination, tarinfo.name)

            if directory_name.startswith(get_feature_root()):
                if os.path.exists(directory_name):
                    shutil.rmtree(directory_name)

        os.system("/bin/sync")
    except Exception as err:
        print(err)


def copy_file(source_file_path, destination_file_path, forced=False):
    output_dir = os.path.dirname(destination_file_path)

    try:
        shutil.copy(source_file_path, destination_file_path)
    except Exception as error:
        if 'No such file' in str(error):
            if not os.path.isdir(output_dir) and forced:
                mk_dest_path(output_dir)
                shutil.copy(source_file_path, destination_file_path)
                return

        raise error


def list_subdirectories(directory_path):
    filenames = os.listdir(directory_path)

    for filename in filenames:
        full_path = os.path.join(directory_path, filename)
        if os.path.isdir(full_path):
            yield full_path


def get_hobart_root():
    root_dir = os.path.join("/", "home", "pi")
    return root_dir


def get_feature_root():
    root_dir = get_hobart_root()
    return root_dir


def get_python_root():
    """
    Gets the root location of the python directory on hobart devices
    """
    python_root = os.path.join(get_hobart_root(), "python")
    return python_root


def send_message(client, port, message):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_address = (client, port)
        sock.connect(client_address)
        sock.sendall(message + '\n')
        sock.close()
        exit_code = 0
    except:
        exit_code = 1
    return exit_code


def set_python_root():
    """
    Set the PATH environment variable to include python root
    """
    sys.path.append(get_python_root())
    return 0


def chmod(filename, permissions='u=rwx,go=rx'):
    """
    Change permissions on a file for user, group, and or other
    You can use the intuitive form in default or octal value
    """
    cmd = ["/bin/chmod", permissions, filename]

    exit_code, stdout, stderr = shellCommand.execute(cmd)
    return exit_code


def extract_tarball_as_is(file_name, root='/'):
    exit_code = 0
    try:
        tar = tarfile.open(file_name, "r:gz")
        tar.extractall(path=root)
        tar.close()
    except:
        exit_code = 1
    return exit_code


def extract_tarball(feature_filename, root='/'):
    """
    Extract the feature tarball including fixing ownership and other particulars
    """
    exit_code = 0

    license_file = os.path.join(get_python_root(), "license.py")
    license_file = license_file[1:]  # get rid of first slash which will not be in tarball name

    # extract the feature tarball all files will be extracted to the correct locations on the filesystem
    tar = tarfile.open(feature_filename, "r:gz")

    # loop thru all tar members
    for tarinfo in tar:

        try:
            tar.extract(tarinfo, root)
        except IOError:
            os.remove(os.path.join(root, tarinfo.name))
            tar.extract(tarinfo, root)

        if os.path.dirname(tarinfo.name) == os.path.join("etc", "init.d"):
            chmod(filename=os.path.join(root, tarinfo.name), permissions='u=rwx,go=rx')

        if not tarinfo.isdir():
            if is_binary(os.path.join(root, tarinfo.name)):
                chmod(filename=os.path.join(root, tarinfo.name), permissions='u=rwx,go=rx')

        # compile the license file in case it was found in the tarball
        if tarinfo.name == license_file:
            py_compile.compile(os.path.join(get_python_root(), "license.py"), os.path.join(get_python_root(),
                                                                                           "license.pyc"))
            os.remove(os.path.join(get_python_root(), "license.py"))

    # close the tarball
    tar.close()
    return exit_code


def remove_installer_files(feature_name, root='/'):
    """
    WARNING:
    This routine is here in case you want to remove installer files but it is not
    recommended since these files must be present to reinstall the feature during first
    boot after upgrade is performed.

    You had better make sure you know what you are doing if you call this!
    Remove all files under feature directory that were used to install the feature
    """
    feature_path = os.path.join(root, get_feature_root(), feature_name)

    files_to_remove = glob.glob(os.path.join(feature_path, "*"))

    for curFile in files_to_remove:
        # remove the feature tarball and install files
        try:
            os.remove(curFile)
        except:
            pass

    if os.name == 'posix':
        os.system('rm -rf {0}'.format(feature_path))
    else:
        os.system('rmdir /s /q {0}'.format(feature_path))

    return 0


def xml_pretty_print(root_element):
    __indent(root_element)
    ET.dump(root_element)


def __indent(elem, level=0, more_sibs=False):
    i = "\n"
    if level:
        i += (level-1) * '  '
#
    num_kids = len(elem)

    if num_kids:
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
            if level:
                elem.text += '  '
#
        count = 0

        for kid in elem:
            __indent(kid, level+1, count < num_kids - 1)
            count += 1
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
            if more_sibs:
                elem.tail += '  '
#
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
            if more_sibs:
                elem.tail += '  '


def xml_string_to_element_tree(xml_license_buffer):
    return ET.fromstring(xml_license_buffer)


def get_installed_features():
    features = {}

    feature_installation_path = get_feature_root()

    for subdirectory in list_subdirectories(feature_installation_path):
        feature_name = os.path.basename(subdirectory)
        features[feature_name] = {'name': feature_name,
                                  'version': 'UNKNOWN',
                                  'activation_date': 'UNKNOWN',
                                  }
    feature_filename = os.path.join(get_feature_root(), 'FEATURES_INSTALLED')

    if os.path.exists(feature_filename):
        with open(feature_filename) as featureFile:
            xml_feature_buffer = featureFile.read()
    else:
        return features

    element_tree = ET.fromstring(xml_feature_buffer)

    for feature_element in element_tree.iter('feature'):
        feature_data = _feature_version_data(feature_element)
        features[feature_data['name']] = feature_data

    return features


def _feature_version_data(feature_element):
    feature_data = {'name': feature_element.get('name'),
                    'version': feature_element.get('version'),
                    'activation_date': feature_element.get('activation_date'),
                    }

    return feature_data


def feature_version_data(xml_feature_buffer, feature):
    if not xml_feature_buffer:
        raise ValueError('Invalid XML buffer data provided.')

    element_tree = ET.fromstring(xml_feature_buffer)

    for feature_element in element_tree.iter('feature'):
        feature_name = feature_element.get('name')

        if feature_name == feature:
            return _feature_version_data(feature_element)

    raise ValueError('Feature {0} is not installed.'.format(feature))


def is_feature_installed(xml_feature_buffer, feature):
    found_feature = False

    if xml_feature_buffer is not None:
        hobart_el = ET.fromstring(xml_feature_buffer)
        for featureEl in hobart_el.iter('feature'):
            # print featureEl.get('name')
            if str(featureEl.get('name')) == feature:
                found_feature = True

    return found_feature


def __xml_remove_feature(feature, xml_license_buffer):
    hobart_el = xml_string_to_element_tree(xml_license_buffer)
    for featureEl in hobart_el.iter('feature'):
        if str(featureEl.get('name')) == feature:
            hobart_el.remove(featureEl)

    return ET.tostring(hobart_el)


def add_feature_to_installed_list(feature_name, version):
    feature = {'name': feature_name, 'version': version}
    date_time_now = datetime.datetime.now()
    feature['activation_date'] = date_time_now.strftime("%m-%d-%Y %H:%M:%S")

    feature_filename = os.path.join(get_feature_root(), 'FEATURES_INSTALLED')

    status, buffer = file_io.read_file_into_buff(feature_filename)

    if status == 'success':
        if is_feature_installed(buffer, feature['name']):
            buffer = __xml_remove_feature(feature['name'], buffer)
#
        buffer = __xml_add_feature_to_buffer(buffer, feature)
        status, detail = file_io.write_buffer_to_file(feature_filename, buffer, mode='w')
    else:
        buffer = _xml_create_initial_feature_buffer()
        buffer = __xml_add_feature_to_buffer(buffer, feature)
        status, detail = file_io.write_buffer_to_file(feature_filename, buffer, mode='w')
#
    if status == 'success':
        return 0
    else:
        return 1


def _xml_create_initial_feature_buffer():
    hobart_el = ET.Element("hobart")
    return ET.tostring(hobart_el)


def __xml_add_feature_to_buffer(xml_feature_buffer, feature):
    hobart_el = ET.fromstring(xml_feature_buffer)
    feature_el = ET.SubElement(hobart_el, 'feature')
    feature_el.set("name", feature['name'])
    feature_el.set("version", feature['version'])
    feature_el.set("activation_date", feature['activation_date'])
    return ET.tostring(hobart_el)


def add_debian_application_startup(command_line, user):
    xsession_file = os.path.join(os.sep, "home", user, ".xsessionrc")
    status, lines = file_io.read_from_file(xsession_file)
    if status == 'success':
        if command_line in lines:
            return 'success'
        else:
            lines = lines + [command_line]
    else:
        lines = ['#!/bin/sh -f', command_line]
    status, details = file_io.write_to_file(xsession_file, lines)
    if status == 'success':
        return 0
    else:
        return 1


def add_initd_script(feature_name, sequence='99'):
    """
    Install an init.d script in the appropriate init.d location and use typical start/kill
    links
    sequence is where in the start/stop the script should run
    """

    # create S and K links to smBackend.sh startup script
    cmd = ["/usr/sbin/update-rc.d",
           feature_name,
           "start",
           sequence,
           "3",
           "4",
           "5",
           ".",
           "stop",
           sequence,
           "0",
           "1",
           "6",
           "."]

    exit_code, stdout, stderr = shellCommand.execute(cmd)
    return exit_code


def execute_install_script(feature_name, python_path_additions):
    # run installer script if one exists in the feature upgrade

    if os.path.exists(os.path.join(get_feature_root(), feature_name, "install.py")):
        exit_code = os.system('cd %s; PYTHONPATH=$PYTHONPATH:%s /usr/bin/python3 install.py' %
                              (os.path.join(get_feature_root(), feature_name), python_path_additions))
    else:
        exit_code = 0

    return exit_code


def sync_file_system():
    """
    Synchronize filesystem RAM with the uSD card
    """
    os.system("/bin/sync")
    return 0


def uboot_set_boot_parameters(partition=1, action='upgrade_scale'):

    exit_code = 0

    cmd = ["fw_setenv", "recovery_action", action]
    code, stdout, stderr = shellCommand.execute(cmd)

    exit_code |= code

    cmd = ["fw_setenv", "active_partition", str(partition)]
    code, stdout, stderr = shellCommand.execute(cmd)

    # check for new SD format
    cmd = ['readlink', '-f', '/dev/root']
    exit_code, stdout, stderr = shellCommand.execute(cmd)

    recovery = None

    if 'mmcblk0p3' in stdout:
        recovery = os.path.join(os.sep, 'media', 'mmcblk0p1', 'recovery')
    elif 'mmcblk1p3' in stdout:
        recovery = os.path.join(os.sep, 'media', 'mmcblk1p1', 'recovery')
    elif 'mmcblk2p3' in stdout:
        recovery = os.path.join(os.sep, 'media', 'mmcblk2p1', 'recovery')

    if recovery is not None:
        with open(recovery, 'w') as recovery_file:
            recovery_file.write(action)

    exit_code |= code
    return code


def reboot(sync=True):
    """
    Reboot the device with optional filesystem RAM sync to uSD card
    """
    if sync:
        sync_file_system()
    os.system("/sbin/reboot")
    return 0


def start_application(feature_filename):
    """
    Launch application in the background.
    """
    cmd = []
    if os.path.splitext(feature_filename)[1] == '.py':
        cmd.append('/usr/bin/python3')
        cmd.append(os.path.join(get_python_root(), feature_filename))
    else:
        cmd.append(feature_filename)
#
    cmd.append("&")
    # NOTE:
    # shellCommand.execute() does not background things correctly
    exit_code = os.system(" ".join(cmd))

    return exit_code


def stop_application(feature_filename):
    """
    Stop an application from running executableFullPathFilename must be the
    same path used when starting the application. Also if the filename is path
    to a python script the python interpreter is used to find the process to kill
    a message to stop the application ideally one would send a message that exits
    the program versus a kill
    """
    cmd = ["/usr/bin/pkill", "-f"]

    if os.path.splitext(feature_filename)[1] == '.py':
        cmd.append('/usr/bin/python3' + ' ' + os.path.join(get_python_root(), feature_filename))
    else:
        cmd.append(feature_filename)
#
    try:
        exit_code, stdout, stderr = shellCommand.execute(cmd)
    except:
        exit_code, stdout, stderr = (0, '', '')
#
    if os.path.splitext(feature_filename)[1] == '.py' and exit_code == 1:
        cmd = ['/usr/bin/pkill', '-f']

        if os.path.splitext(feature_filename)[1] == '.py':
            cmd.append('python3' + ' ' + os.path.basename(feature_filename))
#
        try:
            exit_code, stdout, stderr = shellCommand.execute(cmd)
        except:
            exit_code, stdout, stderr = (0, '', '')
#
    return exit_code


def file_attributes(tarinfo):
    tarinfo.uid = tarinfo.gid = 1000
    tarinfo.uname = tarinfo.gname = "pi"
    return tarinfo


def extract_installer_tarball(feature_name,
                              installer_tarball_filename,
                              destination,
                              validate_checksum=True):
    status_code = 1

    if extract_tarball_as_is(installer_tarball_filename, get_feature_root()) == 0:
        status_code = 0
        if validate_checksum:
            status_code = 1
            if '_new_release' in feature_name:
                feature_name = feature_name[:-(len('_new_release'))]
            if md5checksum.md5_valid_based_on_checksum_file(os.path.join(get_feature_root(),
                                                                         '%s_runtime.tgz' % feature_name)):
                if extract_tarball_as_is(os.path.join(get_feature_root(),
                                                      '%s_runtime.tgz' % feature_name), destination) == 0:
                    status_code = 0
    return status_code


def build_runtime_tarball(feature_name, file_patterns_to_add):
    # where_i_am = os.path.dirname(os.path.realpath(__file__))
    where_i_am = os.getcwd()
    tarball_filename = '%s_runtime.tgz' % feature_name

    if os.path.exists(tarball_filename):
        os.remove(tarball_filename)

    with tarfile.open(tarball_filename, "w:gz") as tar:
        print('Inserting files or directories into %s tarfile' % tarball_filename)
        for file_pattern in file_patterns_to_add:
            for include_file in glob.glob(os.path.join(where_i_am, file_pattern)):
                print("adding the file %s" % include_file)
                tar.add(name=include_file,
                        filter=file_attributes,
                        recursive=True,
                        arcname=os.path.basename(include_file))

        # print('Inserting the DFU library')
        # tar.add(os.path.join(where_i_am, 'pc-nrfutil-0.5.3'), recursive=True, arcname='pc-nrfutil-0.5.3')

        # print('Creating empty updates folder')
        # # add empty updates folder
        # updates_dir = tarfile.TarInfo('updates')
        # updates_dir.type = tarfile.DIRTYPE
        # tar.addfile(updates_dir)

    print('Tarfile Created ' + os.path.abspath(tarball_filename))


def build_installer_tarball(
        product,
        feature_name,
        version,
        company='itw'):

    container_filename = '%s_runtime.tgz' % feature_name
    installer_tarball_filename = '%s-%s-%s-%s.tgz' % (company, product, feature_name, version)

    if md5checksum.generate_md5_checksum_file(container_filename):
        with tarfile.open(installer_tarball_filename, "w:gz") as tar:
            print('Inserting files or directories into %s tarfile' % installer_tarball_filename)
            print("adding the file %s" % container_filename)
            tar.add(name=container_filename, filter=file_attributes)
            md5_filename = md5checksum.md5_path_from_filename(container_filename)
            print("adding the file %s" % md5_filename)
            tar.add(name=md5_filename,
                    filter=file_attributes)


class RetainFile:
    def __init__(self, filepath):
        self.filepath = filepath
        self.backupext = '.bak'

    def backup(self):
        if os.path.exists(self.filepath + self.backupext):
            os.remove(self.filepath + self.backupext)
        if os.path.exists(self.filepath):
            copy_file(self.filepath, self.filepath + self.backupext, forced=False)
        return 0

    def restore(self):
        if os.path.exists(self.filepath + self.backupext):
            copy_file(self.filepath + self.backupext, self.filepath, forced=False)
        if os.path.exists(self.filepath + self.backupext):
            os.remove(self.filepath + self.backupext)
        return 0


if __name__ == '__main__':
    add_feature_to_installed_list("kiosk_now_serving", "1.99.0")
    add_feature_to_installed_list("kiosk_now_serving", "2.10.0")
