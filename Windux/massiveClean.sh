#!/bin/bash
# Removes all files to make sure chobart_d builds from scratch
set -x
# removes all build artifacts that our Makefiles do not
find . -name *.o -exec rm -f \{\} \;
find . -name *.oo -exec rm -f \{\} \;
find . -name ui_*.h -exec rm -f \{\} \;
find . -name moc_* -exec rm -f \{\} \;
find . -name *.a -not -path "./3RDPartySoftware/*" -exec rm -f \{\} \;
