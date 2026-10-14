#!/bin/bash

# renders all SVGs into PNGs using inkscape

for file in *.svg
do
  inkscape -w 1024 -h 1024 -e "$file.png" "$file"
done

