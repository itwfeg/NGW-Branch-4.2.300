# -*- coding: utf-8 -*-

# not tested in python3

import sys
import traceback
import os

from datetime import datetime

KILOBYTES = 1000
MEGABYTES = 1000 * KILOBYTES

class Logger(object):
    ## Logger.ALL: All Log messages.
    ALL = 1
    ## Logger.INFO: Info Log Messages.
    INFO = 2
    ## Logger.WARNING: Warning Log Messages.
    WARNING = 3
    ## Logger.ERROR: Error Log Messages.
    ERROR = 4

    def __init__(self, log_file_path, printing=False, maximum_size = 500 * KILOBYTES):
        self.log_file_path = log_file_path

        self._maximum_file_size = maximum_size
        self.printing = printing

    def __str__(self):
        return 'SMBackend Logger @ ' + self.log_file_path

    def __repr__(self):
        return 'Logger(log_file_path=%s, printing=%s)' % (self.log_file_path, self.printing)

    ##Create a new log message, and optionally write it to stdout as well.
    #
    #  @param status:
    #  A string, either 'info', 'warning', or 'error' determining the level of logging.
    def log(self, status, *parts):
        log_message = self.format_log(status, *parts)
        self._write(log_message)

        if self.printing:
            ##If they log an error, print to stderr, otherwise stdout
            if 'err' in status:
                sys.stderr.write(log_message)
            else:
                print(log_message)

    def log_exc(self, exc):
        traceback_list = traceback.format_list(traceback.extract_tb(sys.exc_info()[2]))
        traceback_list.insert(0, str(exc))

        traceback_string = '\n'.join(traceback_list)

        self.log('error', traceback_string)

    ##Returns all logged messages from the current file, in order of recency.
    #
    #  @param error_level:
    #  The minimum error level of messages to show (Defaults to Logger.ALL).
    #  See: Logger.INFO, Logger.WARNING, Logger.ERROR, Logger.ALL
    #
    #  @return:
    #  A list of the filtered log messages.
    def get_log(self, error_level=None):
        if not error_level:
            error_level = Logger.ALL

        with open(self.log_file_path, 'rb') as log_file:
            log_messages = log_file.readlines()

        log_messages.reverse()
        if error_level != Logger.ALL:
            ##Filter out the unwwanted messages based on the given error level.
            log_messages = self._filter_log(log_messages, error_level)
        return log_messages

    def _filter_log(self, log_messages, error_level):
        filtered_messages = []
        for log_message in log_messages:
            this_error_level = self._get_error_level(log_message)
            if this_error_level >= error_level:
                filtered_message.append(log_message)

        return filtered_messages

    def _get_error_level(self, log_message):
        #Split the message by ] and take the first part, as it will be our status.
        status_part = log_message.split(']')[0]
        #Split the status part by - and take the first part, as the time stamp will be
        #after that.
        status_part = status_part.split('-')[0]
        #Strip off the leading [, and convert that to uppercase in case it isn't,
        #so we don't get confused.
        status_part = status_part.strip('[').upper()

        if not hasattr(self, status_part):
            self.log('error', 'Could not determine error level for log message')
            return Logger.ALL

        error_level = getattr(self, status_part)
        return error_level

    def format_log(self, status, *parts):
        return self._format_status(status) + ': ' + self._format_message(*parts)

    def _format_message(self, *parts):
        message =' '.join(parts)
        return message

    def _format_status(self, status):
        time_stamped_status = self._time_stamp_status(status)
        return '[%s]' % time_stamped_status.upper()

    def _time_stamp_status(self, status):
        return '%s-%s' % (status, datetime.now())

    def _trim_log(self, num_lines):
        with open(self.log_file_path, 'rb') as log_file:
            trimmed_log = log_file.readlines()

        trimmed_log = trimmed_log[num_lines:]

        with open(self.log_file_path, 'wb') as log_file:
            log_file.writelines(trimmed_log)

    def _ensure_log_size(self, format_message):
        #We haven't written to the log yet, so just return.
        if not os.path.exists(self.log_file_path):
            return

        file_size = os.path.getsize(self.log_file_path)
        if file_size >= self._maximum_file_size:
            self._trim_log(num_lines = 5)

    def _write(self, format_message):
        self._ensure_log_size(format_message)

        with open(self.log_file_path, 'a') as log_file:
            log_file.write(format_message + '\n')
