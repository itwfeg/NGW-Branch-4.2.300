#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
#import shellCommand
#import file_io
import string
import languageSettings

def updateTimingsInfo(csv_data):

    if os.name == "posix":
        # LINUX OS
        csv_output_file = "/usr/local/hobart/actuatorTimings.log"
    else:
        csv_output_file = "C:/hobart/actuatorTimings.log"

    with open(csv_output_file, 'w+') as log_file:
        log_file.write(csv_data)
        log_file.close()
	
    if os.name == "posix":
        os.system("/bin/sync")

    status = 0
    return status

if __name__ == "__main__":

    status = 0

    timings_data = sys.argv[1]
    #timings_data   = sys.argv[2]

    #timings_data = "Apr 20, 2018 10:49,97,259,37,74,23,93,48,95,61,132,72,157,455,709,386,159,112,48,29,151,235"

    status = updateTimingsInfo( timings_data )

    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
