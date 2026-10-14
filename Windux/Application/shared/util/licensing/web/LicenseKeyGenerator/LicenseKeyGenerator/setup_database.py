import sys

from LicenseKeyGenerator.models import HTE_PRODUCT_NAMES
from LicenseKeyGenerator.models import SCALE_PRODUCT_NAMES
from LicenseKeyGenerator.models import HTEProductName
from LicenseKeyGenerator.models import ScaleProductName
from LicenseKeyGenerator.models import ScaleSerialNumberType
from LicenseKeyGenerator.models import HteSerialNumberType


def setup_hte_product_names():
    for internal_name, description in HTE_PRODUCT_NAMES:
        HTEProductName.objects.create(
            internal_name=internal_name, description=description)


def setup_scale_product_names():
    for internal_name, description in SCALE_PRODUCT_NAMES:
        ScaleProductName.objects.create(
            internal_name=internal_name, description=description)


def setup_scale_serial_types():
    scale_serial_types = (('usb_serial_number', 'USB Serial Number'),
                          ('ip_address', 'IP Address'),
                          ('hostname', 'Hostname'))

    for internal_name, description in scale_serial_types:
        ScaleSerialNumberType.objects.create(
            internal_name=internal_name, description=description)


def setup_hte_serial_types():
    hte_serial_types = (('csid', 'Computing System ID'), )

    for internal_name, description in hte_serial_types:
        HteSerialNumberType.objects.create(
            internal_name=internal_name, description=description)


def main(args):
    setup_hte_product_names()
    setup_scale_product_names()
    setup_scale_serial_types()
    setup_hte_serial_types()

    sys.exit(0)


if __name__ == '__main__':
    main(sys.argv)
