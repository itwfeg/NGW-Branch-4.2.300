
import argparse
import os
import sys

import shellCommand

CRON_FILE="/tmp/current_crontab.txt"
SCRIPT="walmartHealthCheck.py"
SCRIPT_FULL="/usr/local/hobart/python/" + SCRIPT

parser = argparse.ArgumentParser('Set or update Walmart Health Check cron job.')
parser.add_argument('-t','--time', help='execution time (cron format)', required=True)
parser.add_argument('-d','--delay', help='delay time in seconds', default='0', required=False)

args = parser.parse_args()

# make sure script exists
if not os.path.isfile(SCRIPT_FULL):
    sys.exit()

# remove any existing crontab
if os.path.isfile(CRON_FILE):
    os.remove(CRON_FILE)

# read current jobs
cur_cron = os.popen('crontab -u root -l > ' + CRON_FILE)
cur_cron.read()
cur_cron.close()

# parse current jobs
f = open(CRON_FILE,"r")
lines = f.readlines()
f.close()

# write new jobs
f = open(CRON_FILE,"w")

for line in lines:
    if SCRIPT in line:
        pass
    else:
        f.write(line)

if 'uninstall' != args.time:
    if '0' == args.delay:
        f.write(args.time + ' /usr/bin/python ' + SCRIPT_FULL + ' -i \n')
    else:
        f.write(args.time + ' sleep ' + args.delay + '; /usr/bin/python ' + SCRIPT_FULL + ' -i \n')    

f.close()

# import new crontab
cmd = []
cmd.append('crontab')
cmd.append('-u')
cmd.append('root')
cmd.append(CRON_FILE)
exitCode, stdout, stderr = shellCommand.execute(cmd)

