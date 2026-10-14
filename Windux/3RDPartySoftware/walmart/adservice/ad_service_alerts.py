#!/usr/bin/env python
# -*- coding: utf-8 -*-

import status
import logging


class Alerts:

    WEB_SERVICE_REQUEST_SUCCESS = 200
    WEB_SERVICE_REQUEST_BAD = 400
    WEB_SERVICE_REQUEST_UNAUTHORIZED = 401
    WEB_SERVICE_REQUEST_FORBIDDEN = 403
    WEB_SERVICE_REQUEST_NOT_FOUND = 404
    WEB_SERVICE_REQUEST_INTERNAL_SERVER_ERROR = 500

    def __init__(self, log_facility, alerts_table_filename='ad_service_alerts.dat', ):
        """
        Alerts table lookup
        """

        try:

            self.log_facility = log_facility
            inFile = open(alerts_table_filename, 'r')

            alert_lines = inFile.readlines()
            inFile.close()

            self.status_alerts = status.AlertTable()

            for current_alert_line in alert_lines:
                line = current_alert_line.rstrip('\r\n')
                line = line.lstrip()
                if len(line) > 0:
                    if line[0] != "#":
                        tokens = line.split(":")
                        try:
                            self.status_alerts.add(status.Alert(int(tokens[0]), int(tokens[1]), tokens[2]))
                        except:
                            self.log_facility.error('Invalid alert status line %s in file %s' % (line,
                                                                                                 alerts_table_filename))

        except IOError, exception_details:
            logging.error('Can not open file: ' + alerts_table_filename + ', Reason:' + exception_details[1])

    def lookup(self, code):
        """
        Lookup the status alert by code and return an Alert
        :param code: status code to lookup
        :return: status alert
        """
        alert = None
        try:
            alert = self.status_alerts.get(code)
        except:
            self.log_facility.error('Invalid status code specified, check %d to see if code is in file' % code)

        return alert
