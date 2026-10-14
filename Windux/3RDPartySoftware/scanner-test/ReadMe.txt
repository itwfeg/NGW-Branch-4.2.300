Scanner test readme.
This test was created for manufacturing in Hillsboro. 
This is a go, no go test for testing the Hobart USB scanner for the self service scale.
This software will support a linux OS only at this time.  
To use the scanner test the following python packages must be installed on the target.
python2.7
pyusb
shellCommand
messageBox
pyudev

Modify the rc.local script under etc/init.d to call cameraDetect.py 
copy cameraDetect.py into /usr/local/hobart/python
