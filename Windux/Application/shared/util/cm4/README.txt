To Flash:
https://www.raspberrypi.org/documentation/hardware/computemodule/cm-emmc-flashing.md

grab raspbian here: https://www.raspberrypi.org/software/operating-systems/
I grabbed the Raspberr Pi OS Lite

Before booting, edit the config.txt file in the boot directory ( for me this was /media/hobart/boot after I flashed the image ) at the bottom add the following line:
this will enable USB
dtoverlay=dwc2,dr_mode=host

you should now be able to boot the raspberry pi

when you boot up the defailt login is pi and password is raspberry
when you are logged in you can run sudo raspi-config to configure everything you need

Set up CAN ( 2 channel hat):
follow the guide here starting at "Install libraries":
https://www.waveshare.com/wiki/2-CH_CAN_HAT#Datasheet