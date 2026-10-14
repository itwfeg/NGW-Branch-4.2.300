import os, sys

firstPartCommand  = "sudo echo - n "
secondPartCommand = " > /sys/class/backlight/pwm-backlight.3/brightness"

# Brighten the screen gradually to full brightness
for i in range(70,256):
    os.system(str(firstPartCommand) + str(i) + str(secondPartCommand))
    print(str(firstPartCommand) + str(i) + str(secondPartCommand))

