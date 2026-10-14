#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import updateUbootBitmaps
import sys

if __name__ == '__main__':
    if updateUbootBitmaps.ubootNeedsUpgraded():
        sys.exit(0)
    else:
        sys.exit(1)

