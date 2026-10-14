Feature:

    HTx Extend Desktop
    
Signed Name:
    hti-linux-feature-safeway_remote_maintenance-2.2.301-12022015_022445.package.tgz

Unsigned Names (unsigned):
    htx-win-feature-extenddisplay-1.18.0-03282016_140000.tgz
    htx-win-feature-extenddisplay-1.18.0-03282016_140000.md5

What it does:
    Copies new version of startupSequenceWin32.py to the scale at the location:
        C:\Program Files\Hobart\Python

    Copies new version of version.ini to the scale at the location:
        C:\Program Files\Hobart

    Copies new version of SOFTWARE_VERSION-INFO to the scale at the location:
        C:\Program Files\Hobart

Validation:
    Make sure the display resolution is correct ... i.e. Extended Desktop
    Make sure the About Scale screen shows Application Version of "1.18.0"
    
Will this work under Windows?
    Yes - it will only work on Windows

To generate the feature tarball and md5 file, execute the following:    
    Python build_extenddisplay.py