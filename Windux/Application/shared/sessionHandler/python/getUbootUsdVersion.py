#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import updateUbootBitmaps
import sys

if __name__ == '__main__':
    
    uBootVersionOnCard = updateUbootBitmaps.getUbootUsdCardVersion()

    print(uBootVersionOnCard)
