import installUtils
import file_match_and_replace
import os
import argparse
import compile_forms
import sys


def generate(product, feature, version, test_lan=True, company='itw'):

    if test_lan:
        subnet = '192.168.0.0'
    else:
        subnet = '192.168.1.0'

    if sys.platform == 'win32':
        dos2unix_binary = "C:/users/davisrs/bin/dos2unix.exe"
    else:
        dos2unix_binary = 'dos2unix'

    file_patterns_to_add = ["*.py",
                            "*.sh",
                            "*.ini",
                            "*.imx",
                            "*.conf",
                            "imx_usb",
                            "*.desktop",
                            "usage_guide.html",
                            "acr_board_programmer_usage_diagram.png",
                            "acrBoardProgrammer.png"]

    compile_forms.compile_resource('acrBoardProgrammer')
    compile_forms.compile_forms()

    file_match_and_replace.file_match_and_replace('acrBoardProgrammer.py',
                                                  '^PRODUCT_VERSION[ \t]*=[ \t]*[\'\"][0-9]+\.[0-9]+\.[0-9]+[\'\"]$',
                                                  'PRODUCT_VERSION = "%s"' % version)

    os.chmod('imx_usb', 0o755)
    os.chmod('install_acr_board_programmer.sh', 0o755)
    os.system("%s install_acr_board_programmer.sh" % dos2unix_binary)

    # os.system("pyuic5 acrBoardProgrammer.ui -o Ui_acrBoardProgrammer.py")
    # os.system("pyrcc5 -o acrBoardProgrammer_rc.py acrBoardProgrammer.qrc")

    installUtils.build_runtime_tarball(feature_name=feature,
                                       file_patterns_to_add=file_patterns_to_add)

    installUtils.build_installer_tarball(feature_name=feature,
                                         version=version,
                                         company=company,
                                         product=product)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="create acr board programmer installer file")

    parser.add_argument("version",
                        action="store",
                        help="software version number ex: 1.0.0")

    parser.add_argument("--test_lan",
                        action='store_true',
                        help="Test LAN settings if specified")

    args = parser.parse_args()

    generate('test_station', 'acr_board_programmer', args.version, args.test_lan)
