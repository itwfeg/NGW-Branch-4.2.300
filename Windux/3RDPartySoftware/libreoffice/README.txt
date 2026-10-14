Below are instructions for cross-compiling libreoffice 4.4 for HTi.  I used Ubuntu 16.04 x64 for this process.

DO NOT USE "Terminal"... USE "XTerm" INSTEAD (found this out the hard way)!. (To launch an XTerm window, type 'xterm' from a command prompt.)

prepare dependancies for build system
$ sudo apt-get install dmake
$ sudo apt-get install autoconf
$ sudo apt-get install gperf
$ sudo apt-get install xsltproc
$ sudo apt-get install libicu-dev
$ sudo apt-get install libgnomevfs2-dev
$ sudo apt-get install libgstreamer-plugins-base1.0-dev
$ sudo apt-get install libdbus-glib-1-dev
$ sudo apt-get install libgconf2-dev
$ sudo apt-get install libgl1-mesa-dev
$ sudo apt-get install libglu1-mesa-dev
$ sudo apt-get install libxml2-utils
$ sudo apt-get install bison
$ sudo apt-get install flex
$ sudo apt-get install epm

the nss include folder in my toolchain was not readable... make sure its readable:
$ sudo chmod +r /opt/hb6mxd_combo/toolchain/include/nss/*

add the timesys toolchain path to your environment:
$ PATH=/opt/hb6mxd_combo/toolchain/ccache:/opt/hb6mxd_combo/toolchain/bin:$PATH

cd to folder where you will extract libreoffice sources:
$ cd ~/Source/Local

untar the sources and copy in build config:
$ tar -xf ~/Windux/3RDPartySoftware/libreoffice/libreoffice-4.4.7.2.tar.xz
$ cd libreoffice-4.4.7.2
$ cp ~/Windux/3RDPartySoftware/libreoffice/autogen.input .

the configure script gets confused with python 3 and python 2... so lets clarify things temporarily
$ sudo mv /usr/bin/python3 /usr/bin/python3.orig

configure the sources
$ ./autogen.sh

repair python
$ sudo mv /usr/bin/python3.orig /usr/bin/python3

my source tree had a permissions issue, lets fix it
$ chmod +x bin/unpack-sources

***
*** My build had an issue during the packing process due to an old version of epm. I had to manually
*** modify all the packinfo files and remove revision requirements:
*** 1) browse to setup_native/source/packinfo/
*** 2) open all the packinfo_XXX.txt files
*** 3) for each "requires" line, remove the " %PACKAGEVERSION-%PACKAGEREVISION" text... i just did
***    a replace all using gedit
***

***
*** The build process below will download a number of packages... you *may* need to enable quota
*** time. If so, open a browser and go somewhere to enable quota time (i.e. extras.ubuntu.com).
***

make the sources
$ make

***
*** The build system on my laptop used all of my 8 cores in parallel.
***
*** My build bombed at one point (about 40m into build) trying to find header files for cppunit. I
*** had to build this package explicitly then resume make.
*** $ make cppunit
*** $ make
***
*** My build bomed again (around the 50m mark) trying to find idl something. I just re-ran make.
*** $ make
***
*** Now would be a good time to grab lunch!
***
*** Total build time took 2h 40m on my 8 core system.
***

copy debian package to HTi target
$ scp workdir/installation/LibreOffice_Dev/deb/install/LibreOfficeDev_4.4.7.2_Linux_deb_download/LibreOfficeDev_4.4.7.2_Linux_deb.tar.gz root@10.3.177.5:/root

on target install the core packages
# cd /root
# tar -xzf LibreOfficeDev_4.4.7.2_Linux_deb.tar.gz
# cd LibreOfficeDev_4.4.7.2_Linux_deb/DEBS
# dpkg -i --force-architecture *.deb

then run libreoffice:
# DISPLAY=:0 /opt/libreofficedev4.4/program/soffice --writer
