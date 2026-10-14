######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script updates Linux/WES7 crontab for user root.
##  REQS    : Must have permissions to execute the
##            OS's crontab command without password
##            prompt. All options must be provided, except for -v -h -q
##
##  CRONTAB FORMAT:
# Minute   Hour   Day of Month       Month          Day of Week        Command
# (0-59)  (0-23)     (1-31)    (1-12 or Jan-Dec)  (0-6 or Sun-Sat)
#    0        2          12             *               0,6           /usr/local/hobart/python/somePythonScript.py
######################################################

# Sample command -># python updateCrontab.py -Y 2011 -M 10 -D 17 -H 14 -m 30 -S 20 -T "Eastern Time (US & Canada)" -t +05 -A 1 -o history.txt

# Will set to system and hardware clock to-> October 17, 2011 02:30:20 PM eastern timezone - using a 24 hour clock

# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import time
import sendDeviceEventMessage

cronjobFile = "/usr/local/hobart/python/cronjobs"

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################


##################################
#
#     E N D   G L O B A L S
#
##################################

###########################
p = OptionParser()

# Now add some CLI options
##########################

p.add_option("-m", "--minute",
             dest="minute",
             default="-1",
             help="The minute component 0-59 or *")
p.add_option("-H", "--hour",
             dest="hour",
             default="-1",
             help="The hour component 0-23 or *")
p.add_option("-D", "--dayOfMonth",
             dest="dayOfMonth",
             default="-1",
             help="The month component 1-31 or *")
p.add_option("-M", "--month",
             dest="month",
             default="-1",
             help="The month 1-12 or Jan-Dec or *")
p.add_option("-W", "--dayOfWeek",
             dest="dayOfWeek",
             default="-1",
             help="The day of week component 0-6 or Sun-Sat or * (ex. 0=Sunday 1=Monday 2=Tuesday etc.)")
p.add_option("-R", "--remove",
             dest="removeCron",
             default="-1",
             help="Use This option to remove your cronjob")
p.add_option("-S", "--script",
             dest="script",
             default="-1",
             help="Use This option for the python file name only. Do not give full path.")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()

crontabFile = "/usr/local/hobart/python/cronjobs"
pythonDir = "/usr/local/hobart/python/"
pythonLocation = "/usr/bin/python"

if os.name == "posix":
    #print("executing")
    exitCode = 0
    if str(options.removeCron) == "1":
        trial = 0
        #print("removing")
        # remove line from cronjobs file
        try:
            cronString = ""

            if options.script != "-1":
                print("removing")
                f = open(crontabFile, "r")
                lines = f.readlines()
                f.close()
                f = open(crontabFile, "w")
                for line in lines:
                    print(line)
                    tokens = line.split(' ')

                    if len(tokens) > 12:
                        #if tokens[11].rstrip() != str(pythonDir + options.script):
                        scriptTokens = options.script.split(' ')

                        if tokens[12].rstrip() != str(scriptTokens[7]):
                            if line != '\n':
                                f.write(line)
                        else:
                            print("Matching remove")
                    elif len(tokens) > 6:
                        scriptTokens = options.script.split(' ')
                        print("made it A")
                        print (str(tokens[6].rstrip()))
                        if tokens[6].rstrip() != options.script:
                            if line != '\n':
                                f.write(line)
                        else:
                            print("Matching small remove")
                    elif line != '\n':
                        f.write('\n' + line)
                f.close()

                cmd = []
                cmd.append("sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                f = open(crontabFile, "r")
                lines = f.readlines()
                f.close()
                #print("length lines : " + str(len(lines)))
                if len(lines):
                    cmd = []
                    #print("okay")
                    cmd.append("crontab")
                    cmd.append(crontabFile)
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                else:
                    cmd = []
                    #print("deleting")
                    cmd.append("crontab")
                    cmd.append("-r")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)


                cmd = []
                cmd.append("sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

        except:
            exitCode = 1
            #print("failed")
            sys.exit(exitCode)

    else:
        #Edit or add to crontab
        #print("edit add")
        try:
            noLines = False
            if options.script != "-1":
                if options.minute == "-1":
                    options.minute = "*"
                if options.hour == "-1":
                    options.hour = "*"
                if options.dayOfMonth == "-1":
                    options.dayOfMonth = "*"
                if options.month == "-1":
                    options.month = "*"
                if options.dayOfWeek == "-1":
                    options.dayOfWeek = "*"

                #if options.frequency != "2":
                userCronjob = str(options.minute) + " " + str(options.hour) + " " + str(options.dayOfMonth) + " " + str(options.month) + " " + str(options.dayOfWeek) + " " + str(options.script)
                #print("theCron = " + userCronjob)
                #elif options.frequency == "2":
                #    userCronjob = str(options.minute) + " " + str(options.hour) + " " + str(options.dayOfMonth) + " " + str(options.month) + " * " + "test 'date +\%a' != Sun || /usr/bin/python " + pythonDir + str(options.script)
                #print("here")
                lines = ""
                try:
                    f = open(crontabFile, "r")
                    lines = f.readlines()
                    f.close()
                except:
                    #print("can't open")
                    noLines = True
                    pass

                f = open(crontabFile, "w")
                found = False
                #print("1")
                i = 0
                if len(lines) > 0:
                    for line in lines:
                        #print("3")
                        tokens = line.split(' ')
                        #print("4")
                        #print("token is : " + tokens[12].rstrip())
                        #print("script is: " + options.script)
                        if len(tokens) > 11:
                            scriptTokens = options.script.split(' ')

                            if tokens[12].rstrip() == str(scriptTokens[7]):
                                #print("Match")
                                found = True
                                # 0 = wire      1 = wifi
                                line = userCronjob
                        if len(tokens) > 6:
                            if tokens[6].rstrip() == str(pythonDir + options.script):
                                #print("single")
                                found = True
                                # 0 = wire      1 = wifi
                                line = userCronjob
                        elif len(tokens) > 5:
                            if tokens[5].rstrip() == str(options.script):
                                #print("full path given")
                                found = True
                                line = userCronjob
                        if i > 0:
                            f.write('\n' + line)
                        elif line != '\n':
                            print("X" + line + "D")
                            f.write(line)

                        i = i + 1
                else:
                    noLines = True
                if not found:
                    #print(userCronjob)
                    #print("trying")
                    if noLines:
                        f.write(userCronjob)
                    else:
                        f.write('\n' + userCronjob)
                f.close()

                cmd = []
                cmd.append("sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)


                cmd = []
                cmd.append("crontab")
                cmd.append(crontabFile)
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                cmd = []
                cmd.append("sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
            else:
                pass

        except:
            pass
        cmd = []
        cmd.append("sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    sys.exit(exitCode)

else:
    # Note: This PY script is not used for Windows
    exitCode = 0
    sys.exit(exitCode)
