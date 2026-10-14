#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import filebox

if __name__ == '__main__':
    command_parser = argparse.ArgumentParser(description='Restore a specified media file in the Filebox server')
    command_parser.add_argument("directory_number",
                                action="store",
                                type=int,
                                help='Directory number in Filebox server to restore, integer value')
    command_parser.add_argument("filename",
                                action="store",
                                type=str,
                                help="Filename in Filebox server to restore")
    args = command_parser.parse_args()
    response = filebox.restore_file(args.directory_number, args.filename)
    print response
