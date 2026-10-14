import sys, os
if os.name == "posix":
    #os.system("amixer sset Headphone,0 " + str(options.level) + "%," + str(options.level) + "%")
    os.system("aplay beep.wav")
else:
    print("Windows Sound Volume")
