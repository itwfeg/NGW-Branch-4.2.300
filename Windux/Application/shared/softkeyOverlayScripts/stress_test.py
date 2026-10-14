#!/usr/bin/env python3

# places a large number of rectangles, images, and texts on the screen

for i in range(0,20):
  for j in range(0,20):
    color = "#ffffff" if (i+j) % 2 else "#555555"
    print("rectangle (%s,%s) 5 5 %s" % (i*5, j*5, color))

for i in range(0,5):
  for j in range(0,5):
    print("image (%d,%d) 10 10 stretch /usr/local/hobart/user/softkeyscripts/graphics/gauge.svg.png" % (i*20 + 10, j*20 + 10))

print("font London")
for i in range(0,10):
  for j in range(0,10):
    print("text (%d,%d) 25 #aa00aa %s" % (i*10 + 5, j*10 + 5, i))
