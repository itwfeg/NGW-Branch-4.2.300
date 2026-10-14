#!/usr/bin/env python3

# displays percent CPU usage, yellow if > 50%, red if > 85%

from seven_segment import draw_display
import time


time_0 = None
time_1 = None

with open("/proc/stat") as f:
  u, n, s, i, io, _, _, _, _, _ = [int(i) for i in f.readline().split() if i != "cpu"]

time_0 = (u+n+s, i+io)

time.sleep(1)

with open("/proc/stat") as f:
  u, n, s, i, io, _, _, _, _, _ = [int(i) for i in f.readline().split() if i != "cpu"]

time_1 = (u+n+s, i+io)

cpu_used = time_1[0] - time_0[0]
all_cpu = time_0[1] + time_1[1] + cpu_used

percent = 100 * (time_1[0] - time_0[0])/ ((time_1[1] - time_0[1]) + (time_1[0] - time_0[0]))

if percent > 99:
  percent = 99

second_digit = int(percent) % 10
first_digit = int(percent / 10)

color = "#00ff00"

if percent > 50:
  color = "#ffff00"

if percent > 85:
  color = "#ff0000"

print("rectangle (50,50) 100 100 %s" % color)
print("rectangle (50,50) 98 98 #000000")
draw_display(number=first_digit, x=56, y=50, width=25, height=95, color_on=color, color_off="#101010")
draw_display(number=second_digit, x=84, y=50, width=25, height=95, color_on=color, color_off="#101010")
print("text (20,35) 16 %s CPU" % color)
print("text (20,65) 25 %s %%" % color)
