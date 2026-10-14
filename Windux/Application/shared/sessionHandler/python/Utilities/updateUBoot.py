#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Flashes U-Boot into SPI NOR Flash
# Prepends 1024 bytes to U-Boot image
import os
import shellCommand
import getModelName

def flashUboot(root='/'):

    f1 = open(os.path.join(root, "u-boot.imx.pad"),"wb")
    f1.write(1024 * b'\xff')

    model = getModelName.getModelName()
    
    # FT-1741
    if model and len(model) > 3:
        model = model[:3]

    if model == 'HTs':
        modelLetter = '-s'
    elif model == 'HTi':
        modelLetter = ''
    elif model == 'HTx':
        modelLetter = '-q'

    ubootImage = os.path.join(root, "u-boot%s.imx" % (modelLetter))

    # pad the uboot image up front with 1024 bytes
    f2 = open(ubootImage,"rb")
    while True:
        bytes = f2.read(1024)
        if bytes:
            n=f1.write(bytes)
        else:
            break
    f2.close()
    f1.close()

    # erase mtd0 and mtd1 flash banks
    for i in range(0, 2):

        cmd = []
        cmd.append(os.path.join(os.sep,"usr","sbin","flash_erase"))
        cmd.append("/dev/mtd%s" % (str(i)).strip())
        cmd.append("0")
        cmd.append("0")

        exitCode, stdout, stderr = shellCommand.execute(cmd)

    # program the u-boot image with 1024 byte padding up front
    cmd = []
    cmd.append(os.path.join(os.sep,"usr","sbin","flashcp"))
    cmd.append("-v")
    cmd.append(os.path.join(root,"u-boot.imx.pad"))
    cmd.append("/dev/mtd0")

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    # program the u-boot environment variables
    cmd = []
    cmd.append(os.path.join(os.sep,"usr","sbin","flashcp"))
    cmd.append("-v")
    cmd.append(os.path.join(root,"envSettings.dat"))
    cmd.append("/dev/mtd1")

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    try:
        # remove the file that was just flashed this is the file with padding
        os.remove(os.path.join(root, "u-boot.imx.pad"))
    except:
        pass

    return exitCode

if __name__ == '__main__':
    flashUboot()
