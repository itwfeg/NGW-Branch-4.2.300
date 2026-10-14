from django.contrib import admin

from LicenseKeyGenerator import models
from LicenseKeyGenerator import license_key


class FakeDataDictionary(object):

    def __init__(self, dictionary):
        self._dictionary = dictionary

    def __getitem__(self, key):
        return self._dictionary[key]

    def __setitem__(self, key, value):
        self._dictionary[key] = value

    def getlist(self, key):
        list_ = []

        if type(self._dictionary[key]) is str:
            return [self._dictionary[key]]

        for value in self._dictionary[key]:
            list_.append(value)

        return list_

    def __contains__(self, key):
        return key in self._dictionary


class FakeLicenseForm(object):

    def __init__(self, license_data):
        self.data = FakeDataDictionary(license_data)
        self.cleaned_data = license_data


def resend_emails(modeladmin, request, queryset):
    for license_data in queryset.values():

        email_address = (license_data['email_address'],)
        purchased = license_data['license_type_id'] == 1
        timeout = '0'

        if not purchased:
            timeout = license_data['timeout']

        license_text = license_key.generate_formatted_license(license_data)
        license_key.send_email(license_text, email_address, purchased=purchased, timeout=timeout)


resend_emails.short_description = "Resend Customer Emails"


def date_or_blank(obj):
    return obj.activation_date or ''

date_or_blank.short_description = 'Activation Date'
date_or_blank.admin_order_field = 'activation_date'


class LicenseAdmin(admin.ModelAdmin):
    search_fields = ('contact_name',)
    list_filter = ('activation_date', )
    ordering = ('-activation_date', )
    actions = (resend_emails, )
    list_display = ('contact_name', date_or_blank, 'email_address', 'type', 'sales_rep')


admin.site.register(models.License, LicenseAdmin)
admin.site.register(models.HTEProductName)
admin.site.register(models.ScaleProductName)
admin.site.register(models.HteSerialNumberType)
admin.site.register(models.ScaleSerialNumberType)
admin.site.register(models.TimeoutOption)
admin.site.register(models.LicenseType)
admin.site.register(models.FREDType)
admin.site.register(models.SupportMaintenance)
