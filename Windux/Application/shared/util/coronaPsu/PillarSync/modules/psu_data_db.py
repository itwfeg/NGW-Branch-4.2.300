#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Filename : database
# @Creation Date : 2019-12-03-13-20
# @Project: trunk
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis

import os
import logging
from datetime import datetime, timedelta
from pydal import DAL, Field
import platform
import shutil


def sql_datetime_now():
    return datetime.now().strftime('%Y-%m-%d %H:%M:%S')


def sql_datetime_hours_ago(hours_in_past, initial_time=datetime.now()):
    two_days_ago = initial_time - timedelta(hours=hours_in_past)
    return two_days_ago.strftime('%Y-%m-%d %H:%M:%S')


class PsuDatabase(object):

    def __init__(self,
                 database_folder_location,
                 internal_sd_card_device,
                 external_sd_card_device,
                 connect_per_transaction=True):

        self.internal_sd_card_device = internal_sd_card_device
        self.external_sd_card_device = external_sd_card_device

        self.web2py_db_folder = database_folder_location
        self.web2py_db_filename = 'storage.sqlite'
        self.web2py_db_file_path = os.path.join(self.web2py_db_folder, self.web2py_db_filename)

        self.connect_per_transaction = connect_per_transaction
        self.connection = None
        self.current_db_connection = self.connection

        if not connect_per_transaction:
            self.connection = self.database_connect()

        # create pillar sync sqlite db if it does not exist
        self.create()

    def database_connect(self):
        try:
            connection = DAL("sqlite://%s" % self.web2py_db_filename, folder=self.web2py_db_folder,
                             pool_size=0, auto_import=True)
            self.current_db_connection = connection
            return connection
        except:
            return None

    def database_disconnect(self, connection=None):
        if connection is None:
            try:
                # used to close any pending database transactions interrupted by a signal which
                # occur in caller where database transaction commit and close status would be unknown
                self.current_db_connection.commit()
                self.current_db_connection.close()
            except:
                pass
        else:
            connection.close()

    def insert(self, psu_data):

        connection = self.connection

        if psu_data['created_on'] is None:
            psu_data['created_on'] = sql_datetime_now()

        if self.connect_per_transaction:
            connection = self.database_connect()

        connection.psu_data.insert(
            created_on=psu_data['created_on'],
            power_level=psu_data['power'],
            voltage_level=psu_data['voltage'],
            current_level=psu_data['current'],
            frequency_level=psu_data['frequency'],
            high_temp_fault=psu_data['fault']['high_temp'],
            interlock_fault=psu_data['fault']['interlock'],
            dc_bus_fault=psu_data['fault']['dc_bus'],
            overvolt_fault=psu_data['fault']['overvolt'],
            current_trip_fault=psu_data['fault']['current_trip'],
            inverter_on_fault=psu_data['fault']['inverter_on'])

        connection.commit()

        if self.connect_per_transaction:
            self.database_disconnect(connection)

    def create(self):

        # only create a new database if one does not already exist in root database path
        if not os.path.exists(self.web2py_db_file_path):

            logging.info("INFO: database named %s does not exist, creating database" % self.web2py_db_file_path)

            connection = self.connection

            if self.connect_per_transaction:
                connection = self.database_connect()

            connection.define_table('psu_data',
                                    Field('created_on', label='Created On', type='datetime', notnull=True,
                                          writable=False,
                                          readable=True),
                                    Field('power_level', label='Power (Watts)', type='double'),
                                    Field('voltage_level', label='Voltage (Volts)', type='integer'),
                                    Field('current_level', label='Current (Amps)', type='integer'),
                                    Field('frequency_level', label='Frequency (kHz)', type='integer'),
                                    Field('high_temp_fault', label='High Temp Fault', type='boolean'),
                                    Field('interlock_fault', label='Interlock Fault', type='boolean'),
                                    Field('dc_bus_fault', label='DC Bus Fault', type='boolean'),
                                    Field('overvolt_fault', label='Overvolt Fault', type='boolean'),
                                    Field('current_trip_fault', label='Current Trip Fault', type='boolean'),
                                    Field('inverter_on_fault', label='Inverter On Fault', type='boolean'))

            connection.executesql("CREATE INDEX IF NOT EXISTS CREATED_ON_INDEX ON PSU_DATA (CREATED_ON);")

            connection.commit()

            if self.connect_per_transaction:
                self.database_disconnect(connection)


if __name__ == '__main__':
    import random
    import time
    from datetime import date, timedelta
    corona_db = PsuDatabase()
    simulated_date_time = datetime.today() - timedelta(30)
    for i in range(0, 36000):
        power_level = float(random.randrange(0,101)) + float(random.randrange(0,11)) * 0.01
        voltage_level = random.randrange(0,231)
        frequency_level = random.randrange(0,100)
        current_level = random.randrange(0,31)
        high_temp_fault = random.randrange(0,2)
        interlock_fault = random.randrange(0,2)
        dc_bus_fault = random.randrange(0,2)
        overvolt_fault = random.randrange(0,2)
        current_trip_fault = random.randrange(0, 2)
        inverter_on_fault = random.randrange(0, 2)

        if i % 6 == 0:
            simulated_date_time = simulated_date_time + timedelta(minutes=1)
        if i % (6 * 60) == 0:
            simulated_date_time = simulated_date_time + timedelta(hours=1)
        elif i % (24*60) == 0:
            simulated_date_time = simulated_date_time + timedelta(days=1)
        else:
            simulated_date_time = simulated_date_time + timedelta(seconds=10)

        psu_data = {'id': None,
                    'created_on': simulated_date_time,
                    'power': power_level,
                    'frequency': frequency_level,
                    'voltage': voltage_level,
                    'current': current_level,
                    'fault': {'high_temp': high_temp_fault,
                              'interlock': interlock_fault,
                              'dc_bus': dc_bus_fault,
                              'overvolt': overvolt_fault,
                              'current_trip': current_trip_fault,
                              'inverter_on': inverter_on_fault}}

        corona_db.insert(psu_data)

