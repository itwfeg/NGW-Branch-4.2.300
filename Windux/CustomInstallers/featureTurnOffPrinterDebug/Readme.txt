
Feature:
    Disables debugging in the application by copying 'configdebug.dat', with
    specific debug settings disabled, to the scale.

Signed Name:
    hti-linux-feature-turn_off_printer_debug-2.3.2-04212016_142329.package.tgz

Unsigned Names:
    hti-linux-feature-turn_off_printer_debug-2.3.2-04212016_142329.tgz
    hti-linux-feature-turn_off_printer_debug-2.3.2-04212016_142329.md5

What it does:
    Copies new version of configdebug.dat to the scale at the location:
        /etc/hobart

Validation:
    After installing you will see a message dialog on the screen
    showing "/etc/hobart/configdebug.dat reloaded."


Will this work under Windows?
    No

To generate the feature tarball and md5 file (unsigned), execute the following on your Linux VM:

    $ python build_turn_off_printer_debug.py

To generate the signed feature file on your Linux VM:

    1.  Copy the unsigned .TGZ file to the directory above Windux
    2.  From a command window in the Windux directory, run the following command:

        $ ./create_signed_package.sh hti-linux-feature-XXXXX-N.N.N-MMDDYYYY_HHMMSS (without the file extension)

