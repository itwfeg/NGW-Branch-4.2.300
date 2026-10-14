#!/usr/bin/env python
# -*- coding: utf-8 -*-

OK = 0
INFO = 1
WARNING = 2
ERROR = 3
FATAL = 4


class Alert(object):

    def __init__(self, code, severity, message):
        self._code = code
        self._severity = severity
        self._message = message

    @property
    def code(self):
        return self._code

    @code.setter
    def code(self, value):
        self._code = value

    @property
    def severity(self):
        return self._severity

    @severity.setter
    def severity(self, value):
        self._severity = value

    @property
    def message(self):
        return self._message

    @message.setter
    def message(self, value):
        self._message = value


class AlertTable(object):

    def __init__(self):
        self.lookup_table = {}

    def add(self, alert):
        """
        Adds an alert to the lookup table
        :param alert: alert object to add
        :return: None
        """
        self.lookup_table[alert.code] = alert

    def get(self, code):
        """
        Get Alert object based on status code
        :param code: status code
        :return: Alert object found
        """
        return self.lookup_table[code]
