//
// File : PAL_test_WorkerThread.h
//
// This is the header file for the PAL_test_WorkerThread source
//

#ifndef _PAL_TEST_WORKERTHREAD_H
#define _PAL_TEST_WORKERTHREAD_H

DWORD PAL_API pal_IE(LPVOID inArg);
DWORD PAL_API pal_ID(LPVOID inArg);
DWORD PAL_API pal_II(LPVOID inArg);
DWORD PAL_API pal_workerThread(LPVOID inArg);
DWORD PAL_API pal_csWorkerThread(LPVOID inArg);

#endif  // _PAL_TEST_WORKERTHREAD_H
