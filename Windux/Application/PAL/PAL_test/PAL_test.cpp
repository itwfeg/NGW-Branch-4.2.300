//
// File : PAL_test.cpp
//
// This is a test frame for the PAL layer
//

#include "PAL_platformheader.h"

#include "PAL_test.h"
#include "PAL_test_Matrix.h"
#include "PAL_test_WorkerThread.h"

using namespace std;

// global variables used for active thread tracking
unsigned int gThreadsIn = 0;
unsigned int gThreadsOut = 0;
unsigned int gThreadsFailures = 0;

// global variables used for thread creation
HANDLE *gThreadHndls;
ThreadTracker *gThreadTrackPtr;

// display the attributes of the test about to run
void displayTest(objStruct *obj, void *handle)
{
    printf("Testing %s %ls %p",
            obj->name,
            obj->objName, 
            (void *)handle);
    if (obj->bReleaseObj != TRUE)
        printf(" w/o release");
    if (obj->semValues.initialSet)
        printf(" set to %d", obj->semValues.initialSet);
    switch(obj->waitTime.type)
    {
        case 0:
            printf(" not waiting\n");
            break;
        case FOREVER:
            printf(" waiting forever\n");
            break;
        default:
            printf(" waiting for %dms\n", obj->waitTime.time);
            break;
    }
}

// return back a time based on the type needed
int processTime(int count, timeStruct *timePtr)
{
    int retVal = 0;

    switch(timePtr->type)
    {
        case NO_DELAY:
            retVal = 0;
            break; 
        case RANDOM:
            retVal = rand() % 200000;
            break;
        case RANDOM_MIN:
            retVal = timePtr->time + (rand() % 200000);
            break;
        case RANDOM_MAX:
            retVal = rand() % timePtr->time;
            break;
        case RANDOM_INC:
            retVal = (count*timePtr->time) + (rand() % 200000);
            break;
        case INCREASING:
            retVal = 1+count*timePtr->time;
            break;
        case VALUE:
            retVal = timePtr->time;
        case FOREVER:
            retVal = INFINITE;
            break;
    }
    return retVal;
}

// set the threads up for the next test
void resetThreadProperties(HANDLE hndl, objStruct *obj, int threadCnt, int iterationsToPerform)
{
    int i;
    ThreadTracker *threadTracks = gThreadTrackPtr;

    gThreadsIn = 0;
    gThreadsOut = 0;
    gThreadsFailures = 0;

    //set up default values plus obj values
    for(i=0;i<(threadCnt);i++)
    {
        threadTracks[i].iterations = iterationsToPerform;
        threadTracks[i].threadTotal = threadCnt;
        threadTracks[i].threadNumber = i;
        threadTracks[i].startTime = processTime(i, &obj->startTime);
        threadTracks[i].workTime = processTime(i, &obj->workTime);
        threadTracks[i].waitTime = processTime(i, &obj->waitTime);
        threadTracks[i].threadIngress = &gThreadsIn;
        threadTracks[i].threadEgress = &gThreadsOut;
        threadTracks[i].threadFailures = &gThreadsFailures;
        threadTracks[i].valueToModify = obj->intValues.ptr1;
        threadTracks[i].valueToModify2 = obj->intValues.ptr2;
        threadTracks[i].forMutex = obj->type==MUTEX?TRUE:FALSE;
        threadTracks[i].waitAtStart = obj->waitAtStart;
        threadTracks[i].objHandle = hndl;
        gThreadHndls[i] = PAL_absclass::PAL_createThread(
                NULL,
                0,
                obj->func,
                (LPVOID)(gThreadTrackPtr+i),
                0,
                NULL);
    }
}

// wait for the threads to finish up
void joinThreads(int threadCnt)
{
    printf("gThreadHndls:%p\n", gThreadHndls);
    PAL_absclass::PAL_waitForMultipleObjs(
            threadCnt,
            gThreadHndls,
            TRUE,
            INFINITE);
    for (int i=0;i<threadCnt;i++)
        PAL_absclass::PAL_closeHandle(gThreadHndls[i]);
}

// display the result of the teset
void displayTestResult()
{
    printf("%d failure%s\n", 
            gThreadsFailures,
            gThreadsFailures!=1?"s":"");
}

// global function to kick off a test
void StartThreads(HANDLE hndl, objStruct *obj, int threadCnt, int iterationsToPerform)
{
    displayTest(obj, (void *)hndl);
    resetThreadProperties(hndl, obj, threadCnt, iterationsToPerform);
}

// global function to end a test
void StopThreads(int threadCnt)
{
    joinThreads(threadCnt);
    displayTestResult();
}

// main entry point for C/C++ programs
int main(int argc, char **argv)
{
    long testValue = 99;
    struct timeval tv;
    unsigned int threadsToUse = THREADS;
    unsigned int iterationsToPerform = ITERATIONS;

#if defined(__linux__)
    // seed the random number generator
    // this should be moved to a PAL_ function
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);
#endif	// __linux__

    printf("PAL Test Program: ");
    PAL_absclass::PAL_logging = PAL_VERBOSE | PAL_INFO_LOG|PAL_WARN_LOG|PAL_ERR_LOG;

#if defined(__linux__)
    // I'm sure there's a way to do this in WINAPI but I don't want
    // to spend the time researching this now
    while(1)
    {
        int c = getopt(argc, argv, "v:i:t:");
        if (c == -1)
            break;
        switch(c)
        {
            case 't':
                threadsToUse = atoi(optarg);
                if (threadsToUse < 1)
                {
                    printf("Thread count must be greater than zero\n");
                    threadsToUse = 1;
                }
                else if (threadsToUse > PAL_SEM_MAX_VAL)
                {
                    printf("Thread count must be less than %d\n", PAL_SEM_MAX_VAL);
                    threadsToUse = PAL_SEM_MAX_VAL;
                }
                else
                    threadsToUse = atoi(optarg);
                break;
            case 'i':
                iterationsToPerform = atoi(optarg);
                if (iterationsToPerform < 1)
                {
                    printf("Iteration count must be greater than zero\n");
                    iterationsToPerform = 1;
                }
                else
                    iterationsToPerform = atoi(optarg);
                break;
            case 'v':
                testValue = atoi(optarg);
                break;
            case '?':
                printf("options:\n");
                printf("    --interlocked : enable interlocked incremnet, decrement and exchange tests\n");
                printf("    --mutex : enable mutex tests\n");
                printf("    --semaphore : enable semaphore tests\n");
                printf("    --event : enable event tests\n");
                printf("    -t <NUM> : use <NUM> threads for testing\n");
                printf("    -i <NUM> : use <NUM> iterations for testing\n");
                printf("    -v <VAL> : use <VAL> as the starting value for interlocked tests\n");
            default:
                abort();
        }
    }
#endif  // __linux__

#ifdef DO_FTP_TEST
    string pathAndFile( "ftp://ftp.godlewskis.com/pub/" );
    PAL_INET_HANDLE hndl = NULL;
    PAL_absclass::PAL_internetOpen( "", 0, "", "", 0);
    hndl = PAL_absclass::PAL_internetConnect(
            hndl,
            pathAndFile.c_str(),
            21,
            "hobarttest",
            "h0bart",
            0,
            0,
            0);
    vector<PAL_absclass::ftpFileInfo> files;

    files = PAL_absclass::PAL_ftpFindFilesByExtension(hndl, "");

    for (unsigned int n=0;n<files.size();n++)
    {
        string s = pathAndFile+files[n].name;
        cout << s << endl;

        PAL_absclass::PAL_ftpGetFile(
                hndl,
                s.c_str(),
                files[n].name.c_str(),
                false,
                0,
                0,
                NULL);
    }
    files = PAL_absclass::PAL_ftpFindFilesByExtension(hndl, ".zip");
    
    for (unsigned int n=0;n<files.size();n++)
    {
        string s = pathAndFile+files[n].name;
        cout << s << endl;

        PAL_absclass::PAL_ftpGetFile(
                hndl,
                s.c_str(),
                files[n].name.c_str(),
                false,
                0,
                0,
                NULL);
    }

    PAL_absclass::PAL_internetCloseHandle(hndl);

    exit(0);
#endif      // DO_FTP_TEST
#ifdef DO_FILE_TEST
//    PAL_absclass::PAL_FILE_PARAM fParms;
//    fParms.linux_pathname = _T("/etc/issue");
//    fParms.linux_flags = O_RDONLY;
//    HANDLE file = PAL_absclass::PAL_openFile(&fParms);
//    if ( INVALID_HANDLE_VALUE != file)
//    {
//        DWORD size = PAL_absclass::PAL_getFileSize(file, NULL);
//        printf("filesize is %u\n", size);
//        PAL_absclass::PAL_setFilePointer(file, 0, NULL, PAL_FILE_BEGIN);
//    }

    struct stat fstat;
    if (stat("/etc/issue", &fstat) == 0)
    {
        printf("filesize is %u\n", fstat.st_size);
    }

    exit(0);
#endif      // DO_FILE_TEST
#ifdef DO_TIME_TEST
    PAL_TIME_STRUCT myTime;
    PAL_absclass::PAL_getSystemTime(&myTime);
    printf("System time:\n");
    printf(" Year = %d\n", myTime.wYear);
    printf(" Month = %d\n", myTime.wMonth);
    printf(" Day of the week = %d\n", myTime.wDayOfWeek);
    printf(" Day = %d\n", myTime.wDay);
    printf(" Hour = %d\n", myTime.wHour);
    printf(" Minute = %d\n", myTime.wMinute);
    printf(" Second = %d\n", myTime.wSecond);
    printf(" Millisecond = %d\n", myTime.wMilliseconds);
    PAL_absclass::PAL_getLocalTime(&myTime);
    printf("Local time:\n");
    printf(" Year = %d\n", myTime.wYear);
    printf(" Month = %d\n", myTime.wMonth);
    printf(" Day of the week = %d\n", myTime.wDayOfWeek);
    printf(" Day = %d\n", myTime.wDay);
    printf(" Hour = %d\n", myTime.wHour);
    printf(" Minute = %d\n", myTime.wMinute);
    printf(" Second = %d\n", myTime.wSecond);
    printf(" Millisecond = %d\n", myTime.wMilliseconds);
    exit(0);
#endif      // DO_TIME_TEST
#ifdef DO_FILE_TEST
    PAL_absclass::PAL_createFile(
            _T("test.txt"),
            PAL_FILE_GEN_RD|PAL_FILE_GEN_WR,
            0,
            NULL,
            PAL_FILE_CREATE,
            0,
            NULL);
    PAL_absclass::PAL_deleteFile( _T("test.txt"));
    DWORD bufsize = PAL_absclass::PAL_getCurrentDirectory(0, NULL);
    wchar_t *buffer =  (wchar_t *)calloc(bufsize,sizeof(wchar_t));
    PAL_absclass::PAL_getCurrentDirectory(bufsize, buffer);
    printf("%ls\n", buffer);
#endif      // DO_FILE
    WORD wVersionRequested;                                                
    WSADATA wsaData;                                                       
    int err;                                                               

    wVersionRequested = MAKEWORD( 2, 0 );                                  

    err = PAL_absclass::PAL_wsaStartup( wVersionRequested, &wsaData );     

    PAL_absclass::PAL_getAdaptersInfo(NULL, NULL);

    printf("Using %d thread%c, ", threadsToUse, threadsToUse>1?'s':'\0');
    printf("Using %d iteration%c, ", iterationsToPerform, iterationsToPerform>1?'s':'\0');
    printf("Using %ld as interlocked start value\n", testValue);

    gThreadHndls = (HANDLE *)malloc(threadsToUse * sizeof(HANDLE));
    gThreadTrackPtr = (ThreadTracker*)malloc(threadsToUse * sizeof(ThreadTracker));

    // perform the tests
    testMatrix(threadsToUse, iterationsToPerform, testValue);

    free(gThreadTrackPtr);
    free(gThreadHndls);

    return 0;
}

