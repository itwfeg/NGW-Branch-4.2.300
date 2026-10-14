import glob
import xml.etree.ElementTree as ET
import re
import sys

files = glob.glob('*.ui')

if len(sys.argv) != 3:
    print "Program requires two argument,(width, height)"
    sys.exit()
else:
    screen_width = float(sys.argv[1])
    screen_height = float(sys.argv[2])

WIDTH_RATIO = screen_width/1024.0
HEIGHT_RATIO = screen_height / 600.0
TOTAL_RATIO = (HEIGHT_RATIO + WIDTH_RATIO) / 2

def update_stylesheet (match):
    number = float(match.group(1))
    number = int(number * TOTAL_RATIO)
    return str(number) + 'px'

for file in files:
    tree = ET.parse(file)

    root = tree.getroot()
        
    for height in root.iter('height'):
        new_height = int(float(height.text) * HEIGHT_RATIO)
        height.text = str(new_height)

    for width in root.iter('width'):
        new_width = int(float(width.text) * WIDTH_RATIO)
        width.text = str(new_width)

    for x in root.iter('x'):
        new_x = int(float(x.text) * WIDTH_RATIO)
        x.text = str(new_x)
        
    for y in root.iter('y'):
        new_y = int(float(y.text) * HEIGHT_RATIO)
        y.text = str(new_y)
        
    for font in root.iter('pointsize'):
        new_font = int(float(font.text) * TOTAL_RATIO)
        font.text = str(new_font)
        
    for font in root.iter('weight'):
        new_font = int(float(font.text) * TOTAL_RATIO)
        font.text = str(new_font)
        
    for prop_tag in root.iter('property'):
        if prop_tag.attrib.get('name') == 'styleSheet':
            child = prop_tag.getchildren()[0]
            if child.text:
                child.text = re.sub('([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])px', 
                update_stylesheet, child.text)
    
    tree.write(file)
    
    
    
