#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import license
import argparse
import sys

if __name__ == '__main__':
    exitCode = 0
    parser = argparse.ArgumentParser(description="determine if specified product is licensed")
    parser.add_argument("productName",
                        action="store",
                        help="product to determine if licensed")

    args = parser.parse_args()

    if license.isProductLicensed(license.readFlashKeyBuffer(), args.productName):
        exitCode = 1
        print('1')
    else:
        exitCode = 0
        print('0')
        
    sys.exit(exitCode)
