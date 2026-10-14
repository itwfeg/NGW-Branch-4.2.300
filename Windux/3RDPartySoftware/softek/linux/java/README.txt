Softek Barcode Reader Version 7.1.* Java Interface

Manual pages for the properties and methods can be found at:

http://www.bardecode.com/help/index.html

Note that the following methods have not yet been implemented:

GetBarStringDirection, GetBarStringPos, ScanBarCodeFromDIB and SetScanRect

Linux i386 Installation
=======================

1. From the standard distribution of the barcode reader toolkit for linux, copy the file ../lib/libbardecode.so to /usr/lib or to a location specified by the environment variable LD_LIBRARY_PATH.

2. Copy ../lib/libbardecode_jni.so to /usr/lib or to a location specified by the environment variable LD_LIBRARY_PATH.

3. Set the CLASSPATH environment variable to include this folder (so that the Softek.Barcode class can be found).

4. Run the sample with the command "java sample"

The script go.sh will set LD_LIBRARY_PATH and CLASSPATH before running the sample java program.

Apple OSX Installation
======================

1. From the standard distribution of the barcode reader toolkit for linux, copy the file ../lib/libbardecode.dylib to /usr/lib or to a location specified by the environment variable LD_LIBRARY_PATH.

2. Copy ../lib/libbardecode_jni.jnilib to /usr/lib/java or to a location specified by the environment variable LD_LIBRARY_PATH.

3. Set the CLASSPATH environment variable to include this folder (so that the Softek.Barcode class can be found).

4. Run the sample with the command "java sample"

The script go.sh will set LD_LIBRARY_PATH and CLASSPATH before running the sample java program.
