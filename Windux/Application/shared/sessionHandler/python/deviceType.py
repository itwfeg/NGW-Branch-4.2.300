# -*- coding: utf-8 -*-

import os


def is_device_type(live, device='scale'):
    result = None
    non_scale_devices = ['slicer', 'boardtester', 'blackbox', 'telit', 'hdmiin',
                         'masterblaster', 'hflbProgrammer', 'coronaPsu', 'tdcc', 'testsrv']

    if device == 'scale' or device == 'ngw':
        result = True
        for device in non_scale_devices:
            if os.path.exists(os.path.join(live, "etc", "hobart", device)):
                result = False
                break

    elif device in non_scale_devices:
        result = False
        if os.path.exists(os.path.join(live, "etc", "hobart", device)):
            result = True
    return result

