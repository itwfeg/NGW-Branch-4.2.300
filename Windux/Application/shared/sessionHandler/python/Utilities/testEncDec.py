#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Created on Jan 8, 2013

@author: Alex Zavala
'''
import encryptDecrypt
import hashlib

if __name__ == '__main__':
    '''
    password = "aaa"
    exampleString = "alex likes that this works"
    ciphertxt = encryptDecrypt.encrypt(exampleString, password)
    plaintxt = encryptDecrypt.decrypt(ciphertxt, password)
    print plaintxt'''
    '''
    password = "aaa"
    exampleString = "alex likes that this works"
    encryptDecrypt.exportConfigs(exampleString, password)
    encryptDecrypt.importConfigs(password)'''
    passphrase = "hi"
    encryptDecrypt.exportConfigs("config.txt", passphrase)
