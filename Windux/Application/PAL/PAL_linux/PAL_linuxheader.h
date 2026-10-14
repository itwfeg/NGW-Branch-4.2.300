//
// File : PAL_linuxheader.h
//
// This file, included by PAL_platformheader.h, includes Linux platform depended header files.
//
/// @file PAL_linuxheader.h
/// This file, included by PAL_platformheader.h, includes Linux platform depended header files.

#ifndef _LINUX_HEADER_H_
#define _LINUX_HEADER_H_

/// Right now used to remove tchar.h. Eventually tchar.h should to be moved to PAL_win32.h instead
#define REM_LINUX_ERR

/// Used for using posix threading model
#define LINUX_OS

#define UNICODE

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <linux/rtnetlink.h>
#include <linux/unistd.h>
#include <locale.h>
#include <net/if.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <time.h>
#include <utime.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <curl/curl.h>
#include <curl/easy.h>
//#include <curl/types.h> does not appear to be needed with libcurl4-openssl-dev

#ifndef NAME_MAX
#define NAME_MAX    FILENAME_MAX
#endif

#ifndef _MAX_PATH
#define _MAX_PATH PATH_MAX
#endif

#endif      //_LINUX_HEADER_H_

