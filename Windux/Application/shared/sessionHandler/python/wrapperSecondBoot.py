# -*- coding: utf-8 -*-
import os
import sys
import time
import shellCommand
import logging
import subprocess
import messageBox
import languageSettings

# secondBoot file is used for Pre-Pack only
secondBootFile = os.path.join("/","etc","wrapperSecondBoot")

def main( ):

    scaleType = 'unknown'
    displayMode = 'unknown'

    if os.path.isfile( "/etc/hobart/scale-type.dat" ):
        file = open( "/etc/hobart/scale-type.dat", 'rb' )
        scaleType = file.read()
        scaleType = scaleType.replace( '\n', '' )
        file.close()

    #------------------------------------------
    # SOF-3092: Need to boot again for Pre-Pack
    #------------------------------------------
    if ( scaleType == 'pre-pack' ):

        if not os.path.exists( secondBootFile ):

            # Create second boot file so this does section does not run again
            cmd = []
            cmd.append("/bin/touch")
            cmd.append(secondBootFile)
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # Show 'Second Boot' message
            language = languageSettings.currentLanguage()

            if language == 'spanish':
                message = "Iniciando Segundo Arranque para Pre-Empaque\n\nEspere por favor ..."
            elif language == 'french':
                message = "Initiating Second Boot for Pre-Pack\n\nAttendez s'il vous plaît..."
            else:
                message = "Initiating Second Boot for Pre-Pack\n\nPlease wait..."

            messageBox.display(message=message, timer=5)
            
            # Reboot the device
            cmd=[]
            cmd.append("/sbin/init")
            cmd.append("6")
            exitCode, stdout, stderr = shellCommand.execute(cmd)


if __name__ == "__main__":
    main()
    sys.exit(0)
