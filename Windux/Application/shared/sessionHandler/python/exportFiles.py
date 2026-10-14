import os
import osUtils
import shutil
import argparse
import sys
import shellCommand
import glob
import string

if os.name == 'posix':
    exportDevicePath = os.path.join(os.sep, "media", "thumb", "hobart")
    systemDevicePath = os.path.join(os.sep,"usr","local","hobart")
else:
    exportDevicePath = os.path.join("D:", "export", "hobart")
    systemDevicePath = os.path.join("C:","Program Files","Hobart")

excludeFiles = {}

excludeFiles['audio'] = []
excludeFiles['audio'].append("bothChannels.aac")
excludeFiles['audio'].append("leftChannel.aac")
excludeFiles['audio'].append("rightChannel.aac")
excludeFiles['audio'].append("scanbeep.wav")

excludeFiles['images'] = []
if os.name == 'posix':
    excludeFiles['images'].append("system")
else:
    excludeFiles['images'].append('System')

# jhc: talked to jim highley 10-08-15 and he says we want these. Plus the documentation we
# passed out to Service, and Darrell approved, has all of these directories on it.
# So why were these excluded after the fact? Dont know but Im bringing them all back.
# Also reported as a bug: WEH-98 ESS Chile -Planograms folder is missing
# The previously excluded directories: planograms, products, softkeys, specials, selfServe

excludeFiles['layouts'] = []
excludeFiles['layouts'].append("defaultLayout.xml")
excludeFiles['layouts'].append("TLXML11.xml")
excludeFiles['layouts'].append("TLXML12.xml")
excludeFiles['layouts'].append("TLXML21.xml")
excludeFiles['layouts'].append("TLXML22.xml")
excludeFiles['layouts'].append("TLXML31.xml")
excludeFiles['layouts'].append("TLXML32.xml")
excludeFiles['layouts'].append("TLXML41.xml")
excludeFiles['layouts'].append("TLXML42.xml")
excludeFiles['layouts'].append("TLXML51.xml")
excludeFiles['layouts'].append("TLXML52.xml")
excludeFiles['layouts'].append("TLXML61.xml")
excludeFiles['layouts'].append("TLXML62.xml")
excludeFiles['layouts'].append("TLXML71.xml")
excludeFiles['layouts'].append("TLXML72.xml")
excludeFiles['layouts'].append("annun.png")
excludeFiles['layouts'].append("annunw.png")
excludeFiles['layouts'].append("background.png")
excludeFiles['layouts'].append("backgroundFS.png")
excludeFiles['layouts'].append("background_pick5.png")
excludeFiles['layouts'].append("bakery.png")
excludeFiles['layouts'].append("bakeryFS.png")
excludeFiles['layouts'].append("bakeryw.png")
excludeFiles['layouts'].append("bakeryw_adpanel.png")
excludeFiles['layouts'].append("bakeryw_fspanel.png")
excludeFiles['layouts'].append("deli.png")
excludeFiles['layouts'].append("deliFS.png")
excludeFiles['layouts'].append("deliw.png")
excludeFiles['layouts'].append("deliw_adpanel.png")
excludeFiles['layouts'].append("deliw_fspanel.png")
excludeFiles['layouts'].append("floral.png")
excludeFiles['layouts'].append("floralFS.png")
excludeFiles['layouts'].append("floralw.png")
excludeFiles['layouts'].append("floralw_adpanel.png")
excludeFiles['layouts'].append("floralw_fspanel.png")
excludeFiles['layouts'].append("grey.png")
excludeFiles['layouts'].append("greyFS.png")
excludeFiles['layouts'].append("greyw.png")
excludeFiles['layouts'].append("greyw_adpanel.png")
excludeFiles['layouts'].append("greyw_fspanel.png")
excludeFiles['layouts'].append("mainmarket.png")
excludeFiles['layouts'].append("meat.png")
excludeFiles['layouts'].append("meatFS.png")
excludeFiles['layouts'].append("meatw.png")
excludeFiles['layouts'].append("meatw_adpanel.png")
excludeFiles['layouts'].append("meatw_fspanel.png")
excludeFiles['layouts'].append("outOfService.png")
excludeFiles['layouts'].append("prePackAnnun.png")
excludeFiles['layouts'].append("produce.png")
excludeFiles['layouts'].append("produceFS.png")
excludeFiles['layouts'].append("producew.png")
excludeFiles['layouts'].append("producew_adpanel.png")
excludeFiles['layouts'].append("producew_fspanel.png")
excludeFiles['layouts'].append("seafood.png")
excludeFiles['layouts'].append("seafoodFS.png")
excludeFiles['layouts'].append("seafoodw.png")
excludeFiles['layouts'].append("seafoodw_adpanel.png")
excludeFiles['layouts'].append("seafoodw_fspanel.png")
excludeFiles['layouts'].append("zeroAnnun.png")
excludeFiles['layouts'].append("Readme.txt")

excludeFiles['1024_600'] = []
excludeFiles['1024_600'].append("defaultLayout.xml")
excludeFiles['1024_600'].append("TLXML11.xml")
excludeFiles['1024_600'].append("TLXML12.xml")
excludeFiles['1024_600'].append("TLXML21.xml")
excludeFiles['1024_600'].append("TLXML22.xml")
excludeFiles['1024_600'].append("TLXML31.xml")
excludeFiles['1024_600'].append("TLXML32.xml")
excludeFiles['1024_600'].append("TLXML41.xml")
excludeFiles['1024_600'].append("TLXML42.xml")
excludeFiles['1024_600'].append("TLXML51.xml")
excludeFiles['1024_600'].append("TLXML52.xml")
excludeFiles['1024_600'].append("TLXML61.xml")
excludeFiles['1024_600'].append("TLXML62.xml")
excludeFiles['1024_600'].append("TLXML71.xml")
excludeFiles['1024_600'].append("TLXML72.xml")
excludeFiles['1024_600'].append("adpanel.png")
excludeFiles['1024_600'].append("annun.png")
excludeFiles['1024_600'].append("annunw.png")
excludeFiles['1024_600'].append("background.png")
excludeFiles['1024_600'].append("backgroundFS.png")
excludeFiles['layouts'].append("background_pick5.png")
excludeFiles['1024_600'].append("bakery.png")
excludeFiles['1024_600'].append("bakeryFS.png")
excludeFiles['1024_600'].append("bakeryw.png")
excludeFiles['1024_600'].append("bakeryw_adpanel.png")
excludeFiles['1024_600'].append("bakeryw_fspanel.png")
excludeFiles['1024_600'].append("deli.png")
excludeFiles['1024_600'].append("deliFS.png")
excludeFiles['1024_600'].append("deliw.png")
excludeFiles['1024_600'].append("deliw_adpanel.png")
excludeFiles['1024_600'].append("deliw_fspanel.png")
excludeFiles['1024_600'].append("floral.png")
excludeFiles['1024_600'].append("floralFS.png")
excludeFiles['1024_600'].append("floralw.png")
excludeFiles['1024_600'].append("floralw_adpanel.png")
excludeFiles['1024_600'].append("floralw_fspanel.png")
excludeFiles['1024_600'].append("grey.png")
excludeFiles['1024_600'].append("greyFS.png")
excludeFiles['1024_600'].append("greyw.png")
excludeFiles['1024_600'].append("greyw_adpanel.png")
excludeFiles['1024_600'].append("greyw_fspanel.png")
excludeFiles['1024_600'].append("mainmarket.png")
excludeFiles['1024_600'].append("meat.png")
excludeFiles['1024_600'].append("meatFS.png")
excludeFiles['1024_600'].append("meatw.png")
excludeFiles['1024_600'].append("meatw_adpanel.png")
excludeFiles['1024_600'].append("meatw_fspanel.png")
excludeFiles['1024_600'].append("outOfService.png")
excludeFiles['1024_600'].append("prePackAnnun.png")
excludeFiles['1024_600'].append("produce.png")
excludeFiles['1024_600'].append("produceFS.png")
excludeFiles['1024_600'].append("producew.png")
excludeFiles['1024_600'].append("producew_adpanel.png")
excludeFiles['1024_600'].append("producew_fspanel.png")
excludeFiles['1024_600'].append("seafood.png")
excludeFiles['1024_600'].append("seafoodFS.png")
excludeFiles['1024_600'].append("seafoodw.png")
excludeFiles['1024_600'].append("seafoodw_adpanel.png")
excludeFiles['1024_600'].append("seafoodw_fspanel.png")
excludeFiles['1024_600'].append("zeroAnnun.png")

excludeFiles['1366_768'] = []
excludeFiles['1366_768'].append("defaultLayout.xml")
excludeFiles['1366_768'].append("TLXML11.xml")
excludeFiles['1366_768'].append("TLXML12.xml")
excludeFiles['1366_768'].append("TLXML21.xml")
excludeFiles['1366_768'].append("TLXML22.xml")
excludeFiles['1366_768'].append("TLXML31.xml")
excludeFiles['1366_768'].append("TLXML32.xml")
excludeFiles['1366_768'].append("TLXML41.xml")
excludeFiles['1366_768'].append("TLXML42.xml")
excludeFiles['1366_768'].append("TLXML51.xml")
excludeFiles['1366_768'].append("TLXML52.xml")
excludeFiles['1366_768'].append("TLXML61.xml")
excludeFiles['1366_768'].append("TLXML62.xml")
excludeFiles['1366_768'].append("TLXML71.xml")
excludeFiles['1366_768'].append("TLXML72.xml")
excludeFiles['1366_768'].append("adpanel.png")
excludeFiles['1366_768'].append("annun.png")
excludeFiles['1366_768'].append("annunw.png")
excludeFiles['1366_768'].append("background.png")
excludeFiles['1366_768'].append("backgroundFS.png")
excludeFiles['1366_768'].append("bakery.png")
excludeFiles['1366_768'].append("bakeryFS.png")
excludeFiles['1366_768'].append("bakeryw.png")
excludeFiles['1366_768'].append("bakeryw_adpanel.png")
excludeFiles['1366_768'].append("bakeryw_fspanel.png")
excludeFiles['1366_768'].append("deli.png")
excludeFiles['1366_768'].append("deliFS.png")
excludeFiles['1366_768'].append("deliw.png")
excludeFiles['1366_768'].append("deliw_adpanel.png")
excludeFiles['1366_768'].append("deliw_fspanel.png")
excludeFiles['1366_768'].append("floral.png")
excludeFiles['1366_768'].append("floralFS.png")
excludeFiles['1366_768'].append("floralw.png")
excludeFiles['1366_768'].append("floralw_adpanel.png")
excludeFiles['1366_768'].append("floralw_fspanel.png")
excludeFiles['1366_768'].append("grey.png")
excludeFiles['1366_768'].append("greyFS.png")
excludeFiles['1366_768'].append("greyw.png")
excludeFiles['1366_768'].append("greyw_adpanel.png")
excludeFiles['1366_768'].append("greyw_fspanel.png")
excludeFiles['1366_768'].append("mainmarket.png")
excludeFiles['1366_768'].append("meat.png")
excludeFiles['1366_768'].append("meatFS.png")
excludeFiles['1366_768'].append("meatw.png")
excludeFiles['1366_768'].append("meatw_adpanel.png")
excludeFiles['1366_768'].append("meatw_fspanel.png")
excludeFiles['1366_768'].append("outOfService.png")
excludeFiles['1366_768'].append("prePackAnnun.png")
excludeFiles['1366_768'].append("produce.png")
excludeFiles['1366_768'].append("produceFS.png")
excludeFiles['1366_768'].append("producew.png")
excludeFiles['1366_768'].append("producew_adpanel.png")
excludeFiles['1366_768'].append("producew_fspanel.png")
excludeFiles['1366_768'].append("seafood.png")
excludeFiles['1366_768'].append("seafoodFS.png")
excludeFiles['1366_768'].append("seafoodw.png")
excludeFiles['1366_768'].append("seafoodw_adpanel.png")
excludeFiles['1366_768'].append("seafoodw_fspanel.png")
excludeFiles['1366_768'].append("zeroAnnun.png")
excludeFiles['1366_768'].append("Readme.txt")
excludeFiles['1366_768'].append("1366_768")

excludeFiles['videos'] = []
excludeFiles['videos'].append("Accumulate_Items_Video.mp4")
excludeFiles['videos'].append("FlashKey_Basics_Video.mp4")
excludeFiles['videos'].append("NGW_ChangingLabels.mp4")
excludeFiles['videos'].append("NGW_Cleaning.mp4")
excludeFiles['videos'].append("NGW_CleaningthePrinthead.mp4")
excludeFiles['videos'].append("NGW_LoadingFilm.mp4")
excludeFiles['videos'].append("NGW_Training.mp4")
excludeFiles['videos'].append("Printer_Overview_Video.mp4")
excludeFiles['videos'].append("Scale_Navigation_Video.mp4")
excludeFiles['videos'].append("Search_Capabilities_Video.mp4")
excludeFiles['videos'].append("Super_ConfigOp_Video.mp4")
excludeFiles['videos'].append("Super_Cool_Video.mp4")
excludeFiles['videos'].append("Super_FlashkeyEd_Video.mp4")
excludeFiles['videos'].append("Super_OpScreen_Video.mp4")
excludeFiles['videos'].append("Super_Overview_Video.mp4")
excludeFiles['videos'].append("Tare_Functionality_Video.mp4")

def ignorePath(dirname, filenames):
    files_to_ignore = []
    # if the basename of the full directory path is in keys of excludeFiles
    if os.path.basename(dirname) in list(excludeFiles.keys()):
        # for each filename in the directory
        for i in range(0, len(filenames)):
            # if the filename is in list for direname key
            if filenames[i] in excludeFiles[os.path.basename(dirname)]:
                # ignore the file
                files_to_ignore.append(filenames[i])
    return files_to_ignore

def exportFiles(htPath = systemDevicePath,
                thumbd = exportDevicePath):

    # thumbd (destination) must have hobart on end of path when passed in from Qt Backup/Restore application
    try:
        dirTreeItems = []
        dirTreeItems.append("audio")        # on WIN this src dir is /Audio, on LINUX /audio
        dirTreeItems.append("images")       # on WIN this src dir is /Images, on LINUX /images
        dirTreeItems.append("layouts")      # on WIN this src dir is /Layouts, on LINUX /layouts
        dirTreeItems.append("playlists")    # on WIN this src dir is /Playlists, on LINUX /playlists
        dirTreeItems.append("user")         # on WIN this src dir is /User, on LINUX /user
        dirTreeItems.append("videos")       # on WIN this src dir is /Videos, on LINUX /video

        # NOTE that when we create these root directory names, they do not necessarily match the directory names
        #      that we are exporting from.  For example, on an HTx system, all of the dirs listed above
        #      are all actually uppercase. This is an issue on imports if we import from this export. Also on
        #      the export(HTx), the root dirs are uppercase, but the sub dirs under them will be pulled in with
        #      their uppercase leading chars because we export under these as is under these directories

        for curTreeItem in dirTreeItems:

            # htPath source tree is all files and directories under current tree item being processed
            src = os.path.join(htPath, curTreeItem)

            # thumbd (destination) of copy is the destination plus current tree item
            dst = os.path.join(thumbd, "hobart", curTreeItem)

            # does the directory already exists on thumbdrive device?
            if os.path.exists(dst):

                # remove the thumbdrive directory and all files/folders under it
                if os.name == 'posix':
                    os.system('/bin/rm --recursive --force "%s"' % dst)
                elif os.name == 'nt' or os.name == 'ce':
                    os.system('rd /s /q "%s"' % dst)

            # copy the system files(s) to the thumbdrive destination directory
            if os.name == 'posix':
                #cmd = '/bin/cp --recursive --force %s "%s"' % (src,dst)
                #file_io.writeToFile(os.path.join("/","tmp","out2.dat"),
                #            [cmd])
                #os.system(cmd)

                try:
                    shutil.copytree(src, dst, ignore=ignorePath)
                except:
                    pass
            else:
                shutil.copytree(src, dst, ignore=ignorePath)
                #os.system('xcopy /E /K /I /C /Q /H /R /Y "%s" "%s"' % (src,dst))

        if os.path.exists(os.path.join(os.sep, 'usr', 'local', 'hobart', 'info.log')):
            osUtils.copyFile(os.path.join(os.sep, 'usr', 'local', 'hobart', 'info.log'), os.path.join(thumbd, "hobart"), forced = True)

        return 0
    except:
        return 1

if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="export data files from specific tree items to thumb drive")
    parser.add_argument("systemPath",
                        action="store",
                        help="system path to the export files on the root filesystem which we export FROM")
    parser.add_argument("exportPath",
                        action="store",
                        help="export path is where to place the files on the thumb drive device")

    args = parser.parse_args()

    #file_io.writeToFile(os.path.join("/","tmp","out.dat"),
    #                    [args.systemPath, args.exportPath])

    exitCode = exportFiles(args.systemPath, args.exportPath)
    sys.exit(exitCode)
