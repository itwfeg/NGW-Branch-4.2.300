# Office Viewer install script
# Assumes that script is running from current directory

import sys
import os

sys.path.append("c:\\Program Files\\hobart\\python")
cwd = os.getcwd()

import osUtils

cmd = []
cmd.append("msiexec")
cmd.append("/i")
cmd.append(os.path.join(cwd,"LibreOffice_4.2.6-secfix_Win_x86.msi"))
cmd.append("/qn")
cmd.append("ALLUSERS=1")
cmd.append("ADDLOCAL=ALL")
cmd.append("REBOOT=ReallySuppress")
osUtils.winRunElevated(cmd)

