#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : November 10, 2014
##  OVERVIEW: This script encrypts and copies the memory states to a USB drive and/or prints plaintext to stdout
##  REQS    :
######################################################

# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import shutil
import string
import subprocess
import encryptDecrypt
import codecs

#################################
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

######################################


def dumpStates(fileToSave, save2File, passphrase):
    if os.name == "posix":

        fileContents = ""
        #cat /var/log/dmesg
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>cat /var/log/dmesg</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXX  cat /var/log/dmesg  XXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXX AAAAAAAAXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXX  cat /var/log/dmesg  XXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXX AAAAAAAAXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/bin/cat")
        cmd.append("/var/log/dmesg")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # dmesg
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>dmesg</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXX  dmesg  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXX  dmesg  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/bin/dmesg")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # lsmod
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lsmod</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXX  lsmod  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXX  lsmod  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/usr/bin/lsmod")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + fileContents
        sys.stdout.flush()

        # cat /proc/modules
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>cat /proc/modules</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXX  cat /proc/modules  XXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXX  cat /proc/modules  XXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/bin/cat")
        cmd.append("/proc/modules")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # lsusb
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lsusb</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXX  lsusb  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXX  lsusb  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/usr/bin/lsusb")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # lspci
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lspci</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXX  lspci  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXX  lspci  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/usr/bin/lspci")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # cat /proc/meminfo
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>cat /proc/meminfo</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXX  cat /proc/meminfo  XXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXX  cat /proc/meminfo  XXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/bin/cat")
        cmd.append("/proc/meminfo")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # cat /proc/buddyinfo
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>cat /proc/buddyinfo</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXX  cat /proc/buddyinfo  XXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXX  cat /proc/buddyinfo  XXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        #sys.stdout.flush()
        cmd = []
        cmd.append("/bin/cat")
        cmd.append("/proc/buddyinfo")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        sys.stdout.flush()

        # ps -e -orss=,pid=,args= | sort -b -k1,1n
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>ps -e -orss=,pid=,args= | sort -b -k1,1n</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXX  ps -e -orss=,pid=,args= | sort -b -k1,1n  XXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXX  ps -e -orss=,pid=,args= | sort -b -k1,1n  XXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        p1 = subprocess.Popen(["/usr/bin/ps", "-e", "-orss=,pid=,args="], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["/usr/bin/sort", "-b", "-k1,1n"], stdin=p1.stdout, stdout=subprocess.PIPE)
        p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
        output,err = p2.communicate()
        print(output.decode())
        fileContents += "\n" + output.decode()
        sys.stdout.flush()

        # slabtop -o -s c
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>slabtop -o -s c</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXX  slabtop -o -s c  XXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXX  slabtop -o -s c  XXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/usr/bin/slabtop")
        cmd.append("-o")
        cmd.append("-s")
        cmd.append("c")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # cat /proc/slabinfo
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>cat /proc/slabinfo</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXX  cat /proc/slabinfo  XXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXX  cat /proc/slabinfo  XXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"


        cmd = []
        cmd.append("/bin/cat")
        cmd.append("/proc/slabinfo")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # netstat -an
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>netstat -an</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXX  netstat -an  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXX  netstat -an  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"


        cmd = []
        cmd.append("/bin/netstat")
        cmd.append("-a")
        cmd.append("-n")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # top -n1 b
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>top -n1 b</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXX  top -n1 b   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXX  top -n1 b   XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/usr/bin/top")
        cmd.append("-n")
        cmd.append("1")
        cmd.append("b")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # lsof
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lsof</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXX  lsof  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXX  lsof  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        cmd = []
        cmd.append("/usr/bin/lsof")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        print(stdout)
        fileContents += "\n" + stdout
        sys.stdout.flush()

        # lsof |grep chobart_d |wc
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lsof |grep chobart_d |wc</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXX   lsof |grep chobart_d |wc  XXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()


        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXX   lsof |grep chobart_d |wc  XXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        p1 = subprocess.Popen(["/usr/bin/lsof"], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["/usr/bin/grep", "chobart_d"], stdin=p1.stdout, stdout=subprocess.PIPE)
        p3 = subprocess.Popen(["wc"], stdin=p2.stdout, stdout=subprocess.PIPE)
        p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
        p2.stdout.close()
        output,err = p3.communicate()
        print(output.decode())
        fileContents += "\n" + output.decode()
        sys.stdout.flush()

        # lsof |grep scaleui |wc
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lsof |grep scaleui |wc</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXX   lsof |grep scaleui |wc  XXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()


        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXX   lsof |grep scaleui |wc  XXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        p1 = subprocess.Popen(["/usr/bin/lsof"], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["/usr/bin/grep", "scaleui"], stdin=p1.stdout, stdout=subprocess.PIPE)
        p3 = subprocess.Popen(["wc"], stdin=p2.stdout, stdout=subprocess.PIPE)
        p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
        p2.stdout.close()
        output,err = p3.communicate()
        print(output.decode())
        fileContents += "\n" + output.decode()
        sys.stdout.flush()

        # lsof |grep gcd |wc
        #print ("<TABLE bgcolor=#000000 WIDTH=100%><TR><TD><CENTER><FONT COLOR=#FFFFFF SIZE=+1><B>lsof |grep gcd |wc</B></FONT></CENTER></TD></TR></TABLE>")
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXX   lsof |grep gcd |wc  XXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()
        print ("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
        sys.stdout.flush()

        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXX   lsof |grep gcd |wc  XXXXXXXXXXXXXXXXXXXXXXXXXXX"
        fileContents += "\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

        p1 = subprocess.Popen(["/usr/bin/lsof"], stdout=subprocess.PIPE)
        p2 = subprocess.Popen(["/usr/bin/grep", "gcd"], stdin=p1.stdout, stdout=subprocess.PIPE)
        p3 = subprocess.Popen(["wc"], stdin=p2.stdout, stdout=subprocess.PIPE)
        p1.stdout.close()  # Allow p1 to receive a SIGPIPE if p2 exits.
        p2.stdout.close()
        output,err = p3.communicate()
        print(output.decode())
        fileContents += "\n" + output.decode()
        sys.stdout.flush()

        cmd = []
        cmd.append("/bin/sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if save2File == "1":
            # do a save in db/settingswizard

            try:
                # Encrypt the content
                # Test debug below
                if len(passphrase) > 4:
                    fileContents = encryptDecrypt.encrypt(fileContents, passphrase)

                    #Save the encrypted content to a file
                    destination = codecs.open("/usr/local/hobart/db/settingswizard/dumpStates.txt", 'w+', 'base64')
                    destination.write(fileContents)
                    destination.close()

                    if fileToSave == "0":
                        #this is a save all to tarball
                        destination = "do nothing"
                    elif len(fileToSave) > 0:
                        destination = codecs.open(fileToSave, 'w+', 'base64')
                        source = codecs.open('/usr/local/hobart/db/settingswizard/dumpStates.txt', 'r', 'base64')
                        destination.write(source.read())
                        destination.close()
                        source.close()

                    cmd = []
                    cmd.append("sync")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                
            except:
                pass
            return "1"
        else:
            if fileToSave is not None:
                if len(fileToSave) > 0:
                    file = open(fileToSave, "w")
                    file.write(fileContents)
                    file.close()

    else:
        TBD = "windows"


if __name__ == "__main__":

    # Let's get a parser to use
    ###########################
    p = OptionParser()

    # Now add some CLI options
    ##########################
    p.add_option("-F", "--fileToSave",
                 dest="fileToSave",
                 help="Full path, with filename, to save on USB drive.")
    p.add_option("-S", "--save2File",
                 dest="save2File",
                 help="Whether to save a file with ooutput content to disk or not. 0 = no, 1 = yes")
    p.add_option("-P", "--passphrase",
                 dest="passphrase",
                 help="passphrase to encrypt and decrypt")
    p.add_option("-v", "--verbose",
                 dest="verbose", default=None, action="store_true",
                 help="Print detailed status")
    p.add_option("-q", "--quiet",
                 dest="verbose", default=None, action="store_false",
                 help="No status printing (default)")

    # Parse User's CLI args
    #######################
    options, args = p.parse_args()
    exitCode = 0
    dumpStates(options.fileToSave,options.save2File, options.passphrase)


    sys.exit(0)
