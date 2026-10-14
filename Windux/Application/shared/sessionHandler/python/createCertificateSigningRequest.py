#!/usr/bin/env python2
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script generates a CSR file to a certain location Linux/WES7.
##  REQS    : All options must be provided, except for -v -h -q
######################################################


# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import sendDeviceEventMessage
import shutil

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

######################################

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-D", "--destination",
             dest="destination",
             help="Full path of the directory to copy files to. Most Likely on a removable flash drive.")
p.add_option("-F", "--filename",
             dest="filename",
             help="Filename to create. Commonly the serial number only.")
p.add_option("-T", "--csrType",
             dest="csrType",
             help="The type of the CSR to generate.")
p.add_option("-E", "--expiration",
             dest="expiration",
             help="The number of days until key will expire.")
p.add_option("-P", "--password",
             dest="password",
             help="Password to allow creating the CSR.")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()

# User this for debugging
#########################

#print 'outputfile: %s'%options.outputFile
#print 'verbose   : %s'%options.verbose
#print 'args      : %r',args

insidePassword = "woozal"

if os.name == "posix":
    exitCode = 0
    if options.password == insidePassword:
        if options.filename != None:

            # creating a client CSR
            cmd = []
            cmd.append("openssl")
            cmd.append("req")
            cmd.append("-new")
            cmd.append("-nodes")
            cmd.append("-keyout")
            cmd.append("/etc/ssl/" + options.filename + "_key.pem")
            cmd.append("-out")
            cmd.append("/etc/ssl/" + options.filename + "_req.pem")
            cmd.append("-days")
            cmd.append(options.expiration)
            cmd.append("-config")
            cmd.append("/etc/ssl/openssl.cnf")
            cmd.append("-subj")
            cmd.append("/OU=Store/CN="+options.filename)

            exitCode, stdout, stderr = shellCommand.execute(cmd)

            shutil.copy2("/etc/ssl/" + options.filename + "_key.pem", "/usr/local/hobart/db/settingswizard")
            shutil.copy2("/etc/ssl/" + options.filename + "_req.pem", "/usr/local/hobart/db/settingswizard")

            shutil.copy2("/etc/ssl/" + options.filename + "_req.pem", options.destination)
            shutil.copy2("/etc/ssl/" + options.filename + "_key.pem", options.destination)
            if exitCode == 0:
                cmd = []
                cmd.append("/bin/sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

    sys.exit(exitCode)

else:
    # WINDOWS BLOCK GOES HERE

    sys.exit(exitCode)
