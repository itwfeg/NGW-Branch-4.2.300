#!/usr/bin/env bash
# To install test station client
# sudo sh -x install_test_station.sh -c

# To install test station server
# sudo sh -x install_test_station.sh -s

# check if install has internet connection to download
# support packages. Done by attempting to ping google

export DISPLAY=:0.0

subnet="192.168.0"

if /bin/ping -c 3 -w 4 8.8.8.8 > /dev/null
then
    echo "Internet available pulling support packages down to install"
    sudo apt-get update
    sudo apt-get install qt5-default pyqt5-dev pyqt5-dev-tools
    sudo apt-get install eject
    sudo apt-get install sqlite3
    sudo apt-get install exfat-fuse
    sudo apt-get install exfat-utils
    sudo apt-get install dos2unix
else
    echo "Internet not available installing application only"
fi

# increase size of screen real estate by removing black borders
sed -e 's/^#disable_overscan=1$/disable_overscan=1/g' /boot/config.txt | /usr/bin/sudo tee /boot/config.txt > /dev/null

if ! grep -q "^dtparam=eee=off$" /boot/config.txt; then
    # disable ethernet energy efficiency feature of the NIC since it causes carrier lost
    /bin/echo "dtparam=eee=off" | /usr/bin/sudo tee -a /boot/config.txt > /dev/null
fi

while getopts "cs" OPTION; do
    case ${OPTION} in
    c)
        /bin/touch client_install

        /usr/bin/sudo rm -f /etc/hostname
        /bin/echo "rpi-test-client" | /usr/bin/sudo tee /etc/hostname > /dev/null
        /bin/sed -e "s/^127\.0\.1\.1[ \t]*raspberrypi$/127.0.1.1\trpi-test-client/g" /etc/hosts | /usr/bin/sudo tee /etc/hosts > /dev/null

        if grep -q "^interface eth0$" /etc/dhcpcd.conf; then
            /bin/sed -e '/^interface eth0$/,$d' /etc/dhcpcd.conf | /usr/bin/sudo tee /etc/dhcpcd.conf > /dev/null
        fi
        /bin/echo "interface eth0" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null
        /bin/echo "static ip_address=${subnet}.3/24" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null
        /bin/echo "static routers=${subnet}.1" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null
        /bin/echo "static domain_name_servers=8.8.8.8" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null

        # Install desktop wallpaper for test station on RPI do the following.
        # Only needs to be done once when the RPI Test Station is first setup
        # on a client to the Test Station server
        /usr/bin/pcmanfm --set-wallpaper hobart-client-wallpaper.png
        /bin/sync
        # install desktop configuration
        if [ -d /home/pi/.config ]
        then
            /bin/rm -rf /home/pi/.config
        fi
        /bin/cp -r .config /home/pi
        # Install desktop icon file for restart board test server
        /bin/rm -f /home/pi/Desktop/test_station_client.desktop
        /bin/sync
        /usr/bin/dos2unix boardTestStationExecutive.py
        /bin/cp -f test_station_client.desktop /home/pi/Desktop
        /bin/sync
        if [ -f /etc/init.d/board_test_server.sh ]
        then
            # stop a running board test server
            /usr/bin/sudo /etc/init.d/board_test_server.sh stop
            /usr/bin/sudo /usr/sbin/update-rc.d -f board_test_server.sh remove
        fi
        # if a new release was just installed it will be found in
        # board_test_server_new_release
        if [ -d /home/pi/test_station_client_new_release ]
        then
            # copy previous release ini config file to new release
            /bin/cp -f /home/pi/test_station_client/board_test_server.ini /home/pi/test_station_client_new_release
            # remove the old version of board test server
            /usr/bin/sudo /bin/rm -rf /home/pi/test_station_client
            # move new version to be current version
            /usr/bin/sudo /bin/mv /home/pi/test_station_client_new_release /home/pi/test_station_client
        fi
        /bin/sync
        /usr/bin/sudo /sbin/reboot
        ;;
    s)
        /bin/touch server_install

        /usr/bin/sudo rm -f /etc/hostname
        /bin/echo "rpi-test-server" | /usr/bin/sudo tee /etc/hostname > /dev/null
        /bin/sed -e "s/^127\.0\.1\.1[ \t]*raspberrypi$/127.0.1.1\trpi-test-server/g" /etc/hosts | /usr/bin/sudo tee /etc/hosts > /dev/null

#        if /bin/ping -c 3 -w 4 8.8.8.8 > /dev/null
#        then
#            # install dhcp server
#            /usr/bin/sudo apt-get install isc-dhcp-server
#        fi
#
#        if [ ! -f /var/lib/dhcp/dhcpd.leases ]
#        then
#            /bin/touch /var/lib/dhcp/dhcpd.leases
#        fi
#
#        remove INTERFACES lines in dhcp server default config
#        /bin/sed -e 's/INTERFACESv.*$//g' /etc/default/isc-dhcp-server | /usr/bin/sudo tee /etc/default/isc-dhcp-server > /dev/null
#        /bin/echo 'INTERFACESv4="eth0"' | /usr/bin/sudo tee -a /etc/default/isc-dhcp-server > /dev/null

#        if [ ! -f /etc/dhcp/dhcpd.conf ]
#        then
#            /bin/touch /etc/dhcp/dhcpd.conf
#        fi
#
#        if ! grep -q "^subnet 192\.168\.0\.0 netmask 255\.255\.255\.0 {$" /etc/dhcp/dhcpd.conf; then
#            /bin/echo "subnet 192.168.0.0 netmask 255.255.255.0 {" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo "    authoritative;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo "    option routers 192.168.0.1;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo "    option broadcast-address 192.168.0.255;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo "    option subnet-mask 255.255.255.0;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo '    option domain-name "mydomain.local";' | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo '    option domain-name-servers 8.8.8.8;' | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            # set default lease time to 4 days
#            /bin/echo "    default-lease-time 345600;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            # change the max-lease-time to 7 days
#            /bin/echo "    max-lease-time 604800;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo "    range 192.168.0.20 192.168.0.250;" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#            /bin/echo "}" | /usr/bin/sudo tee -a /etc/dhcp/dhcpd.conf > /dev/null
#        fi

        if grep -q "^interface eth0$" /etc/dhcpcd.conf; then
            /bin/sed -e '/^interface eth0$/,$d' /etc/dhcpcd.conf | /usr/bin/sudo tee /etc/dhcpcd.conf > /dev/null
        fi

        /bin/echo "interface eth0" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null
        /bin/echo "static ip_address=${subnet}.2/24" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null
        /bin/echo "static routers=${subnet}.1" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null
        /bin/echo "static domain_name_servers=8.8.8.8" | /usr/bin/sudo tee -a /etc/dhcpcd.conf > /dev/null

        # To install desktop wallpaper for test station on RPI do the following:
        /usr/bin/pcmanfm --set-wallpaper hobart-server-wallpaper.png

        # install desktop configuration
        if [ -d /home/pi/.config ]
        then
            /bin/rm -rf /home/pi/.config
        fi
        /bin/cp -r .config /home/pi

        # Install desktop icon file for restart board test server
        /bin/rm -f /home/pi/Desktop/test_station_server.desktop
        /bin/sync
        /usr/bin/dos2unix boardTestStationExecutive.py
        /bin/cp -f test_station_server.desktop /home/pi/Desktop
        /bin/sync
        if [ -f /etc/init.d/board_test_server.sh ]
        then
            # stop a running board test server
            /usr/bin/sudo /etc/init.d/board_test_server.sh stop
        fi

        # Install the board test server init.d daemon startup script by doing the following:
        /usr/bin/dos2unix boardTestServer.py
        /usr/bin/dos2unix board_test_server.sh
        /usr/bin/sudo /bin/chmod 755 boardTestServer.py
        /usr/bin/sudo /bin/chmod 755 boardTestStationExecutive.py
        /usr/bin/sudo /bin/cp -f board_test_server.sh /etc/init.d
        /usr/bin/sudo /bin/chmod 755 /etc/init.d/board_test_server.sh
        /usr/bin/sudo /usr/sbin/update-rc.d board_test_server.sh defaults

        # if a new release was just installed it will be found in
        # board_test_server_new_release
        if [ -d /home/pi/test_station_server_new_release ]
        then
            # if there is an existing board test database then back it up to new release
            # before removal of old version of board test server
            if [ -f /home/pi/test_station_server/board_test_server.sqlite ]
            then
                # copy sqlite db to new released version
                /bin/cp -f /home/pi/test_station_server/board_test_server.sqlite /home/pi/test_station_server_new_release
            fi
            # copy previous release ini config file to new release
            /bin/cp -f /home/pi/test_station_server/board_test_server.ini /home/pi/test_station_server_new_release
            # remove the old version of board test server
            /usr/bin/sudo /bin/rm -rf /home/pi/test_station_server
            # move new version to be current version
            /usr/bin/sudo /bin/mv /home/pi/test_station_server_new_release /home/pi/test_station_server
        fi
        /bin/sync
        /usr/bin/sudo /sbin/reboot
        ;;
    *)
        echo "Incorrect options provided"
        exit 1
        ;;
    esac
done
