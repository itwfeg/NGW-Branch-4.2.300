#!/usr/bin/env python3
# -*- coding: utf-8 -*-

""" This script takes a command line argument for a folder path, and executes code specified in DAEMON= in all .config files in said folder.

    Author: Ian Davis
    Last Updated: July 1, 2015
"""

import argparse
import glob
import os
import sys


def find_config_files(path):
    """ Find all files in the given path that end in .conf.

        :param path: A path to the folder to search.
        :return: A list of filenames that are config files.
    """
    return glob.glob(os.path.join(path, '*.conf'))


def find_daemon_statement(filepath):
    """ Find a line in the given filepath that starts with "DAEMON=" and return the statement that follows, stripped of quotes.

        :param filepath: The path to the config file to read and search.
        :return: The statement that follows the DAEMON= line, stripped of quotes.
    """
    with open(filepath, 'r') as config_file:
        for line in config_file:
            if line.startswith('DAEMON_SHUTDOWN='):
                return line.strip().replace('DAEMON_SHUTDOWN=', '').strip('"')

    raise IOError("No DAEMON execution line found in config file {0}".format(filepath))


def main():
    """ Entry point of the application, parses command line arguments and contains the main application logic.

        :return: int exitcode 0
    """
    parser = argparse.ArgumentParser(description="Execute Daemon lines from folder.")
    parser.add_argument('--folder', action='store', help='Path to the folder to search.')

    args = parser.parse_args()
    
    if os.path.exists(args.folder):
        config_filepaths = find_config_files(args.folder)
        for config_filepath in config_filepaths:
            try:
                daemon_statement = find_daemon_statement(config_filepath)
            except IOError as err:
                print((str(err)))
                continue

            os.system(daemon_statement)

    return 0

if __name__ == '__main__':
    sys.exit(main())
