#!/usr/bin/env python3
# -*- coding: utf-8 -*-

""" Now serving queue management.

    Author: Ian Davis
"""


import os
import re
import sys
import socket
import time
import shellCommand

import usb.core
import usb.util
import zpl

import license
import file_io
import ticketPrinter

from collections import deque
from datetime import datetime
from threading import Thread
from configparser import ConfigParser

from twisted.internet import threads
from twisted.internet.protocol import Factory
from twisted.protocols.basic import LineReceiver

from logger import Logger


queue_file_list = '/usr/local/hobart/queues.dat'
now_serving_feature = 'SCALE_NOW_SERVING'

if os.name == 'posix':

    if ( os.path.exists(os.path.join("/","etc","hobart","enable_mysql") ) ):
        licensed = True
    else:
        try:
            license_buffer = license.readFlashKeyBuffer()
            licensed = license.isProductLicensed(license_buffer, now_serving_feature)
        except UnboundLocalError:
            licensed = False
else:
    licensed = False


def reload_license():
    """ Try to reload now serving license from flash. """
    global licensed

    if os.name == 'posix':
        if ( os.path.exists(os.path.join("/","etc","hobart","enable_mysql") ) ):
            licensed = True
        else:
            try:
                license_buffer = license.readFlashKeyBuffer()
            except UnboundLocalError:
                licensed = False
                return

            licensed = license.isProductLicensed(license_buffer, now_serving_feature)
    else:
        licensed = False


def print_serving_ticket(ticket_number):
    """ Find an attached printer, and print a ticket with the given number.

        @param ticket_number: The ticket number to print.
    """
    now_serving_file = "now_serving.zpl"
    printer = ticketPrinter.zebra()

    if not printer.isTicketPrinterAttached():
        raise NoTicketPrinterError()

    printer.generateCustomerTicket(ticket_number)
    printer.printCustomerTicket(now_serving_file)


def require_license(function):
    """ Wrap a function, requiring a now serving license to run.

        @param function: The function to wrap.
    """
    def wrapped(*args, **kwargs):
        licensed = False
        if ( os.path.exists(os.path.join("/","etc","hobart","enable_mysql") ) ):
            licensed = True

        if not licensed:
            raise LicenseRequiredException()

        return function(*args, **kwargs)

    return wrapped


def print_debug(function):
    """ Wrap a function, printing out the name of the function everytime it's called.

        @param function: The name of the function to wrap.
    """
    def wrapped(*args, **kwargs):
        print(('Calling function ' + function.__name__))
        return function(*args, **kwargs)

    return wrapped


def write_bin(file_path, obj):
    """ Dump a python object to a file using cPickle.

        @param file_path: The path to the binary file to write.
        @param obj: The python object to dump.
    """
    cmd = []
    cmd.append("sync")
    exitCode, stdout, stderr= shellCommand.execute(cmd)
    file_io.writeBinObjectToFile(file_path, obj)
    exitCode, stdout, stderr= shellCommand.execute(cmd)

def read_bin(file_path):
    """ Read a binary object from a cPickle dump file.

        @param file_path: The path to the cPickle dump file to read.
        :return: A python object loaded from the file.
        @raise InvalidPickleError: If the dump file could not be read.
    """
    status, obj = file_io.readBinObjectFromFile(file_path)
    if status != 'success':
        raise InvalidPickleError()

    return obj


def format_timedelta(time_delta, format_string='{days}:{hours}:{minutes}:{seconds}'):
    """ Split a timedelta up into days, hours, minutes, and seconds, and format a format string with the result.

        Default format string (as example):
            {days}:{hours}:{minutes}:{seconds}

        @param time_delta: The timedelta object to calculate.
        @param format_string: The template string that will be formatted.

            The following keys will be provided to this template (by name):
                * days: The number of days represented by the timedelta object.
                * hours: The number of hours represented by the timedelta object.
                * minutes: The number of minutes represented by the timedelta object.
                * seconds: The number of seconds represented by the timedelta object.
    """
    time_pieces = {'days': time_delta.days}
    time_pieces['hours'], remainder = divmod(time_delta, 3600)
    time_pieces['minutes'], time_pieces['seconds'] = divmod(remainder, 60)
    return format_string.format(**time_pieces)


class Manager(Factory):
    """ Handle top level functions for the now serving queue. """

    queue_files = {'default': '/usr/local/hobart/queue.dat'}
    config_file = '/usr/local/hobart/now_serving.ini'

    @require_license
    def __init__(self, default_starting_number=1):
        """ Constructor.

            @param default_starting_number: The number to start new queues at (defaults to 1).
        """
        self.logger = Logger('/usr/local/hobart/now_serving.debug', printing=True)
        self.queues = {}
        self.server = None
        self.usb_listener = USBListener(notify_callback=self.dispatch, logger=self.logger)
        self.logger.log('info', 'Class Manager:__init__ - set up debug file')
        self.default_starting_number = default_starting_number

        self.monitor_host, self.monitor_port = self._load_monitor_settings()
        self._initialize_queues()

    def dispatch(self, command):
        """ Dispatch a now serving command to its proper handler.

            Dispatching is done using python's builtin __getattr__ method,
            matching the command name given to a method defined on our Manager instance.

            Methods that are designed to support automatic dispatching should be labeled with
            a one line comment before their definition, like so:
            # Command Dispatch Handler
            Methods without this distinction should be assumed as internal or helper methods.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            | 2    | Device is not licensed for now serving queue  |
            +------+-----------------------------------------------+
            | 3    | The queue named {queue_name} does not exist   |
            +------+-----------------------------------------------+

            @param command: The command to parse and dispatch.
        """
        if not hasattr(self, command.name):
            message = "now_serving::dispatch if not hasattr Unrecognized now serving command: " + command.name
            self.logger.log('info', message + '\n')
            command.notify_sender_error(code=1, description="Unrecognized command sent")
            raise AttributeError('Unrecognized now serving command')

        method = getattr(self, command.name)

        try:
            message = "now_serving::dispatch try with command: " + command.name
            self.logger.log('info', message)
            return method(command=command)
        except LicenseRequiredException:
            message = "now_serving::dispatch Device is not licensed for now serving queue"
            self.logger.log('info', message + '\n')
            command.notify_sender_error(code=2, description=message)
            return
        except InvalidQueueNameError:
            message = 'now_serving::dispatch The queue named {queue_name} does not exist\n'.format(queue_name=command.args[0])
            self.logger.log('info', message)
            command.notify_sender_error(code=3, description=message)
            return

    def set_serving_number(self, command):
        """ Set the next serving number of the given queue.

            The serving number identifies the next number that will be printed.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            | 7    | Invalid number to set as next printed number. |
            +------+-----------------------------------------------+

            @param command: set_serving_number|{queue_name}|{customer_number}

                * queue_name: The name of the queue to modify.
                * customer_number: The number of the next customer to serve.
        """
        number = command.args[1]
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)

        try:
            self.logger.log('info', 'now_serving::set_serving_number try to set number with command: ' + command.name)
            queue.current_number = int(number)
        except ValueError:
            self.logger.log('info', 'now_serving::set_serving_number Invalid number to set as next printed number')
            command.notify_sender_error(code=7, description="Invalid number to set as next printed number")
            return

        self.save_queue(queue_name=queue_name)
        command.notify_sender_success()
        self.notify_monitor(queue_name, queue)

    def get_serving_number(self, command):
        """ Get the next serving number of the given queue.

            The serving number identifies the next number that will be printed.

            @param command: get_serving_number|{queue_name}
                * queue_name: The name of the queue to lookup.
        """
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)

        command.notify_sender_success(str(queue.current_number), str(len(queue)))

    def increment_serving_number(self, command):
        """ Increment the next serving number of the given queue.

            The serving number identifies the next number that will be printed.
            This is a shorthand of set_serving_number to simply increment the current number.

            @param command: increment_serving_number|{queue_name}
                * queue_name: The name of the queue to modify.
        """
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)

        command.notify_sender_success(str(queue.current_number), str(len(queue)))

        self.notify_monitor(queue_name, queue)

        queue.current_number += 1

    def increment_serving_queue(self, command):
        """ Push a new number to the given serving queue without printing a ticket.

            @param command: increment_serving_queue|{queue_name}
                * queue_name: The name of the queue to modify.
        """
        queue_name = command.args[0]
        customer_name = command.args[1] if len(command.args) > 1 else ""
        queue = self.get_queue(queue_name=queue_name)
        ticket_number = self.put_next(queue_name=queue_name, customer_name=customer_name)

        self.logger.log('info', 'Added new ticket to queue: ' + str(ticket_number))
        command.notify_sender_success(str(ticket_number), str(len(queue)))
        self.notify_monitor(queue_name)

    def get_customer_name(self, command):
        queue_name = command.args[0]
        ticket_number = int(command.args[1])
        queue = self.get_queue(queue_name=queue_name)
        customer_name = queue.get_name(ticket_number)
        if customer_name:
            command.notify_sender_success("1", customer_name)
        else:
            command.notify_sender_success("0")

    def print_next_ticket(self, command):
        """ Push the next customer number to the queue, and print a ticket for that number.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            |  5   | No ticket template files found                |
            +------+-----------------------------------------------+
            |  6   | No ticket printer found                       |
            +------+-----------------------------------------------+
            |  8   | Could not print ticket                        |
            +------+-----------------------------------------------+

            @param command: print_next_ticket|{queue_name}
                * queue_name: The name of the queue to modify.
        """
        queue_name = command.args[0]
        customer_name = command.args[1] if len(command.args) > 1 else ""
        ticket_number = self.put_next(queue_name=queue_name, customer_name=customer_name)

        try:
            print_serving_ticket(ticket_number=ticket_number)
        except NoTicketTemplateError:
            self.logger.log('error', 'No ticket template files found\n')
            command.notify_sender_error(code=5, description="No ticket template files found")
            return
        except NoTicketPrinterError:
            self.logger.log('error', 'No ticket printer found\n')
            command.notify_sender_error(code=6, description="No ticket printer found")
            return
        except Exception as err:
            self.logger.log_exc(err)
            command.notify_sender_error(code=8, description="Could not print ticket")
            return

        self.logger.log('info', 'Printed Ticket: ' + str(ticket_number))

        command.notify_sender_success()
        self.notify_monitor(queue_name)

    def serve_customer(self, command):
        """ Pop a waiting customer number from the queue.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            |  2   | Invalid number to set as next printed number  |
            +------+-----------------------------------------------+
            |  3   | The queue named {queue_name} does not exist   |
            +------+-----------------------------------------------+

            @param command: serve_customer|{queue_name}|{customer_number}
                queue_name: The name of the queue to modify.
                customer_number: The customer number to remove.
        """
        queue_name = command.args[0]

        try:
            customer_number = int(command.args[1])
        except ValueError:
            self.logger.log('error', 'Invalid number to set as next printed number\n')
            command.notify_sender_error(code=7, description="Invalid number to set as next printed number")
            return

        queue = self.get_queue(queue_name=queue_name)

        try:
            queue.remove(customer_number)
        except ValueError:
            pass

        command.notify_sender_success()
        self.notify_monitor(queue_name, queue)

    def cancel_serve_customer(self, command):
        """ Add customer customer number back into the queue.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            |  2   | Invalid number to set as next printed number  |
            +------+-----------------------------------------------+
            |  3   | The queue named {queue_name} does not exist   |
            +------+-----------------------------------------------+

            :param command: cancel_serve_customer|{queue_name}|{customer_number}

                queue_name: The name of the queue to modify.
                customer_number: The customer number to remove.

        """
        queue_name = command.args[0]

        try:
            customer_number = int(command.args[1])
        except ValueError:
            self.logger.log('error', 'Invalid number to cancel\n')
            command.notify_sender_error(code=7, description="Invalid number to cancel")
            return

        queue = self.get_queue(queue_name=queue_name)

        try:
            queue.appendleft(customer_number)
        except ValueError:
            pass

        command.notify_sender_success()
        self.notify_monitor(queue_name, queue)

    # Command Dispatch Handler
    def serve_next_customer(self, command):
        """ Pop the next waiting customer number from the queue.

            This is a shorthand to serve_customer that serves the next customer in the queue.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            |  4   | No customers to serve                         |
            +------+-----------------------------------------------+

            @param command: serve_customer|{queue_name}

                * queue_name: The name of the queue to modify.
        """
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)
        try:
            current_number = self.get_next(queue_name=queue_name)
        except IndexError:
            self.logger.log('error', 'No customers to serve\n')
            command.notify_sender_error(code=4, description="No customers to serve")
            return

        command.notify_sender_success(str(current_number), str(len(queue)))
        self.notify_monitor(queue_name, serving_number=str(current_number))

    def get_next_customer(self, command):
        """ Return the next customer number waiting to be served.

            This does not modify the queue, and is simply to get data about the next customer.

            @param command: get_next_customer|{queue_name}
                * queue_name: The name of the queue to lookup.
        """
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)

        command.notify_sender_success(str(queue.peek(0)), str(len(queue)))

    def get_queue_size(self, command):
        """ Get the number of customers in the queue.

            @param command: get_queue_size|{queue_name}
                * queue_name: The name of the queue to lookup.
        """
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)

        command.notify_sender_success(str(len(queue)))

    def clear_queue(self, command):
        """ Clear out a given queue.

            @param command: clear_queue|{queue_name}
                * queue_name: The name of the queue to modify.
        """
        queue_name = command.args[0]
        queue = self.get_queue(queue_name=queue_name)

        queue.flush()

        command.notify_sender_success()
        self.notify_monitor(queue_name, queue)

    def reset_queue(self, command):
        """ Clear a queue, and reset its next serving number to the default.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            |  7   | Invalid number to set as next printed number  |
            +------+-----------------------------------------------+

            @param command: reset_queue|{queue_name}|{starting_number}
                * queue_name: The name of the queue to modify.
                * starting_number: The number to use as the queue's next serving number.
        """
        queue_name = command.args[0]
        starting_number = command.args[1]

        queue = self.get_queue(queue_name=queue_name)
        queue.flush()

        try:
            queue.current_number = int(starting_number)
        except ValueError:
            self.logger.log('error', 'Invalid number to set as next printed number\n')
            command.notify_sender_error(code=7, description="Invalid number to set as next printed number")

        self.save_queue(queue_name=queue_name)

        command.notify_sender_success()
        self.notify_monitor(queue_name, queue)

    def reset_all_queues(self, command):
        """ Clear all existing queues and reset their starting numbers to the default.

            +------------------------------------------------------+
            |                   Error Responses                    |
            +------+-----------------------------------------------+
            | code | description                                   |
            +======+===============================================+
            |  7   | Invalid number to set as next printed number  |
            +------+-----------------------------------------------+

            @param command: reset_all_queues|{starting_number}
                * starting_number: The number to use as the queue's next serving number.
        """
        starting_number = command.args[0]

        for queue_name, queue in list(self.queues.items()):
            queue.flush()

            try:
                queue.current_number = int(starting_number)
            except ValueError:
                self.logger.log('error', 'Invalid number to set as next printed number\n')
                command.notify_sender_error(code=7, description="Invalid number to set as next printed number")
                continue

            self.save_queue(queue_name=queue_name)
            self.notify_monitor(queue_name, queue)

        command.notify_sender_success()

    def cleanup(self):
        """ Clean up listeners before shutting down. """
        self.usb_listener.cleanup()

    def new_queue(self, queue_name, queue_file):
        """ Create a new now serving queue.

            @param queue_name: The name of the new queue.
            @param queue_file: The path to save the new queue file too.
        """
        self.queues[queue_name] = ServingQueue(starting_number=self.default_starting_number)
        self.queue_files[queue_name] = queue_file

        write_bin(file_path=queue_file_list, obj=self.queue_files)

        self.save_queue(queue_name=queue_name)

    def save_queue(self, queue_name='default'):
        """ Save the queue to its file.

            @param queue_name: The name of the queue to save.
        """
        queue = self.get_queue(queue_name=queue_name)
        queue_file = self.queue_files[queue_name]

        write_bin(file_path=queue_file, obj=queue)

    def save_queues(self):
        """ Save all current now serving queues to their files. """
        for queue_name in self.queue_files:
            self.save_queue(queue_name=queue_name)

    def get_queue(self, queue_name='default'):
        """ Lookup and return the given queue.

            @param queue_name: The name of the queue to lookup.
            @return: The ServingQueue instance looked up.
            @raise InvalidQueueNameError: When the queue name given is not found.
        """
        try:
            queue = self.queues[queue_name]
        except KeyError:
            raise InvalidQueueNameError(queue_name=queue_name)

        return queue

    def put_next(self, queue_name='default', customer_name=""):
        """ Put the next serving number into the given queue, and save the queue.

            @param queue_name: The name of the queue to modify.
            @return: The customer number that was added to the queue.
        """
        queue = self.get_queue(queue_name=queue_name)
        number_added = queue.put_next(customer_name)

        self.save_queue(queue_name=queue_name)

        return number_added

    def get_next(self, queue_name='default'):
        """ Get the next serving number of the given queue.

            @param queue_name: The name of the queue to lookup.
            @return: The customer number that will be served next.
        """
        queue = self.get_queue(queue_name=queue_name)
        number = queue.get_next()

        self.save_queue(queue_name=queue_name)

        return number

    def notify_monitor(self, queue_name, queue=None, serving_number=None):
        """ Notify the now serving monitor of a change to a queue.

            @param queue_name: The name of the queue that was modified.
            @param queue: The ServingQueue instance (will be looked up if None).
            @param serving_number: The next serving number of the queue.
        """
        if not queue:
            queue = self.get_queue(queue_name=queue_name)

        message = MonitorMessage(queue_name, queue, serving_number=serving_number)

        self.monitor_socket = ThreadedSocket(self.monitor_host, self.monitor_port)
        self.monitor_socket.send_message(message)

    def buildProtocol(self):
        """ Build a Connection instance for a new socket connection created. """
        connection = Connection(dispatch_callback=self.dispatch)
        return connection

    def _load_queue_file_list(self):
        """ Load a list of all the now serving queues to manage from an internal cPickle file. """
        try:
            temp_queue_files = read_bin(file_path=queue_file_list)
        except InvalidPickleError:
            # Write out our default queues if we can't read from the file
            write_bin(file_path=queue_file_list, obj=self.queue_files)
        else:
            # Only load in the object given by read_bin if it worked successfully
            self.queue_files = temp_queue_files

    def _initialize_queues(self):
        """ Load ServingQueue instances for all loaded queue files. """
        self._load_queue_file_list()

        for queue_name, queue_file in list(self.queue_files.items()):
            try:
                self.logger.log('info', '_initialize_queues: before read_bin queuefile{0}'.format(queue_file))
                queue = read_bin(file_path=queue_file)
            except InvalidPickleError:
                self.logger.log('info', '_initialize_queues: InvalidPickleError')
                self.new_queue(queue_name=queue_name, queue_file=queue_file)
            else:
                self.logger.log('info', '_initialize_queues: before read_bin queuefile{0}'.format(queue))
                self.queues[queue_name] = queue

    def _load_monitor_settings(self):
        """ Load now serving monitor host settings from our configuration file, or defaults. """
        defaults = {}
        defaults['host'] = '127.0.0.1'
        defaults['port'] = 5081

        config_parser = ConfigParser()

        if not os.path.exists(self.config_file):
            config_parser.add_section('monitor')
            config_parser.set('monitor', 'host', defaults['host'])
            config_parser.set('monitor', 'port', str(defaults['port']))

            with open(self.config_file, 'wt') as config_file:
                config_parser.write(config_file)

            return defaults['host'], defaults['port']

        config_parser.read(self.config_file)

        try:
            monitor_host = config_parser.get('monitor', 'host')
        except Exception as err:
            self.logger.log_exc(err)
            monitor_host = defaults['host']

        try:
            monitor_port = config_parser.getint('monitor', 'port')
        except Exception as err:
            self.logger.log_exc(err)
            monitor_port = defaults['port']

        return monitor_host, monitor_port


class Command(object):
    """ Represent a command sent from the smBackend listener. """

    delimiter = '|'

    def __init__(self, text, connection=None):
        """ Constructor.

            @param text: The text of the command that was sent.
            @param connection: The socket connection the command was sent on (or None).
        """
        self.logger = Logger('/usr/local/hobart/now_serving_command.debug', printing=True)
        self.connection = connection
        self.tokens = text.rstrip(Connection.delimiter).split(self.delimiter)
        self.name = None
        self.args = []

        self.parse_tokens()

    def parse_tokens(self):
        """ Separate the name token from the argument tokens of the command. """
        self.name = self.tokens[0]
        self.args = self.tokens[1:]

    def notify_sender_success(self, *args):
        """ Respond to the socket connection that sent our command that the command succeeded.

            @param args: Any arguments to be appened to the response message.
        """
        if not self.connection:
            return

        arg_message = '|'.join(args)
        success_message = 'success'
        if arg_message:
            success_message += '|' + arg_message

        self.logger.log('info', 'notify_sender_success success_message: ' + success_message)

        self.connection.sendLine(success_message.encode('utf-8'))

    def notify_sender_error(self, code, description):
        """ Respond to the socket connection that sent our command that the command failed.

            @param code: An error code that identifies the error that occurred.
            @param description: A description of the error that occurred.
        """

        self.logger.log('info', 'notify_sender_error: ')

        if not self.connection:
            return

        self.logger.log('info', 'notify_sender_error: ' + "error|{code}|{description}".format(code=code, description=description))

        error_message = "error|{code}|{description}".format(code=code, description=description)
        self.connection.sendLine(error_message.encode('utf-8'))


class Connection(LineReceiver):
    """ Handle receiving data from a socket connection opened to the twisted server. """

    delimiter = '\n'

    def __init__(self, dispatch_callback):
        """ Constructor.

            @param dispatch_callback: A callable that is called for every command received from the socket.
        """
        self.dispatch_callback = dispatch_callback

    def lineReceived(self, line):
        """ Handle a line of data received on the socket.

            @param line: The line of data.
        """
        command = Command(text=line.strip())
        self.dispatch_callback(self, command)


class ServingTemplate(object):
    """ Represent a printer template for a now serving ticket. """

    date_format_regex = re.compile("\\^FO100.*18\\^FD")

    def __init__(self, template_path="/etc/hobart/cups/serving_template.zpl"):
        """ Constructor.

            @param template_path: The path to the template file to read.
        """
        self.template_path = template_path

        self.template_string = None
        self.ticket_number_index = None
        self.date_format_template = None

        self._read_template_file()

    def write(self, now_serving_path, ticket_number, date):
        """ Write a ticket printer file to the attached printer.

            @param now_serving_path: This is the path to the file to write too.
            @param ticket_number: The number of the ticket to print.
            @param date: The date to print on the ticket.
        """
        date_string = self.date_format_template.format(date=date)
        ticket_number_string = self._generate_ticket_number_string(ticket_number=ticket_number)

        with open(now_serving_path, "wb") as now_serving_file:
            now_serving_file.write(ticket_number_string + date_string)
            now_serving_file.flush()

    def _read_template_file(self):
        """ Read the template file and parse the data to reproduce it. """
        with open(self.template_path, 'r+') as serving_template:
            self.template_string = serving_template.read()

        self.ticket_number_index = self.template_string.find("100^FD") + len("100^FD")
        self.date_format_template = self._load_date_format_template()

    def _load_date_format_template(self):
        """ Load the date format from the ticket template. """
        date_format_template = self.date_format_regex.findall(self.template_string)[-1]
        date_format_template += '{date}'
        date_format_template += zpl.zpl_commands().getFieldSeperator() + '\n'
        date_format_template += zpl.zpl_commands().getEndFormat() + '\n'
        return date_format_template

    def _generate_ticket_number_string(self, ticket_number):
        """ Generate a new ticket template filled out with the given ticket number. """
        ticket_number_string = ''

        if int(str(ticket_number), 10) < 10:
            ticket_number_string = self.template_string[: self.ticket_number_index]
            ticket_number_string += str(ticket_number)
            ticket_number_string += zpl.zpl_commands().getFieldSeperator() + '\n'
            return ticket_number_string

        x_index = self.template_string.find("^FO250")
        ticket_number_string = self.template_string[: x_index]

        zpl.zpl_commands().setFieldOriginX("225")

        ticket_number_string += zpl.zpl_commands().getFieldOrigin()

        x_index = self.template_string.find("500")

        ticket_number_string = self.template_string[x_index: self.ticket_number_index]
        ticket_number_string += str(ticket_number)
        ticket_number_string += zpl.zpl_commands().getFieldSeperator() + '\n'

        return ticket_number_string


class Customer(object):
    """ Represent a single customer in a ServingQueue. """

    def __init__(self, number, time_added=None, name=""):
        """ Constructor.

            @param number: The customer number.
            @param time_added: The time the customer was added to the queue, as a datetime.time object (or now).
        """
        self.number = number
        self.time_added = time_added
        self.name = name

        if not self.time_added:
            self.time_added = datetime.now()

    @property
    def time_waiting(self):
        """ Calculate and return the amount of time since this customer was added to the queue. """
        time_delta = datetime.now() - self.time_added
        return format_timedelta(time_delta)

    def __eq__(self, other):
        """ Compare two customers' equality based on their customer number. """
        if isinstance(other, Customer):
            return self.number == other.number
        else:
            return self.number == other

    def __ne__(self, other):
        """ Compare two customers' inequality based on their customer number. """
        if isinstance(other, Customer):
            return not self.number == other.number
        else:
            return not self.number == other

    def __lt__(self, other):
        """ Compare a customer as lesser than another based on their customer number. """
        if isinstance(other, Customer):
            return not self.number < other.number
        else:
            return not self.number < other

    def __gt__(self, other):
        """ Compare a customer as greater than another based on their customer number. """
        if isinstance(other, Customer):
            return not self.number > other.number
        else:
            return not self.number > other

    def __le__(self, other):
        """ Compare a customer as lesser than or equal to another based on their customer number. """
        if isinstance(other, Customer):
            return not self.number <= other.number
        else:
            return not self.number <= other

    def __ge__(self, other):
        """ Compare a customer as greater than or equal to another based on their customer number. """
        if isinstance(other, Customer):
            return not self.number >= other.number
        else:
            return not self.number >= other

    def __int__(self):
        """ Represent a customer as their customer number for int conversion. """
        return self.number

    def __str__(self):
        """ Represent customers as their customer number to support command methods that expect this. """
        return '{0}({1}, {2}, {3})'.format(self.__class__.__name__, self.number, self.time_added, self.name)


class ServingQueue(deque):
    """ Represent a deque collection that manages the currently waiting customers in a now serving system. """

    def __init__(self, iterable=None, maxlen=None, starting_number=1):
        """ Constructor.

            @param iterable: An iterable of items to initially add to the ServingQueue (default None).
            @param maxlen: The maximum length of the queue (default None).
            @param starting_number: The inital number to add to the queue (default 1).
        """
        if not iterable:
            iterable = ()

        deque.__init__(self, iterable, maxlen)

        self.current_number = starting_number
        self.last_served = None
        self.names = {}

    def put_next(self, name=""):
        """ Push the next customer number into the queue, and increment the next customer number.

            @return: The customer number that was added.
        """
        number_added = self.current_number

        self.append(self.current_number)
        
        self.names[self.current_number] = name

        if self.current_number == 99:
            self.current_number = 0

        self.current_number += 1
        return number_added
        
    def get_name(self, customer):
        if customer in self.names:
            return self.names[customer]
        else:
            return ""

    def get_next(self):
        """ Pop the next customer to be served from the queue and return it. """
        self.last_served = str(self.popleft())
        return self.last_served

    def peek(self, index):
        """ Return the customer number at a specific position in the queue.

            Does not remove from the queue.

            @param index: The position to peek at.
        """
        try:
            return self[index]

        except IndexError:
            return -1

    def flush(self):
        """ Clear out all customers from the queue. """
        while bool(self):
            self.get_next()


class MonitorMessage(object):
    """ Represent a message to be sent to the now serving monitor when a now serving queue is modified. """

    MONITOR_COMMAND = 'queue_status'

    def __init__(self, queue_name, queue, serving_number=None):
        """ Constructor.

            @param queue_name: The name of the queue that was modified.
            @param queue: The queue instance that was modified.
            @param serving_number: The current number being served.
        """
        self.queue_name = queue_name
        self.serving_number = serving_number if serving_number else queue.last_served
        self.queue_size = str(len(queue))

    def __str__(self):
        """ Return the command format expected by the monitor.

            Example command:
            queue_status|{queue_name}|{serving_number}|{queue_size}
        """
        parameters = (self.MONITOR_COMMAND, self.queue_name, self.serving_number, self.queue_size)
        return '|'.join(parameters) + '\n'


class BroadcastSocket(socket.socket):
    """ Represent a UDP socket that broadcasts a command on the local network."""

    def __init__(self, port=5081):
        """ Constructor.

            @param port: The port to broadcast on (default 5081).
        """
        super(BroadcastSocket, self).__init__(socket.AF_INET, socket.SOCK_DGRAM)

        self.port = port

        self.bind(('', 0))
        self.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    def send_message(self, message):
        """ Broadcast a message on the UDP socket.

            @param message: The message to broadcast.
        """
        return self.sendto(str(message), ('<broadcast>', self.port))


class ThreadedSocket(socket.socket):
    """ Represent a socket that sends all messages on a separate thread of execution. """

    def __init__(self, host, port):
        """ Constructor.

            @param host: The hostname to connect to.
            @param port: The port to connect to.
        """
        super(ThreadedSocket, self).__init__(socket.AF_INET, socket.SOCK_STREAM)

        self.host = host
        self.port = port

    def _send_message(self, message):
        """ Connect to the host, send a message, and close the socket.

            Runs in a separate thread.

            @param message: The message to send.
        """
        self.settimeout(5)
        self.connect((self.host, self.port))
        self.sendall(message if type(message) == bytes else str(message).encode())
        self.close()

    def send_message(self, message):
        """ Connect to the host, send a message, and close the socket.

            Starts a thread to do the actual socket work.

            @param message: The message to send.
        """
        threaded_send = Thread(target=self._send_message, args=(message,))
        threaded_send.start()


class USBDevice(object):
    """ Represent an attached USB Device. """

    def __init__(self, button_vendor_id=0x16c0, button_product_id=0x0482, interface=0):
        """ Constructor.

            @param button_vendor_id: The vendor id of the now serving button.
            @param button_product_id: The product id of the now serving button.
            @param interface: The interface to connect to the button on.
        """
        self.button_vendor_id = button_vendor_id
        self.button_product_id = button_product_id
        self.interface = interface

        self._usb_device = None
        self._usb_endpoint = None

        self._initialize_usb_device()

    def __enter__(self):
        """ Context manager entry point. """
        self._initialize_usb_device()
        return self

    def __exit__(self, type, value, traceback):
        """ Context manager cleanup. """
        self.cleanup()

    def cleanup(self):
        """ Cleanup attached usb device interfaces and release resources. """
        usb.util.release_interface(self._usb_device, self.interface)
        self._usb_device.attach_kernel_driver(self.interface)

    def _initialize_usb_device(self):
        """ Find our usb device if attached, and connect to its interface.

            @raise: NoUSBDeviceError: If the now serving button isn't attached.
        """
        try:
            self._usb_device = usb.core.find(idVendor=self.button_vendor_id, idProduct=self.button_product_id)
        except ValueError:
            raise NoUSBDeviceError()
        except AttributeError:
            raise NoUSBDeviceError()

        if not self._usb_device:
            raise NoUSBDeviceError()

        if self._usb_device.is_kernel_driver_active(self.interface):
            self._usb_device.detach_kernel_driver(self.interface)

        usb.util.claim_interface(self._usb_device, self.interface)
        self._usb_endpoint = self._usb_device[0][(0, 0)][0]

    def read(self):
        """ Read data from the button. """
        return self._usb_device.read(self._usb_endpoint.bEndpointAddress,
                                     self._usb_endpoint.wMaxPacketSize,
                                     timeout=5000)


class USBListener(object):
    """ Represent a listener that reads data from an attached usb device and notifies of events. """

    def __init__(self, notify_callback, logger, button_vendor_id=0x16c0, button_product_id=0x0482, interface=0):
        """ Constructor.

            @param notify_callback: A callable that will be invoked when the pressed event is triggered by usb device.
            @param logger: A logger instance that event messages will be written too.
            @param button_vendor_id: The vendor id of the now serving button.
            @param button_product_id: The product id of the now serving button.
            @param interface: The interface to connect to the button with.
        """
        self.logger = logger
        self.last_command_time = None
        self.button_vendor_id = button_vendor_id
        self.button_product_id = button_product_id
        self.interface = interface
        self.usb_device = None
        self.usb_endpoint = None
        self.notify_callback = notify_callback

        try:
            self._initialize_usb_device()
        except NoUSBDeviceError:
            self.logger.log('error', 'No USB Device connected\n')
        else:
            self._start_deferred()

    def cleanup(self):
        """ Release resources to the attached USB device. """
        if self.usb_device:
            self.usb_device.cleanup()

    def read(self):
        """ Read for expected commands from the attached usb device. """
        raw_to_proper_commands = {"array('B', [0, 0, 62, 0, 0, 0, 0, 0])": 'print_next_ticket|default'}

        if not self.usb_device:
            time.sleep(2)

            try:
                self._initialize_usb_device()
            except NoUSBDeviceError:
                self.logger.log('error', 'USB Button not present or unplugged\n')
                return

        raw_command_key = self._poll_usb_device()

        if not raw_command_key:
            return

        if self.last_command_time:
            flood_filter = (datetime.now() - self.last_command_time).total_seconds() < 0.5
            self.last_command_time = datetime.now()
            if flood_filter:
                return

        proper_command_string = raw_to_proper_commands[str(raw_command_key)]
        self.last_command_time = datetime.now()
        return Command(text=proper_command_string)

    def notify_and_loop(self, command):
        """ Start an asynchronous usb read, notify of events, and loop.

            @param command: The command that was invoked.
        """
        if not self.usb_device:
            return

        if command:
            self.notify_callback(command)

        try:
            self._start_deferred()
        except Exception as err:
            self.logger.log_exc(err)
            self.notify_and_loop(None)

    def _poll_usb_device(self):
        """ Attempt to read from the usb device, returning any data read.

            @return: Data read from the usb device, if any.
        """
        try:
            with self.usb_device:
                raw_command_key = self.usb_device.read()
                self.logger.log('info', str(raw_command_key))
        except usb.core.USBError:
            return
        except NoUSBDeviceError:
            self.logger.log('error', 'USB Button not present or unplugged\n')
            time.sleep(2)
            return

        return raw_command_key

    def _initialize_usb_device(self):
        """ Initialize and attach our USBDevice manager. """
        self.usb_device = USBDevice(button_vendor_id=self.button_vendor_id,
                                    button_product_id=self.button_product_id,
                                    interface=self.interface)

    def _start_deferred(self):
        """ Start a twisted deferred in a thread that will read from the usb and notify of data received. """
        deferred = threads.deferToThread(self.read)
        deferred.addCallback(self.notify_and_loop)


class LicenseRequiredException(Exception):
    """ Raise an exception when a license is required to perform a function. """

    pass


class InvalidPickleError(Exception):
    """ Raise an exception when a python object can't be loaded from a pickle file. """

    pass


class NoTicketPrinterError(Exception):
    """ Raise an exception when there is no ticket printer attached. """

    pass


class NoTicketTemplateError(Exception):
    """ Raise an exception when there is no template to print a ticket with. """

    pass


class TicketPrintingError(Exception):
    """ Raise an exception where there is an error printing a ticket. """

    def __init__(self, stdout, stderr):
        """ Constructor.

            @param stdout: The stdout stream from the ticket print process.
            @param stderr: The stderr stream from the ticket print process.
        """
        Exception.__init__(self)

        self.msg = stdout + stderr

    def __str__(self):
        """ Return the stdout and stderr from the ticket print process. """
        return self.msg


class InvalidQueueNameError(Exception):
    """ Raise an exception when the queue name given doesn't exist. """

    def __init__(self, queue_name):
        """ Constructor.

            @param queue_name: The invalid queue name given.
        """
        Exception.__init__(self)

        self.queue_name = queue_name

    def __str__(self):
        """ Return an error message with the invalid queue name. """
        return "No queue named {queue_name}".format(queue_name=self.queue_name)


class NoUSBDeviceError(Exception):
    """ Raise an exception when no USB device is attached. """

    pass


class ShutdownException(Exception):
    """ Raise an exception when the now serving queue is shutdown. """

    pass


if __name__ == '__main__':
    manager = Manager()
    sys.exit(0)
