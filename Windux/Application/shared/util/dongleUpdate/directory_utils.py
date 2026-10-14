import os
import requests
import shutil

from datetime import datetime

from bs4 import BeautifulSoup


DATETIME_FORMAT = '%d-%b-%Y %H:%M'


def computeFileSize(htmlSize):
    try:
        htmlSize = htmlSize.strip()
        if htmlSize[-1:] == 'K':
            return int(float(htmlSize[:-1]) * 1024.0)
        elif htmlSize[-1:] == 'M':
            return int(float(htmlSize[:-1]) * 1024.0 * 1024.0)
        else:
            return int(htmlSize)
    except:
        return 0

def getTimeStamp(fmt=DATETIME_FORMAT):
    return datetime.now().strftime(fmt)

def get_last_update(url):
    try:
        response = requests.get(url+'/last_update')
        response.raise_for_status()
        last_update = datetime.fromtimestamp(response.text).strftime(DATETIME_FORMAT)
        return last_update
    except:
        return None

def list_remote_files(url, lastCacheUpdate):
    try:
        lastServerUpdate = get_last_update(url)
        if lastServerUpdate > lastCacheUpdate:

            response = requests.get(url  + '/manifest')
            response.raise_for_status()

            ascii_data = response.text
            lines = ascii_data.split('\n')
            file_data = {}

            for i in range(1, len(lines)):
                dataItems = lines[i].split(',')
                filename = dataItems[0]
                last_modified = dataItems[1]
                size = int(dataItems[2])
                file_data[filename] = {'modified' : last_modified,  'size' : size}

            return file_data

    except:
        return None

def list_remote_directory(url, subdirectory=None):
    if not subdirectory:
        subdirectory = ''

    response = requests.get(url + subdirectory)
    response.raise_for_status()

    html_data = response.text
    html_soup = BeautifulSoup(html_data, 'html.parser')
    file_data = {}

    for table_row in html_soup.find_all('tr'):
        table_items = table_row.find_all('td')
        if not table_items:
            continue

        filename = table_items[1].text.rstrip(' ')
        last_modified = table_items[2].text.rstrip(' ')
        size = computeFileSize(table_items[3].text.rstrip(' '))

        if filename == 'Parent Directory':
            continue

        if subdirectory:
            filename = subdirectory.rstrip('/') + '/' + filename

        if filename[-1] == '/':
            file_data.update(list_remote_directory(url, subdirectory=filename))
            continue
        else:
            file_data[filename] = {'modified' : last_modified,  'size' : size}

#        if filename[-1:] == '/':
#            path = filename[:-1]
#            file_data.append({'path' : path, 'filename' : None, 'modified' : last_modified})
#            file_data = file_data + list_remote_directory(url + '/' + path)
#        else:
#            file_data.append({'path' : '', 'filename' : filename, 'modified' : last_modified})

    return file_data


def list_local_directory(directory_path):
    file_data = {}

    for (path, dirs, files) in os.walk(directory_path):
        relative_location = path.replace(directory_path, '').lstrip('/')

        for filename in files:
            filepath = os.path.join(path, filename)
            last_modified = datetime.fromtimestamp(os.path.getmtime(filepath)).strftime(DATETIME_FORMAT)
            relative_filepath = os.path.join(relative_location, filename).replace('\\', '/').lstrip('/')
            file_data[relative_filepath] = last_modified

#        last_modified = datetime.fromtimestamp(os.path.getmtime(path)).strftime(DATETIME_FORMAT)
#        file_data.append({'path' : path, 'filename' : None, 'modified' : last_modified})

#        for filename in files:
#            filepath = os.path.join(path, filename)
#            last_modified = datetime.fromtimestamp(os.path.getmtime(filepath)).strftime(DATETIME_FORMAT)
#            file_data.append({'path' : path, 'filename' : filename, 'modified' : last_modified})

    return file_data


def find_matching_dictionary(file_dictionary, file_data):
    for check_file_dictionary in file_data:
        file_location = file_dictionary['path']
        filename = file_dictionary['filename']

        if not file_location == file_dictionary['path']:
            continue
        elif not filename == file_dictionary['filename']:
            continue
        else:
            return check_file_dictionary

    return None


def delete_removed_files(local_file_data,  remote_file_data, home_path):
    files_removed = False

    for relative_filepath, local_data in local_file_data.iteritems():
        if relative_filepath not in remote_file_data:
            local_path = os.path.join(home_path,  relative_filepath.lstrip('\\/'))

            if os.path.isdir(local_path):
                shutil.rmtree(local_path)
                files_removed = True
            elif os.path.exists(local_path):
                os.remove(local_path)
                files_removed = True

            continue

    return files_removed


def find_new_files(local_file_data, remote_file_data):
    changed_file_data = {}

    for relative_filepath, remote_data in remote_file_data.iteritems():
        if relative_filepath not in local_file_data:
            changed_file_data[relative_filepath] = remote_data['modified']
            continue

        local_modified_string = local_file_data[relative_filepath]

        local_last_modified = datetime.strptime(local_modified_string, DATETIME_FORMAT)
        remote_last_modified = datetime.strptime(remote_data['modified'], DATETIME_FORMAT)

        if remote_last_modified > local_last_modified:
            changed_file_data[relative_filepath] = remote_last_modified

#    for remote_file_dictionary in remote_file_data
#        local_file_dictionary = find_matching_dictionary(remote_file_dictionary, local_file_data)
#
#        if not local_file_dictionary:
#            changed_file_data.append(remote_file_dictionary)
#            continue
#
#        local_last_modified = datetime.strptime(local_file_dictionary['modified'], DATETIME_FORMAT)
#        remote_last_modified = datetime.strptime(remote_file_dictionary['modified'], DATETIME_FORMAT)
#
#        if remote_last_modified > local_last_modified:
#            changed_file_data.append(remote_file_dictionary)

    return changed_file_data


def download_remote_file(url, download_filepath):
    status_code = 0
    try:
        file_data = requests.get(url, stream=True)
        file_data.raise_for_status()

        with open(download_filepath,  'wb') as download_file:
            for chunk in file_data.iter_content(chunk_size=512 * 1024):
                if chunk:
                    download_file.write(chunk)
    except:
        status_code = 1

    return status_code


if __name__ == '__main__':
    url = "http://wwengrel1.itwweighwrap.com/downloads/"
    # local_directory = "C:/Users/davisrs/AppData/Roaming/usbKeyUpdater"
    local_directory = "C:/Users/rober/AppData/Roaming/usbKeyUpdater"
    remote_data = list_remote_directory(url)
    local_data = list_local_directory()
    changed_data = find_new_files(local_data, remote_data)
    print(remote_data)
    print(local_data)
    print(changed_data)
#    directory_path = os.getcwd()
#   print list_local_directory(directory_path)
