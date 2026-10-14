#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import osUtils
import shutil
import argparse
import sys
import shellCommand
import glob

if os.name == 'posix':
    importDevicePath = os.path.join(os.sep,"media","thumb","import")
    systemDevicePath = os.path.join(os.sep,"usr","local","hobart")
else:
    importDevicePath = os.path.join("d:","hobart")
    systemDevicePath = os.path.join("c:","Program Files","Hobart")


def directory_rename(base_dir, dir_names):
    # I need to rename dpecific directories on a thumbdrive depending what type of OS we are importing into
    # starts from the bottom so paths further up remain valid after renaming, walk(dirpath, dirnames, filenames).

    for root, dirs, files in os.walk( base_dir, topdown=False ):
        for dir_name in dir_names:
            new_dir = os.path.join( base_dir.lower(),  dir_name.lower() )
            if( root.lower() == new_dir ):
                try:
                    os.rename( root, os.path.join( base_dir, dir_name ) )   # rename(source, newname)
                    break
                except OSError:
                    pass # can't rename it, so what do we do now?

    # now as a last step, rename the hobart part of base_dir directory too, but this is going to be a little tricky
    # This worked on Windows, except the renaming part. Got the dirs ok, Access is denied when doing it.
    # Hopefully I wont need this block of code but Im leaving it here thru the first past just in case.
#    if os.name == 'posix':
#        hobart_base = 'hobart'
#    else:
#        hobart_base = 'Hobart'
#
#    new_base_dir = base_dir
#    if new_base_dir.endswith( hobart_base.lower() ):
#        j = base_dir.rfind( hobart_base.lower() )
#        new_base_dir = base_dir[:j] + hobart_base
#
#        try:
#            os.rename( root, new_base_dir )   # rename(source, newname)
#        except OSError:
#            pass # can't rename it, so what do we do now?

def importFiles(thumbd = importDevicePath,
                htPath = systemDevicePath):
    try:

        dirTreeItems = []

        # Windows thumbdrive directory structure required. The thumbdrive will have been updated to this with directory_rename()
        if os.name == 'posix':
            dirTreeItems.append("audio")
            dirTreeItems.append("images")
            dirTreeItems.append("images/categories")
            dirTreeItems.append("images/planograms")
            dirTreeItems.append("images/products")
            dirTreeItems.append("images/selfServe")
            dirTreeItems.append("images/softkeys")
            dirTreeItems.append("images/specials")
            dirTreeItems.append("layouts")
            dirTreeItems.append("merge")
            dirTreeItems.append("playlists")
            dirTreeItems.append("system")
            dirTreeItems.append("system/graphics")
            dirTreeItems.append("user")
            dirTreeItems.append("user/graphics")
            dirTreeItems.append("videos")
            dirTreeItems.append("hobart")
        else:
            dirTreeItems.append("Audio")
            dirTreeItems.append("Images")
            dirTreeItems.append("Images\Categories")
            dirTreeItems.append("Images\Planograms")
            dirTreeItems.append("Images\Products")
            dirTreeItems.append("Images\SelfServe")
            dirTreeItems.append("Images\Softkeys")
            dirTreeItems.append("Images\Specials")
            dirTreeItems.append("Layouts")
            dirTreeItems.append("Merge")
            dirTreeItems.append("Playlists")
            dirTreeItems.append("System")
            dirTreeItems.append("System\Graphics")
            dirTreeItems.append("User")
            dirTreeItems.append("User\Graphics")
            dirTreeItems.append("Videos")

        directory_rename(thumbd, dirTreeItems)

        # FT-1503 backup layouts, we are not allowing layout xml files to be imported
        if os.name == 'posix':
            cmd = []
            cmd.append("/bin/mkdir")
            cmd.append("/tmp/.layouts")
            os.system(" ".join(cmd))

            cmd = []
            cmd.append("/bin/cp")
            cmd.append("-r")
            cmd.append(os.path.join(htPath, "layouts", "*.xml"))
            cmd.append("/tmp/.layouts/")
            os.system(" ".join(cmd))

        for curTreeItem in dirTreeItems:
            src = os.path.join(thumbd, curTreeItem,"*")
            dst = os.path.join(htPath, curTreeItem)

            # only copy files under images and not subdirectories and their contents
            # Also note that copy EXPECTS the destination dirs to exist, which they better
            cmd = []

            if os.name == 'posix':
                cmd.append("/bin/cp")
                # Large Elevated Customer Display - we will need to copy layouts recursively.
                if curTreeItem == "layouts":
                    cmd.append("-r")
                cmd.append("--force")
            else:
                cmd.append("copy")

            # do not perform a recursive copy on copy files immediately under the specified directory
            cmd.append(src)
            cmd.append(dst)
            os.system(" ".join(cmd))

        # FT-1503 restore layouts
        if os.name == 'posix':
            cmd = []
            cmd.append("/bin/rm")
            cmd.append("-f")
            cmd.append(os.path.join(htPath, "layouts", "*.xml"))
            os.system(" ".join(cmd))

            cmd = []
            cmd.append("/bin/cp")
            cmd.append("-r")
            cmd.append("--force")
            cmd.append("/tmp/.layouts/*.xml")
            cmd.append(os.path.join(htPath, "layouts"))
            os.system(" ".join(cmd))

            cmd = []
            cmd.append("/bin/rm")
            cmd.append("-rf")
            cmd.append("/tmp/.layouts")
            os.system(" ".join(cmd))

        return 0
    except:
        return 1

if __name__ == "__main__":
    exitCode = 1

    parser = argparse.ArgumentParser(description="import data files from specific tree items")
    parser.add_argument("importPath",
                        action="store",
                        help="source path from which to import files from such as thumb drive")
    parser.add_argument("systemPath",
                        action="store",
                        help="destination path to place files in on the HTx scale")

    args = parser.parse_args()

    exitCode = importFiles(args.importPath, args.systemPath)

    sys.exit(exitCode)
