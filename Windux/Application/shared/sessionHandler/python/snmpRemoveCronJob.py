#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand

def removeCronJob():
    cur_cron = os.popen('crontab -u root -l > /usr/local/hobart/python/current_crontab.txt; sync')
    cur_cron.read()
    cur_cron.close()

    f = open("/usr/local/hobart/python/current_crontab.txt","r")
    lines = f.readlines()
    f.close()
    f = open("/usr/local/hobart/python/current_crontab.txt","w")

    for line in lines:
        if 'snmpPersistent.py' in line or 'SNMP' in line or 'snmp' in line:
            pass
        else:
            f.write(line)
    f.write("\n")
    f.close()

    cmd = []
    cmd.append('crontab')
    cmd.append('-u')
    cmd.append('root')
    #cmd.append('-l')
    cmd.append('/usr/local/hobart/python/current_crontab.txt')
    exitCode, stdout, stderr = shellCommand.execute(cmd)

if __name__== "__main__":
    removeCronJob()

