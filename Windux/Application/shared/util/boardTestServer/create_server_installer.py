import installUtils
import os
import pathlib
import file_match_and_replace
import argparse
import compile_forms
import sys


def generate(product, feature, version, test_lan=True, company='itw'):

    if test_lan:
        subnet = '192.168.0'
    else:
        subnet = '192.168.1'

    if sys.platform == 'win32':
        dos2unix_binary = "C:/users/davisrs/bin/dos2unix.exe"
    else:
        dos2unix_binary = 'dos2unix'

    file_patterns_to_add = ["*.py",
                            "*.sh",
                            "*.ini",
                            "server_install",
                            "*wallpaper*.png",
                            "test_server_window_icon.png",
                            "*.desktop",
                            "test_station_usage_diagram.png",
                            "usage_guide.html",
                            ".config"]

    remove_files = ['install.py',
                    "client_install",
                    "server_install"]

    for current_file in remove_files:
        try:
            os.remove(current_file)
        except:
            pass

    pathlib.Path("server_install").touch(mode=0o644, exist_ok=True)

    with open("install.py", "w") as install_file:
        lines = ['#!/usr/bin/env python3\n',
                 'import os\n',
                 'os.system("/bin/sh -x install_test_station.sh -s")\n\n']
        install_file.writelines(lines)

    compile_forms.compile_resource('boardTestStationExecutive')
    compile_forms.compile_forms()

    # os.system("pyuic5 boardTestStationExecutive.ui -o Ui_boardTestStationExecutive.py")
    # os.system("pyrcc5 -o boardTestStationExecutive_rc.py boardTestStationExecutive.qrc")

    if sys.platform == 'darwin':
        file_match_and_replace.file_match_and_replace("boardTestStationExecutive.py",
                                                      '[ \t]*INSTALLATION_DIRECTORY[ \t]*=[ \t]*\".+\"[ \t]*$',
                                                      'INSTALLATION_DIRECTORY = "/home/pi/test_station_server"')
    else:
        file_match_and_replace.file_match_and_replace("boardTestStationExecutive.py",
                                                      '[ \t]*INSTALLATION_DIRECTORY[ \t]*=[ \t]*\".+\"[ \t]*$',
                                                      'INSTALLATION_DIRECTORY = "/home/pi/test_station_server"')

    file_match_and_replace.file_match_and_replace('boardTestStationExecutive.py',
                                                  '^VERSION[ \t]*=[ \t]*[\'\"][0-9]+\.[0-9]+\.[0-9]+[\'\"]$',
                                                  'VERSION = "%s"' % version)

    file_match_and_replace.file_match_and_replace('install_test_station.sh',
                                                  '^subnet[ \t]*=[ \t]*[\'\"][0-9]+\.[0-9]+\.[0-9]+[\'\"]$',
                                                  'subnet="%s"' % subnet)

    os.chmod('install_test_station.sh', 0o755)
    os.chmod('board_test_server.sh', 0o755)

    os.system("%s install_test_station.sh" % dos2unix_binary)
    installUtils.build_runtime_tarball(feature_name=feature,
                                       file_patterns_to_add=file_patterns_to_add)

    installUtils.build_installer_tarball(feature_name=feature,
                                         version=version,
                                         company=company,
                                         product=product)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="create test station server installer file")

    parser.add_argument("version",
                        action="store",
                        help="software version number ex: 1.0.0")

    parser.add_argument("--test_lan",
                        action='store_true',
                        help="Test LAN settings if specified")

    args = parser.parse_args()

    generate('test_station', 'test_station_server', args.version, args.test_lan)
