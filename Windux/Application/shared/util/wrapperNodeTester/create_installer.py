import installUtils
import file_match_and_replace
import os
import argparse
import compile_forms
import sys


def generate(product, feature, version, test_lan=False, company='itw'):

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
                            "*.desktop",
                            "usage_guide.html",
                            "node_controller_tester_usage_diagram.png",
                            "wrapperNodeTester.png"]

    compile_forms.compile_resource('wrapperNodeTester')
    compile_forms.compile_forms()

    file_match_and_replace.file_match_and_replace('wrapper_node_tester.ini',
                                                  '^serial_port[ \t]*=[ \t]*.+$',
                                                  'serial_port = /dev/ttys0')

    file_match_and_replace.file_match_and_replace('wrapper_node_tester.ini',
                                                  '^barcode_scanner_device[ \t]*=[ \t]*.+$',
                                                  'barcode_scanner_device = /dev/ttyACM0')

    file_match_and_replace.file_match_and_replace('wrapperNodeTester.py',
                                                  '^PRODUCT_VERSION[ \t]*=[ \t]*[\'\"][0-9]+\.[0-9]+\.[0-9]+[\'\"]$',
                                                  'PRODUCT_VERSION = "%s"' % version)

    os.chmod('install_wrapper_node_tester.sh', 0o755)
    os.system("%s install_wrapper_node_tester.sh" % dos2unix_binary)

    # os.system("pyuic5 wrapperNodeTester.ui -o Ui_wrapperNodeTester.py")
    # os.system("pyrcc5 -o wrapperNodeTester_rc.py wrapperNodeTester.qrc")

    installUtils.build_runtime_tarball(feature_name=feature,
                                       file_patterns_to_add=file_patterns_to_add)

    installUtils.build_installer_tarball(feature_name=feature,
                                         version=version,
                                         company=company,
                                         product=product)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="create wrapper node tester installer file")

    parser.add_argument("version",
                        action="store",
                        help="software version number ex: 1.0.0")

    parser.add_argument("--test_lan",
                        action='store_true',
                        help="Test LAN settings if specified")

    args = parser.parse_args()

    generate('test_station', 'wrapper_node_tester', args.version, args.test_lan)


