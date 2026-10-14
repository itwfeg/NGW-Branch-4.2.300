import os
import sys
import configInfo
import installUtils
import messageBox
import glob
import setConfigValues

if __name__ == '__main__':

    with open('/etc/hobart/configc.dat', 'a') as config_file:
        config_file.write('\n')
        config_file.write('selfServeWeightRequiredToSelectItem=T')

    exit_code, process_id = messageBox.display('Feature installation is complete. About to reboot the scale.\n\nPlease remove the thumb drive.', timer=10)

    os.system('reboot')
