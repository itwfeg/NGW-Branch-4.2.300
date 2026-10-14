#!/usr/bin/env python3

# seven segment display
# when run as a script, this presents a clock like HH:MM in red
# draw_display may be imported by other scripts to render digits similarly

import datetime

def rect(x, y, w, h, color):
  print("rectangle (%s,%s) %s %s %s" % (int(x), int(y), int(w), int(h), color))

def draw_display(number, x, y, width, height, color_on="#ff0000", color_off="#100000", color_bg="#000000"):
  # seven segment display regions
  #
  #  AAA
  # F   B
  # F   B
  #  GGG
  # E   C
  # E   C
  #  DDD 
  
  status = {}

  # bg
  rect(x, y, width, height, color_bg)
  
  # middle horizontal
  
  status["G"] = color_on if number in [2,3,4,5,6,8,9] else color_off
    
  # bottom horizontal
  status["D"] = color_on if number in [2,3,5,6,8,0,9] else color_off
    
  # top horizontal
  status["A"] = color_on if number in [2,3,5,6,7,8,9,0] else color_off
    
  # top left vertical
  status["F"] = color_on if number in [4,5,6,8,9,0] else color_off

  # top right vertical
  status["B"] = color_on if number in [1,2,3,4,7,8,9,0] else color_off
    
  # bottom left vertical
  status["E"] = color_on if number in [2,6,8,0] else color_off
    
  # bottom right vertical
  status["C"] = color_on if number in [1,3,4,5,6,7,8,9,0] else color_off
  
  rect(x, y, width*0.5, height*0.07, status["G"])
  rect(x, y+0.9*height/2, width*0.5, height*0.07, status["D"])
  rect(x, y-0.9*height/2, width*0.5, height*0.07, status["A"])
  
  rect(x-width*0.4, y+(0.45*height)/2, width*0.2, height*0.4, status["E"])
  rect(x+width*0.4, y+(0.45*height)/2, width*0.2, height*0.4, status["C"])
  rect(x-width*0.4, y-(0.45*height)/2, width*0.2, height*0.4, status["F"])
  rect(x+width*0.4, y-(0.45*height)/2, width*0.2, height*0.4, status["B"])

if __name__ == "__main__":
  now = datetime.datetime.now()

  first_digit = " "
  second_digit = " "

  if 9 < now.hour < 13:
    first_digit = 1

  second_digit = now.hour % 10 if now.hour < 13 else (now.hour - 12) % 10

  rect(50,50,100,100,"#000000")
  draw_display(number=first_digit, x=12, y=50, width=22, height=95)
  draw_display(number=second_digit, x=22+2+11, y=50, width=22, height=95)
  draw_display(number=int(now.minute/10), x=89-2-22, y=50, width=22, height=95)
  draw_display(number=now.minute%10, x=88, y=50, width=22, height=95)
  
  print("circle (%d,%d) %d %s" % (50,50+(0.45*95)/2,4,"#ff0000"))
  print("circle (%d,%d) %d %s" % (50,50-(0.45*95)/2,4,"#ff0000"))
