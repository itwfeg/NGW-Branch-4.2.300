import os

from django.forms import FileField
from django.forms import Form
from django.forms import ModelForm
from django.forms import ValidationError
from django.forms.widgets import SelectMultiple
from django.forms.widgets import TextInput
from django.forms.widgets import Widget
from django.utils.html import format_html, escape
from datetime import timedelta
from datetime import datetime
from LicenseKeyGenerator import license_key
from LicenseKeyGenerator.models import License
from LicenseKeyGenerator.models import LicenseType
from LicenseKeyGenerator.models import TimeoutOption


CHAR_FIELD_WIDGET = lambda: TextInput(attrs={'class': 'char_field'})


class Label(Widget):
    def __init__(self, **kwargs):
        super(Label, self).__init__(**kwargs)

    def render(self, name, value, attrs=None):
        opening_tag = '<span'

        if attrs.get('class', False):
            opening_tag += ' class="{name}"'.format(name=attrs['class'])

        if attrs.get('id', False):
            opening_tag += ' id="{id}"'.format(id=attrs['id'])

        opening_tag += '>'
        closing_tag = '</span>'
        hidden_input = '<input type="hidden" name="{name}" value="{value}">'

        return format_html(opening_tag + '{text}' + closing_tag + hidden_input,
                           text=escape(value), name=name, value=value)


class LicenseForm(ModelForm):
    class Meta:
        model = License
        fields = '__all__'
        widgets = {'scale_product_names': SelectMultiple,
                   'contact_name': CHAR_FIELD_WIDGET(),
                   'company_name': CHAR_FIELD_WIDGET(),
                   'company_address': CHAR_FIELD_WIDGET(),
                   'allotment': CHAR_FIELD_WIDGET(),
                   'sales_rep': CHAR_FIELD_WIDGET(),
                   'phone_number': CHAR_FIELD_WIDGET(),
                   'email_address': CHAR_FIELD_WIDGET(),
                   'sales_rep_email_address': CHAR_FIELD_WIDGET(),
                   'po_file_name': CHAR_FIELD_WIDGET(),
                   'license_file_name': CHAR_FIELD_WIDGET(),
                   'request_file_name': CHAR_FIELD_WIDGET(),
                   'purchase_order_number': CHAR_FIELD_WIDGET(),
                   'login_name': Label(attrs={'class': 'char_field'}),
                   }

    def clean(self):
        cleaned_data = super(LicenseForm, self).clean()
        license_type = cleaned_data.get('license_type', LicenseType.objects.filter(id=1).values()[0])

        if license_type.description in ('Customer Purchased', 'Hobart Salesman'):
            purchase_order_given = cleaned_data.get('purchase_order_number', False)
            if not purchase_order_given:
                error = ValidationError('Purchase Order Number is required for a customer purchase.', code='3')
                self.add_error('purchase_order_number', error)
                #if license is purchased, set follow up emails to 0
                cleaned_data['follow_up_reminder'] = 0
        elif license_type.description == 'Customer Demo':
            timeout_object = cleaned_data.get('timeout', None)
            #If license is demo, set a follow up per 30 days with a maximum of 3 follow ups
            if not timeout_object or timeout_object.timeout < 30:
                error = ValidationError('Timeout must be at least 30 days for a demo license')
                self.add_error('timeout', error)
            elif timeout_object.timeout < 60:
                cleaned_data['follow_up_reminder'] = 1
            elif timeout_object.timeout < 90:
                cleaned_data['follow_up_reminder'] = 2
            else:
                cleaned_data['follow_up_reminder'] = 3

        cleaned_data['key'] = self.generate_key(cleaned_data)

        #adjust demo expire date to be current date + timeout days
        timeout_time = cleaned_data.get('timeout', None)
        timedelta_timeout = timedelta(days=timeout_time.timeout)
        current_time = datetime.today()
        cleaned_data['demo_expire_date'] = current_time + timedelta_timeout

        return cleaned_data

    def generate_key(self, cleaned_data):
        key = None

        if self.errors:
            return key

        try:
            key = license_key.generate_key(cleaned_data)
        except license_key.InvalidProductNameSelection:
            type_ = self.data['type']
            error = ValidationError('No product names selected.', code='13')

            if type_ == 'HTE':
                self.add_error('hte_product_names', error)

            elif type_ == 'SCALE':
                self.add_error('scale_product_names', error)

        return key

    @staticmethod
    def cleaned_dictionary(cleaned_data):
        return {'individualName': cleaned_data['contact_name'],
                'companyName': cleaned_data['company_name'],
                'address': cleaned_data['company_address'],
                'licenseTimeout': str(cleaned_data.get('timeout', TimeoutOption.objects.filter(id=1)).timeout),
                'licenseType': cleaned_data['type']}


class LicenseRequestForm(Form):
    request_file = FileField()

    def clean_file(self):
        request_file = self.cleaned_data['request_file']

        raw_size = os.fstat(request_file.fileno()).st_size
        kilobytes = raw_size / 1024
        megabytes = kilobytes / 1024
        if megabytes > 15:
            raise ValidationError('Request Files cannot be larger than 15MB, please ensure you selected the right file',
                                  code='1')

        request_text = request_file.read()

        for group, regex in license_key.groups_to_regexes.items():
            match = regex.search(request_text)
            if not match.group(group):
                raise ValidationError('Request File is missing required data: {group}'.format(group=group), code='1')
