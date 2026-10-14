Asura OS login info:
user: root
pwd: thermal

You can use Qt Creator by setting up a new kit:
Debugger: /usr/local/angstrom_1.1/arm/bin/arm-angstrom-linux-gnueabi-gdb
Compiler /usr/local/angstrom_1.1/arm/bin/arm-angstrom-linux-gnueabi-g++
Make: /usr/local/Trolltech/Qt-4.8.1/bin/qmake

----

A cross compiler is necessary to build C/C++ based applications for the Asura. While it should be possible to use any cross compiler that can target the Arm eabi platform, Star provides a pre-built toolchain for x86 Linux, intended to help you begin development quickly.

The Star toolchain is recommended for most developers since it:

Provides a high level of optimization for the Asura's CPU, including NEON optimization.
Is built around the same libraries that are supplied with the ACP OS
Ready to use support for Qt4.8, including QtQuick and QtCreator integration.
Installation

Download the toolchain archive.
With superuser permissions, extract the archive into your systems root folder:

sudo tar -xvzf asura-toolchain-1.1.tar.gz -C /

This will extract the toolchain to "/usr/local/angstrom_1.1/arm" and an ACP compatible Qt to "/usr/local/Trolltech/Qt-4.8.1/", these locations may vary with future updates.

Command line usage
------------------

To use the toolchain from a Linux terminal, first you must run the environment set-up script. This should be invoked from the folder that contains your project source code:

source /usr/local/angstrom_1.1/arm/environment-setup

Invoking GCC directly
---------------------

Now, you can invoke the standard gcc compile tools (gcc, g++, ln, ldd etc.), but each has the prefix "arm-angstrom-linux-gnueabi-". For example, to compile the traditional "hello.c":

arm-angstrom-linux-gnueabi-gcc -o hello hello.c

Compiling a Qt4 project
-----------------------

Qt produces .pro files, which store information about the project files and how they should be compiled. A .pro is platform neutral, it will be created for you by QtCreator (if used), and is used to build your project either locally (desktop Linux) or for the Asura hardare.

When building a Qt project, the first task is to generate a "Makefile" that is suitable for your target device, this is done with the "qmake" utility. To build your project for the ACP:

Change Directory into the folder that contains your project file (*.pro) and run the cross compiler environment set-up script.
If any existing build files are in place, these should be removed:
make clean
rm Makefile

Run qmake, with the Asura platform specification, to generate a suitable makefile:
qmake-acp

A standard "Makefile" will be generated in your current folder, which can be used to build you project.
Simply execute "make" to compile your project.
The resulting binary can be copied over to an ACP and executed directly from a terminal. Or it can be packaged as an ACP application (with a suitable "application.conf" file) to be launched from the AsuraDesktop, or at boot time.
