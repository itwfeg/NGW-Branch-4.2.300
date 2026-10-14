# something's broken

import os
import shellCommand

def addCronJob():
    cur_cron = os.popen('crontab -u root -l > /usr/local/hobart/python/current_crontab.txt; sync')
    cur_cron.read()
    cur_cron.close()

    f = open("/usr/local/hobart/python/current_crontab.txt","r")
    lines = f.readlines()
    f.close()
    f = open("/usr/local/hobart/python/current_crontab.txt","w")

    for line in lines:
        if ('snmpPersistent.py' in line) or ('SNMP' in line) or ('snmp' in line) or (line in ['\n', '\r\n']):
            pass
        else:
            f.write(line)
    f.close()
    fopen_cron = open('/usr/local/hobart/python/current_crontab.txt', 'a')
    fopen_cron.write("\n### The is for SNMP persistence")
    fopen_cron.write("\n* * * * * /usr/bin/python /usr/local/hobart/python/snmpPersistent.py\n")
    fopen_cron.close()

    cmd = []
    cmd.append('crontab')
    cmd.append('-u')
    cmd.append('root')
    #cmd.append('-l')
    cmd.append('/usr/local/hobart/python/current_crontab.txt')
    exitCode, stdout, stderr = shellCommand.execute(cmd)

if __name__== "__main__":
    addCronJob()

