#!/usr/bin/env python
# -*- coding: utf-8 -*-

import filebox
import argparse

if __name__ == '__main__':
    command_parser = argparse.ArgumentParser(description='Remove all media files in the Filebox server')
    args = command_parser.parse_args()
    response = filebox.remove_all_files()
    print response
