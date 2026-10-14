import datetime
import hashlib
import string
import xml.etree.ElementTree as ET

import csid
import file_io
import getMacAddress
import getModelName
import hteVersionInfo
import scaleKeys
import usbQuery
import volumeInfo

VERSION = hteVersionInfo.LICENSE_KEY_GENERATOR_VERSION

COMPANY_KEY = "HoBaRt241@#@-_@%$#)(^Products"

HTE_PRODUCTS = \
    ["HTE_ALERTS",
     "HTE_FLASHKEY_DESIGNER",
     "HTE_LABEL_DESIGNER",
     "HTE_LEGACY_SUPPORT",
     "HTE_MEDIA_CONTENT_MANAGER",
     "HTE_REMOTE_MONITOR",
     "HTE_REMOTESCALEACCESS",
     "HTE_SCALEMGMT_LITE",
     "HTE_SCALEMGMT_MULTISTORE",
     "HTE_SCALEMGMT_SINGLESTORE",
     "HTE_USBKEYGEN"]

SCALE_PRODUCTS = \
    ["SCALE_AD_SERVICE",
     "SCALE_CAT3_AUDITTRAIL",
     "SCALE_CODE_CHECKER",                  # requires ARK hardware
     "SCALE_DIGIMARC",
     "SCALE_FPP",
     "SCALE_FULLSCREEN_FLASHKEYS",
     "SCALE_HANDSFREEDEVICE",               # requires HFPD hardware and feature to be installed
     "SCALE_INTERNATIONAL_ROUNDING",
     "SCALE_MEAL_BUILDER",
     "SCALE_NOW_SERVING",
     "SCALE_OFFICE_VIEWER",
     "SCALE_PDF_VIEWER",                    # requires feature to be installed
     "SCALE_PICK5",
     "SCALE_PRINTER_EPP",
     "SCALE_REMOTE_PRINTING",
     "SCALE_REMOTE_MONITOR",
     "SCALE_TRANS_GCD",
     "SCALE_WEB_BROWSER",
     "SCALE_ENHANCED_CENTERSCREEN",
     "SCALE_ONLINE_ORDERING",
     "SCALE_SELFSERVE_AUTO_REZERO"]

# For HTs scale these products are disabled. This means you must license them
# in order to use them. For HTi and above they are included automatically.
HTS_DISABLED_PRODUCTS = \
    ["SCALE_FPP",
     "SCALE_FULLSCREEN_FLASHKEYS",
     "SCALE_TRANS_GCD",
     "SCALE_WEB_BROWSER"]

# For HTm scale these products are enabled. This means if you have an HTm scale
# you do not need to license them in order to use them (you still need to
# install apps or whatnot). For HTi and below you must license them in order to
# use them.
HTM_ENABLED_PRODUCTS = \
    ["SCALE_CAT3_AUDITTRAIL",
     "SCALE_CODE_CHECKER",
     "SCALE_MEAL_BUILDER",
     "SCALE_OFFICE_VIEWER",
     "SCALE_PDF_VIEWER",
     "SCALE_PICK5",
     "SCALE_PRINTER_EPP"]


PRODUCTS = HTE_PRODUCTS + SCALE_PRODUCTS

KEY_TYPES = ["CSID", "USB_SERIAL_NUMBER", "SERVER_IP_ADDRESS"]
PROMPTS = {'individual name': "Responsible Individual Full Name",
           'company name': "Company Name",
           'company address': "Company Address",
           'product names': "Product Name(s)",
           'license type': 'License Key Type',
           'license activation date': 'License Activation Date',
           'license activated': 'License Activated',
           'license allotment': "License Allotment",
           'license timeout': "License Timeout",
           'license key': "License Key",
           }
HOBART_LICENSE_FILENAME = "itwhobart.lic"
NULL_MAC_ADDRESS = "000000000000"


def getLicensedFeatures():
    flashXmlBuffer = readFlashKeyBuffer()

    if not flashXmlBuffer:
        return {}

    element_tree = ET.fromstring(flashXmlBuffer)
    licensed_features = {}

    for license_element in element_tree.iter('license'):
        for product_element in license_element.iter('product'):
            feature_name = product_element.get('name')
            responsible_individual = license_element.find('responsible_individual_name').text
            license_type = license_element.find('type').text
            activation_date = license_element.find('activation_date').text
            allotment = license_element.find('allotment').text
            timeout = license_element.find('timeout').text

            company_element = license_element.find('company')
            company_data = {}
            company_data['name'] = company_element.find('name').text.replace('\n', ' - ').replace('\r', '')
            company_data['address'] = company_element.find('address').text.replace('\n', ' - ').replace('\r', '')

            licensed_features[feature_name] = {'name': feature_name,
                                               'responsible_individual': responsible_individual,
                                               'license_type': license_type,
                                               'activation_date': activation_date,
                                               'allotment': allotment,
                                               'timeout': timeout,
                                               'company': company_data,
                                               }

    return licensed_features


def readFlashKeyBuffer():
    try:
        # read the keys from flash
        status, data = scaleKeys.readAllKeys()
        
        if status != 'success':
            return None

        if data != None:
            data = data.rstrip(b'\xff').decode('utf-8')
            
            # validate buffer contains xml
            if len(data) <= 0 or '<' != data[0]:
                return None
                
        return data

    except:
        pass

    return None


def writeFlashKeyBuffer(xmlLicenseBuffer):
    # write the license buffer to flash
    return scaleKeys.writeAllKeys(xmlLicenseBuffer)


def eraseFlashKeyBuffer():
    return scaleKeys.eraseAllKeys()


def xmlCreateInitialLicenseBuffer():
    hobartEl = ET.Element("hobart")
    return ET.tostring(hobartEl)


def isProductLicensed(xmlLicenseBuffer, feature):
    # check if feature is a feature only disabled on an HTs
    # but a core feature on all other scale lines
    # if the scale model is not an HTs

    model = getModelName.getModelName()

    # FT-1741
    if model and len(model) > 3:
        model = model[:3]

    if model != 'HTs':
        # if the feature being checked is typically a core feature except on HTs
        if feature in HTS_DISABLED_PRODUCTS:
            # do not check license since this is a core product on all other
            # scale lines so product is licensed
            return True

    if model == 'HTm':
        if feature in HTM_ENABLED_PRODUCTS:
            return True

    # SOF-5049: Enable CAT3 on FSSS scales
    if model == 'FS-' and feature == 'SCALE_CAT3_AUDITTRAIL':
        return True

    # check for license in XML license buffer that typically comes from flash
    # on system controller board
    if xmlLicenseBuffer != None:
        hobartEl = ET.fromstring(xmlLicenseBuffer)
        for license in hobartEl.iter('license'):
            foundProduct = False
            for product in license.iter('product'):
                if str(product.get('name')) == feature:
                    foundProduct = True
                    break
            if foundProduct:
                try:
                    timeout = int(str(license.find('timeout').text))
                    # print timeout
                    # print str(license.find('activation_date').text)
                    if timeout != 0:
                        if not licenseExpired(str(license.find('activation_date').text), int(timeout)):
                            return True
                    else:
                        return True
                except:
                    return True
    return False


def xmlStringToElementTree(xmlLicenseBuffer):
    return ET.fromstring(xmlLicenseBuffer)


def xmlAddLicenseToBuffer(xmlLicenseBuffer, individualName, companyName, companyAddress, productNames, licenseType,
                          licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey):
    hobartEl = ET.fromstring(xmlLicenseBuffer)
    licenseEl = ET.SubElement(hobartEl, 'license')
    individualNameEl = ET.SubElement(licenseEl, "responsible_individual_name")
    individualNameEl.text = individualName
    companyEl = ET.SubElement(licenseEl, "company")
    companyNameEl = ET.SubElement(companyEl, "name")
    companyNameEl.text = companyName
    companyAddressEl = ET.SubElement(companyEl, "address")
    companyAddressEl.text = companyAddress
    for i in range(0, len(productNames)):
        productEl = ET.SubElement(licenseEl, 'product')
        productEl.set("name", productNames[i])
    licenseTypeEl = ET.SubElement(licenseEl, "type")
    licenseTypeEl.text = licenseType
    licenseActivationDateEl = ET.SubElement(licenseEl, "activation_date")
    licenseActivationDateEl.text = licenseActivationDate
    licenseAllotmentEl = ET.SubElement(licenseEl, "allotment")
    licenseAllotmentEl.text = licenseAllotment
    licenseTimeoutEl = ET.SubElement(licenseEl, "timeout")
    licenseTimeoutEl.text = licenseTimeout
    licenseKeyEl = ET.SubElement(licenseEl, "key")
    licenseKeyEl.text = licenseKey
    return ET.tostring(hobartEl)


def xmlPrettyPrint(rootElement):
    indent(rootElement)
    ET.dump(rootElement)


def indent(elem, level=0, more_sibs=False):
    i = "\n"
    if level:
        i += (level - 1) * '  '
    num_kids = len(elem)
    if num_kids:
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
            if level:
                elem.text += '  '
        count = 0
        for kid in elem:
            indent(kid, level + 1, count < num_kids - 1)
            count += 1
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
            if more_sibs:
                elem.tail += '  '
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i
            if more_sibs:
                elem.tail += '  '


def getProductList():
    return PRODUCTS


def readLicenseFile(licenseFilename):
    status = 'error'
    data = None
    try:
        status, data = file_io.readFromFile(licenseFilename)
    except:
        pass
    return status, data


def createLicenseFile(licenseKeyFilename, individualName, companyName, address, productNames, licenseType,
                      licenseActivationDate, licenseAllotment, licenseTimeout, macAddress=NULL_MAC_ADDRESS, serialNumber=None,
                      licenseKey=None):
    if licenseKey is None:
        licenseActivationDate = datetime.date.today()
        licenseActivationDate = licenseActivationDate.strftime("%Y-%m-%d")

        retVal, licenseKey = genLicenseKey(individualName,
                                           companyName,
                                           address,
                                           productNames,
                                           licenseType,
                                           licenseActivationDate,
                                           licenseTimeout,
                                           macAddress,
                                           serialNumber,
                                           COMPANY_KEY)
    else:
        retVal = 'success'

    if retVal == 'success':
        license_data = []

        license_data.append(PROMPTS['individual name'] + ': ' + individualName.upper())
        license_data.append(PROMPTS['company name'] + ': ' + companyName.upper())
        license_data.append(PROMPTS['company address'] + ': ' + address.upper())
        license_data.append(PROMPTS['product names'] + ': ' + ",".join(productNames))
        license_data.append(PROMPTS['license type'] + ': ' + licenseType)
        license_data.append(PROMPTS['license activation date'] + ': ' + licenseActivationDate)
        license_data.append(PROMPTS['license allotment'] + ': ' + licenseAllotment)
        license_data.append(PROMPTS['license timeout'] + ': ' + licenseTimeout)
        license_data.append(PROMPTS['license key'] + ': ' + licenseKey)

        retVal, output = file_io.writeToFile(filename=licenseKeyFilename, data=license_data)

        if retVal == 'success':
            status = ('success', '', license_data)

        else:
            status = ('error', 'Could not write key file', None)

    else:
        status = ('error', 'Could not generate license key value', None)

    return status


def getLicenseElements(licenseFilename):
    individualName = None
    companyName = None
    companyAddress = None
    productNames = None
    licenseType = None
    licenseActivationDate = None
    licenseAllotment = None
    licenseTimeout = None
    licenseKey = None

    status, data = readLicenseFile(licenseFilename)

    if status == 'success':
        for i in range(0, len(data)):
            line = data[i].strip().split(':')
            if len(line) == 2:
                if line[0] == PROMPTS['individual name']:
                    individualName = line[1].strip()
                elif line[0] == PROMPTS['company name']:
                    companyName = line[1].strip()
                elif line[0] == PROMPTS['company address']:
                    companyAddress = line[1].strip()
                elif line[0] == PROMPTS['product names']:
                    productNames = line[1].strip().upper().split( ',')
                elif line[0] == PROMPTS['license type']:
                    licenseType = line[1].strip()
                elif line[0] == PROMPTS['license activation date']:
                    licenseActivationDate = line[1].strip()
                elif line[0] == PROMPTS['license allotment']:
                    licenseAllotment = line[1].strip()
                elif line[0] == PROMPTS['license timeout']:
                    licenseTimeout = line[1].strip()
                elif line[0] == PROMPTS['license key']:
                    licenseKey = line[1].strip()

    return (status, individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate,
            licenseAllotment, licenseTimeout, licenseKey)


def containsScaleProducts(productNames):
    for i in range(0, len(productNames)):
        if getFeatureType(productNames[i]) == 'scale':
            return True
    return False


def getFeatureType(feature):
    if feature in SCALE_PRODUCTS:
        return 'scale'
    return 'hte'


def licenseExpired(licenseActivationDate, timeout=0):
    if timeout != 0:
        today = datetime.datetime.today()
        # get creation date of license file
        expirationDate = datetime.datetime.strptime(licenseActivationDate, "%Y-%m-%d") + datetime.timedelta(timeout)
        if today <= expirationDate:
            return False
        else:
            return True
    else:
        return False


def getSerialNumberForCSID():
    exitCode, serialNumber = volumeInfo.getVolumeInfo("C:\\")

    if exitCode != 0:
        raise ValueError("Could not get drive info for C:")

    return serialNumber


def getSerialNumberFromThumbDrive(licenseFilename):
    usbDevices = usbQuery.UsbDeviceQuery()
    devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)

    if not devices:
        raise ValueError("No attached thumb drives to get serial number from!")

    driveLetter = licenseFilename[:licenseFilename.index(":") + 1]

    for device in devices:
        deviceLetter = usbDevices.getDeviceLetter(device)

        if deviceLetter == driveLetter:
            return usbDevices.getSerialNumber(device)

    raise ValueError("No attached thumb drives matched drive letter location of license key: {0}".format(driveLetter))


def validLicenseKey(licenseFilename, productName, macAddress=NULL_MAC_ADDRESS, serialNumber=None):
    try:
        # get elements of the license file
        status, individualName, companyName, companyAddress, productNames, licenseType, \
            licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey = getLicenseElements(licenseFilename)

        if not status == 'success':
            return False

        if productName.upper() not in productNames:
            return False

        if not serialNumber:
            if licenseType == 'CSID':
                try:
                    serialNumber = getSerialNumberForCSID()
                except ValueError:
                    return False
            elif licenseType == "USB_SERIAL_NUMBER":
                try:
                    serialNumber = getSerialNumberFromThumbDrive(licenseFilename)
                except ValueError:
                    return False

        status, generatedLicense = genLicenseKey(individualName, companyName, companyAddress, productNames,
                                                 licenseType, licenseActivationDate, licenseTimeout,
                                                 macAddress, serialNumber, COMPANY_KEY)
        if not status == 'success':
            return False

        # if generated license key in license file
        return generatedLicense == licenseKey and not licenseExpired(licenseActivationDate, int(licenseTimeout))
    except:
        return False


def genLicenseKey(individualName, companyName, address,
                  productNames, licenseType, licenseActivationDate,
                  licenseTimeout, macAddress, serialNumber, productKey):
    if individualName is not None and companyName is not None \
            and address is not None and productNames is not None \
            and macAddress is not None and serialNumber is not None \
            and licenseTimeout is not None and productKey is not None \
            and licenseActivationDate is not None and licenseType is not None:

        m = hashlib.sha1()
        m.update(individualName.encode("utf-8"))
        m.update(companyName.encode("utf-8"))
        m.update(address.encode("utf-8"))

        for i in range(0, len(productNames)):
            m.update(productNames[i].encode("utf-8"))

        # early licenses did not have allotment in them so if no allotment found in license key
        # then do not compute in key since assumption is 1 allotment
        # we do not incorporate number of licenses into computation of key

        m.update(licenseType.encode("utf-8"))
        m.update(licenseActivationDate.encode("utf-8"))
        m.update(licenseTimeout.encode("utf-8"))
        m.update(macAddress.encode("utf-8"))
        m.update(serialNumber.encode("utf-8"))
        m.update(productKey.encode("utf-8"))

        return 'success', m.hexdigest().upper()
    else:
        return 'error', None


if __name__ == "__main__":
    a = xmlCreateInitialLicenseBuffer()
    a = xmlAddLicenseToBuffer(a, "Scott", "ITW", "401 W Market St", ['SCALE_OFFICE_VIEWER'], 'SCALE', '2014-02-27', '1',
                              '30', '2asdfasdffasdf3aqe')
    a = xmlAddLicenseToBuffer(a, "Todd", "ITW", "401 W Market St", ['SCALE_PDF_VIEWER'], 'SCALE', '2014-02-10', '1',
                              '1', '2asdfasdffasdf3aqe')
    isProductLicensed(a, 'SCALE_PDF_VIEWER')

#    parser = argparse.ArgumentParser(description="Software product license key generator, contact HT.support@itwfeg.com for details.")
#    parser.add_argument("--individualName",
#                        help="Enter responsible individual's full name associated with license")
#    parser.add_argument("--companyName",
#                        help="Enter customer's company name associated with license")
#    parser.add_argument("--address",
#                        help="Enter customer's company address associated with license")
#    parser.add_argument("--productnames",
#                        help="Enter product name(s) separated with a comma associated with the license, %s" % (string.join(PRODUCTS)))
#    parser.add_argument("--licenseType",
#                        help="Enter type of license, valid values are %s" % (string.join(KEY_TYPES)))
#    parser.add_argument("--allotment",
#                        help="Enter number of licenses")
#    parser.add_argument("--timeout",
#                        help="Enter license timeout in days")
#    parser.add_argument("--csid",
#                        help="Enter output from customer executing csid command on target licensed computing system")
#
#    args = parser.parse_args()
#    if not args.individualName and not args.companyName and not args.address and not args.productnames and not args.allotment and not args.timeout and not args.csid:
#        individualName, companyName, address, productnames, activationDate, allotment, timeout, macAddress,  serialNumber = getLicenseKeyInfo()
#    else:
#        individualName = string.upper(string.strip(args.individualName))
#        companyName = string.upper(string.strip(args.companyName))
#        address = string.upper(string.strip(args.address))
#        products = string.upper(string.strip(args.productnames))
#        productnames = string.split(products)
#        licenseType = string.upper(string.strip(args.licenseType))
#        activationDate = datetime.date.today()
#        activationDate = activationDate.strftime("%Y-%m-%d")
#        allotment = string.strip(args.allotment)
#        timeout = string.strip(args.timeout)
#        macAddress,  serialNumber = csid.extractCSInfo(args.csid)
#
#    status,  licenseKey = genLicenseKey(individualName,
#                                companyName,
#                                address,
#                                productnames,
#                                licenseType,
#                                activationDate,
#                                timeout,
#                                macAddress,
#                                serialNumber,
#                                COMPANY_KEY)
#
#    if status == 'success':
#        license_data = []
#        license_data.append(PROMPTS['individual name'] + ': ' + individualName)
#        license_data.append(PROMPTS['company name'] + ': ' + companyName)
#        license_data.append(PROMPTS['company address'] + ': ' + address)
#        license_data.append(PROMPTS['product names'] + ': ' + string.join(productnames, ','))
#        license_data.append(PROMPTS['license type'] + ': ' + licenseType)
#        license_data.append(PROMPTS['license allotment'] + ': ' + allotment)
#        license_data.append(PROMPTS['license activation date'] + ': ' + activationDate)
#        license_data.append(PROMPTS['license timeout'] + ': ' + timeout)
#        license_data.append(PROMPTS['license key'] + ': ' + licenseKey)
#        status,  output = file_io.writeToFile(filename=HOBART_LICENSE_FILENAME,data=license_data)
#        if status == 'success':
#            print "Product(s) License Key File Created"
#        else:
#            print 'Error creating product license key'
#    else:
#        print 'Error generating product license key'
#
#    # test if valid license key produced for machine license key was generated on
#    if validLicenseKey(HOBART_LICENSE_FILENAME,'HTE_SCALEMGMT'):
#        print 'Valid license key'
#    else:
#        print 'Invalid license key'
