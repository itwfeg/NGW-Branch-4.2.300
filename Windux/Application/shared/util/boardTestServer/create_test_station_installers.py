import os
import shutil
import sys

product = 'test_station'
version = '1.6.0'
test_lan = '--test_lan'
copy_to_scotts_usb = True
if sys.platform == 'darwin':
    usb_device_path = "/Volumes/Samsung_T5/test_station/installers"
else:
    usb_device_path = "F:/test_station/installers"
# test_lan = ''

if sys.platform == 'win32':
    python_binary = 'python'
else:
    python_binary = 'python3'

os.system("%s create_client_installer.py %s %s" % (python_binary, test_lan, version))
os.system("%s create_server_installer.py %s %s" % (python_binary, test_lan, version))

os.chdir("../acrBoardProgrammer")
os.system("%s create_installer.py %s %s" % (python_binary, test_lan, version))

os.chdir("../cpuMemoryTester")
os.system("%s create_installer.py %s %s" % (python_binary, test_lan, version))

os.chdir("../wrapperNodeTester")
os.system("%s create_installer.py %s %s" % (python_binary, test_lan, version))

# if sys.platform == 'win32':
#    os.system("buildWrapperNodeTester.bat")

os.chdir("../boardTestServer")

if copy_to_scotts_usb:
    if os.path.exists(usb_device_path):
        shutil.copy('itw-test_station-test_station_client-%s.tgz' % version,
                    usb_device_path)
        shutil.copy('itw-test_station-test_station_server-%s.tgz' % version,
                    usb_device_path)
        shutil.copy('../acrBoardProgrammer/itw-test_station-acr_board_programmer-%s.tgz' % version,
                    usb_device_path)
        shutil.copy('../cpuMemoryTester/itw-test_station-cpu_memory_tester-%s.tgz' % version,
                    usb_device_path)
        shutil.copy('../wrapperNodeTester/itw-test_station-wrapper_node_tester-%s.tgz' % version,
                    usb_device_path)
        # if sys.platform == 'win32':
        #    shutil.copy('../wrapperNodeTester/itw-win-node_controller_tester_setup_utility-%s.exe' % version,
        #                usb_device_path)
