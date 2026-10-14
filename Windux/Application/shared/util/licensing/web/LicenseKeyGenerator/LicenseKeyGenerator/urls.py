from django.conf.urls import patterns, include, url
from django.contrib import admin


urlpatterns = patterns('',
                       url(r'^admin/', admin.site.urls),
                       url(r'^license/', 'LicenseKeyGenerator.views.license_page'),
                       url(r'^import/', 'LicenseKeyGenerator.views.import_request'),
                       url(r'^send_email/', 'LicenseKeyGenerator.views.send_email')
                       )
