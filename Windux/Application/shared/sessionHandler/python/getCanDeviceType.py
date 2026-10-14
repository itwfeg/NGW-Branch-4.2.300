#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# not tested in python3

import os
import sys
import can

# add the following environment variables for PYCAN

os.environ['PYTHONPATH'] = \
    '/usr/lib/python3.6/dist-packages:/usr/lib/python3.6/site-packages:/usr/local/hobart/python'
os.environ['CAN_INTERFACE'] = 'socketcan_ctypes'
os.environ['CAN_CHANNEL'] = 'can0'


###################################################
# Script used to return what type (model) KPC on ARK board if detected.
# Script assumes CAN interface is up and that the KPC is out of reset and running
# it's bootloader.
#
#   Models:
#   Service scale KPC:                              0x220 or 544
#   Service scale KPC with Value Max:    0x221 or 545
#   Service scale Printer only:                 0x230 or 560
#   ARK board self serive peripheral controller   0x250 or 592
#   Pre-pack Weigher:                              0x240 or 576
#   Pre-pack Printer1:                              0x250 or 592
#   Pre-pack Printer2:                              0x260 or 608
#   Pre-pack Printer3:                              0x260 or 624
###################################################

def detectModel():
    model = 0
    if sys.platform.startswith('linux'):
        try:
            bus = can.interface.Bus(channel='can0',
                                    bustype='socketcan_ctypes')
            msg = can.Message(arbitration_id=0x040, data=bytes([0x00]),
                              extended_id=False)
            bus.send(msg)
            msg = bus.recv()

            if len(msg) > 0:

                responseType = int(msg.arbitration_id)
                # if response is a KPCBootStatus - 0x070 (112)

                if responseType == 112:
                    data = msg.data

                    if len(data) > 0:

                        # From case 'KpcBootStatus' in CanTranslator.cpp
                        model = data[0] << 8 | data[1]
        except:
            pass

        print (('model: ' + str(model)))

    return model


if __name__ == '__main__':
    detectModel()
