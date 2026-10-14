import sys
import os
import shutil

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import installUtils
import shellCommand

exitCode = 0

shutil.copyfile(os.path.join("/", "usr", "local", "hobart", "features", "monolithic_core_preinstall", "verifyFeatureInstaller.sh"), os.path.join('/', 'usr', 'local', 'hobart', 'python', 'verifyFeatureInstaller.sh'))

os.system('/bin/chmod +x {0}'.format(os.path.join('/', 'usr', 'local', 'hobart', 'python', 'verifyFeatureInstaller.sh')))

# update ssh keys [root]
cmd = []
cmd.append("/bin/mkdir")
cmd.append("-p")
cmd.append("/root/.ssh")
exitCode, stdout, stderr = shellCommand.execute(cmd)

cmd = []
cmd.append("/bin/touch")
cmd.append("/root/.ssh/authorized_keys")
exitCode, stdout, stderr = shellCommand.execute(cmd)

os.system("/bin/cat /usr/local/hobart/features/monolithic_core_preinstall/hobart_ssh.pub >> /root/.ssh/authorized_keys")

# update add root ca
os.system("/bin/cp /usr/local/hobart/features/monolithic_core_preinstall/*.crt /usr/local/share/ca-certificates")
os.system("/usr/sbin/update-ca-certificates")

# HT-501: Need to have a eGTouch0.param file
if not os.path.exists(os.path.join("/","etc","eGTouch0.param")):
    os.system("/bin/cp /usr/local/hobart/features/monolithic_core_preinstall/eGTouch0.param /etc/.")

# HT-242 Remove all files and directories under /usr/local/hobart/features on the scale
os.system("/bin/rm -rf /usr/local/hobart/features/*")

exitCode |= installUtils.removeInstallerFiles("monolithic_core_preinstall")

sys.exit(exitCode)
