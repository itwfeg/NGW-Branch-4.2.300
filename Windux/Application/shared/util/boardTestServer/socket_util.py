
import socket


def dir_filter_protected(obj):
    """ Custom dir behavior that hides any method prefixed by an _
        as these are supposed to be private, and not accessed.
    """
    hidden_modifier = '_'
    shown_attributes = []

    # Do a dir on the type of object to avoid infinite recursion if this method is used in the __dir__ method of an
    # object
    for attribute in dir(type(obj)):
        if not attribute.startswith(hidden_modifier):
            shown_attributes.append(attribute)
            #
    return shown_attributes


class BaseSocket(socket.socket):
    """ BaseSocket is used as a layer over python's builtin socket.socket class, hiding all of the setup
        and initialization that is standard for our usage but not automatic.
    """

    def __init__(self, host, port, timeout=15):
        """ BaseSocket initializer.

            :param host: The IP or hostname the socket will use for all operations
            :type host: str

            :param port: The remote port the socket will use for all operations
            :type port: int

            :param timeout: Optional parameter that will specify the timeout value of the socket connection.
            :type timeout: int
        """
        super(BaseSocket, self).__init__(socket.AF_INET, socket.SOCK_STREAM)
        self.timeout = timeout
        self.host = host
        self.port = port

        if type(self.port) is not int:
            self.port = int(self.port)

        self._address = (self.host, self.port)
        self._bound = False
        self._connected = False

        self.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.settimeout(self.timeout)

    def update_address(self):
        """ Mostly an internal method, but must be called whenever our self.host or self.port are modified
            as we store a tuple representation of these two values that must be updated as well.
        """
        self._address = (self.host, self.port)

    def host(self, host=None):
        """ Dual purpose getter/setter method for self.host, optional parameter host will set the host if bool(host) == True
        """
        if host:
            self.host = host
            self.update_address()

        return self.host

    def port(self, port=None):
        """ Dual purpose getter/setter method for self.port, optional parameter port will set the port if bool(port) == True
        """
        if port:
            self.port = port
            self.update_address()

        return self.port

    def timeout(self, timeout=None):
        """ Dual purpose getter/setter method for self.timeout.

            optional parameter timeout will set the timeout if bool(timeout) == True
        """
        if timeout:
            self.timeout = timeout
            self.settimeout(self.timeout)

        return self.timeout

    def connect(self, source_host=None, source_port=None):
        """ Attempt to open the socket connection using the previously provided remote host and port.
            If source_host or source_port are given, python will attempt to bind to the source host
            and source port on the local machine.
        """
        if self._connected:
            return

        if source_host is not None and source_port is not None:
            if type(source_port) is not int:
                source_port = int(source_port)

            super(BaseSocket, self).bind((source_host, source_port))

        super(BaseSocket, self).connect(self._address)

        self._connected = True

    def bind(self):
        """ Attempt to bind to the previously configured host and port.
        """
        if self._bound:
            return

        super(BaseSocket, self).bind(self._address)

        self._bound = True

    def listen(self, backlog):
        """ Attempt to bind and listen on the local machine on the
            previously configured host and port
        """
        self.settimeout(self.timeout)
        super(BaseSocket, self).listen(backlog)

    def __enter__(self):
        return self

    def __exit__(self, exc_type, value, traceback):
        self.close()
        return

    def __dir__(self):
        return dir_filter_protected(self)


class SocketServer(BaseSocket):
    def __init__(self, port, timeout=15):
        host = ''
        BaseSocket.__init__(self, host, port, timeout)

        self.bind()
        self.listen(5)

    def run(self):
        input = (self, )
        outputs = ()
