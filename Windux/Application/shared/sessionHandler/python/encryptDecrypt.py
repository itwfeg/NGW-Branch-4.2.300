#!/usr/bin/env python2
# -*- coding: utf-8 -*-

'''
Created on Jan 7, 2013

@author: Alex Zavala
'''

import Crypto.Random
from Crypto.Cipher import AES
import logging
import hashlib
import codecs
import shellCommand
import os
import sys
import configInfo
import traceback

from collections import OrderedDict as OrderedDictionary


def handle_exception(type_, value, tb):
    traceback.print_exception(type_, value, tb)


# sys.stdout = open('/usr/local/hobart/encrypt-debug-log.log', 'w')
# sys.stderr = open('/usr/local/hobart/encrypt-error-log.log', 'w')
sys.excepthook = handle_exception


''' DEFINE GLOBALS '''
# The size of the block in ECB
BLOCK_SIZE = 32
# The length of the salt
SALT_SIZE = 32
# The number of iterations used to generate the key
NUMBER_OF_ITERATIONS = 20
# The number of bytes to use for AES encryption
AES_MULTIPLE = 32


# logging.basicConfig(filename="/usr/local/hobart/encryptDecryptDebug.log", level=logging.DEBUG)


def generate_key(passphrase, salt, iterations):
    """
    This function generates a key by using the passphrase and salt in a SHA-256 hash.
    @param passphrase: the passphrase used to generate the key
    @param salt: a 32 byte long string used for salting
    @param iterations: the number of iterations this function hashes the key
    """
    assert iterations > 0

    key = passphrase.encode('utf-8') + salt

    for i in range(iterations):
        key = hashlib.sha256(key).digest()

    return key


def pad_text(text, multiple):
    """
    This function pads the text using PKCS#5 padding.
    @param multiple: the number of bytes of the AES encryption (AES-256=32 bytes)
    """
    extra_bytes = len(text) % multiple
    padding_size = multiple - extra_bytes
    padding = chr(padding_size) * padding_size
    padded_text = text + padding
    return padded_text


def unpad_text(padded_text):
    """
    This function removes the padding from the padded_text using PKCS#5 padding.
    @param padded_text: the text with the padded characters using PKCS#5 padding
    """
    # SOF-4766 - Handled difference in encoding between HT Device Setup Utility
    # and scale ExportConfig.py script
    if(padded_text[-1] == 0):
        padding_size = 2*int(padded_text[-2])
        text = padded_text[:-padding_size]
        text = text.decode('utf-16')
    else:
        padding_size = int(padded_text[-1])
        text = padded_text[:-padding_size]
        text = text.decode('utf-8')

    return text


def encrypt(plaintext, passphrase):
    """
    This function encrypts the plaintext using a 50 byte salt and AES-256.
    @param plaintext: the plain text to be encrypted
    @param passphrase: the passphrase to be used to generate the key
    """
    # logging.debug("Encrypting plaintext data: " + plaintext)
    salt = Crypto.Random.get_random_bytes(SALT_SIZE)

    key = generate_key(passphrase, salt, NUMBER_OF_ITERATIONS)

    cipher = AES.new(key, AES.MODE_ECB)

    padded_plaintext = pad_text(plaintext, AES_MULTIPLE)

    ciphertext = cipher.encrypt(padded_plaintext)

    ciphertext_with_salt = salt + ciphertext

    # logging.debug("Encrypted plaintext")
    return ciphertext_with_salt


def decrypt(ciphertext, passphrase):
    """
    This function decrypts the ciphered text using AES-256
    @param ciphertext: the encrypted text to be deciphered
    @param passphrase: the passphrase used to generate the key
    """
    salt = ciphertext[0:SALT_SIZE]
    
    ciphertext_without_salt = ciphertext[SALT_SIZE:]
    
    key = generate_key(passphrase, salt, NUMBER_OF_ITERATIONS)
    
    cipher = AES.new(key, AES.MODE_ECB)
    
    padded_plaintext = cipher.decrypt(ciphertext_without_salt)

    plaintext = unpad_text(padded_plaintext)

    if "SSID" not in plaintext:
        # logging.debug("Unpadded plaintext did not contain the SSID column as expected!")
        # logging.debug("padded_plaintext = " + padded_plaintext)
        plaintext = "NO"

    # logging.debug("plaintext = " + plaintext)
    return plaintext


def decryptAny(ciphertext, passphrase):
    """
    This function decrypts the ciphered text using AES-256
    @param ciphertext: the encrypted text to be deciphered
    @param passphrase: the passphrase used to generate the key
    """
    salt = ciphertext[0:SALT_SIZE]

    ciphertext_without_salt = ciphertext[SALT_SIZE:]

    key = generate_key(passphrase, salt, NUMBER_OF_ITERATIONS)

    cipher = AES.new(key, AES.MODE_ECB)

    padded_plaintext = cipher.decrypt(ciphertext_without_salt)

    plaintext = unpad_text(padded_plaintext)
    return plaintext


def saveConfiguration(plaintxt, configName):
    """
    This function saves the configuration of the scale to a file.
    @param plaintxt: the configuration being saved
    @param configName: the name of the file to save to
    """
    print((str(configName)))
    destination = codecs.open(configName, 'w+', 'base64')
    destination.write(plaintxt)
    destination.close()
    
    # NOTE: This snippet is for debugging without encryption.
    # print "Saving configuration data", plaintxt
    # print "to file", configName

    # with open(configName, 'w+') as destination:
    #     destination.write(plaintxt)


configFullPath = ''
try:
    if configInfo.getRootFolder() is not None:
        configFullPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db', 'settingswizard', 'config.txt')
        # print "path is " + configFullPath
    else:
        configFullPath = os.path.join(os.sep, 'usr', 'local', 'hobart', 'db', 'settingswizard', 'config.txt')
        # print "no root folder"
except:
    configFullPath = '/usr/local/hobart/db/settingswizard/config.txt'
    pass


def saveConfigs(file_contents, passphrase):
    """
    This function exports the network configurations to a specific file
    @param file_contents: the contents to be written to a file
    @param passphrase: the passphrase being used for encryption/decryption
    """
    ciphertxt = encrypt(file_contents, passphrase)
    saveConfiguration(ciphertxt, configFullPath)


def saveConfigsToFile(file_contents, passphrase, filename):
    """
    This function exports the network configurations to a specific file
    @param file_contents: the contents to be written to a file
    @param passphrase: the passphrase being used for encryption/decryption
    @param filename: file location to save configs
    """
    ciphertxt = encrypt(file_contents, passphrase)
    saveConfiguration(ciphertxt, filename)


def exportConfigs(filename):
    """
    This function exports the network configurations to the given filename
    @param filename: the filename given from the export button
    """
    # NOTE: This snippet is for debugging exporting without encryption on the file.
    # with open(filename, 'w+') as destination:
    #     with open(configFullPath, 'r') as source:
    #         destination.write(source.read())
    #
    # cmd = ["sync"]
    # shellCommand.execute(cmd)

    destination = codecs.open(filename, 'w+', 'base64')

    try:
        source = codecs.open(configFullPath, 'r', 'base64')
    except:
        try:
            source = codecs.open('/usr/local/hobart/db/settingswizard/config.txt', 'r', 'base64')
            pass
        except:
            source = codecs.open('/usr/local/hobart/python/config.txt', 'r', 'base64')
            pass

    destination.write(source.read())
    destination.close()
    source.close()
    cmd = []
    cmd.append("sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)


def importConfigs(filename, passphrase):
    """
    This function imports the network configurations from a file
    @param filename: the filename being imported
    @param passphrase: the passphrase being used for encryption/decryption
    """
    plainTxt = "NO"
    
    # NOTE: This snippet is for testing the import without encryption for debugging purposes.
    # with open(filename, 'r') as config_file:
    #     plainTxt = config_file.read()

    cipheredTxt = codecs.open(filename, 'r', 'base64')
    encryptedTxt = cipheredTxt.read()
    plainTxt = decrypt(encryptedTxt, passphrase)
    cipheredTxt.close()
    
    return plainTxt


def formatFile(options):
    """
    This function formats the file for the network configuration of the scale.
    @param options: a list of strings containing the configurations
    """
    lines = OrderedDictionary()

    lines["SSID : "] = options.ssid
    lines["AUTHENTICATION : "] = options.securityMethod
    lines["CIPHER : "] = options.encryption

    if options.EAPUsername:
        lines["USERNAME : "] = options.EAPUsername
    if options.EAPPassword:
        lines["PASSWORD : "] = options.EAPPassword

    if options.securityMethod == "OPEN":
        lines["DEFAULT : "] = options.defaultKey
        lines["KEY 1 : "] = options.WEPKey1
        lines["KEY 2 : "] = options.WEPKey2
        lines["KEY 3 : "] = options.WEPKey3
        lines["KEY 4 : "] = options.WEPKey4
    elif options.securityMethod == "WPA-Personal" or options.securityMethod == "WPA2-Personal":
        lines["PASSPHRASE : "] = options.wpaPassPhrase
    elif options.securityMethod == "WPA EAP-TLS" or options.securityMethod == "WPA2 EAP-TLS":
        lines["TRUSTEDROOTFILENAME : "] = options.serverCertFilename
        lines["CLINETCERTSERIALNUM : "] = options.clientCertSerialNumFilename
        lines["CLIENTCERTFILENAME : "] = options.clientCertFilename
        lines["CLIENTCERTPASSWORD : "] = options.clientCertPassword

    lines["LINK RATE : "] = options.linkRate
    lines["AUTOMATICLINKRATE : "] = options.automaticLinkRate
    lines["CHANNEL : "] = options.channel

    lines["ISSTATIC : "] = options.isStatic
    lines["IPADDRESS : "] = options.ipAddress
    lines["SUBNET : "] = options.subnetMask
    lines["GATEWAY : "] = options.gateway

    lines["AUTODNS : "] = options.isAutoDNS
    lines["PRIMARY : "] = options.primaryDNS
    lines["SECONDARY : "] = options.secondaryDNS
    lines["TERTIARY : "] = options.tertiaryDNS

    lines["DNSSUFFIX1 : "] = options.DNSSuffix1
    lines["DNSSUFFIX2 : "] = options.DNSSuffix2
    lines["DNSSUFFIX3 : "] = options.DNSSuffix3
    lines["DNSSUFFIX4 : "] = options.DNSSuffix4
    lines["DNSSUFFIX5 : "] = options.DNSSuffix5
    lines["DNSSUFFIX6 : "] = options.DNSSuffix6

    if options.hostname:
        lines["HOSTNAME : "] = options.hostname

    if options.timeKeeper:
        lines["TIMEKEEPER : "] = options.timeKeeper

    if options.ntpServer:
        lines["NTPSERVER : "] = options.ntpServer
    if options.ntpFrequency:
        lines["NTPFREQUENCY : "] = options.ntpFrequency
    if options.ntpDayOfWeek:
        lines["NTPDAYOFWEEK : "] = options.ntpDayOfWeek
    if options.ntpTimeOfDay:
        lines["NTPTIMEOFDAY : "] = options.ntpTimeOfDay
    if options.ntpTimeZone:
        lines["NTPTIMEZONE : "] = options.ntpTimeZone
    if options.ntpAntiMeridian:
        lines["NTPANTIMERIDIAN : "] = options.ntpAntiMeridian

    if options.dhcpHostname:
        lines["DHCPHOSTNAME : "] = options.dhcpHostname

    try:
        if options.targetType:
            lines["TARGETTYPE : "] = options.targetType
    except:
        pass

    try:
        if options.wirelessDriver:
            lines["WIRELESSDRIVER : "] = options.wirelessDriver
    except:
        pass

    try:
        if options.addNetwork:
            lines["ADDNETWORK : "] = options.addNetwork
    except:
        pass

    try:
        if options.addNTP:
            lines["ADDNTP : "] = options.addNTP
    except:
        pass

    try:
        if options.addHostname:
            lines["ADDHOSTNAME : "] = options.addHostname
    except:
        pass

    try:
        if options.addMasterLink:
            lines["ADDMASTERLINK : "] = options.addMasterLink
    except:
        pass

    try:
        if options.masterLink:
            lines["MASTERLINK : "] = options.masterLink
    except:
        pass


    format_ = ""

    for key, value in list(lines.items()):
        if value is None:
            value = ''

        format_ += key + value + "\n"

    return format_
