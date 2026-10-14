
Building NGW upgrades (This can run executed from Windows or your Linux VM):
============================================================================

1. Checkout out latest firmware files from http://engdev1.itwfeg.biz/svn/wrapper/firmwareFiles.

   There are three directories here named "aws3", "ngw" and "camera", with sub-directories
   named with the version (e.g. 8.25, 800.25, etc).

2. cd to Windux/buildWrapperImageTools

3. Copy s-record files from the firmware files you checked out above to Windux/buildWrapperImageTools

   The filenames should be:

       Node1_APP.elf.p.S
       Node2_APP.elf.p.S
       Node3_APP.elf.p.S
       Node4_APP.elf.p.S
       Node5_APP.elf.p.S
       WrapperCameraSystem.elf.p.S

4. Open a command window and execute buildWrapperImage.py, passing appropriate version info:

    python buildWrapperImage.py ngw 8.1.0   8.1 7.1
                                 ^    ^      ^   ^
                                 |    |      |   |
                                 |    |      |   |---------------------------- revision of node 6 (camera)
                                 |    |      |------------------------ revision of the nodes (1 - 5)
                                 |    |----------------------- revision of the upgrade file (in case different)
                                 |----------------------- the type of node board release: ngw or aws3

5. A tarball named htseries-any-ngw-X.X.X-release.package.tgz or htseries-any-aws3-X.X.X-release.package.tgz
   will be generated.  Copy the tarball to Windux/CustomInstallers/featureWrapperFirmware and check it into SVN.

6. If the name of the tarball changed, modify Windux/installToTarget.sh to copy the new tarball from
   CustomInstallers/featureWrapperFirmware to ${target}${liveRfs}/root/firmware/
   
   Make sure you commit the change of installToTarget.sh to SVN.

   UPDATE: Beginning with release 3.4.0, the wrapper firmware upgrade file can be built as part of the
           application build (i.e. installToTarget), and the version of the upgrade file is
           set by a shell variable inside installToTarget.sh --> i.e. wrapperFirmwareVer
