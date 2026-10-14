
Feature:
    Copies the "debug.log" file from the scale to the thumb drive.

    ASSUMES the "debug.log" file is created in /usr/local/hobart.

Signed Name:
    hti-linux-feature-copy_printer_debug-2.3.2-04212016_142329.package.tgz

Unsigned Names:
    hti-linux-feature-copy_printer_debug-2.3.2-04212016_142329.tgz
    hti-linux-feature-copy_printer_debug-2.3.2-04212016_142329.md5

What it does:
    Copies the "debug.log" file from /usr/local/hobart
    to the thumb drive (/media/thumb).

Validation:
    After installing the feature, make sure the "debug.log" file
    is now on the thumb drive.

Will this work under Windows?
    No

To generate the feature tarball and md5 file (unsigned), execute the following on your Linux VM:

    $ python build_copy_printer_debug.py

To generate the signed feature file on your Linux VM:

    1.  Copy the unsigned .TGZ file to the directory above Windux
    2.  From a command window in the Windux directory, run the following command:

        $ ./create_signed_package.sh hti-linux-feature-XXXXX-N.N.N-MMDDYYYY_HHMMSS (without the file extension)

