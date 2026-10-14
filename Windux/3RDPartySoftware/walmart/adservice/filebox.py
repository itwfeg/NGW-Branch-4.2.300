#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import urlparse
import requests
import os
import shutil
from ConfigParser import SafeConfigParser
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import sys

sys.path.append('./../../../Application/shared/sessionHandler/python')
import md5checksum


root_folder = "filebox"
syncable = True


def set_syncable(flag):
    """
    Sets the syncable flag state of the Filebox server so requests can be made
    :param flag: True or False to set state
    :return: response from Filebox Server
    """
    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    set_syncable_request = configuration.get("SERVICE", "set_syncable_request")
    file_request_url = set_syncable_request.replace("${syncable}", str(flag).lower())

    response = requests.post(url=file_request_url,
                             headers={'accept': '*/*'},
                             timeout=(3.0,
                                      5.0))
    return response


def create_file(local_filename, filebox_directory_number, filebox_filename):
    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    response = None
    create_file_request = configuration.get("SERVICE", "create_file_request")
    file_request_url = create_file_request.replace("${directory_number}", filebox_directory_number)
    file_request_url = file_request_url.replace("${filename}", filebox_filename)
    if os.path.exists(local_filename):
        with open(local_filename, 'rb') as content:
            fs = os.fstat(content.fileno())
            json_header = {'Content-Type': 'application/octet-stream',
                           'Content-Length': str(fs.st_size)}
            response = requests.post(file_request_url, headers=json_header, data=content, stream=True, verify=False)
    else:
        response = 'Filename specified does not exist: %s' % local_filename
        print response

    return response


def get_list_of_active_files():
    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    get_list_of_active_files_request = configuration.get("SERVICE", "get_list_of_active_files_request")

    response = requests.get(url=get_list_of_active_files_request,
                            headers={'accept': '*/*'},
                            timeout=(3.0,
                                     5.0))

    file_list = []
    files = response.json()['pvFiles']
    for file_entry in files:
        file_list.append(file_entry['name'])

    return json.dumps(file_list)


def remove_file(directory_number, filename):

    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    remove_file_request = configuration.get("SERVICE", "remove_file_request")
    file_request_url = remove_file_request.replace("${directory_number}", directory_number)
    file_request_url = file_request_url.replace("${filename}", filename)

    response = requests.post(url=file_request_url,
                             headers={'accept': '*/*'},
                             timeout=(3.0,
                                      5.0))

    return response


def restore_file(directory_number, filename):

    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    remove_file_request = configuration.get("SERVICE", "restore_file_request")
    file_request_url = remove_file_request.replace("${directory_number}", directory_number)
    file_request_url = file_request_url.replace("${filename}", filename)

    response = requests.post(url=file_request_url,
                             headers={'accept': '*/*'},
                             timeout=(3.0,
                                      5.0))

    return response


def remove_all_files():
    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    remove_all_files_request = configuration.get("SERVICE", "remove_all_files_request")

    response = requests.post(url=remove_all_files_request,
                             headers={'accept': '*/*'},
                             timeout=(3.0,
                                      5.0))

    return response


def restore_all_files():
    configuration = SafeConfigParser()
    configuration.read("filebox_server.cfg")
    restore_all_files_request = configuration.get("SERVICE", "restore_all_files_request")

    response = requests.post(url=restore_all_files_request,
                             headers={'accept': '*/*'},
                             timeout=(3.0,
                                      5.0))
    return response


class Server(BaseHTTPRequestHandler):

    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def do_GET(self):

        global root_folder
        global syncable

        parsed = urlparse.urlparse(self.path)
        command_tokens = parsed.path.split("/")
        command = command_tokens[2]

        if command == "getListOfActiveFiles":

            self._set_headers()

            configuration = SafeConfigParser()
            configuration.read("filebox_server.cfg")

            if syncable:

                list_of_active_files = {'pvFiles': [], 'syncable': syncable}

                directory_tree = self.get_directory_structure(root_folder)
                resource_id = 0
                for directory_entry in directory_tree[root_folder].keys():
                    if os.path.isdir(os.path.join(root_folder, directory_entry)):
                        for file_or_folder in directory_tree[root_folder][directory_entry].keys():
                            if os.path.isfile(os.path.join(root_folder, directory_entry, file_or_folder)):
                                resource_id = resource_id + 1
                                list_of_active_files['pvFiles'].append({'directory': directory_entry,
                                                                        'id': str(resource_id),
                                                                        'name': file_or_folder,
                                                                        'hash': md5checksum.md5_for_file(
                                                                            os.path.join(root_folder,
                                                                                         directory_entry,
                                                                                         file_or_folder))})

                            elif os.path.isdir(os.path.join(root_folder, directory_entry, file_or_folder)):
                                print "Found a folder that is more than one level deep under root folder %s" % \
                                      root_folder
                                print "Ignoring folder named: %s" % file_or_folder

                    else:
                        print "Found a file named %s directly under %s folder. " \
                              "Needs to be under a directory number folder." % \
                              (root_folder, directory_entry)

            else:
                list_of_active_files = {'pvFiles': [], 'syncable': syncable}

            self.wfile.write(json.dumps(list_of_active_files))

        elif command == 'getFile':

            # self._set_headers()
            directory_number = command_tokens[3]
            file_name = command_tokens[4]
            with open(os.path.join(root_folder, directory_number, file_name), 'rb') as content:
                fs = os.fstat(content.fileno())
                self.send_response(200)
                self.send_header("Content-Type", 'application/octet-stream')
                self.send_header("Content-Disposition", 'attachment; filename="{}"'.format(os.path.basename(file_name)))
                self.send_header("Content-Length", str(fs.st_size))
                self.end_headers()
                shutil.copyfileobj(content, self.wfile)

    def do_POST(self):
        self._set_headers()

        global root_folder
        global syncable

        parsed = urlparse.urlparse(self.path)
        command_tokens = parsed.path.split("/")
        command = command_tokens[2]

        if command == 'removeFile':

            directory_number = command_tokens[3]
            file_name = command_tokens[4]

            response = {'directory_number': directory_number,
                        'name': file_name,
                        'status': 'error deleting'}

            try:

                if os.path.isfile(os.path.join(root_folder,
                                               directory_number,
                                               file_name)):

                    os.remove(os.path.join(root_folder,
                                           directory_number,
                                           file_name))

                    response = {'directory_number': directory_number,
                                'name': file_name,
                                'status': 'deleted'}

            except:
                pass

            self.wfile.write(json.dumps(response))

        elif command == 'setSyncable':

            response = {'status': 'error setting syncable flag, invalid flag specified'}

            syncable_flag = command_tokens[3].lower()

            if syncable_flag == 'true':
                syncable = True
                response = {'status': 'syncable flag set to true'}

            elif syncable_flag == 'false':
                syncable = False
                response = {'status': 'syncable flag set to false'}

            self.wfile.write(json.dumps(response))

        elif command == 'createFile':

            directory_number = command_tokens[3]
            file_name = command_tokens[4]

            response = {'directory_number': directory_number,
                        'name': file_name,
                        'status': 'error creating'}

            content_length = self.headers['Content-length']

            if not os.path.exists(os.path.join(root_folder, directory_number, file_name)):

                if not os.path.exists(os.path.join(root_folder, directory_number)):
                    os.mkdir(os.path.join(root_folder, directory_number), 0755)

                with open(os.path.join(root_folder, directory_number, file_name), 'wb') as content:
                    shutil.copyfileobj(self.rfile, content)
                    response = {'directory_number': directory_number,
                                'name': file_name,
                                'status': 'created file %s of size %s' % (os.path.join(root_folder,
                                                                                       directory_number,
                                                                                       file_name),
                                                                          content_length)}

            else:

                os.remove(os.path.join(root_folder, directory_number, file_name))

                response = {'directory_number': directory_number,
                            'name': file_name,
                            'status': 'file %s of size %s replaced' % (os.path.join(root_folder,
                                                                                    directory_number,
                                                                                    file_name),
                                                                       content_length)}

            self.wfile.write(json.dumps(response))

        elif command == 'removeAllFiles':

            try:
                directory_tree = self.get_directory_structure(root_folder)

                for directory_entry in directory_tree[root_folder].keys():

                    if os.path.isdir(os.path.join(root_folder, directory_entry)):

                        for file_or_folder in directory_tree[root_folder][directory_entry].keys():

                            if os.path.isfile(os.path.join(root_folder, directory_entry, file_or_folder)):

                                os.remove(os.path.join(root_folder,
                                                       directory_entry,
                                                       file_or_folder))

                            elif os.path.isdir(os.path.join(root_folder, directory_entry, file_or_folder)):
                                print "Found a folder that is more than one level deep under root folder %s" % \
                                      root_folder
                                print "Ignoring folder named: %s" % file_or_folder

                    else:
                        print "Found a file named %s directly under %s folder. " \
                              "Needs to be under a directory number folder." % \
                              (root_folder, directory_entry)

                response = {'name': root_folder,
                            'status': 'deleted'}

            except:

                response = {'name': root_folder,
                            'status': 'error removing all files'}

            self.wfile.write(json.dumps(response))

        elif command == 'restoreFile':

            directory_number = command_tokens[3]
            file_name = command_tokens[4]

            response = {'directory_number': directory_number,
                        'name': file_name,
                        'status': 'error restoring'}

            try:
                shutil.copyfile(os.path.join(".", "filebox_store", directory_number, file_name),
                                os.path.join(".", "filebox", directory_number, file_name))
                response = {'directory_number': directory_number,
                            'name': file_name,
                            'status': 'restored'}
            except:
                pass

            self.wfile.write(json.dumps(response))

        elif command == 'restoreAllFiles':

            response = {'name': root_folder,
                        'status': 'error restoring files'}

            try:
                shutil.rmtree(os.path.join(".", root_folder))
                shutil.copytree(os.path.join(".", "%s_store" % root_folder),
                                os.path.join(".", root_folder))
                if not os.path.exists(os.path.join(".", root_folder, "607")):
                    os.mkdir(os.path.join(".", root_folder, "607"), 0775)
                response = {'name': root_folder,
                            'status': 'restored'}
            except:
                pass

            self.wfile.write(json.dumps(response))

    def do_HEAD(self):
        self._set_headers()

    def get_directory_structure(self, root_directory='./%s' % root_folder):
        """
        Creates a nested dictionary that represents the folder structure of root_directory
        """
        directory = {}
        root_of_tree = root_directory.rstrip(os.sep)
        start = root_of_tree.rfind(os.sep) + 1
        for path, dirs, files in os.walk(root_folder):
            folders = path[start:].split(os.sep)
            subdir = dict.fromkeys(files)
            parent = reduce(dict.get, folders[:-1], directory)
            parent[folders[-1]] = subdir
        return directory


def run(server_class=HTTPServer, handler_class=Server, port=8000):

    global root_folder

    # create root folder for filebox files
    if not os.path.exists(os.path.join(".", root_folder)):
        os.mkdir(os.path.join(".", root_folder), 0755)

    # make empty 607 directory for testing of empty folder
    if not os.path.exists(os.path.join(".", root_folder, "607")):
        os.mkdir(os.path.join(".", root_folder, "607"), 0755)

    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print 'Starting File Box Simulation Server...'
    httpd.serve_forever()


if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
