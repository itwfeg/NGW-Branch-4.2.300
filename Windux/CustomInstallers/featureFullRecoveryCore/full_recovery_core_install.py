import os
import sys
import messageBox
import subprocess

featureName = "full_recovery_core"
tarfileName = 'recovery.tgz'

if (os.path.isfile(os.path.join(os.sep, "media", "mmcblk0p1", "SOFTWARE_VERSION_INFO")) and os.path.isfile(
        os.path.join(os.sep, "SOFTWARE_VERSION_INFO"))):
    try:
        if (os.path.samefile(os.path.join(os.sep, "media", "mmcblk0p1", "SOFTWARE_VERSION_INFO"),
                             os.path.join(os.sep, "SOFTWARE_VERSION_INFO"))):
            recoveryPartition = os.path.join(os.sep, "media", "mmcblk0p2")
            recovery = os.path.join(recoveryPartition, "usr", "local", "hobart")
            live = os.path.join(os.sep, "media", "mmcblk0p1")
            liveHobart = os.path.join(live, "usr", "local", "hobart")
    except:
        print("Failed mmcblk0")
        pass
elif (os.path.isfile(os.path.join(os.sep, "media", "mmcblk1p1", "SOFTWARE_VERSION_INFO")) and os.path.isfile(
        os.path.join(os.sep, "SOFTWARE_VERSION_INFO"))):
    try:
        if (os.path.samefile(os.path.join(os.sep, "media", "mmcblk1p1", "SOFTWARE_VERSION_INFO"),
                             os.path.join(os.sep, "SOFTWARE_VERSION_INFO"))):
            recoveryPartition = os.path.join(os.sep, "media", "mmcblk1p2")
            recovery = os.path.join(recoveryPartition, "usr", "local", "hobart")
            live = os.path.join(os.sep, "media", "mmcblk1p1")
            liveHobart = os.path.join(live, "usr", "local", "hobart")
    except:
        print("Failed mmcblk1")
        sys.exit(1)
else:
    print("Found Neither file")
    sys.exit(1)

# extract the update to the primary partition
exitCode, pid = messageBox.display("Extracting files to Recovery partition. Please Wait...")
os.system('tar -xvzf {0} -C {1}'.format(tarfileName, recoveryPartition))
os.system('sync')
messageBox.destroy(pid)
exitCode, pid = messageBox.display("File Extraction Complete.", timer=2)
subprocess.Popen("python -c \"import shutil, time; time.sleep(1); print('{0}'); shutil.rmtree('{0}');\"".format(os.path.dirname(os.path.realpath(__file__))), shell=True)
sys.exit(0)
