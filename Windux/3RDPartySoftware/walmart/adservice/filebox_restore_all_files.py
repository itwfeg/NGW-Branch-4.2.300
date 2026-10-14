#!/usr/bin/env python
# -*- coding: utf-8 -*-

import filebox
import argparse

if __name__ == '__main__':
    command_parser = argparse.ArgumentParser(description='Restore all test media files in the Filebox server')
    args = command_parser.parse_args()
    response = filebox.restore_all_files()
    print response
