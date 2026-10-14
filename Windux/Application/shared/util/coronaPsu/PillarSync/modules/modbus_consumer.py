#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Filename : modbus_consumer
# @Creation Date : 2019-11-30-21-07
# @Project: coronaPsu
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis
# To Start Mosquito MQTT server run the following command:
# /usr/local/sbin/mosquitto -c /usr/local/etc/mosquitto/mosquitto.conf

import sys
sys.path.append('/usr/lib/python2.7/dist-packages')
sys.path.append('/usr/lib/python2.7/site-packages')
sys.path.append('/usr/local/hobart/python')

import logging
import logging.handlers
import os
import platform
import re
import shutil
import string
import struct
import sys
import time
from ConfigParser import SafeConfigParser
from signal import signal, SIGINT, SIGHUP, SIGTERM, SIGUSR1, SIGUSR2

from functools32 import lru_cache

import getMacAddress
import mqtt
import psu_data_db
import rs485
import shellCommand

# TODO
# Need to resolve google.protbuf package not installed for sparkplug
# before the import below can be uncommented
# import sparkplug

re_base_zero = re.compile(r"(?i)\s*[\+\-]?0(x)?")

@lru_cache(10)
def strtol_re(base):
    if not (0 <= base <= 36):
        raise ValueError('Expected base between 0 and 36.')
    a = r"(?i)\s*(?:[\+\-]?)"
    if base == 16:
        r = a + r"(?:0x)?[\u0030-\u0039\u0041-\u0046]*"
    elif base > 10:
        r = (a + "[\u0030-\u0039\u0041-{}]*".format(
            chr(ord('A') + base - 10)))
    else:
        r = (a + "[\u0030-{}]*".format(chr(ord('0') + base - 1)))
    return re.compile(r)


def get_sd_card_device_names():

    root_folder = None
    root_device = None

    if platform.system() == 'Linux':

        root_folder = "/media"
        root_device = os.readlink(os.path.join("/", "dev", "root"))

    elif platform.system() == 'Darwin':
        # used only for testing on mac os x
        root_folder = os.path.join("/", "Volumes", "Data", "media")
        root_device = "mmcblk0p1"

    internal_sd_card_device = os.path.join(root_folder, root_device)

    external_sd_card_device = None
    for mounted_device in os.listdir(root_folder):
        if "mmcblk" in mounted_device:
            if root_device[:7] not in mounted_device:
                external_sd_card_device = os.path.join(root_folder, mounted_device)
                break
    return internal_sd_card_device, external_sd_card_device


def get_application_location():
    return os.path.join("/", "usr", "local", "hobart", "coronaPsu")


def get_pid_file():
    pid = None
    try:
        with open(os.path.join(get_web2py_application_location(), 'modules', 'modbus_consumer.pid'), 'r') as pid_file:
            pid = int(pid_file.readline().strip())
    except:
        pass
    return pid


def stop_modbus_consumer():
    try:
        pid = get_pid_file()
        if pid is not None:
            os.kill(pid, SIGTERM)
        time.sleep(5.0)
    except:
        pass


def start_modbus_consumer():
    os.system("/usr/bin/python %s/modbus_consumer.py &" % (os.path.join(get_web2py_application_location(), "modules")))
    time.sleep(3.0)


def stop_communication_and_recording():
    try:
        pid = get_pid_file()
        if pid is not None:
            os.kill(pid, SIGUSR1)
        time.sleep(3.0)
    except:
        pass


def start_communication_and_recording():
    try:
        pid = get_pid_file()
        if pid is not None:
            os.kill(pid, SIGUSR2)
        time.sleep(3.0)
    except:
        pass


def get_web2py_application_location():
    internal_sd_card_device, external_sd_card_device = get_sd_card_device_names()
    web2py_folder = os.path.join(internal_sd_card_device, get_application_location(),
                                 "web2py", "applications", "PillarSync")
    return web2py_folder


def generate_pid_file():
    with open(os.path.join(get_web2py_application_location(), 'modules', 'modbus_consumer.pid'), 'w') as pid_file:
        pid_file.write("%d\n" % os.getpid())


def create_db_storage_links():
    internal_sd_card_device, external_sd_card_device = get_sd_card_device_names()
    web2py_folder = get_web2py_application_location()
    web2py_db_folder = os.path.join(web2py_folder, "databases")
    internal_pillarsync_db_folder = os.path.join(web2py_folder, "internal_database")
    external_pillarsync_db_folder = None

    if external_sd_card_device is not None:
        external_pillarsync_db_folder = os.path.join(external_sd_card_device, "PillarSync", "databases")

    # need to remove any non linked pillar sync db on the internal sd card
    # if the internal sd card pillar sync db exists
    # this is to start with a clean slate
    # removes symbolic link and any actual directories and files it pointed to
    if os.path.exists(web2py_db_folder):

        # if the the internal db folder exists and it not a link
        if os.path.isdir(web2py_db_folder) and not os.path.islink(web2py_db_folder):
            # remove the pillar sync folder completely since it should not be stored at that path
            # should be on the external SD card or at internal sd card db path
            shutil.rmtree(web2py_db_folder)

        # if databases path is linked to another hard path remove it
        if os.path.islink(web2py_db_folder):
            os.unlink(web2py_db_folder)

    # if an external sd card was mounted
    if external_sd_card_device is not None and os.path.exists(external_sd_card_device):
        # if no external pillar sync folder created on the sd card
        if not os.path.exists(external_pillarsync_db_folder):
            # make directory path so one exists to ultimately link to
            os.makedirs(external_pillarsync_db_folder)
        # create the pillar sync symbolic link on internal sd card to point to the
        # pillar sync storage folder on external sd card
        try:
            os.symlink(external_pillarsync_db_folder, web2py_db_folder)
        except:
            pass

    # else no external sd card mounted
    else:
        # if the internal sd card hard path does not exist then create it
        # to actuall store the db files
        if not os.path.exists(internal_pillarsync_db_folder):
            # make the directory path on internal sd card
            os.makedirs(internal_pillarsync_db_folder)
        # create the symoblic link to point to the internal sd card folder where the
        # db files are actually stored
        try:
            os.symlink(internal_pillarsync_db_folder, web2py_db_folder)
        except:
            pass


def is_process_running(process_id):
    try:
        os.kill(process_id, 0)
        return True
    except OSError:
        return False


def check_usd_card_integrity():
    response = None
    status = 'error'
    internal_sd_card_device, external_sd_card_device = get_sd_card_device_names()
    if external_sd_card_device is None:
        response = 'Micro SD card can not be found in external slot. Please insert a micro SD Card.'
    else:
        cmd = []
        cmd.append("/bin/findmnt")
        cmd.append("-n")
        cmd.append("-o")
        cmd.append("FSTYPE")
        cmd.append(external_sd_card_device)
        exitcode, stdout, stderr = shellCommand.execute(cmd)
        if exitcode == 0:
            if string.strip(stdout) != 'ext4':
                response = 'Inserted micro SD card has the wrong filesystem type.  Please format the micro SD card.'
            else:
                # See if modbus_consumer is running and if not run it
                if not is_process_running(get_pid_file()):
                    os.system("/usr/bin/python /usr/local/hobart/coronaPsu/web2py/applications/PillarSync/modules/modbus_consumer.py &")
                status = 'ok'
    return status, response


def format_usd_card(sd_card_device_name):
    command_suffix = ''
    # command_suffix = ' > %s/format_usd_card.log 2>&1' % os.path.join(get_web2py_application_location(), "modules")
    os.system('/bin/sh %s/format_usd_card.sh %s 32 %s' % (os.path.join(get_web2py_application_location(), "modules"),
                                                          sd_card_device_name,
                                                          command_suffix))
    os.system("/sbin/reboot")


class Strtol:
    """Small object to store result of conversion of string to int

    Arguments:
        s       : string to read
        base=10 : integer base between 0 and 36 inclusive.
        pos=0   : position where to read in the string.

    Strtol members:
        value   : an integer value parsed in the string.
        string  : the string that was read.
        pos     : the position where the integer was parsed.

        endpos  : the position in the string after the integer.

    Errors:
        If no valid conversion could be performed, ValueError is raised.

    see also:
        the linux manual of strtol
    """
    __slots__ = ('value', 'string', 'pos', 'endpos')

    def __init__(self, s, base=10, pos=0):
        if base == 0:
            m = re_base_zero.match(s, pos=pos)
            if m:
                base = 16 if m.group(1) else 8
            else:
                base = 10
        r = strtol_re(base)
        # print(r)
        # print(s[pos:])
        m = r.match(s, pos=pos)
        if m:
            self.value = int(m.group(0), base)
            self.string = s
            self.pos = pos
            self.endpos = m.end()
        else:
            raise ValueError('Cannot convert to int')


class ModbusManager:

    def __init__(self):

        signal(SIGINT, self.sigint_handler)
        signal(SIGHUP, self.sighup_handler)
        signal(SIGTERM, self.sigterm_handler)
        signal(SIGUSR1, self.sigusr1_handler)
        signal(SIGUSR2, self.sigusr2_handler)

        # change to default ad_service installation directory
        os.chdir(os.path.join(get_web2py_application_location(), "modules"))

        self.log_level = 'ERROR'
        self.sample_frequency = 30
        self.mqtt_enabled = False
        self.sparkplug_enabled = False
        self.broker_ip_address = '127.0.0.1'
        self.broker_port = 1883
        self.broker_username = None
        self.broker_password = None

        self.configuration = SafeConfigParser()
        self.read_configuration()

        self.log_filename = 'modbus_consumer.log'
        self._setup_enhanced_logging(self.log_filename)

        self.serial_modbus = rs485.ModbusASCII()
        self.serial_modbus.open()

        self.message = None

        self.internal_sd_card_device, self.external_sd_card_device = get_sd_card_device_names()
        create_db_storage_links()
        self.psu_database = psu_data_db.PsuDatabase(database_folder_location=os.path.join(
            get_web2py_application_location(),
            "databases"),
            internal_sd_card_device=self.internal_sd_card_device,
            external_sd_card_device=self.external_sd_card_device,
            connect_per_transaction=True)

        self.remote_addr_hi = 0x00
        self.remote_addr_lo = 0x01
        self.func_code_hi = 0x80
        self.func_code_lo = 0x00
        self.start_time = time.clock()
        self.number_psu_samples = 0

        self.psu_sample = {'power': {'min': 0.0, 'max': 0.0, 'total': 0.0},
                           'voltage': {'min': 0, 'max': 0, 'total': 0},
                           'current': {'min': 0, 'max': 0, 'total': 0},
                           'frequency': {'min': 0, 'max': 0, 'total': 0},
                           'fault': {'high_temp': {'previous': 0, 'current': 0},
                                     'interlock': {'previous': 0, 'current': 0},
                                     'dc_bus': {'previous': 0, 'current': 0},
                                     'overvolt': {'previous': 0, 'current': 0},
                                     'current_trip': {'previous': 0, 'current': 0},
                                     'inverter_on': {'previous': 0, 'current': 0}}}

        self.previous_psu_data = {'id': None,
                                  'created_on': None,
                                  'power': 0.0,
                                  'voltage': 0.0,
                                  'current': 0.0,
                                  'frequency': 0.0,
                                  'fault': {'high_temp': False,
                                            'interlock': False,
                                            'dc_bus': False,
                                            'overvolt': False,
                                            'current_trip': False,
                                            'inverter_on': False}}

        logging.info("modbus consumer PID is %d" % os.getpid())
        generate_pid_file()

        if self.mqtt_enabled:
            exit_code,  wired_mac_address = getMacAddress.getMacAddress(lanType='wire')

            if self.sparkplug_enabled:
                self.mqtt_client = sparkplug.Client(client_id=wired_mac_address,
                                                    group_id="pillarsync_gateways",
                                                    node_name='pillarsync_' + wired_mac_address,
                                                    device_name='corona_p6000',
                                                    reboot_callback=self.reboot_node)

            else:
                self.mqtt_client = mqtt.Client(client_id=wired_mac_address,
                                               group_id="pillarsync_gateways",
                                               node_name='pillarsync_' + wired_mac_address,
                                               device_name='corona_p6000')

    def reboot_node(self):
        os.system('/sbin/reboot')

    def read_configuration(self):
        self.configuration.read("modbus_consumer.cfg")
        self.log_level = self.configuration.get('MODBUS_CONSUMER', "logging_level")
        self.sample_frequency = \
            self.configuration.getint("MODBUS_CONSUMER",
                                      'sample_frequency')

        self.mqtt_enabled = self.configuration.getboolean("MQTT", "enabled")
        self.sparkplug_enabled = self.configuration.getboolean("MQTT", "sparkplug_enabled")
        self.broker_ip_address = self.configuration.get('MQTT', "broker_ip_address")

        self.broker_username = self.configuration.get('MQTT', "broker_username")
        if self.broker_username.strip() == '' or self.broker_username.lower() == 'none':
            self.broker_username = None

        self.broker_password = self.configuration.get('MQTT', "broker_password")
        if self.broker_password.strip() == '' or self.broker_password.lower() == 'none':
            self.broker_password = None

        self.broker_port = None
        try:
            self.broker_port = \
                self.configuration.getint("MQTT",
                                          'broker_port')
        except:
            pass

        if self.sample_frequency < 30:
            self.sample_frequency = 30

    def _setup_enhanced_logging(self,
                                filename,
                                maximum_file_size_bytes=5 * 1024 * 1024,
                                backup_logs=5):
        """
        Setup the python default logger to include additional elements in logged message
        as well as log backups which rotate when the maximum log size is reached
        :param filename: filename of the log file
        :param maximum_file_size_bytes: maximum size of log file before it is closed and put in rotation
        :param backup_logs: maximum number of backup logs maintained
        :return: None
        """

        log_levels = {'DEBUG': logging.DEBUG,
                      'INFO': logging.INFO,
                      'WARNING': logging.WARNING,
                      'ERROR': logging.ERROR,
                      'CRITICAL': logging.CRITICAL}

        try:
            logging_level = log_levels[self.log_level.upper()]
        except:
            logging_level = logging.DEBUG

        logging.basicConfig(filename=filename,
                            filemode='w',
                            format='[%(asctime)s:%(levelname)s:%(module)s:%(funcName)s:%(lineno)d]-%(message)s',
                            level=logging_level,
                            datefmt='%m/%d/%Y %I:%M:%S %p')

        logger = logging.getLogger('EnhancedLogger')

        # Add the log message handler to the logger
        handler = logging.handlers.RotatingFileHandler(
            filename, maxBytes=maximum_file_size_bytes, backupCount=backup_logs)

        logger.addHandler(handler)

    def convert_message_to_data_elements(self):
        status_code = 'error'
        psu_data = None

        try:
            psu_data = {'id': None,
                        'created_on': None,
                        'power': None,
                        'voltage': None,
                        'current': None,
                        'frequency': None,
                        'fault': {'high_temp': None,
                                  'interlock': None,
                                  'dc_bus': None,
                                  'overvolt': None,
                                  'current_trip': None,
                                  'inverter_on': None}}

            if len(self.message) >= 25:
                data_element = ''.join(self.message[1:3])
                remote_address = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % remote_address)

                data_element = ''.join(self.message[3:5])
                function_code = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % function_code)

                data_element = ''.join(self.message[5:9])
                power = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % power)
                psu_data['power'] = float(power) / 10.0

                data_element = ''.join(self.message[9:13])
                psu_data['voltage'] = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % psu_data['voltage'])
                psu_data['voltage'] = psu_data['voltage'] * 10

                data_element = ''.join(self.message[13:17])
                psu_data['current'] = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % psu_data['current'])

                data_element = ''.join(self.message[17:21])
                psu_data['frequency'] = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % psu_data['frequency'])

                data_element = ''.join(self.message[21:23])
                fault = Strtol(data_element, 16, 0).value
                logging.info("0x%04X" % fault)

                psu_data['fault']['high_temp'] = str(bool(fault & (1 << 0)))
                psu_data['fault']['interlock'] = str(bool(fault & (1 << 1)))
                psu_data['fault']['dc_bus'] = str(bool(fault & (1 << 2)))
                psu_data['fault']['overvolt'] = str(bool(fault & (1 << 3)))
                psu_data['fault']['current_trip'] = str(bool(fault & (1 << 4)))
                psu_data['fault']['inverter_on'] = str(bool(fault & (1 << 5)))
                status_code = 'ok'
        except:
            pass

        return status_code, psu_data

    def hex_to_ascii(self, hex_byte):
        send_packet = bytearray(2)
        ascii_byte = 0

        # move upper nibble to lower
        # byte and make ascii w / STOP bit
        ascii_byte = ((0xF0 & hex_byte) / 0x10) + 0xB0

        if ascii_byte > 0xB9:  # for A - F
            ascii_byte += 0x07
        send_packet[0] = ascii_byte

        ascii_byte = (0x0F & hex_byte) + 0xB0  # look at lower nibble, add 0x30 for ascii and 0x80 8th bit STOP

        if ascii_byte > 0xB9:  # for ascii A - F add 0x07 (A == 0x41)
            ascii_byte += 0x07

        send_packet[1] = ascii_byte  # put low ascii byte in buffer

        return send_packet

    def test_send_psu_data_message(self, psu_data):
        #  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22
        # 30 31 38 30 30 30 30 31 30 30 31 45 30 30 30 45 30 30 30 30 30 31
        message = bytearray(
            [self.serial_modbus.message_prefix] +
            #                            1  1  1  1  1  1  1  1  1  1  2  2  2
            # 1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9  0  1  2
            [ self.remote_addr_hi, self.remote_addr_lo, self.func_code_hi, self.func_code_lo,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ] +
            self.serial_modbus.message_terminators)

        # little endian unsigned short, i.MX6 ARA board is setup as little endian, need to verify with P6000
        little_endian = "<"
        unsigned_short = "H"
        pack_format = little_endian + unsigned_short
        message[5:9] = bytearray(struct.pack(pack_format, psu_data.power))
        message[9:13] = bytearray(struct.pack(pack_format, psu_data.voltage))
        message[13:17] = bytearray(struct.pack(pack_format, psu_data.current))
        message[17:21] = bytearray(struct.pack(pack_format, psu_data.frequency))
        faults = 0x0
        if psu_data['fault']['high_temp']:
            faults = faults | 0x1
        if psu_data['fault']['interlock']:
            faults = faults | 0x2
        if psu_data['fault']['dc_bus']:
            faults = faults | 0x4
        if psu_data['fault']['overvolt']:
            faults = faults | 0x8
        if psu_data['fault']['current_trip']:
            faults = faults | 0x10
        if psu_data['fault']['inverter_on']:
            faults = faults | 0x20
        little_endian = "<"
        unsigned_byte = "B"
        pack_format = little_endian + unsigned_byte
        message[21:22] = bytearray(struct.pack(pack_format, faults))
        return self.serial_modbus.write_message(message)

    def evaluate_min_max_avg_over_samples(self, psu_data=None, operation='clear'):

        if operation == 'clear':
            self.psu_sample = {'power': {'min': 0.0, 'max': 0.0, 'total': 0.0},
                               'voltage': {'min': 0, 'max': 0, 'total': 0},
                               'current': {'min': 0, 'max': 0, 'total': 0},
                               'frequency': {'min': 0, 'max': 0, 'total': 0},
                               'fault': {'high_temp': {'previous': 0, 'current': 0},
                                         'interlock': {'previous': 0, 'current': 0},
                                         'dc_bus': {'previous': 0, 'current': 0},
                                         'overvolt': {'previous': 0, 'current': 0},
                                         'current_trip': {'previous': 0, 'current': 0},
                                         'inverter_on': {'previous': 0, 'current': 0}}}

            self.number_psu_samples = 0

        elif operation == 'store':

            if psu_data is not None:
                self.number_psu_samples += 1
                if psu_data['power'] < self.psu_sample['power']['min']:
                    self.psu_sample['power']['min'] = psu_data['power']
                if psu_data['power'] > self.psu_sample['power']['max']:
                    self.psu_sample['power']['max'] = psu_data['power']
                self.psu_sample['power']['total'] += psu_data['power']

                if psu_data['voltage'] < self.psu_sample['voltage']['min']:
                    self.psu_sample['voltage']['min'] = psu_data['voltage']
                if psu_data['voltage'] > self.psu_sample['voltage']['max']:
                    self.psu_sample['voltage']['max'] = psu_data['voltage']
                self.psu_sample['voltage']['total'] += psu_data['voltage']

                if psu_data['frequency'] < self.psu_sample['frequency']['min']:
                    self.psu_sample['frequency']['min'] = psu_data['frequency']
                if psu_data['frequency'] > self.psu_sample['frequency']['max']:
                    self.psu_sample['frequency']['max'] = psu_data['frequency']
                self.psu_sample['frequency']['total'] += psu_data['frequency']

                if psu_data['current'] < self.psu_sample['current']['min']:
                    self.psu_sample['current']['min'] = psu_data['current']
                if psu_data['current'] > self.psu_sample['current']['max']:
                    self.psu_sample['current']['max'] = psu_data['current']
                self.psu_sample['current']['total'] += psu_data['current']

                if psu_data['fault']['high_temp'] == 'True':
                    if self.psu_sample['fault']['high_temp']['previous'] == 0:
                        self.psu_sample['fault']['high_temp']['current'] += 1

                if psu_data['fault']['interlock'] == 'True':
                    if self.psu_sample['fault']['interlock']['previous'] == 0:
                        self.psu_sample['fault']['interlock']['current'] += 1

                if psu_data['fault']['dc_bus'] == 'True':
                    if self.psu_sample['fault']['dc_bus']['previous'] == 0:
                        self.psu_sample['fault']['dc_bus']['current'] += 1

                if psu_data['fault']['overvolt'] == 'True':
                    if self.psu_sample['fault']['overvolt']['previous'] == 0:
                        self.psu_sample['fault']['overvolt']['current'] += 1

                if psu_data['fault']['current_trip'] == 'True':
                    if self.psu_sample['fault']['current_trip']['previous'] == 0:
                        self.psu_sample['fault']['current_trip']['current'] += 1

                if psu_data['fault']['inverter_on'] == 'True':
                    if self.psu_sample['fault']['inverter_on']['previous'] == 0:
                        self.psu_sample['fault']['inverter_on']['current'] += 1

                self.psu_sample['fault']['high_temp']['previous'] = psu_data['fault']['high_temp']
                self.psu_sample['fault']['interlock']['previous'] = psu_data['fault']['interlock']
                self.psu_sample['fault']['dc_bus']['previous'] = psu_data['fault']['dc_bus']
                self.psu_sample['fault']['overvolt']['previous'] = psu_data['fault']['overvolt']
                self.psu_sample['fault']['current_trip']['previous'] = psu_data['fault']['current_trip']
                self.psu_sample['fault']['inverter_on']['previous'] = psu_data['fault']['inverter_on']

        elif operation == 'result':
            psu_data = {'id': None,
                        'created_on': None,
                        'power': self.psu_sample['power']['total'] / self.number_psu_samples,
                        'voltage': self.psu_sample['voltage']['total'] / self.number_psu_samples,
                        'current': self.psu_sample['current']['total'] / self.number_psu_samples,
                        'frequency': self.psu_sample['frequency']['total'] / self.number_psu_samples,
                        'fault': {'high_temp': self.psu_sample['fault']['high_temp']['current'],
                                  'interlock': self.psu_sample['fault']['interlock']['current'],
                                  'dc_bus': self.psu_sample['fault']['dc_bus']['current'],
                                  'overvolt': self.psu_sample['fault']['overvolt']['current'],
                                  'current_trip': self.psu_sample['fault']['current_trip']['current'],
                                  'inverter_on': self.psu_sample['fault']['inverter_on']['current']}}
            return psu_data

    def set_fault_value(self, fault_status):
        if fault_status:
            return 'on'
        else:
            return 'off'

    def mqtt_publish_data(self, psu_data):

        if psu_data['power'] != self.previous_psu_data['power']:
            self.mqtt_client.publish(topic="corona_p6000/power", payload=psu_data['power'])
            self.previous_psu_data['power'] = psu_data['power']

        if psu_data['voltage'] != self.previous_psu_data['voltage']:
            self.mqtt_client.publish(topic="corona_p6000/voltage", payload=psu_data['voltage'])
            self.previous_psu_data['voltage'] = psu_data['voltage']

        if psu_data['frequency'] != self.previous_psu_data['frequency']:
            self.mqtt_client.publish(topic="corona_p6000/frequency", payload=psu_data['frequency'])
            self.previous_psu_data['frequency'] = psu_data['frequency']

        if psu_data['current'] != self.previous_psu_data['current']:
            self.mqtt_client.publish(topic="corona_p6000/current", payload=psu_data['current'])
            self.previous_psu_data['current'] = psu_data['current']

        if psu_data['fault']['high_temp'] != self.previous_psu_data['fault']['high_temp']:
            self.mqtt_client.publish(topic="corona_p6000/fault/high_temp", payload=psu_data['fault']['high_temp'])
            self.previous_psu_data['fault']['high_temp'] = psu_data['fault']['high_temp']

        if psu_data['fault']['interlock'] != self.previous_psu_data['fault']['interlock']:
            self.mqtt_client.publish(topic="corona_p6000/fault/interlock", payload=psu_data['fault']['interlock'])
            self.previous_psu_data['fault']['interlock'] = psu_data['fault']['interlock']

        if psu_data['fault']['dc_bus'] != self.previous_psu_data['fault']['dc_bus']:
            self.mqtt_client.publish(topic="corona_p6000/fault/dc_bus", payload=psu_data['fault']['dc_bus'])
            self.previous_psu_data['fault']['dc_bus'] = psu_data['fault']['dc_bus']

        if psu_data['fault']['overvolt'] != self.previous_psu_data['fault']['overvolt']:
            self.mqtt_client.publish(topic="corona_p6000/fault/overvolt", payload=psu_data['fault']['overvolt'])
            self.previous_psu_data['fault']['overvolt'] = psu_data['fault']['overvolt']

        if psu_data['fault']['current_trip'] != self.previous_psu_data['fault']['current_trip']:
            self.mqtt_client.publish(topic="corona_p6000/fault/current_trip", payload=psu_data['fault']['current_trip'])
            self.previous_psu_data['fault']['current_trip'] = psu_data['fault']['current_trip']

        if psu_data['fault']['inverter_on'] != self.previous_psu_data['fault']['inverter_on']:
            self.mqtt_client.publish(topic="corona_p6000/fault/inverter_on", payload=psu_data['fault']['inverter_on'])
            self.previous_psu_data['fault']['inverter_on'] = psu_data['fault']['inverter_on']

    def handle_communication(self):

        self.start_time = time.time()
        mqtt_server_connected = False

        while True:
            if self.serial_modbus.device_is_open():
                # print "Reading Serial port"
                status_code, self.message = self.serial_modbus.read_message()
                # print self.message
                if status_code == 'ok':
                    logging.debug("Convert modbus ASCII message to data elements")
                    status_code, psu_data = self.convert_message_to_data_elements()
                    # print psu_data
                    if status_code == 'ok':
                        if self.mqtt_enabled:
                            if not self.mqtt_client.broker_is_connected():
                                if self.sparkplug_enabled:
                                    initial_topic_values = {'power': psu_data['power'],
                                                            'current': psu_data['current'],
                                                            'frequency': psu_data['frequency'],
                                                            'voltage': psu_data['voltage'],
                                                            'fault': {'high_temp': psu_data['fault']['high_temp'],
                                                                      'interlock': psu_data['fault']['interlock'],
                                                                      'dc_bus': psu_data['fault']['dc_bus'],
                                                                      'overvolt': psu_data['fault']['overvolt'],
                                                                      'current_trip': psu_data['fault']['current_trip'],
                                                                      'inverter_on': psu_data['fault']['inverter_on']}}
                                    self.mqtt_client.connect(self.broker_ip_address,
                                                             self.broker_username,
                                                             self.broker_password,
                                                             self.broker_port,
                                                             initial_topic_values)
                                else:

                                    self.mqtt_client.connect(self.broker_ip_address,
                                                             self.broker_username,
                                                             self.broker_password,
                                                             self.broker_port)

                            else:
                                self.mqtt_publish_data(psu_data)

                        logging.debug("Evaluating min, max, average over sample time for data")
                        self.evaluate_min_max_avg_over_samples(psu_data=psu_data, operation='store')
                        # print 'time elapsed %f' % (time.time() - self.start_time)
                        if (time.time() - self.start_time) >= float(self.sample_frequency):
                            logging.debug('writing record %f' % (time.time() - self.start_time))
                            psu_data = self.evaluate_min_max_avg_over_samples(psu_data, operation='result')
                            print psu_data
                            logging.debug("storing serial data into db")
                            self.psu_database.insert(psu_data)
                            self.start_time = time.time()
                            self.evaluate_min_max_avg_over_samples(operation='clear')

            else:
                time.sleep(1.0)

    def stop_communication_and_recording(self):
        try:
            # logging.debug("Stopping communication and recording")
            # logging.debug("Closing serial port")
            self.serial_modbus.close()
        except:
            pass

        try:
            # logging.debug("Closing db connection")
            self.psu_database.database_disconnect()
        except:
            pass

        try:
            # logging.debug("Resetting sample evaluation data")
            self.evaluate_min_max_avg_over_samples(psu_data=None, operation='clear')
        except:
            pass

        if self.mqtt_enabled:
            try:
                self.mqtt_client.disconnect()
            except:
                pass

    def start_communication_and_recording(self):
        # logging.debug("Starting communication and database recording")
        # logging.debug("Opening serial port")
        self.serial_modbus.open()
        # create database if it does not already exist
        # logging.debug("Creating database if needed")
        self.psu_database.create()
        # logging.debug("Start handling serial port communication and db storage")
        self.handle_communication()

    def sighup_handler(self, signal_received, frame):
        # commit and close current database transaction if any pending
        self.stop_communication_and_recording()
        # re-read configuration file
        self.read_configuration()
        # start handling serial communications again since it was interrupted
        self.start_communication_and_recording()

    def sigusr1_handler(self, signal_received, frame):
        # logging.debug("SIGUSR1 signal received")
        self.stop_communication_and_recording()

    def sigusr2_handler(self, signal_received, frame):
        # logging.debug("SIGUSR2 signal received")
        self.start_communication_and_recording()

    def sigint_handler(self, signal_received, frame):
        self.stop_communication_and_recording()
        sys.exit(0)

    def sigterm_handler(self, signal_received, frame):
        self.stop_communication_and_recording()
        sys.exit(0)


if __name__ == '__main__':
    modbus = ModbusManager()
    modbus.handle_communication()
    modbus.stop_communication_and_recording()
