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
                            "memtester",
                            "*.desktop",
                            "usage_guide.html",
                            "cpu_memory_tester_usage_diagram.png",
                            "cpu_memory_tester.png"]

    compile_forms.compile_resource('cpu_memory_tester')
    compile_forms.compile_forms()

    # os.system("pyuic5 cpu_memory_tester.ui -o Ui_cpu_memory_tester.py")
    # os.system("pyrcc5 -o cpu_memory_tester_rc.py cpu_memory_tester.qrc")

    file_match_and_replace.file_match_and_replace('cpu_memory_tester.py',
                                                  '^VERSION[ \t]*=[ \t]*[\'\"][0-9]+\.[0-9]+\.[0-9]+[\'\"]$',
                                                  'VERSION = "%s"' % version)

    file_match_and_replace.file_match_and_replace('cpu_memory_tester.py',
                                                  '^SUBNET[ \t]*=[ \t]*[\'\"][0-9]+\.[0-9]+\.[0-9]+.[0-9]+[\'\"]$',
                                                  'SUBNET = "%s"' % subnet)

    os.chmod('memtester', 0o755)
    os.chmod("install_cpu_memory_tester.sh", 0o755)
    os.system("%s install_cpu_memory_tester.sh" % dos2unix_binary)

    installUtils.build_runtime_tarball(feature_name=feature,
                                       file_patterns_to_add=file_patterns_to_add)

    installUtils.build_installer_tarball(feature_name=feature,
                                         version=version,
                                         company=company,
                                         product=product)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="create cpu memory tester installer file")

    parser.add_argument("version",
                        action="store",
                        help="software version number ex: 1.0.0")

    parser.add_argument("--test_lan",
                        action='store_true',
                        help="Test LAN settings if specified")

    args = parser.parse_args()

    generate('test_station', 'cpu_memory_tester', args.version, args.test_lan)
