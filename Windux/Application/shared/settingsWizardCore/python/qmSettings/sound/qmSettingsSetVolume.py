import sys, os
from optparse import OptionParser
p = OptionParser()
p.add_option("-L", "--level",
             dest="level",
             help="percentage sound level")
p.add_option("-D", "--device",
             dest="",
             help="sound device 1=operator, 2=customer")
options, args = p.parse_args()
if os.name == "posix":
    # As of 1/13/2013 only the headphone jack hardware is defined. TBD the operator sound device control    
    os.system("amixer sset Headphone,0 " + str(options.level) + "%," + str(options.level) + "%")
    #    os.system("aplay beep.wav")
else:
    print("Windows Sound Volume")
