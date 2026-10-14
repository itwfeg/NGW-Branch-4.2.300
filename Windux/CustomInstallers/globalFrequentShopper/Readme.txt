Feature:
    Global Frequent Shopper Update (for Walmart Mexico)
    Walmart Mexico is currently on release 2.2.0.  This feature update contains
    the changes to get from 2.2.0 to 2.3.0, and then the Global Frequent
    Shopper enhancements.

    Note:  It was decided to NOT include the 2.3.0 enhancement to automatically
           install Office Viewer and PDF viewer in this feature update.

Signed Name:
    hti-linux-feature-globalfrequentshopperupdate-2.3.2-04212016_142329.package.tgz

Unsigned Names:
    hti-linux-feature-globalfrequentshopperupdate-2.3.2-04212016_142329.tgz
    hti-linux-feature-globalfrequentshopperupdate-2.3.2-04212016_142329.md5

What it does:
    Copies new version of chobart_d to the scale at the location:
        /usr/bin
    Copies new version of scaleui to the scale at the location:
       /usr/bin
    Copies new version of version.ini to the scale at the location:
        /etc/hobart
    Copies new version of SOFTWARE_VERSION_INFO to the scale at the location:
        /
    Copies new version of connectNetworkStartupNew.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of encryptDecrypt.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of killStartupNetworking.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of moduleFinder.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of processDeviceEvents.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of saveConfigs.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of setNetwork.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of startupSequence.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of updateCrontab.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of updateTimeNTP.py to the scale at the location:
        /usr/local/hobart/python
    Copies new version of dbftl.db to the scale at the location:
        /usr/local/hobart/db/scale/dbempty
    Copies new version of asound.conf to the scale at the location:
        /etc
    Copies vpu_fw_imx6d.bin to the scale at the location:
        /lib/firmware/vpu
    Copies vpu_fw_imx6q.bin to the scale at the location:
        /lib/firmware/vpu

Validation:
    Make sure the Global Frequent Shopper enhancements function correctly.
    Make sure the bug fixes and enhancements for 2.3.0 function correctly.
    Make sure the About Scale screen shows Application Version of "2.3.2"
    Make sure videos play on an HTs scale (this fix was added in also, as
       Walmart Mexico mainly uses HTs scales). 

Will this work under Windows?
    No

To generate the feature tarball and md5 file, execute the following on your Linux VM:

    python build_globalfrequentshopperupdate.py

