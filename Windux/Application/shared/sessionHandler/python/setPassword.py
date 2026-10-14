#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
import argparse
import shellCommand
import hashlib
import base64
import array

def makePassword(value,  salt = None):
    import random
    if ((value is None) or (len(value) <= 0)):
        return None
    if (salt is None):
        chars = '0123456789abcdef'
        salt = ''.join(random.choice(chars) for _ in range(2))
    value = str(value)
    salt = str(salt)
    digest = str(hashlib.md5(value+salt).hexdigest())
    return (salt+digest)

def doHobartXOR(s, key):
    a = array.array('B', s)
    for i in range(0, len(a), 2):
        if (239<=key):
            key=7
        a[i] ^= key
        key += 3
        a[i+1] ^= key
        key -= 2
    return str(a.tobytes(), 'utf-8')

def doHobartEncrypt(s):

    password = str(s)

    if ((password is None) or (0 == len(password))):
        return None

    # obfuscate password
    password = password.encode('ascii')
    password = base64.b32encode(password)
    password = password[::-1]
    password = base64.b64encode(password)

    password = doHobartXOR(password, 42)

    password = password.encode('ascii')
    password = base64.b16encode(password)
    password = password[::-1]
    password = base64.b64encode(password)

    return password.decode('ascii')

def doHobartDecrypt(s):

    hash = str(s)

    try:
        # de-obfuscate password
        hash = hash.encode('ascii')
        hash = base64.b64decode(hash)
        hash = hash[::-1]
        hash = base64.b16decode(hash)

        hash = doHobartXOR(hash, 42)

        hash = hash.encode('ascii')
        hash = base64.b64decode(hash)
        hash = hash[::-1]
        hash = base64.b32decode(hash)

    except:
        return None

    return hash.decode('ascii')

def setPassword(user, pwd):
    
    publicKey = None
    privateKey = None
    exitCode = 1
    
    # LINUX OS
    if os.name == "posix":
        
        # hash password passed in and then invoke usermod to set
        try:
            cmd = []
            cmd.append("mkpasswd")
            cmd.append(str(pwd))
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            
            password = None
            
            if ((0==exitCode) and (stdout is not None)):
                lines = stdout.split("\n")
                
                if (1 <= len(lines)):
                    password = lines[0]
                    
            if (password is not None):
                cmd = []
                cmd.append("usermod")
                cmd.append("-p")
                cmd.append(str(password))
                cmd.append(str(user))
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                
                privateKey = "/usr/local/hobart/" + user + ".key"

        except:
            exitCode = 1

    # WINDOWS OS
    else:
        
        # hash password passed in and save to file
        hash = makePassword(pwd)
        
        if (hash is None):
            exitCode = 1
        else:
            
            if os.path.isfile("C:\\Program Files\\Hobart\\Access Scale\\HobartSSHD.exe"):
                publicKey = privateKey = "C:\\Program Files\\Hobart\\Access Scale\\"
            elif os.path.isfile("C:\\Program Files\\Hobart\\HobartSSHD.exe"):
                publicKey = privateKey = "C:\\Program Files\\Hobart\\"
                
            publicKey = publicKey + user + ".pub"
            privateKey = privateKey + user + ".key"
                
            if (publicKey is None):
                exitCode = 1
            else:
                f = open(publicKey, 'w')
                f.write(hash)
                f.close()
                
                exitCode = 0
    
    # encrypt password and save (scale backend uses this file)
    if ((0 == exitCode) and (privateKey is not None)):
        f = open(privateKey, 'w')
        f.write(doHobartEncrypt(pwd))
        f.close()

    return exitCode

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="sets user password")
    parser.add_argument("user", action="store", help="username")
    parser.add_argument("pwd", action="store", help="password")

    args = parser.parse_args()

    exitCode  = setPassword(args.user, args.pwd)
    sys.exit(exitCode)
