#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import argparse

def linux_tts(input,output,lang):
    if not os.path.exists("/usr/bin/speak"):
        return 1

    cmd = []
    cmd.append("speak")
    # voice
    cmd.append("-v%s+f2" % lang)
    # amplitude 0-200
    cmd.append("-a200")
    # words per minute 80-450
    cmd.append("-s160")

    cmd.append("-w")
    cmd.append(output)

    cmd.append('"%s"' % input)

    exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
    return exitCode

def linux_play_wav_file(number, gender='male', lang ='english'):

    WAVFiles = ['Now_Serving_Number.wav']
    teen = False
    digits = []
    while number > 0: #put digits of number in list
        digits.append(number % 10)
        number = number / 10
    print(digits)
    if len(digits) == 3: # if hundred digit exists append properly
        if digits[2] == 9:
            WAVFiles.append('9.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 8:
            WAVFiles.append('8.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 7:
            WAVFiles.append('7.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 6:
            WAVFiles.append('6.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 5:
            WAVFiles.append('5.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 4:
            WAVFiles.append('4.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 3:
            WAVFiles.append('3.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 2:
            WAVFiles.append('2.wav')
            WAVFiles.append('100.wav')
        elif digits[2] == 1:
            WAVFiles.append('1.wav')
            WAVFiles.append('100.wav')
    
    if len(digits) == 2 or len(digits) == 3: # tens digit
        print(WAVFiles)
        if digits[1] == 9:
            WAVFiles.append('90.wav')
        elif digits[1] == 8:
            WAVFiles.append('80.wav')
        elif digits[1] == 7:
            WAVFiles.append('70.wav')
        elif digits[1] == 6:
            WAVFiles.append('60.wav')
        elif digits[1] == 5:
            WAVFiles.append('50.wav')
        elif digits[1] == 4:
            WAVFiles.append('40.wav')
        elif digits[1] == 3:
            WAVFiles.append('30.wav')
        elif digits[1] == 2:
            WAVFiles.append('20.wav')
        if lang == 'spanish' and digits[0] == 0:
            WAVFiles[len(WAVFiles) - 1] = '0' + WAVFiles[len(WAVFiles) - 1]
        elif digits[1] == 1:
            if digits[0] == 0:
                WAVFiles.append('10.wav')
                teen = True
            elif digits[0] == 1:
                WAVFiles.append('11.wav')
                teen = True
            elif digits[0] == 2:
                WAVFiles.append('12.wav')
                teen = True
            elif digits[0] == 3:
                WAVFiles.append('13.wav')
                teen = True
            elif digits[0] == 4:
                WAVFiles.append('14.wav')
                teen = True
            elif digits[0] == 5:
                WAVFiles.append('15.wav')
                teen = True
            elif digits[0] == 6:
                WAVFiles.append('16.wav')
                teen = True
            elif digits[0] == 7:
                WAVFiles.append('17.wav')
                teen = True
            elif digits[0] == 8:
                WAVFiles.append('18.wav')
                teen = True
            elif digits[0] == 9:
                WAVFiles.append('19.wav')
                teen = True

    if not teen: # if teen, do not need pronounce the ones digit
        if digits[0] == 9:
            WAVFiles.append('9.wav')
        elif digits[0] == 8:
            WAVFiles.append('8.wav')
        elif digits[0] == 7:
            WAVFiles.append('7.wav')
        elif digits[0] == 6:
            WAVFiles.append('6.wav')
        elif digits[0] == 5:
            WAVFiles.append('5.wav')
        elif digits[0] == 4:
            WAVFiles.append('4.wav')
        elif digits[0] == 3:
            WAVFiles.append('3.wav')
        elif digits[0] == 2:
            WAVFiles.append('2.wav')
        elif digits[0] == 1:
            WAVFiles.append('1.wav')

    if gender == 'female' and lang != 'spanish': #gender for spanish is not implemented a this time
        female_WAVFiles = []
        for wav in WAVFiles:
            female_WAVFiles.append('/usr/local/hobart/system/audio/' +'w' + wav) #for female we add w at the beginning of the filename to properly adjust. 
        return female_WAVFiles

    if lang == 'spanish':
        spanish_WAVFiles = []
        for wav in WAVFiles:
            spanish_WAVFiles.append('/usr/local/hobart/system/audio/' + 's' + wav)
        return spanish_WAVFiles
    
    male_WAVFiles = []
    for wav in WAVFiles:
        male_WAVFiles.append('/usr/local/hobart/system/audio/' + wav)
        
    return male_WAVFiles

    if not teen: # if teen, do not need pronounce one's digit
        if digits[0] == 9:
            WAVFiles.append('9.wav')
        elif digits[0] == 8:
            WAVFiles.append('8.wav')
        elif digits[0] == 7:
            WAVFiles.append('7.wav')
        elif digits[0] == 6:
            WAVFiles.append('6.wav')
        elif digits[0] == 5:
            WAVFiles.append('5.wav')
        elif digits[0] == 4:
            WAVFiles.append('4.wav')
        elif digits[0] == 3:
            WAVFiles.append('3.wav')
        elif digits[0] == 2:
            WAVFiles.append('2.wav')
        elif digits[0] == 1:
            WAVFiles.append('1.wav')

    if gender == 'female' and lang != 'spanish': #gender for spanish is not implemented a this time
        female_WAVFiles = []
        for wav in WAVFiles:
            female_WAVFiles.append('w' + wav) #for female we add w at the beginning of the filename to properly adjust. 
        return female_WAVFiles

    if lang == 'spanish':
        spanish_WAVFiles = []
        for wav in WAVFiles:
            spanish_WAVFiles.append('s' + wav)
        return spanish_WAVFiles

    return WAVFiles

def windows_tts(input,output,lang):
    # TODO
    return 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Speaks Now Serving Dialogue")
    
    parser.add_argument("-p","--play",
                        dest="play",
                        action="store_true",
                        help="play file after conversion")
    
    parser.add_argument('integer', type=str, action="store",  
                                help = "Integer representing the number to serve")
    
    parser.add_argument('gender',  type=str, default = "male", nargs = '?',  action="store",  
                                help = "Gender desired(male or female)")
    
    parser.add_argument('lang',  type=str,  default = "english", nargs = '?', action="store",  
                                help = "The language desire(english or spanish)")
    
    args = parser.parse_args()
    
#    f = open('tts.txt', 'a')
#    for arg in sys.argv:
#        f.write(arg + " ")
#    f.write("\n")
    
    if os.name == "posix":
        #Requires first arg for serving number
        wavfiles = []
        number = ""
        
        for s in args.integer:
            if s.isdigit():
                number += s
                
        try:
            wavfiles = linux_play_wav_file(int(number), args.gender, args.lang)
            print(wavfiles)
        except Exception as error:
            f.write(str(error))
        
        cmd = []
            #os.system("amixer sset Headphone,0 " + str(options.level) + "%," + str(options.level) + "%")
        for WAV in wavfiles:
            #cmd.append("aplay")
            cmd.append(WAV)
        
        cmd.insert(0,  "aplay")
        print(cmd)
        
    elif os.name == 'nt':
                #print("Windows Sound Volume")
                cmd = []
                cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, 'bin', 'sounder.exe'))
                cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, 'Audio', 'beep.wav'))
    # Windows
    else:
        exitCode = windows_tts(str(args.text),str(args.output),str(args.lang))

    exitCode, stdout, stderr = shellCommand.executeShellCommand(cmd=cmd)
    f.write(stdout + "\n\n")
    sys.exit(exitCode)
