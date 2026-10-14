#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Filename : status
# @Creation Date : 2019-12-03-13-51
# @Project: trunk
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis

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
        self.lookup_table[alert.code] = alert

    def get(self, code):
        return self.lookup_table[code]
