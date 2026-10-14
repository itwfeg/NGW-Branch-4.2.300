
Feature:
    Enable debugging in the application by copying 'configdebug.dat', with
    specific debug settings enabled, to the scale.

    The "debug.log" file MUST be created in /usr/local/hobart, to be in
    sync with the corresponding "CopyDebug" feature.


Signed Name:
    hti-linux-feature-turn_on_printer_debug-2.3.2-04212016_142329.package.tgz

Unsigned Names:
    hti-linux-feature-feature-turn_on_printer_debug-2.3.2-04212016_142329.tgz
    hti-linux-feature-feature-turn_on_printer_debug-2.3.2-04212016_142329.md5

What it does:
    Copies new version of configdebug.dat to the scale at the location:
        /etc/hobart

Validation:
    After installing you will see a message dialog on the screen
    showing "/etc/hobart/configdebug.dat reloaded."

    You can also log into the scale remotely and make sure
    the "debug.log" file is generated in /usr/local/hobart.


Will this work under Windows?
    No

To generate the feature tarball and md5 file, execute the following on your Linux VM:

    $ python build_turn_on_printer_debug.py

To generate the signed feature file on your Linux VM:

    1.  Copy the unsigned .TGZ file to the directory above Windux
    2.  From a command window in the Windux directory, run the following command:

        $ ./create_signed_package.sh hti-linux-feature-XXXXX-N.N.N-MMDDYYYY_HHMMSS (without the file extension)

