@echo on
::
:: deploy.cmd
:: Copy all HTe binaries and required files into one location suitable for deployment.
::
:: Copyright (c) 2013 Hobart Corporation. All Rights Reserved.
::
:: Note:
:: This script is intended to be run from the Windux\Application folder.
::
:: Prerequisites:
:: - Qt Libraries 4.8.5 for Visual Studio 2010
::

:: Qt4
::set QTDIR=C:\Qt\4.8.5

:: Qt5
set DISTRIBUTIONSHAREDIR=C:\DSSP1

set SVNDIR=C:\HTxx86

:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

cd C:\HTxx86\Windux\Application\

buildall.cmd

Deploy.cmd

cd C:\HTxx86\Os\windows\wes7\builds

deploy.cmd


:done
