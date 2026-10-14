
from fabric2 import Connection, exceptions
from patchwork.files import exists
from paramiko import Transport, SFTPClient
import paramiko
import invoke
# import logging


def file_exists(ip_address, filename, port=22, user='root', password='ho1bart2', connect_timeout=5):
    try:
        with Connection(ip_address, port=port, user=user, connect_kwargs={'password': password}) as c:
            if exists(c, filename):
                return True
    except:
        return False


def remote_background_command(ip_address, command, port=22, user='root', password='ho1bart2', log_file=None):
    status_code = 1
    pid = None
    if log_file:
        command = 'nohup %s > %s 2>&1 & echo $!' % (command, log_file)
    else:
        command = 'nohup %s > /dev/null 2>&1 & echo $!' % command

    try:
        with Connection(ip_address, port=port, user=user, connect_kwargs={'password': password}) as c:
            # logging.debug("starting run of background command")
            command_result = c.run(command, pty=False, hide=True)
            # logging.debug("completed run of background command")
            status_code = command_result.return_code
            if command_result.ok:
                pid = command_result.stdout.strip()
    except:
        pass

    return status_code, pid


def remote_file_put(ip_address, local_filename, remote_filename,
                    port=22, user='root', password='ho1bart2', preserve_mode=True, progress_callback=None):
    result = 1

    if ip_address:

        if progress_callback:

            try:
                # logging.debug("Using parmiko with callback to send file")
                # logging.debug("setting up transport to remote system")
                transport = Transport((ip_address, port))
                # logging.debug("setting up connect to remote system")
                transport.connect(username=user, password=password)
                # logging.debug("setting up client object to remote system")
                sftp_client = SFTPClient.from_transport(transport)
                # logging.debug("put file to remote system")
                sftp_client.put(local_filename, remote_filename, callback=progress_callback, confirm=True)
                # logging.debug("transmitted file to remote system")
                sftp_client.close()
                # logging.debug("closing client to remote system")
                transport.close()
                # logging.debug("closing transport to remote system")
                result = 0
            except:
                pass

        else:

            try:
                # logging.debug("Using fabric to send file")
                with Connection(ip_address, port=port, user=user, connect_kwargs={'password': password}) as c:
                    # logging.debug("transmitting file to remote system")
                    result = c.put(local=local_filename, remote=remote_filename, preserve_mode=preserve_mode)
                    # logging.debug("file transmitted to remote system")
                    result = 0
            except:
                pass

    return result


def remote_file_puta(ip_address, local_filename, remote_filename,
                    port=22, user='root', password='ho1bart2', preserve_mode=True, progress_callback=None):
    result = 1

    if ip_address:

        if progress_callback:

            try:
                ssh_client = paramiko.SSHClient()
                ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
                ssh_client.connect(hostname=ip_address, username=user, password=password, timeout=5)
                sftp_client = ssh_client.open_sftp()
                sftp_client.put(local_filename, remote_filename)
                sftp_client.close()
            except:
                pass

        else:

            try:
                # logging.debug("Using fabric to send file")
                with Connection(ip_address, port=port, user=user, connect_kwargs={'password': password}) as c:
                    # logging.debug("transmitting file to remote system")
                    result = c.put(local=local_filename, remote=remote_filename, preserve_mode=preserve_mode)
                    # logging.debug("file transmitted to remote system")
                    result = 0
            except:
                pass

    return result


def remote_command(ip_address, command, port=22, user='root', password='ho1bart2', connect_timeout=5):
    result = 1
    try:
        with Connection(host=ip_address,
                        port=port,
                        user=user,
                        connect_timeout=connect_timeout,
                        connect_kwargs={'password': password}) as c:
            # logging.debug("executing command on remote system")
            result = c.run(command)
            # logging.debug("command executed")
            # logging.debug("result = %s" % result)
            result = result.return_code
            #if result != 0:
            #   logging.debug("return code from running command is not 0")

    except invoke.exceptions.UnexpectedExit as e:
        return e.result.return_code
    except invoke.CommandTimedOut as e:
        return e.result.return_code
    return result
