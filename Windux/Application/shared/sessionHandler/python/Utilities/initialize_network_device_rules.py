#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# @Filename : initialize_network_device_rules
# @Creation Date : 2019-11-01-14-37
# @Project: python
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis

import os
import sys


def main():

    status_code = 0  # file exists and contents is good
    net_rules_file = "/etc/udev/rules.d/70-persistent-net.rules"
    if os.path.exists(net_rules_file):
        with open(net_rules_file, "r") as f:
            lines = f.readlines()
            for line in lines:
                if 'KERNEL=="eth*"' in line.decode('utf-8'):
                    if 'NAME="eth0"' not in line.decode('utf-8'):
                        os.remove(net_rules_file)
                        status_code = 1  # file exists but eth device is wrong so remove file

    else:
        status_code = 2  # file does not exist

    sys.exit(status_code)


if __name__ == '__main__':
    main()
