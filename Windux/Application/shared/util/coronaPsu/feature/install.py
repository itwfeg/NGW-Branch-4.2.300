import os
import shutil
import signal
import sys
import time

sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))
import installUtils
import messageBox

BACKEND_PATH = os.path.join('/', 'usr', 'local', 'hobart', 'backend')
FEATURE_NAME = 'pillar_sync'
FEATURE_INSTALLER_NAME = 'install.py'
FEATURE_STAGING_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'black_box'
SUPPORTED_OS = 'linux'
RELEASE_NUMBER = '1.2.0'
FEATURE_FILENAME = 'blackbox-%s-feature-%s-%s.tgz' % (SUPPORTED_OS, FEATURE_NAME, RELEASE_NUMBER)
FEATURE_DESTINATION_PATH = os.path.join("/", "usr", "local", "hobart", "coronaPsu")


def get_pid_file(application_name):
    pid = None
    application_pid_path = None
    try:
        if application_name == 'modbus_consumer':
            application_pid_path = os.path.join(FEATURE_DESTINATION_PATH, "web2py", "applications",
                                                "PillarSync", 'modules', 'modbus_consumer.pid')
        elif application_name == 'web_server':
            application_pid_path = os.path.join(FEATURE_DESTINATION_PATH, "web2py", 'httpserver.pid')

        with open(application_pid_path, 'r') as pid_file:
            pid = int(pid_file.readline().strip())
    except:
        pass
    return pid


def stop_application(application_name):
    try:
        pid = get_pid_file(application_name)
        if pid is not None:
            os.kill(pid, signal.SIGTERM)
        time.sleep(5.0)
    except:
        pass


exitCode = 0

cwd = os.getcwd()

exitCode |= installUtils.setPythonRoot()

exit_code, process_id = messageBox.display('Preparing to install Black Box Pillar Sync web based'
                                           'application\nPlease wait...')

exit_code, process_id = messageBox.display('Stopping Pillar Sync web server and modbus consumer, please wait...')

stop_application("web_server")
stop_application("modbus_consumer")

exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_STAGING_PATH,
                                                         FEATURE_FILENAME),
                                            FEATURE_STAGING_PATH)

# backup databases and errors directory
if os.path.exists(os.path.join(FEATURE_DESTINATION_PATH, "web2py", "applications", "PillarSync", "databases")):
    shutil.rmtree(os.path.join(FEATURE_STAGING_PATH, "web2py", "applications", "PillarSync", "databases"))
    shutil.copytree(os.path.join(FEATURE_DESTINATION_PATH, "web2py", "applications", "PillarSync", "databases"),
                    os.path.join(FEATURE_STAGING_PATH, "web2py", "applications", "PillarSync", "databases"))

if os.path.exists(os.path.join(FEATURE_DESTINATION_PATH, "web2py", "applications", "PillarSync", "errors")):
    shutil.rmtree(os.path.join(FEATURE_STAGING_PATH, "web2py", "applications", "PillarSync", "errors"))
    shutil.copytree(os.path.join(FEATURE_DESTINATION_PATH, "web2py", "applications", "PillarSync", "errors"),
                    os.path.join(FEATURE_STAGING_PATH, "web2py", "applications", "PillarSync", "errors"))

shutil.copyfile(os.path.join(FEATURE_STAGING_PATH, "autoconfig.py"),
                os.path.join(BACKEND_PATH, "autoconfig.py"))

easy_install_packages = {'pyserial-2.7-py2.7.egg': 'pyserial-2.7.tar.gz',
                         'paho_mqtt-1.2-py2.7.egg': 'paho-mqtt-1.2.tar.gz'}

for package in easy_install_packages:
    if not os.path.exists(os.path.join("/", "usr", "lib", "python2.7", "site-packages", package)):
        os.system('/usr/bin/tar xzvfm %s/%s -C /tmp' % (FEATURE_STAGING_PATH, easy_install_packages[package]))
        os.system("/bin/sync")
        os.chdir("/tmp")
        os.system("/usr/bin/easy_install /tmp/%s" % easy_install_packages[package].split('.tar.gz')[0])
        os.system("/bin/sync")
        os.chdir(cwd)

pip_packages = {'web2py-2.1.1-py2.7.egg-info': 'web2py-2.1.1.tar.gz',
                'pygal-2.4.0.dist-info': 'pygal-2.4.0-py2.py3-none-any.whl',
                'pydal-20190915.2-py2.7.egg-info': 'pydal-20190915.2.tar.gz',
                'functools32-3.2.3_2-py2.7.egg-info': 'functools32-3.2.3-2.tar.gz',
                'protobuf-3.14.0.dist-info': 'protobuf-3.14.0-py2.py3-none-any.whl'}

for package in pip_packages:
    if not os.path.exists(os.path.join("/", "usr", "lib", "python2.7", "site-packages", package)):
        os.system("/usr/bin/pip install %s/%s" % (FEATURE_STAGING_PATH, pip_packages[package]))
        os.system("/bin/sync")

if os.path.exists(FEATURE_DESTINATION_PATH):
    shutil.rmtree(FEATURE_DESTINATION_PATH)
    os.makedirs(FEATURE_DESTINATION_PATH, 0755)

shutil.copytree(FEATURE_STAGING_PATH, FEATURE_DESTINATION_PATH)

messageBox.destroy(process_id)

print 'installation complete, removing installation files and rebooting black box...'
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
exit_code, process_id = messageBox.display('Installation is complete, rebooting Black Box...', timer=5)

# sync filesystem and reboot
os.system("/bin/sync")
os.system("/sbin/reboot")

sys.exit(0)
