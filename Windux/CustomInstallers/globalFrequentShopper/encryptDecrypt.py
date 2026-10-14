'''
Created on Jan 7, 2013

@author: Alex Zavala
'''

import Crypto.Random
from Crypto.Cipher import AES
import hashlib
import codecs
import shellCommand
import sys, os
import configInfo

''' DEFINE GLOBALS '''
#The size of the block in ECB
BLOCK_SIZE = 32
#The length of the salt
SALT_SIZE = 32
#The number of iterations used to generate the key
NUMBER_OF_ITERATIONS = 20
#The number of bytes to use for AES encryption
AES_MULTIPLE = 32

def generate_key(passphrase, salt, iterations):
    '''
    This function generates a key by using the passphrase and salt in a SHA-256 hash.
    @param passphrase: the passphrase used to generate the key
    @param salt: a 32 byte long string used for salting
    @param iterations: the number of iterations this function hashes the key
    '''
    assert iterations > 0

    key = str(passphrase) + str(salt)

    for i in range(iterations):
        key = hashlib.sha256(key).digest()

    return key

def pad_text(text, multiple):
    '''
    This function pads the text using PKCS#5 padding.
    @param multiple: the number of bytes of the AES encryption (AES-256=32 bytes)
    '''
    extra_bytes = len(text) % multiple

    padding_size = multiple - extra_bytes

    padding = chr(padding_size) * padding_size

    padded_text = text + padding

    return padded_text

def unpad_text(padded_text):
    '''
    This function removes the padding from the padded_text using PKCS#5 padding.
    @param padded_text: the text with the padded characters using PKCS#5 padding
    '''
    padding_size = ord(padded_text[-1])

    text = padded_text[:-padding_size]

    return text

def encrypt(plaintext, passphrase):
    '''
    This function encrypts the plaintext using a 50 byte salt and AES-256.
    @param plaintext: the plain text to be encrypted
    @param passphrase: the passphrase to be used to generate the key
    '''
    salt = Crypto.Random.get_random_bytes(SALT_SIZE)

    key = generate_key(passphrase, salt, NUMBER_OF_ITERATIONS)

    cipher = AES.new(key, AES.MODE_ECB)

    padded_plaintext = pad_text(plaintext, AES_MULTIPLE)

    ciphertext = cipher.encrypt(padded_plaintext)

    ciphertext_with_salt = salt + ciphertext

    return ciphertext_with_salt

def decrypt(ciphertext, passphrase):
    '''
    This function decrypts the ciphered text using AES-256
    @param ciphertext: the encrypted text to be deciphered
    @param passphrase: the passphrase used to generate the key
    '''
    salt = ciphertext[0:SALT_SIZE]

    ciphertext_without_salt = ciphertext[SALT_SIZE:]

    key = generate_key(passphrase, salt, NUMBER_OF_ITERATIONS)

    cipher = AES.new(key, AES.MODE_ECB)

    padded_plaintext = cipher.decrypt(ciphertext_without_salt)

    plaintext = unpad_text(padded_plaintext)
    if plaintext[0:4] != "SSID":
        plaintext = "NO"
    return plaintext
	
def decryptAny(ciphertext, passphrase):
    '''
    This function decrypts the ciphered text using AES-256
    @param ciphertext: the encrypted text to be deciphered
    @param passphrase: the passphrase used to generate the key
    '''
    salt = ciphertext[0:SALT_SIZE]

    ciphertext_without_salt = ciphertext[SALT_SIZE:]

    key = generate_key(passphrase, salt, NUMBER_OF_ITERATIONS)

    cipher = AES.new(key, AES.MODE_ECB)

    padded_plaintext = cipher.decrypt(ciphertext_without_salt)

    plaintext = unpad_text(padded_plaintext)
    return plaintext

def saveConfiguration(plaintxt, configName):
    '''
    This function saves the configuration of the scale to a file.
    @param plaintxt: the configuration being saved
    @param configName: the name of the file to save to
    '''
    destination = codecs.open(configName, 'w+', 'base64')
    destination.write(plaintxt)
    destination.close()

configFullPath = ''
if configInfo.getRootFolder() is not None:
    configFullPath = configInfo.getRootFolder() + os.path.join(os.sep, 'db', 'settingswizard', 'config.txt')
    #print "path is " + configFullPath
else:
    configFullPath = os.path.join(os.sep, 'usr','local','hobart','db', 'settingswizard', 'config.txt')
    #print "no root folder"
    
def saveConfigs(file_contents, passphrase):
    '''
    This function exports the network configurations to a specific file
    @param file_contents: the contents to be written to a file
    @param passphrase: the passphrase being used for encryption/decryption
    '''
    print configFullPath
    ciphertxt = encrypt(file_contents, passphrase)
    saveConfiguration(ciphertxt, configFullPath)
    #print "F"

def saveConfigsToFile(file_contents, passphrase, filename):
    '''
    This function exports the network configurations to a specific file
    @param file_contents: the contents to be written to a file
    @param passphrase: the passphrase being used for encryption/decryption
    @param filename: file location to save configs
    '''
    ciphertxt = encrypt(file_contents, passphrase)
    saveConfiguration(ciphertxt, filename)
    
def exportConfigs(filename):
    '''
    This function exports the network configurations to the given filename
    @param filename: the filename given from the export button
    '''


    #print filename
    destination = codecs.open(filename, 'w+', 'base64')
    source = codecs.open(configFullPath, 'r', 'base64')
    destination.write(source.read())
    destination.close()
    source.close()
    cmd = []
    cmd.append("sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

def importConfigs(filename, passphrase):
    '''
    This function imports the network configurations from a file
    @param filename: the filename being imported
    @param passphrase: the passphrase being used for encryption/decryption
    '''
    plainTxt = "NO"
    try:
        cipheredTxt = codecs.open(filename, 'r', 'base64')
        encryptedTxt = cipheredTxt.read()
        plainTxt = decrypt(encryptedTxt, passphrase)
        cipheredTxt.close()
    except:
        pass
    #print plainTxt
    return plainTxt

def formatFile(networkConfigs):
    '''
    This function formats the file for the network configuration of the scale.
    @param networkConfigs: a list of strings containing the configurations
    '''
    format = ""
    baseFields = ['SSID : ','AUTHENTICATION : ','CIPHER : ']
    wpaFields = ['PASSPHRASE : ']
    eapTLSFields = ['TRUSTEDROOTFILENAME : ','CLIENTCERTSERIALNUM : ','CLIENTCERTFILENAME : ','CLIENTCERTPASSWORD : ']
    wepFields = ['DEFAULT : ', 'KEY 1 : ', 'KEY 2 : ' , 'KEY 3 : ', 'KEY 4 : ']
    leapPeapFields = ['USERNAME : ', 'PASSWORD : ']
    linkRateFields = ['LINKRATE : ', 'AUTOMATICLINKRATE : ','CHANNEL : ']
    IPFields = ['ISSTATIC : ', 'IPADDRESS : ', 'SUBNET : ', 'GATEWAY : ']
    DNSFields = ['AUTODNS : ', 'PRIMARY : ', 'SECONDARY : ', 'TERTIARY : ']
    dnsSuffixes = ['DNSSUFFIX1 : ', 'DNSSUFFIX2 : ', 'DNSSUFFIX3 : ']

    howMany = 0
    for i in range(0,3):
        format += baseFields[i] + networkConfigs[i] + '\n'
    if networkConfigs[1] == "OPEN":
        for i in range(len(wepFields)):
            format += wepFields[i] + networkConfigs[i+3] + '\n'
            howMany = i+3
    elif networkConfigs[1] == "WPA-Personal" or networkConfigs[1] == "WPA2-Personal" :
        for i in range(len(wpaFields)):
            format += wpaFields[i] + networkConfigs[i+3] + '\n'
            howMany = i+3
    elif networkConfigs[1] == "WPA EAP-TLS" or networkConfigs[1] == "WPA2 EAP-TLS":
        for i in range(len(eapTLSFields)):
            format += eapTLSFields[i] + networkConfigs[i+3] + '\n'
            howMany = i+3
    else:
        for i in range(len(leapPeapFields)):
            format += leapPeapFields[i] + networkConfigs[i+3] + '\n'
            howMany = i+3

    for i in range(0,3):
        format += linkRateFields[i] + networkConfigs[howMany+i+1] + '\n'
    for i in range(0,4):
        format += IPFields[i] + networkConfigs[howMany+i+4] + '\n'
    for i in range(0,4):
        format += DNSFields[i] + networkConfigs[howMany+i+8] + '\n'
    # DNS SUFFIXES
    for i in range(0,3):
        format += dnsSuffixes[i] + networkConfigs[howMany+i+12] + '\n'

    '''
    for i in range(0,4):
        format += IPFields[i] + networkConfigs[howMany+i+4] + '\n'
    for i in range(0,4):
        format += DNSFields[i] + networkConfigs[howMany+i+8] + '\n'
    '''
    #print(format)
    return format

def addParseOptionsInOrder(options):
    '''
    This function orders the parser options
    @param options: the options parser to set up
    '''
    list = []
    orderedList = []
    #Add options in order
    list.append(options.ssid)
    list.append(options.securityMethod)
    list.append(options.encryption)
    list.append(options.EAPUsername)
    list.append(options.EAPPassword)
    list.append(options.defaultKey)
    list.append(options.WEPKey1)
    list.append(options.WEPKey2)
    list.append(options.WEPKey3)
    list.append(options.WEPKey4)
    list.append(options.wpaPassPhrase)
    list.append(options.serverCertFilename)
    list.append(options.clientCertSerialNumFilename)
    list.append(options.clientCertFilename)
    list.append(options.clientCertPassword)
    list.append(options.linkRate)
    list.append(options.automaticLinkRate)
    list.append(options.channel)
    list.append(options.isStatic)
    list.append(options.ipAddress)
    list.append(options.subnetMask)
    list.append(options.gateway)
    list.append(options.isAutoDNS)
    list.append(options.primaryDNS)
    list.append(options.secondaryDNS)
    list.append(options.tertiaryDNS)
    list.append(options.DNSSuffix1)
    list.append(options.DNSSuffix2)
    list.append(options.DNSSuffix3)

    #Remove empty options
    for i in list:
        if i == None:
            pass
        else:
            orderedList.append(i)
    return orderedList

