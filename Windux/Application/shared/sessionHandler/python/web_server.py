#!/usr/bin/env python3

import os
import host_ip_address
import logging
import socket
import time
import subprocess


class Web2Py:

    def __init__(self,
                 application,
                 admin_password,
                 server_key_prefix_name,
                 server_certificate_life_in_years=10,
                 server_certificate_state='Ohio',
                 server_certificate_location='401 W Market Street, Troy Ohio 45373',
                 server_certificate_domain_name='hobartcorp.com',
                 server_certificate_business_unit='Hobart',
                 server_certificate_company_name="ITW",
                 server_certificate_country="US"):

        self.admin_password = admin_password
        self.web_server_key_prefix = server_key_prefix_name
        application = 'coronaPsu'
        certificate_components = {'country': server_certificate_country,
                                  'state': server_certificate_state,
                                  'location': server_certificate_location,
                                  'organization': server_certificate_company_name,
                                  'organization_unit': server_certificate_business_unit,
                                  'common_name': server_certificate_domain_name}

        self.certificate_life_in_years = server_certificate_life_in_years
        self.certificate_life_in_days = str(self.certificate_life_in_years * 365)

        self.certificate = \
            "/C=" + certificate_components['country'] + \
            "/ST=" + certificate_components['state'] + \
            "/L=" + certificate_components['location'] + \
            "/O=" + certificate_components['organization'] + \
            "/OU=" + certificate_components['organization_unit'] + \
            "/CN=" + certificate_components['common_name']

        self.web_server_location = os.path.join("/", "usr", "local", "hobart", application, "web2py")
        self.web_server_key = os.path.join(self.web_server_location, "%s.key" % self.web_server_key_prefix)
        self.web_server_crt = os.path.join(self.web_server_location, "%s.crt" % self.web_server_key_prefix)
        self.web_server_csr = os.path.join(self.web_server_location, "%s.csr" % self.web_server_key_prefix)

        #self.local_ip_address = host_ip_address.get_local_addr(remote=None,
        #                                                       ipv6=False)
        self.local_ip_address = None
        attempt = 0
        while self.local_ip_address is None and attempt < 60:
            self.local_ip_address = self.get_local_ip_address()
            time.sleep(1.0)
            attempt = attempt + 1

    def get_local_ip_address(self):
        proc = subprocess.Popen("ifconfig | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1'",
                                shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = proc.communicate()
        if proc.returncode != 0:
            return None
        else:
            value = output.strip()
            if value != '':
                return value
            else:
                return None

    def generate_certificate(self, on_every_boot=True):

        if not os.path.exists(self.web_server_crt) or on_every_boot:
            logging.info("generating web server certificate")
            # create ssl certificate
            os.system("/usr/bin/openssl genrsa -out %s 2048" % self.web_server_key)
            os.system(
                '/usr/bin/openssl req -new -key %s -out %s -subj "%s"' % (self.web_server_key,
                                                                          self.web_server_csr,
                                                                          self.certificate))
            os.system(
                '/usr/bin/openssl x509 -req -days %s -in %s -signkey %s -out %s' % (
                    self.certificate_life_in_days, self.web_server_csr, self.web_server_key, self.web_server_crt))
            logging.info("generated web server certificate")

    def delete_sessions(self):
        logging.info("deleting session files")
        os.system("/usr/bin/python %s/web2py.py -S PillarSync -M -R scripts/sessions2trash.py -A -o -x 0" %
                  self.web_server_location)

    def run(self):
        # start the web server
        logging.info("starting the web server")
        os.system(
            "/usr/bin/python %s/web2py.py -i %s -p 443 -a %s -c %s -k %s &" % (
                self.web_server_location,
                self.local_ip_address,
                self.admin_password,
                self.web_server_crt,
                self.web_server_key))
        logging.info("web server started")


if __name__ == '__main__':
    server = Web2Py(application='coronaPsu',
                    admin_password='itw1pillar2',
                    server_key_prefix_name='pillartech',
                    server_certificate_life_in_years=10,
                    server_certificate_state='Wisconsin',
                    server_certificate_location='475 Industrial Drive, Hartland WI 53029-0110',
                    server_certificate_business_unit='Pillar Technologies',
                    server_certificate_company_name="ITW",
                    server_certificate_country="US",
                    server_certificate_domain_name='pillartech.com')
    server.generate_certificate()
    server.delete_sessions()
    server.run()
