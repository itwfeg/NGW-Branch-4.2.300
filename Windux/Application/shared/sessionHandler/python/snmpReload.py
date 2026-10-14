# -*- coding: utf-8 -*-

# something's broken

import os
import sys
import shellCommand
import subprocess
import snmpAddCronJob

def snmpReload():

    try:
        
        snmpAddCronJob.addCronJob()

        cmd = []
        cmd.append("pkill")
        cmd.append("-9")
        cmd.append("-f")
        cmd.append("snmpHobart.py")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    except Exception as error:
        import traceback
        traceback.print_exc(error)
        sys.exit(1)

    os.system('/usr/bin/python /usr/local/hobart/python/snmpHobart.py &')

if __name__== "__main__":
    snmpReload()

    print("0")

    sys.exit(0)
