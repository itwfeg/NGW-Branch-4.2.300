========================================================================
    CONSOLE APPLICATION : appInstaller Project Overview
========================================================================

AppWizard has created this appInstaller application for you.  

This file contains a summary of what you will find in each of the files that
make up your appInstaller application.


appInstaller.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

appInstaller.cpp
    This is the main application source file.
    

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named appInstaller.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.


This application is to be run once at bootup. it simply copies the new application
from the temporary directory (C:\Program Files\Hobart\vmTmp). It copies to the
working application directory (C:\Program Files\Hobart\Access Scale). It then
reenables the auto start of the scale service and the web service. It also
enables ie to be launched automatically at bootup. It then starts each service
and launches ie.

Versionmanager in scaleService.exe has put the new software in the temporary install
directory. After it had staged the new application image, it disabled the auto start of
the services. It also deleted the key to launch ie on bootup. It added appInstaller.exe
to the run key so that it would run on bootup. It then rebooted the scale.
/////////////////////////////////////////////////////////////////////////////
