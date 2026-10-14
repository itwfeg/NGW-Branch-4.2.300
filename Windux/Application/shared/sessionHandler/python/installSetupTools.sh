#!/bin/bash

# we replace this with a newer version via pip later in the install process
if [ ! -f /usr/lib/python2.7/site-packages/setuptools.pth ]; then
    cd /usr/local/hobart/python
    pathToSetup="/usr/local/hobart/python/"
    chmod a+x $pathToSetup"setuptools-0.6c11-py2.7.egg"
    wait
    /bin/sh /usr/local/hobart/python/setuptools-0.6c11-py2.7.egg
    wait
    /bin/sync
    sleep 2
fi

