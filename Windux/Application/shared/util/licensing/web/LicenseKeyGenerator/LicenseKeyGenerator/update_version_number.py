#!/usr/bin/python
from __future__ import print_function

import sys
import os
import inspect

sys.path.append('/var/www/django/LicenseKeyGenerator/')

import LicenseKeyGenerator

from datetime import datetime


def main():
    version_number = int(LicenseKeyGenerator.VERSION.replace('.', ''))
    version_number += 1

    raw_version = str(version_number).zfill(3)
    dotted_version = '.'.join(raw_version)

    now = datetime.now()
    last_updated = now.strftime('%m/%d/%Y %I:%M %p')

    module_file = os.path.abspath(inspect.getfile(LicenseKeyGenerator))

    with open(module_file, 'w') as module_file:
        module_file.write('LAST_UPDATED = "' + last_updated + '"\n')
        module_file.write('VERSION = "' + dotted_version + '"\n')

if __name__ == '__main__':
    main()
