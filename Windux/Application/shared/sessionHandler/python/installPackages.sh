#!/bin/bash

# to download all readily-installable python packages:
# pip3 download --no-deps requirements-python3.txt

preinstall_dir=/usr/local/hobart/preinstall

echo "Installing python2 and python3 packages."

# check for pip, if not, use pip package to install itself
if [ ! -f /usr/local/bin/pip3 ]; then
    python3 ${preinstall_dir}/pip-19.1.1-py2.py3-none-any.whl/pip install --no-index ${preinstall_dir}/pip-19.1.1-py2.py3-none-any.whl
fi

# attempt to install packages from package path
# pip checks if each is installed first
/usr/local/bin/pip3 install --no-deps --no-index --find-links=${preinstall_dir} -r ${preinstall_dir}/requirements-python3.txt

# check for web2py
if [ ! -d /usr/lib/python3.6/site-packages/gluon/ ]; then
    if [-f /etc/hobart/blackbox]; then
      /usr/local/bin/pip3 install --no-deps --no-index ${preinstall_dir}/web2py-2.18.4.tar.gz
    fi
fi

# check for dpkg
if [ ! -d /var/lib/dpkg ]; then
    /bin/mkdir /var/lib/dpkg
fi

if [ ! -f /var/lib/dpkg/status ]; then
    /bin/touch /var/lib/dpkg/status
fi

if [ ! -f /var/lib/dpkg/available ]; then
    /bin/touch /var/lib/dpkg/available
fi

mkdir -p /root/logs

# install pyqt5 (with sip) via dpkg
if [ ! -d /usr/lib/python3.6/dist-packages/PyQt5 ]; then
    /usr/bin/dpkg -i ${preinstall_dir}/hobart-hti-pyqt_5.5-1_armhf.deb
fi

# check for twisted
if ! python3 -c "import twisted" ; then
    tar xzvf ${preinstall_dir}/Twisted-19.10.0.linux-armv7l.tar.gz -C /
fi

# check for zope
if ! python3 -c "import zope.interface" ; then
    tar xzvf ${preinstall_dir}/zope.interface-4.7.1.linux-armv7l.tar.gz -C /
fi

wait
sync
