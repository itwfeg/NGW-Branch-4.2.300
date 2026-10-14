import sys
sys.path.append('/var/www/django/shared/python/')

import csid
import license as license_
import re
import os

from datetime import datetime

from HTMLParser import HTMLParser

from django.core.mail import EmailMultiAlternatives

from LicenseKeyGenerator.models import HTEProductName
from LicenseKeyGenerator.models import HteSerialNumberType
from LicenseKeyGenerator.models import ScaleProductName
from LicenseKeyGenerator.models import ScaleSerialNumberType
from LicenseKeyGenerator.models import TimeoutOption


resp_indv_regex = re.compile("Responsible Individual:(?P<resp_indv>.*)")
company_name_regex = re.compile("Company Name:(?P<company_name>.*)")
company_address_regex = re.compile("Company Address:(?P<company_address>.*)")
email_address_regex = re.compile("Email Address:(?P<email_address>.*)")
phone_number_regex = re.compile("Phone Number:(?P<phone_number>.*)")
sales_rep_regex = re.compile("Sales Rep:(?P<sales_rep>.*)")
request_type_regex = re.compile("Request Type:(?P<request_type>.*)")
license_type_regex = re.compile("License Type Requested: \\[(?P<license_type>.*)\\]")
license_feature_regex = re.compile("License Feature\\(s\\) Requested: \\[(?P<license_features>.*)\\]")
num_licenses_regex = re.compile("Number of Licenses:(?P<num_licenses>.*)")
csid_tag_regex = re.compile("(?P<csid_tag>.*)=(?P<csid>.*)")


groups_to_regexes = {'resp_indv': resp_indv_regex,
                     'company_name': company_name_regex,
                     'company_address': company_address_regex,
                     'email_address': email_address_regex,
                     'phone_number': phone_number_regex,
                     'sales_rep': sales_rep_regex,
                     'request_type': request_type_regex,
                     'license_type': license_type_regex,
                     'license_features': license_feature_regex,
                     'num_licenses': num_licenses_regex,
                     'csid': csid_tag_regex,
                     }


class LicenseRequest(object):

    def __init__(self, request_text):
        self.resp_indv = None
        self.company_name = None
        self.company_address = None
        self.email_address = None
        self.phone_number = None
        self.sales_rep = None
        self.request_type = None
        self.license_type = None
        self.license_features = []
        self.num_licenses = None
        self.csid = None

        self.parse_request(request_text)

    def parse_request(self, request_text):
        for group, regex in groups_to_regexes.items():
            match = regex.search(request_text)

            if not match:
                continue

            text = match.group(group).strip()

            if not hasattr(self, group):
                continue

            if group == 'license_features':
                text = text.split(',')
                if '' in text:
                    text.remove('')

            setattr(self, group, text)

        license_type_match = groups_to_regexes['license_type'].search(request_text)
        if not license_type_match:
            # TODO: Raise an exception, catch it, and render an error on the page.
            return

        license_type = license_type_match.group('license_type')

        product_object = HTEProductName.objects.filter(internal_name=license_type)[0]

        self.license_features = product_object.id

    def as_dictionary(self):
        request_type_ids = {'HTE_DEMO': 'Customer Demo',
                            'HTE_PURCHASE': 'Customer Purchased'}

        return {'contact_name': self.resp_indv,
                'company_name': self.company_name,
                'company_address': self.company_address,
                'email_address': self.email_address,
                'phone_number': self.phone_number,
                'sales_rep': self.sales_rep,
                'license_type': request_type_ids.get(self.license_type, 'Customer Demo'),
                'type': 'HTE',
                'hte_product_names': self.license_features,
                'hte_serial_type': 'CSID',
                'serial_number': self.csid,
                'allotment': self.num_licenses}

    def __str__(self):
        return 'LicenseRequest(' + str(self.as_dictionary()) + ')'

    def __repr__(self):
        return str(self)


class InvalidProductNameSelection(Exception):
    pass


def today():
    date_time = datetime.now()
    return '{0:0<4}-{1:0>2}-{2:0>2}'.format(str(date_time.year), str(date_time.month), str(date_time.day))


def _lookup_product_names(license_data):
    if license_data['type'] == 'HTE':
        data_key = u'hte_product_names'
        model = HTEProductName

        if data_key not in license_data:
            data_key += '_id'

            if data_key not in license_data:
                raise InvalidProductNameSelection()

        product_records = [license_data[data_key], ]

    else:
        data_key = u'scale_product_names'
        model = ScaleProductName

        if data_key not in license_data:
            data_key += '_id'

            if data_key not in license_data:
                raise InvalidProductNameSelection()

        product_records = license_data[data_key]

    if len(product_records) == 1:
        feature_sets = {'Scale Management Lite': ('HTE_SCALEMGMT_LITE', ),
                        'Scale Management Pro': ('HTE_SCALEMGMT_SINGLESTORE', ),
                        'Scale Management Multi Store': ('HTE_SCALEMGMT_MULTISTORE', )}

        # HTe only has one product name ID now.
        product_name_record = product_records[0]

        if isinstance(product_name_record, basestring) or isinstance(product_name_record, int):
            # We were given the internal id of the record, not the record itself.
            query_results = model.objects.filter(id=product_name_record).values()

            if not query_results:
                raise InvalidProductNameSelection()

            product_name_record = query_results[0]

            return product_name_record['internal_name']

        else:
            return product_name_record.internal_name

    product_names = []

    for product_record in product_records:
        product_names.append(str(product_record.internal_name))

    return product_names


def _uppercase_license_data(license_data):
    for key, value in license_data.iteritems():
        if isinstance(value, basestring):
            license_data[key] = value.upper()

    return license_data


def generate_formatted_license(license_data):
    license_template = "Responsible Individual Full Name: {contact_name}\n" + \
                       "Company Name: {company_name}\n" + \
                       "Company Address: {company_address}\n" + \
                       "Product Name(s): {product_names}\n" + \
                       "License Key Type: {type}\n" + \
                       "License Activation Date: {activation_date}\n" + \
                       "License Allotment: {allotment}\n" + \
                       "License Timeout: {timeout}\n" + \
                       "License Key: {key}"

    license_data = license_data.copy()
    license_data = _uppercase_license_data(license_data)

    corrected_data = dict(**license_data)

    corrected_data['product_names'] = _lookup_product_names(license_data)
    corrected_data['activation_date'] = today()
    corrected_data['type'] = get_license_type(license_data)

    timeout_translations = {'No Timeout': 0,
                            '30 Days': 30,
                            '60 Days': 60,
                            '90 Days': 90}

    data_key = 'timeout'

    if data_key not in corrected_data:
        data_key += '_id'

    corrected_data['timeout'] = timeout_translations.get(str(corrected_data[data_key]), corrected_data[data_key])

    for key, value in corrected_data.items():
        corrected_data[key] = str(value)
        if type(value) is list or type(value) is tuple:
            corrected_data[key] = ','.join(value)

    license_data = license_template.format(**corrected_data)

    return license_data


def get_mac_and_serial(license_data):
    license_type = str(license_data['type'])

    if license_type == 'HTE':
        data_key = 'hte_serial_type'
    else:
        data_key = 'scale_serial_type'

    if data_key not in license_data:
        data_key += '_id'

    serial_type = license_data[data_key].internal_name
    serial_number = None

    if serial_type in ('csid', 'usb_serial_number', 'ip_address', 'hostname'):
        serial_number = license_data['serial_number']

    return str(serial_number)


def get_license_type(license_data):
    type_ = license_data['type']
    data_key = 'hte_serial_type'
    model = HteSerialNumberType

    if type_ == 'HTE':
        data_key = 'hte_serial_type'
        model = HteSerialNumberType

    elif type_ == 'SCALE':
        data_key = 'scale_serial_type'
        model = ScaleSerialNumberType

    translated_types = {'csid': 'CSID',
                        'hostname': 'SERVER_IP_ADDRESS',
                        'ip_address': 'SERVER_IP_ADDRESS',
                        'usb_serial_number': 'USB_SERIAL_NUMBER',
                        }

    if data_key not in license_data:
        data_key += '_id'

    serial_type_number = license_data[data_key]

    if isinstance(serial_type_number, basestring) or isinstance(serial_type_number, int):
        query_results = model.objects.filter(id=serial_type_number).values()
        serial_type = query_results[0]['internal_name']

    else:
        serial_type = serial_type_number.internal_name

    license_type = translated_types[str(serial_type)]

    return license_type


def generate_key(cleaned_data):
    license_data = cleaned_data.copy()
    parser = HTMLParser()
    gen_license_args = {'individualName': unicode(parser.unescape(cleaned_data['contact_name'].upper())),
                        'companyName': unicode(parser.unescape(cleaned_data['company_name'].upper())),
                        'address': unicode(parser.unescape(cleaned_data['company_address'].upper())),
                        'licenseTimeout': unicode(license_data.get('timeout', TimeoutOption.objects.filter(id=1)).timeout),
                        }

    serial_number = get_mac_and_serial(cleaned_data)

    gen_license_args['serialNumber'] = serial_number
    gen_license_args['licenseActivationDate'] = today()
    gen_license_args['productKey'] = license_.COMPANY_KEY
    gen_license_args['productNames'] = _lookup_product_names(cleaned_data)
    gen_license_args['licenseType'] = get_license_type(cleaned_data)

    license_key = license_.genLicenseKey(**gen_license_args)[1]

    return license_key


def replace_demo_timeout(html_body, text_body, timeout):
    timeout_regex = re.compile('Your demo copy will expire in 30')
    timeout_message = 'Your demo copy will expire in {timeout}'.format(timeout=timeout)
    html_replaced = timeout_regex.sub(timeout_message, html_body)
    text_replaced = timeout_regex.sub(timeout_message, text_body)

    return html_replaced, text_replaced


def send_email(license_data, email_address, purchased=True, timeout='0'):
    with open('/tmp/activation_license.lic', 'w') as license_file:
        license_file.write(license_data)

    license_dir = '/var/www/django/LicenseKeyGenerator/LicenseKeyGenerator/'
    if purchased:
        license_html = license_dir + 'license_letter.html'
        license_text = license_dir + 'license_letter.txt'

    else:
        license_html = license_dir + 'demo_license_letter.html'
        license_text = license_dir + 'demo_license_letter.txt'

    with open(license_html, 'r') as license_letter:
        html_body = license_letter.read().decode('windows-1252')

    with open(license_text, 'r') as license_letter:
        text_body = license_letter.read().decode('windows-1252')

    if not purchased:
        html_body, text_body = replace_demo_timeout(html_body, text_body, timeout)

    message = EmailMultiAlternatives('Welcome to HTe!',
                                     text_body,
                                     'HT.support@itwfeg.com',
                                     email_address)

    message.attach_file('/tmp/activation_license.lic')
    message.attach_alternative(html_body, 'text/html')
    message.send()

    os.remove('/tmp/activation_license.lic')
