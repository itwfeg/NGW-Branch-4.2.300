#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os

if __name__ == "__main__":

    if os.name == "nt":
        # WINDOWS OS
        try:
            ver = sys.getwindowsversion()
            ver_format = str(ver[0]) + "." + str(ver[1])
        except:
            ver_format = "0.0"
        print(ver_format)

        sys.exit(0)
