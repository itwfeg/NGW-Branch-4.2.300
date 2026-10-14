from django.core.validators import RegexValidator
from django.db import models


LICENSE_TYPES = (('HTE', 'HTe'),
                 ('SCALE', 'HTseries Scale'),
                 )


CUSTOM_CHAR_FIELD = lambda: models.CharField(max_length=200)

#classes editable by django Administrator
class HTEProductName(models.Model):
    description = models.CharField(max_length=200)
    internal_name = models.CharField(max_length=200)

    def __str__(self):
        return self.description


class ScaleProductName(models.Model):
    description = models.CharField(max_length=200)
    internal_name = models.CharField(max_length=200)

    def __str__(self):
        return self.description


class ScaleSerialNumberType(models.Model):
    description = models.CharField(max_length=200)
    internal_name = models.CharField(max_length=200)

    def __str__(self):
        return self.description


class HteSerialNumberType(models.Model):
    description = models.CharField(max_length=200)
    internal_name = models.CharField(max_length=200)

    def __str__(self):
        return self.description

class FREDType(models.Model):
    description = models.CharField(max_length=200)
    def __str__(self):
	return self.description

class SupportMaintenance(models.Model):
    description = models.CharField(max_length=200)
    def __str__(self):
	return self.description

class TimeoutOption(models.Model):
    description = models.CharField(max_length=200)
    timeout = models.IntegerField(default=0)

    def __str__(self):
        return self.description


class LicenseType(models.Model):
    description = models.CharField(max_length=200)

    def __str__(self):
        return self.description



INVALID_PHONE_MESSAGE = "Phone number must be entered in the format: " + \
                        "'+99 999-999-9999'. Up to 15 digits allowed."


class License(models.Model):
    company_name = models.CharField(max_length=200, verbose_name='Company Name')

    company_address = models.CharField(max_length=200, verbose_name='Company Address')

    contact_name = models.CharField(max_length=200, verbose_name='Contact Name')

    email_address = models.EmailField(max_length=254, verbose_name='Contact Email Address')

    phone_regex = RegexValidator(regex=r'^\+?((1)|(52))? ?\d{3}-?\d{3}-?\d{4}$', message=INVALID_PHONE_MESSAGE)

    phone_number = models.CharField(max_length=200, validators=[phone_regex], verbose_name='Contact Phone Number')

    sales_rep = models.CharField(max_length=200, verbose_name='Sales Representative')

    sales_rep_email_address = models.EmailField(max_length=254, verbose_name='Sales Rep Email Address')
    #ForeignKey is a django model that stores a number id in a sqlit3 database which corresponds to categories created in django Administrator
    fred_type = models.ForeignKey(FREDType, default=2, verbose_name='F.R.E.D.')

    activation_date = models.DateTimeField(auto_now_add=True, verbose_name='Activation Date')

    timeout = models.ForeignKey(TimeoutOption, default=2)

    allotment = models.IntegerField(default=1)

    license_type = models.ForeignKey(LicenseType, default=2, verbose_name='License Type')

    purchase_order_number = models.CharField(max_length=64, blank=True, verbose_name='Purchase Order Number')

    hte_serial_type = models.ForeignKey(HteSerialNumberType, default=1)

    scale_serial_type = models.ForeignKey(ScaleSerialNumberType, default=1)

    serial_number = models.CharField(max_length=200)

    key = models.CharField(max_length=200, verbose_name='License Key', blank=True)

    hte_product_names = models.ForeignKey(HTEProductName, verbose_name='HTe Feature Set', default=12)

    scale_product_names = models.ManyToManyField(ScaleProductName, blank=True, verbose_name='Scale Features')

    type = models.CharField(max_length=5, choices=LICENSE_TYPES, default='HTE', verbose_name='Target Device Type')

    license_file_data = models.TextField(blank=True)

    login_name = models.CharField(max_length=64)

    last_follow_up_email = models.DateTimeField(blank=True, null=True, auto_now_add=True, verbose_name='Last Follow-Up Email Date')

    follow_up_reminder = models.IntegerField(blank=True, null=True, verbose_name='Follow-Up\'s Remaining')

    demo_expire_date = models.DateTimeField(blank=True, verbose_name='Demo Expire Date')

    support_maintenance = models.ForeignKey(SupportMaintenance, default=1, verbose_name='Installation and Maintenance Options')

    po_file_name = models.CharField(max_length=200, blank=True, verbose_name='Product Order File Name')

    request_file_name = models.CharField(max_length=200, blank=True, verbose_name='Request File Name')

    license_file_name = models.CharField(max_length=200, blank=True, verbose_name='License File Name')

    def __str__(self):
        return self.contact_name + '-' + str(self.activation_date)
