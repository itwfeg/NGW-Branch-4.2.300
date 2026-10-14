/*++

Copyright (c) 2003-2007  Microsoft Corporation

Module Name:

    fbwfapi.h

Abstract:

    This is the exported header file that defines the prototype of the FBWF APIs
    and the structures used in these APIs

Environment:

    User mode

Revision History:

--*/

#ifndef __FBWF_API_H__
#define __FBWF_API_H__

#define FBWF_MIN_CACHE_THRESHOLD        16          /* 16 MB */
#define FBWF_DEFAULT_CACHE_THRESHOLD    64          /* 64 MB */
#define FBWF_MAX_CACHE_THRESHOLD        1024        /* 1024 MB */

typedef struct _FbwfCacheDetail {
    ULONG  cacheSize;               // size of cache used by the file
    ULONG  openHandleCount;         // number of currently opened handles
    ULONG  fileNameLength;          // file name length in bytes
    WCHAR  fileName[1];             // file name (may not be null-terminated)
} FbwfCacheDetail, *PFbwfCacheDetail;

typedef struct _FbwfMemoryUsage {
    ULONG   currCacheThreshold;     // current session cache threshold
    ULONG   nextCacheThreshold;     // next session cache threshold
    ULONG   dirStructure;           // memory used to store directory structure
    ULONG   fileData;               // memory used to cache file data
} FbwfMemoryUsage, *PFbwfMemoryUsage;

typedef struct _FbwfFSInfo {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FbwfFSInfo, *PFbwfFSInfo;

#ifdef __cplusplus
extern "C" {
#endif

ULONG WINAPI FbwfEnableFilter(void);
ULONG WINAPI FbwfDisableFilter(void);
ULONG WINAPI FbwfProtectVolume(__in PWCHAR volume);
ULONG WINAPI FbwfUnprotectVolume(__in PWCHAR volume, ULONG removeExclusionList);
ULONG WINAPI FbwfSetCacheThreshold(ULONG threshold);
ULONG WINAPI FbwfGetVolumeList(ULONG currSession, 
                               PVOID volumeList, 
                               __inout_ecount_opt(1) PULONG size);
ULONG WINAPI FbwfIsFilterEnabled(__out_ecount(1) PULONG currentSession, 
                                 __out_ecount(1) PULONG nextSession);
ULONG WINAPI FbwfIsVolumeProtected(__in PWCHAR volume, 
                                   __out_ecount(1) PULONG currentSession, 
                                   __out_ecount(1) PULONG nextSession);
ULONG WINAPI FbwfGetMemoryUsage(PFbwfMemoryUsage usage);
ULONG WINAPI FbwfFindClose(void);
ULONG WINAPI FbwfFindNext(PFbwfCacheDetail cacheDetail, __inout_ecount(1) PULONG size);
ULONG WINAPI FbwfFindFirst(__in PWCHAR volume, 
                           PFbwfCacheDetail cacheDetail, 
                           __inout_ecount(1) PULONG size);
ULONG WINAPI FbwfEnableCompression(void);
ULONG WINAPI FbwfDisableCompression(void);
ULONG WINAPI FbwfIsCompressionEnabled(__out_ecount(1) PULONG currentSession, 
                                      __out_ecount(1) PULONG nextSession);
ULONG WINAPI FbwfEnableCachePreAllocation(void);
ULONG WINAPI FbwfDisableCachePreAllocation(void);
ULONG WINAPI FbwfIsCachePreAllocationEnabled(__out_ecount(1) PULONG currentSession, 
                                             __out_ecount(1) PULONG nextSession);
ULONG WINAPI FbwfAddExclusion(__in PWCHAR volume, __in PWCHAR path);
ULONG WINAPI FbwfRemoveExclusion(__in PWCHAR volume, __in PWCHAR path);
ULONG WINAPI FbwfGetExclusionList(__in PWCHAR volume, 
                                  ULONG currSession, 
                                  PVOID exclusionList, 
                                  __inout_ecount_opt(1) PULONG size);
ULONG WINAPI FbwfCommitFile(__in PWCHAR volume, __in PWCHAR path);
ULONG WINAPI FbwfRestoreFile(__in PWCHAR volume, __in PWCHAR path);
ULONG WINAPI FbwfCacheThresholdNotification(ULONG threshold, HANDLE event);
ULONG WINAPI FbwfGetSizeDisplay(__out_ecount(1) PULONG currentSession, 
                                             __out_ecount(1) PULONG nextSession);
ULONG WINAPI FbwfSetSizeDisplay( ULONG displayMode);
ULONG WINAPI FbwfGetActualSize( __in PWCHAR volume, PFbwfFSInfo fsInfo);
ULONG WINAPI FbwfGetVirtualSize( __in PWCHAR volume, PFbwfFSInfo fsInfo);


#ifdef __cplusplus
}
#endif

#endif
