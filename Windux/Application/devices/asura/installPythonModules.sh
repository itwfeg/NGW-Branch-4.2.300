#!/bin/bash
cd /etc/opkg
opkg update
opkg install python-modules
opkg install python-dbus
opkg install procps
