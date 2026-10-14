rem Copy Linux i.MX6 Blow Fuse Programmer to build site
net use w: \\engdev1\builds build1mgr /user:buildmgr

copy /y acrBoardProgrammer.tgz w:\HTseries\test

net use w: /delete
