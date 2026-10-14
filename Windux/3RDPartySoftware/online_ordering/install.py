# online ordering install script
# This feature installs the web application and enables mysql+lighttpd
#
# This script must be executed in directory where feature is located
# For instance /usr/local/hobart/features/online_ordering

import os
import sys
import glob
import subprocess

# Must add reference to python tree so import such as 'import shellCommand' will work
sys.path.append(os.path.join("/", "usr", "local", "hobart", "python"))

import installUtils
import shellCommand

feature_name = "online_ordering"
feature_version = "1.0"

enable_mysql_file = "/etc/hobart/enable_mysql"
enable_lighttpd_file = "/etc/hobart/enable_lighttpd"
target_folder = "/var/www/"

exitCode = 0

#------------------------------------------------------
# SOF-4527: Add job to cron that will delete old orders
#------------------------------------------------------
def addCronJob():
    os.system('crontab -u root -l > /tmp/current_crontab.txt; sync')

    with open("/tmp/current_crontab.txt","r") as f:
        lines = f.readlines()
    
    with open("/tmp/current_crontab.txt","w") as f:
        for line in lines:
            if ('purge_old_orders.py' in line):
                pass
            else:
                # Save the 'other' current entries
                f.write(line)

    # Append the new entry for on-line ordering
    with open('/tmp/current_crontab.txt', 'a') as fopen_cron:
        fopen_cron.write("0 2 * * * /usr/bin/python /usr/local/hobart/features/online_ordering/purge_old_orders.py\n")

    # Re-write all of the entries in the crontab file
    cmd = []
    cmd.append('crontab')
    cmd.append('-u')
    cmd.append('root')
    cmd.append('/tmp/current_crontab.txt')
    exitCode, stdout, stderr = shellCommand.execute(cmd)


exitCode |= shellCommand.execute(["/bin/bash", "config_services.sh"])[0]

# populate rc folders
exitCode |= installUtils.addInitdScript("hydra-booda")
exitCode |= installUtils.addInitdScript("web2py-scgihandler")

# add to list of installed features
exitCode |= installUtils.addFeatureToInstalledList(feature_name, feature_version)

addCronJob()

sys.exit(exitCode)
