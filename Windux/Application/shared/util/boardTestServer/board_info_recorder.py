
import logging
import sqlite_util
from socket_util import BaseSocket
from collections import OrderedDict

# Pull this up into our namespace so don't have to worry about importing extra modules into implementers.
DatabaseError = sqlite_util.DatabaseError


class DuplicateRecordError(Exception):
    """ Raised when a record insert is executed with a primary key that already exists.
    """
    pass


class DatabaseRecorder:

    def __init__(self, database_filename='board_test_server.sqlite'):
        self.database_filename = database_filename
        self.database = sqlite_util.Database(self.database_filename)
        self.setup_database()

    def record_board_information(self, serial_number, **kwargs):
        """ Record information for the scale with serial number and keyword-arg data.

        :param serial_number: The serial number of the board.
        :param kwargs: Any data for extra columns.
        """
        kwargs['SerialNumber'] = serial_number

        # Dictionary for where clause. Cant use kwargs because we only want to check the serial number column
        mydict = {}
        mydict['SerialNumber'] = serial_number
        try:
            result = self.database.select("*", mydict)
            if not result:
                self.database.insert(kwargs)
            else:
                logging.error('Duplicate serial number {0} found, updating record'.format(serial_number))
                raise DuplicateRecordError('Duplicate serial number {0} found'.format(serial_number))
        except sqlite_util.DatabaseError as err:
            logging.error('Database Error: '.format(err))
            raise err

    def update_board_information(self, serial_number, **kwargs):
        """ Update record information for an existing serial number.

        :param serial_number: The serial number for the board.
        :param kwargs: The column data to update.
        """
        self.database.update(kwargs, where={'SerialNumber': serial_number})

    def setup_database(self):
        """ Setup the database file if it doesn't exist.
        """
        table_name = 'BoardInformation'
        table_columns = OrderedDict()
        table_columns['SerialNumber'] = 'TEXT'
        table_columns['Hostname'] = 'TEXT'
        table_columns['WirelessMAC'] = 'TEXT'
        table_columns['EthernetMAC'] = 'TEXT'
        table_columns['Model'] = 'TEXT'
        table_columns['LoaderVersion'] = 'TEXT'
        table_columns['PeripheralVersion'] = 'TEXT'
        table_columns['BoardAssemblyNumber'] = 'TEXT'
        table_columns['BoardRevNumber'] = 'TEXT'
        table_columns['ManufactureDate'] = 'TEXT'
        table_columns['Date'] = 'TEXT'
        self.database.create_table(table_name,
                              columns=table_columns,
                              primary_key='SerialNumber',
                              ignore_exists=True)
        self.database.table = 'BoardInformation'


def test_recording():
    """ Test case to record a fake board serial number record.
    """
    board_info_recorder = DatabaseRecorder('board_test_server.sqlite')
    board_info_recorder.setup_database()

    starting_serial_number = 549
    data = OrderedDict()
    data['Hostname'] = 'testhostname'
    data['WirelessMAC'] = '00:02:6B:13:BE:C3'
    data['EthernetMAC'] = '60:02:B4:07:87:3A'
    data['Model'] = 'ARK'
    data['LoaderVersion'] = '1.7.a'
    data['PeripheralVersion'] = '1.1'
    data['BoardAssemblyNumber'] = '45-1084-982'
    data['BoardRevNumber'] = '2.2A'
    data['ManufactureDate'] = '04/15/2014'
    data['Date'] = '05/15/2015 04:52:34 PM'

    arguments = '|'.join(data.values())

    def send_message(serial_number):
        """ Send the serial number record to the server.

        :param serial_number: The serial number for the board
        """
        with BaseSocket('127.0.0.1', 5080) as raspberry_pi:
            raspberry_pi.connect()

            message = 'storeBoardData|' + 'BR-' + str(serial_number) + '|' + arguments
            logging.debug(message)
            raspberry_pi.sendall(bytes(message))

    for this_serial_number in range(starting_serial_number, 560):
        send_message(this_serial_number)


if __name__ == '__main__':
    test_recording()
