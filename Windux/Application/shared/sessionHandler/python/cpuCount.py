#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess
import platform


def getCPUCount():
    p1 = subprocess.Popen(["/bin/cat", "/proc/cpuinfo"], stdout=subprocess.PIPE)
    p2 = subprocess.Popen(["/usr/bin/grep","processor"], stdin=p1.stdout, stdout=subprocess.PIPE)
    p3 = subprocess.Popen(["/usr/bin/wc","-l"], stdin=p2.stdout, stdout=subprocess.PIPE)

    p1.stdout.close()
    p2.stdout.close()
    output,err = p3.communicate()
    return str(int(output))


if __name__ == '__main__':
    numCPUs = getCPUCount()

    print(numCPUs)
