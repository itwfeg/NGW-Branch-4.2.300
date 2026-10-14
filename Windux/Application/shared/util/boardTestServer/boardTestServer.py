#!/usr/bin/env python3

import configparser
import logging
import logging.handlers
import os
import socket
import time
import select

import board_info_recorder

INSTALLATION_DIRECTORY = "/home/pi/test_station_server"
SERVICE_DIRECTORY = INSTALLATION_DIRECTORY

BOARD_TEST_SERVER_IP_ADDRESS = 'localhost'
BOARD_TEST_SERVER_PORT = 5081
BOARD_TEST_SERVER_TIMEOUT = 3.0


def get_pid_file():
    pid = None
    try:
        with open('%s/board_test_server.pid' % SERVICE_DIRECTORY, 'r') as pid_file:
            pid = int(pid_file.readline().strip())
    except:
        pass
    return pid


def is_board_test_server_running():
    if get_pid_file() is None:
        return False
    else:
        return True


def send_message_with_response(board_test_server_ip_address=BOARD_TEST_SERVER_IP_ADDRESS,
                               board_test_server_port=BOARD_TEST_SERVER_PORT,
                               board_test_server_timeout=BOARD_TEST_SERVER_TIMEOUT,
                               message='',
                               status_in_response=False):

    server_address = (board_test_server_ip_address, board_test_server_port)
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.settimeout(board_test_server_timeout)

    try:
        connection.connect(server_address)
    except:
        return ''

    connection.sendall(message.encode("utf-8"))
    connection.settimeout(board_test_server_timeout)

    try:
        current_response = connection.recv(1024).decode("utf-8")

        if status_in_response:
            while 'error' not in current_response and 'done' not in current_response:
                current_response += connection.recv(1024).decode("utf-8")

                if not current_response:

                    raise ValueError('No response received from socket, quitting')

                time.sleep(0.1)
        else:
            return current_response

    finally:
        logging.warning('Message %s with no response received from Board Test Server' % message)
        connection.close()


def restart_board_test_server():
    status_code = os.system("/usr/bin/sudo /etc/init.d/board_test_server.sh restart")
    time.sleep(1.0)
    return status_code


def start_board_test_server():
    status_code = os.system("/usr/bin/sudo /etc/init.d/board_test_server.sh start")
    time.sleep(1.0)
    return status_code

    # cmd = ['/usr/bin/python3', '%s/boardTestServer.py' % INSTALLATION_DIRECTORY, '&']
    # self.board_test_server = subprocess.Popen(cmd, stdin=None, stdout=None, stderr=None, close_fds=True)


def status_board_test_server():
    response = send_message_with_response(
        board_test_server_ip_address=BOARD_TEST_SERVER_IP_ADDRESS,
        board_test_server_port=BOARD_TEST_SERVER_PORT,
        board_test_server_timeout=BOARD_TEST_SERVER_TIMEOUT,
        message="ping\n",
        status_in_response=False)
    if response.strip() != 'on-line':
        return 'stopped'
    else:
        return 'running'


def stop_board_test_server():
    status_code = os.system("/usr/bin/sudo /etc/init.d/board_test_server.sh stop")
    time.sleep(1.0)
    return status_code

    # try:
    #     pid = get_pid_file()
    #     if pid is not None:
    #         os.kill(pid, signal.SIGTERM)
    #     time.sleep(1.0)
    # except:
    #     pass


class Manager:

    def __init__(self):

        self.mac_address_pool = {
            0: {'start': None,
                'end': None,
                'current': None},
            1: {'start': None,
                'end': None,
                'current': None},
            2: {'start': None,
                'end': None,
                'current': None},
            3: {'start': None,
                'end': None,
                'current': None},
            4: {'start': None,
                'end': None,
                'current': None},
            5: {'start': None,
                'end': None,
                'current': None},
            6: {'start': None,
                'end': None,
                'current': None},
            7: {'start': None,
                'end': None,
                'current': None},
            8: {'start': None,
                'end': None,
                'current': None},
            9: {'start': None,
                'end': None,
                'current': None},
            10: {'start': None,
                 'end': None,
                 'current': None},
            11: {'start': None,
                 'end': None,
                 'current': None},
            12: {'start': None,
                 'end': None,
                 'current': None},
            13: {'start': None,
                 'end': None,
                 'current': None},
            14: {'start': None,
                 'end': None,
                 'current': None},
            15: {'start': None,
                 'end': None,
                 'current': None}

        }

        self.board_test_server_configuration_file = '%s/board_test_server.ini' % INSTALLATION_DIRECTORY
        self.configuration = configparser.ConfigParser()
        self.configuration.read(self.board_test_server_configuration_file)

        self.log_level = self.configuration.get('GENERAL', "logging_level")

        log_filename = os.path.join(SERVICE_DIRECTORY, "board_test_server.log")

        self._setup_enhanced_logging(log_filename)

        self.port = self.configuration.getint("GENERAL", "port")
        self.organization_unique_identifier = self.configuration.get("GENERAL",
                                                                     "organization_mac_address_prefix")

        for pool_number in self.mac_address_pool.keys():
            if self.configuration.has_option('MAC_ADDRESS_POOLS', 'pool_%d_start' % pool_number):
                self.mac_address_pool[pool_number]['start'] = \
                    self.configuration.get('MAC_ADDRESS_POOLS', 'pool_%d_start' % pool_number)
                self.mac_address_pool[pool_number]['end'] = \
                    self.configuration.get('MAC_ADDRESS_POOLS', 'pool_%d_end' % pool_number)
                self.mac_address_pool[pool_number]['current'] = \
                    self.configuration.get('MAC_ADDRESS_POOLS', 'pool_%d_current' % pool_number)
            pool_number += 1

        self.board_info_recorder = board_info_recorder.DatabaseRecorder('%s/board_test_server.sqlite' %
                                                                        INSTALLATION_DIRECTORY)

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET,  socket.SO_REUSEADDR, 1)

        self.server_address = ('', self.port)

        print('Board Test Server %s port %s' % self.server_address)

        self.server.bind(self.server_address)
        self.server.listen(20)
        self.shutdown = False

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

    def generate_pid_file(self):
        with open('%s/board_test_server.pid' % SERVICE_DIRECTORY, 'w') as pid_file:
            pid_file.write("%d\n" % os.getpid())

    def send_message(self, client, message):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_address = (client, self.port)
            # print >>sys.stderr, 'connecting to %s port %s' % client_address
            sock.connect(client_address)
            message = message + '\n'
            sock.sendall(message.encode("utf-8"))
            sock.close()
            return True
        except:
            return False

    # retrieve next MAC address
    # there are 16 different pools, '0'-'F'
    def get_next_mac_address(self, pool=0):

        # validate MAC range
        next_mac_address = self.mac_address_pool[pool]['current'].strip()
        last_mac_address = self.mac_address_pool[pool]['end'].strip()

        if next_mac_address > last_mac_address or next_mac_address[-6:] == 'FFFFFF':
            return "ERROR: Out of MAC Addresses, update the configuration file on board test server"

        # generate MAC for next iteration and update file
        incremented_mac_address = "{:012X}".format(int(next_mac_address, 16) + 1)

        self.configuration.set('MAC_ADDRESS_POOLS', 'pool_%d_current' % pool, incremented_mac_address)

        self.mac_address_pool[pool]['current'] = incremented_mac_address

        with open(self.board_test_server_configuration_file, 'w') as config_file:
            self.configuration.write(config_file)

        # sync filesystem
        os.system('sync')

        # give formatted address to caller
        formatted_mac_address = ':'.join([
            next_mac_address[0] + next_mac_address[1],
            next_mac_address[2] + next_mac_address[3],
            next_mac_address[4] + next_mac_address[5],
            next_mac_address[6] + next_mac_address[7],
            next_mac_address[8] + next_mac_address[9],
            next_mac_address[10] + next_mac_address[11],
        ])

        return formatted_mac_address

    def process_connection(self, connection, client_info=None):

        message = connection.recv(4096).decode("utf-8")

        loc = message.find("\n")
        if message.startswith("storeBoardData|", 0):
            pass
        elif loc <= 0:
            return

        # split the command into sub strings
        args = message[:loc].strip()
        argument_list = args.split('|')

        print(argument_list)

        if argument_list[0] == 'storeBoardData':

            # connection.close()
            if len(argument_list) < 12:
                logging.error('Received invalid command arguments for storeBoardData')
                logging.error(message)

            serial_number = argument_list[1].lstrip('"').rstrip('"')
            wired_mac_address = argument_list[2].lstrip('"').rstrip('"')
            wireless_mac_address = argument_list[3].lstrip('"').rstrip('"')
            hostname = argument_list[4].lstrip('"').rstrip('"')
            board_model = argument_list[5].lstrip('"').rstrip('"')
            loader_version = argument_list[6].lstrip('"').rstrip('"')
            peripheral_version = argument_list[7].lstrip('"').rstrip('"')
            board_assembly_number = argument_list[8].lstrip('"').rstrip('"')
            board_rev_number = argument_list[9].lstrip('"').rstrip('"')
            manufacture_date = argument_list[10].lstrip('"').rstrip('"')
            date = argument_list[11].lstrip('"').rstrip('"')

            try:
                self.board_info_recorder.record_board_information(serial_number,
                                                                  Hostname=hostname,
                                                                  EthernetMAC=wired_mac_address,
                                                                  WirelessMAC=wireless_mac_address,
                                                                  Model=board_model,
                                                                  LoaderVersion=loader_version,
                                                                  PeripheralVersion=peripheral_version,
                                                                  BoardAssemblyNumber=board_assembly_number,
                                                                  BoardRevNumber=board_rev_number,
                                                                  ManufactureDate=manufacture_date,
                                                                  Date=date)
            except board_info_recorder.DuplicateRecordError:
                self.board_info_recorder.update_board_information(serial_number,
                                                                  Hostname=hostname,
                                                                  EthernetMAC=wired_mac_address,
                                                                  WirelessMAC=wireless_mac_address,
                                                                  Model=board_model,
                                                                  LoaderVersion=loader_version,
                                                                  PeripheralVersion=peripheral_version,
                                                                  BoardAssemblyNumber=board_assembly_number,
                                                                  BoardRevNumber=board_rev_number,
                                                                  ManufactureDate=manufacture_date,
                                                                  Date=date)

            message = 'success' + '\n'
            connection.sendall(message.encode("utf-8"))
            connection.close()

        # getNextMACAddress|pool\n
        elif argument_list[0] == 'getNextMACAddress':
            data = self.get_next_mac_address(int(argument_list[1]))
            message = data + '\n'
            connection.sendall(message.encode("utf-8"))
            connection.close()

        elif argument_list[0] == 'ping':
            message = 'on-line' + '\n'
            connection.sendall(message.encode("utf-8"))
            connection.close()

        elif argument_list[0] == 'stop':
            try:
                os.remove('%s/board_test_server.pid' % SERVICE_DIRECTORY)
            except:
                pass
            connection.close()
            self.shutdown = True

        else:
            connection.close()

    def handle_requests(self):

        inputs = [self.server]
        outputs = []

        while not self.shutdown:

            # block on event coming in
            try:
                readable, writable, exceptional = select.select(inputs, outputs, inputs)

                # event occurred either socket connection or touchscreen event
                for s in readable:

                    # socket message coming in to process
                    if s is self.server:

                        # accept network connection
                        connection, client_info = s.accept()

                        # process connection
                        self.process_connection(connection, client_info)

            except:
                pass

        self.server.close()


if __name__ == '__main__':
    board_test_server = Manager()
    board_test_server.handle_requests()
