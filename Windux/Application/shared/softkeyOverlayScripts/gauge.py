#!/usr/bin/env python3

# displays the current CPU temperature as a gauge

temp = 0
image_path = "/usr/local/hobart/user/softkeyscripts/graphics/"

with open("/sys/devices/virtual/thermal/thermal_zone0/temp", 'r') as f:
	temp = int(int(f.read())/1000.0)

print("image (50,50) 100 100 fit %s" % image_path + "gauge.svg.png")
print("text (50,75) 12 #ff0000 Temp")
print("text (50,25) 12 #ff0000 CPU")
print("image (50,50) 100 100 fit %s" % image_path + "gauge_" + str(temp) + ".svg.png")
