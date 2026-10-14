import requests
import sys


def send_authorized_request(host):
    if host[-1] != '/':
        host += '/'

    url = host + "license/"
    data = {'authorized': 'True'}

    with open('test.html', 'wb') as html_file:
        html_file.write(requests.post(url, data=data).text)

    url = host + 'import/'
    data = {'authorized': 'True'}

    with open('test_import.html', 'wb') as html_file:
        html_file.write(requests.post(url, data=data).text)

if __name__ == '__main__':
    send_authorized_request(sys.argv[1])
