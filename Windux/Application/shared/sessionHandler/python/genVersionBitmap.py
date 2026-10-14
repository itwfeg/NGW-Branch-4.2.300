#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw
import argparse
import sys
import os

def writeVersionText(version, filename, rotate):
    # 195x28 black background image
    # Version 9.9.9 format
    try:
        if os.path.exists(filename):
            os.remove(filename)

        img = Image.new('L', (195, 28), 'black')
        draw = ImageDraw.Draw(img)

        font = None

        try:
            font = ImageFont.truetype("arial.ttf", 26)
        except:
            pass
        
        if font is None:
            font = ImageFont.load_default()

        size = draw.textsize("Version %s" % (version),  font,  0)
        centerOffset = (195 - size[0]) / 2

        draw.text((centerOffset, 0),"Version %s" % (version), 255,  font=font)

        if ((90==rotate) or (180==rotate) or (270==rotate)):
            img = img.rotate(rotate, expand=True)

        img.save(filename)
        return 0
    except:
        return 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Generate bitmap with version string in it.")
    parser.add_argument("version", action="store", help="version number in format 9.9.9")
    parser.add_argument("bitmap", action="store", help="bitmap filename to create")
    parser.add_argument("--rotate", type=int, default=0, dest='rotate', help="bitmap rotation")
    parser.set_defaults(rotate=0)

    args = parser.parse_args()

    exitCode = writeVersionText(args.version, args.bitmap, args.rotate)
    sys.exit(exitCode)
