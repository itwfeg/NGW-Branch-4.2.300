import csid
import os
import sys
import string
import hashlib
import file_io
import getModelName
import volumeInfo
import argparse
import getMacAddress
import time
import datetime
import xml.etree.ElementTree as ET
import scaleKeys

import hteVersionInfo

VERSION = hteVersionInfo.LICENSE_KEY_GENERATOR_VERSION

COMPANY_KEY = "HoBaRt241@#@-_@%$#)(^Products"

HTE_PRODUCTS = \
    ["HTE_USBKEYGEN",
    "HTE_SCALEMGMT_SINGLESTORE",
    "HTE_SCALEMGMT_MULTISTORE",
    "HTE_SCALEMGMT_LITE",
    "HTE_ALERTS",
    "HTE_LABEL_DESIGNER",
    "HTE_MEDIA_CONTENT_MANAGER",
    "HTE_FLASHKEY_DESIGNER",
    "HTE_REMOTE_MONITOR",
    "HTE_REMOTESCALEACCESS",
    "HTE_LEGACY_SUPPORT"]

HTE_PRODUCT_NAMES = (('HTE_USBKEYGEN', 'USB Key Generator'),
                     ('HTE_SCALEMGMT_SINGLESTORE', 'Scale Management Single Store'),
                     ('HTE_SCALEMGMT_MULTISTORE', 'Scale Management Multi Store'),
                     ('HTE_SCALEMGMT_LITE', 'Scale Management Lite'),
                     ('HTE_ALERTS', 'Scale Management Alerts'),
                     ('HTE_LABEL_DESIGNER', 'Scale Management Label Designer'),
                     ('HTE_MEDIA_CONTENT_MANAGER', 'Scale Management Media Content Management'),
                     ('HTE_FLASHKEY_DESIGNER', 'Flashkey Designer'),
                     ('HTE_REMOTE_MONITOR', 'Scale Management Remote Monitor'),
                     ('HTE_REMOTESCALEACCESS', 'Remote Scale Access'),
                     ('HTE_LEGACY_SUPPORT', 'Scale Management Legacy Support'),
                     )

SCALE_PRODUCTS = \
    ["SCALE_OFFICE_VIEWER",
    "SCALE_PDF_VIEWER",
    "SCALE_REMOTE_MONITOR",
    "SCALE_NOW_SERVING",
    ]

SCALE_PRODUCT_NAMES = (('SCALE_OFFICE_VIEWER', 'Office Viewer'),
                       ('SCALE_PDF_VIEWER', 'PDF Viewer'),
                       ('SCALE_REMOTE_MONITOR', 'Remote Monitor'),
                       ('SCALE_NOW_SERVING', 'Now Serving'),
                       )

HTS_DISABLED_PRODUCTS = ["SCALE_FULLSCREEN_FLASHKEYS","SCALE_FPP", "SCALE_WEB_BROWSER"]

PRODUCTS = HTE_PRODUCTS + SCALE_PRODUCTS + HTS_DISABLED_PRODUCTS

KEY_TYPES = ["CSID","USB_SERIAL_NUMBER","SERVER_IP_ADDRESS"]
PROMPTS = {'individual name' : "Responsible Individual Full Name",
                   'company name' :  "Company Name",
                   'company address' : "Company Address",
                   'product names' : "Product Name(s)",
                   'license type' : 'License Key Type',
                   'license activation date' : 'License Activation Date',
                   'license allotment' : "License Allotment",
                   'license timeout' : "License Timeout",
                   'license key' : "License Key"}

HOBART_LICENSE_FILENAME="itwhobart.lic"
NULL_MAC_ADDRESS = "000000000000"

def readFlashKeyBuffer():
    # read the keys from flash
    status,  data = scaleKeys.readAllKeys()
    if status == 'success':
        # find first hex FF in stream and truncate buffer where found
        for i in xrange(0, len(data)):
            if data[i] == scaleKeys.BLANK_KEY:
                if i == 0:
                    return None
                else:
                    return data[:i]
        return data
    else:
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
    if getModelName.getModelName() != 'HTs':
        # if the feature being checked is typically a core feature except on HTs
        if feature in HTS_DISABLED_PRODUCTS:
            # do not check license since this is a core product on all other
            # scale lines so product is licensed
            return True

    # check for license in XML license buffer that typically comes from flash
    # on system controller board
    if xmlLicenseBuffer != None:
        hobartEl = ET.fromstring(xmlLicenseBuffer)
        for license in hobartEl.iter('license'):
            print license
            foundProduct = False
            for product in license.iter('product'):
                print str(product.get('name'))
                if str(product.get('name')) == feature:
                   foundProduct = True
                   break
            if foundProduct:
                try:
                    timeout = int(str(license.find('timeout').text))
                    #print timeout
                    #print str(license.find('activation_date').text)
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

def xmlAddLicenseToBuffer(xmlLicenseBuffer,  individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey):
    hobartEl = ET.fromstring(xmlLicenseBuffer)
    licenseEl = ET.SubElement(hobartEl,'license')
    individualNameEl = ET.SubElement(licenseEl, "responsible_individual")
    individualNameEl.text = individualName
    companyEl = ET.SubElement(licenseEl, "company")
    companyNameEl = ET.SubElement(companyEl, "name")
    companyNameEl.text = companyName
    companyAddressEl = ET.SubElement(companyEl, "address")
    companyAddressEl.text = companyAddress
    for i in xrange(0, len(productNames)):
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
        i += (level-1) * '  '
    num_kids = len(elem)
    if num_kids:
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
            if level:
                elem.text += '  '
        count = 0
        for kid in elem:
            indent(kid, level+1, count < num_kids - 1)
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

def getLicenseKeyInfo():
    print "ITW Hobart Product License Key Creator Version " + VERSION
    individualName = raw_input("Enter Licensee %s: " % (PROMPTS['individual name']))
    companyName = raw_input("Enter Licensee %s: " % (PROMPTS['company name']))
    address = raw_input("Enter Licensee %s: " % (PROMPTS['company address']))
    inputValid = False
    while not inputValid:
        response = string.upper(string.strip(raw_input("Enter Licensee %s {%s} (separated by commas): " % (PROMPTS['product names'],  string.join(PRODUCTS)))))
        productNames = string.split(response,',')

        matchesFound = 0
        for i in xrange(0,len(productNames)):
            if string.strip(productNames[i]) in PRODUCTS:
                matchesFound = matchesFound + 1
        if matchesFound == len(productNames):
            inputValid = True
        if not inputValid:
            print "Invalid Entry"

    inputValid = False
    while not inputValid:
        response = string.upper(string.strip(raw_input("Enter Licensee %s {%s}: " % (PROMPTS['license type'],  string.join(KEY_TYPES)))))
        licenseType = string.strip(response)
        if string.strip(licenseType) in KEY_TYPES:
            inputValid = True
        else:
            print "Invalid Entry"

    licenseActivationDate = datetime.date.today()
    licenseActivationDate = licenseActivationDate.strftime("%Y-%m-%d")
    licenseAllotment = raw_input("Enter %s: " % (PROMPTS['license allotment']))
    licenseTimeout = raw_input("Enter %s: " % (PROMPTS['license timeout']))
    csdata = raw_input("Enter csid output generated on customer's target licensed computing system: ")
    macAddress,  serialNumber = csid.extractCSInfo(csdata)

    return (string.upper(string.strip(individualName)),
            string.upper(string.strip(companyName)),
            string.upper(string.strip(address)),
            productNames,
            string.upper(string.strip(licenseType)),
            string.upper(string.strip(licenseActivationDate)),
            string.upper(string.strip(licenseAllotment)),
            string.upper(string.strip(licenseTimeout)),
            string.upper(string.strip(macAddress)),
            string.upper(string.strip(serialNumber)))

def readLicenseFile(licenseFilename):
    status = 'error'
    data = None
    try:
        status,  data = file_io.readFromFile(licenseFilename)
    except:
        pass
    return (status, data)

def createLicenseFile(licenseKeyFilename, individualName, companyName, address, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, macAddress=None, serialNumber=None, licenseKey=None):

    if licenseKey is None:
        licenseActivationDate = datetime.date.today()
        licenseActivationDate = licenseActivationDate.strftime("%Y-%m-%d")

        retVal,  licenseKey = genLicenseKey(individualName,
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
        license_data.append(PROMPTS['individual name'] + ': ' + individualName)
        license_data.append(PROMPTS['company name'] + ': ' + companyName)
        license_data.append(PROMPTS['company address'] + ': ' + address)
        license_data.append(PROMPTS['product names'] + ': ' + string.join(productNames, ','))
        license_data.append(PROMPTS['license type'] + ': ' + licenseType)
        license_data.append(PROMPTS['license activation date'] + ': ' + licenseActivationDate)
        license_data.append(PROMPTS['license allotment'] + ': ' + licenseAllotment)
        license_data.append(PROMPTS['license timeout'] + ': ' + licenseTimeout)
        license_data.append(PROMPTS['license key'] + ': ' + licenseKey)
        retVal,  output = file_io.writeToFile(filename=licenseKeyFilename,data=license_data)
        if retVal == 'success':
            status = ('success', '',  license_data)
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

    status,  data = readLicenseFile(licenseFilename)
    if status == 'success':
        for i in xrange(0, len(data)):
            line = string.split(string.strip(data[i]), ':')
            if len(line) == 2:
                if line[0] == PROMPTS['individual name']:
                    individualName = string.strip(line[1])
                elif line[0] == PROMPTS['company name']:
                    companyName = string.strip(line[1])
                elif line[0] == PROMPTS['company address']:
                    companyAddress = string.strip(line[1])
                elif line[0] == PROMPTS['product names']:
                    productNames = string.split(string.upper(string.strip(line[1])), ',')
                elif line[0] == PROMPTS['license type']:
                    licenseType = string.strip(line[1])
                elif line[0] == PROMPTS['license activation date']:
                    licenseActivationDate = string.strip(line[1])
                elif line[0] == PROMPTS['license allotment']:
                    licenseAllotment = string.strip(line[1])
                elif line[0] == PROMPTS['license timeout']:
                    licenseTimeout = string.strip(line[1])
                elif line[0] == PROMPTS['license key']:
                    licenseKey = string.strip(line[1])

    return (status,  individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey)

def containsScaleProducts(productNames):
    for i in xrange(0, len(productNames)):
        if getFeatureType(productNames[i]) == 'scale':
            return True
    return False

def getFeatureType(feature):
    if feature in SCALE_PRODUCTS:
        return 'scale'
    if feature in HTS_DISABLED_PRODUCTS:
        return 'scale'
    return 'hte'

def licenseExpired(licenseActivationDate, timeout=0):
    if timeout != 0:
        today = datetime.datetime.today()
        # get creation date of license file
        expirationDate = datetime.datetime.strptime(licenseActivationDate,"%Y-%m-%d") + datetime.timedelta(timeout)
        if today <= expirationDate:
            return False
        else:
            return True
    else:
        return False

def validLicenseKey(licenseFilename, productName, macAddress=None, serialNumber=None):
    try:
        # get elements of the license file
        status,  individualName,  companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseAllotment, licenseTimeout, licenseKey = getLicenseElements(licenseFilename)
        # if successfully got elements
        if status == 'success':
            # if product name to check is in license product names
            if string.upper(productName) in productNames:
                # if serial number is none
                if serialNumber is None:
                    # copmute the serial number
                    exitCode,  serialNumber = volumeInfo.getVolumeInfo("C:\\")
                    if exitCode != 0:
                        serialNumber = None
                # if the mac address is none
                if macAddress is None:
                    # get the mac address
                    exitCode, macAddress = getMacAddress.getMacAddress('wire')
                    if exitCode == 0 and macAddress:
                        # convert mac address to not have :
                        macAddress = string.replace(macAddress, ':', '')
                    else:
                        exitCode, macAddress = getMacAddress.getMacAddress('wireless')
                        if exitCode == 0 and macAddress:
                            # convert mac address to not have :
                            macAddress = string.replace(macAddress, ':', '')
                        else:
                            macAddress = None

                if macAddress is not None and serialNumber is not None:
                    # generate the license key
                    status,  generatedLicense = genLicenseKey(individualName, companyName, companyAddress, productNames, licenseType, licenseActivationDate, licenseTimeout, macAddress, serialNumber, COMPANY_KEY)
                    if status == 'success':
                        # if generated license key in license file
                        if generatedLicense == licenseKey:
                            if not licenseExpired(licenseActivationDate, int(licenseTimeout)):
                                return True
        return False
    except:
        return False

def genLicenseKey(individualName, companyName, address, productNames, licenseType, licenseActivationDate, licenseTimeout, macAddress, serialNumber, productKey):
    if individualName != None and companyName != None and address != None and productNames != None and serialNumber != None and licenseTimeout != None and macAddress != None and productKey != None and licenseActivationDate != None and licenseType != None:
        m = hashlib.sha1()
        m.update(individualName)
        m.update(companyName)
        m.update(address)
        for i in xrange(0, len(productNames)):
            m.update(productNames[i])
        # early licenses did not have allotment in them so if no allotment found in license key
        # then do not compute in key since assumption is 1 allotment
        # we not incorporate number of licenses into computation of key
        m.update(licenseType)
        m.update(licenseActivationDate)
        m.update(licenseTimeout)
        m.update(macAddress)
        m.update(serialNumber)
        m.update(productKey)
        return ('success',  string.upper(m.hexdigest()))
    else:
        return ('error',  None)

if __name__ == "__main__":
    a = xmlCreateInitialLicenseBuffer()
    a = xmlAddLicenseToBuffer(a,"Scott","ITW","401 W Market St",['SCALE_OFFICE_VIEWER'],'SCALE','2014-02-27','1','30','2asdfasdffasdf3aqe')
    a = xmlAddLicenseToBuffer(a,"Todd","ITW","401 W Market St",['SCALE_PDF_VIEWER'],'SCALE','2014-02-10','1','1','2asdfasdffasdf3aqe')
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
