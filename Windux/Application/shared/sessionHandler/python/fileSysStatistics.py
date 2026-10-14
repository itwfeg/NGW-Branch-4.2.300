# -*- coding: utf-8 -*-

import os

class DirSizeError(Exception): pass

def availableSpace(partition):
    if os.name == 'nt':
        import win32api
        try:
            details = win32api.GetDiskFreeSpace(partition)
            # Free bytes = (Sectors Per Cluster)*(Bytes Per Sector)*(Number of Free Clusters)
            return details[0] * details[1] * details[2]
        except:
            return 0
    else:
        stat = os.statvfs(partition)
        return stat.f_frsize * stat.f_bfree

def totalSize(partition):
    if os.name == 'nt':
        import win32api
        try:
            details = win32api.GetDiskFreeSpace(partition)
            # Free bytes = (Sectors Per Cluster)*(Bytes Per Sector)*(Number of Total Clusters)
            return details[0] * details[1] * details[3]
        except:
            return 0
    else:
        stat = os.statvfs(partition)
        return stat.f_blocks * stat.f_frsize

def usedSpace(partition):
    if os.name == 'nt':
        import win32api
        try:
            details = win32api.GetDiskFreeSpace(partition)
            # Free bytes = (Sectors Per Cluster)*(Bytes Per Sector)*(Number of Total Clusters - Number of Free Clusters)
            return details[0] * details[1] * (details[3] - details[2])
        except:
            return 0
    else:
        stat = os.statvfs(partition)
        return (stat.f_blocks - stat.f_bfree) * stat.f_frsize

def fileSize(file):
    if os.path.exists(file):
        return os.path.getsize(file)
    else:
        return 0

def directorySize(start, follow_links=0, start_depth=0, max_depth=0, skip_errs=0):

    # Get a list of all names of files and subdirectories in directory start
    try:
        dir_list = os.listdir(start)
    except:
        # If start is a directory, we probably have permission problems
        if os.path.isdir(start):
            raise DirSizeError('Cannot list directory %s'%start)
        else:  # otherwise, just re-raise the error so that it propagates
            raise

    total = 0
    for item in dir_list:
        # Get statistics on each item--file and subdirectory--of start
        path = os.path.join(start, item)
        try:
            stats = os.stat(path)
        except:
            if not skip_errs:
                raise DirSizeError('Cannot stat %s'%path)
        # The size in bytes is in the seventh item of the stats tuple, so:
        total += stats[6]
        # recursive descent if warranted
        if os.path.isdir(path) and (follow_links or not os.path.islink(path)):
            bytes = directorySize(path, follow_links, start_depth+1, max_depth)
            total += bytes
#            if max_depth and (start_depth < max_depth):
#                print_path(path, bytes)
    return total

def willFileFitOnPartition(file, partition):
    willFit = False

    #is there enough partition free space to copy the tarball
    if availableSpace(partition) > fileSize(file):
        willFit = True
    else:
        willFit = False
    return willFit
