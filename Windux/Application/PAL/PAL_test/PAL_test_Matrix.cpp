//
// File : PAL_test_Matrix.cpp
//
// This is the source for the actual tests
//

#include "PAL_platformheader.h"

#include "PAL_test.h"
#include "PAL_test_WorkerThread.h"

int testMatrix(unsigned int threadCnt, unsigned int iterationsToPerform, long testValue)
{
    HANDLE hndl = NULL;
    long testValue2 ,rndValue;
    long passedValue = testValue;

    // set up a random second value to pass 
    // to the atomic math functions
    testValue2 = rndValue = rand()%100;

    // set up some default structures so the test 
    // structure is a little easier to navigate
    semStruct notSem = { 0,0 };
    intStruct notInc = { NULL, NULL };
    timeStruct no_delay = { NO_DELAY, 0 };
    timeStruct forever = { FOREVER, 0 };
    timeStruct rndMax10000 = { RANDOM_MAX, 10000 };

    PAL_CRITICAL_SECTION cs;

    // these are the actual tests that run
    objStruct objs[] = 
    {
        {CS, "Critical Section", _T("\0"), notSem, no_delay, forever, no_delay, TRUE, TRUE, pal_csWorkerThread, notInc},

        /*
        {AUTO_EVENT, "Auto Event", _T("\0"), notSem, no_delay, forever, no_delay, TRUE, TRUE, pal_workerThread, notInc},
        {MANUAL_EVENT, "Manual Event", _T("\0"), notSem, no_delay, forever, no_delay, TRUE, TRUE, pal_workerThread, notInc},
        {PULSE_EVENT, "Pulse Event", _T("\0"), notSem, {INCREASING, 10000}, forever, no_delay, FALSE, TRUE, pal_workerThread, notInc},

        {MUTEX, "Unnamed mutex", _T("\0"), notSem, no_delay, forever, no_delay, FALSE, TRUE, pal_workerThread, notInc},
        {MUTEX, "Named mutex", _T("TestMutex"), notSem, no_delay, forever, no_delay, FALSE, TRUE, pal_workerThread, notInc},
        {MUTEX, "Named mutex", _T("/ToastMutex"), notSem, no_delay, forever, no_delay, FALSE, TRUE, pal_workerThread, notInc},

        {SEMAPHORE, "Unnamed semaphore", _T("\0"), {0, 1}, no_delay, forever, no_delay, TRUE, TRUE, pal_workerThread, notInc},
        {SEMAPHORE, "Unnamed semaphore", _T("\0"), {0, 1}, no_delay, {INCREASING, 10000}, no_delay, TRUE, TRUE, pal_workerThread, notInc},
        {SEMAPHORE, "Named semaphore", _T("TestSem"), {2, 2}, no_delay, no_delay, no_delay, TRUE, FALSE, pal_workerThread, notInc},
        {SEMAPHORE, "Named semaphore", _T("TestSem"), {2, 2}, no_delay, no_delay, no_delay, TRUE, TRUE, pal_workerThread, notInc},
        {SEMAPHORE, "Named semaphore", _T("/ToastSem"), {2, 2}, no_delay, forever, no_delay, TRUE, TRUE, pal_workerThread, notInc},

        {INCREMENT, "Atomic increment", _T("\0"), notSem, rndMax10000, no_delay, rndMax10000, FALSE, FALSE, pal_II, {&passedValue, &rndValue}},
        {DECREMENT, "Atomic decrement", _T("\0"), notSem, rndMax10000, no_delay, rndMax10000, FALSE, FALSE, pal_ID, {&passedValue, &rndValue}},
        {EXCHANGE, "Atomic exchange", _T("\0"), notSem, rndMax10000, no_delay, rndMax10000, FALSE, FALSE, pal_IE, {&passedValue, &rndValue}},
        */
    };

    // go through each of the entries in the test list and execute
    for(unsigned int j=0;j<_countof(objs);j++)
    {
        unsigned int semsLeft=0;
        passedValue = testValue;
        testValue2 = rndValue;

        // create an object
        switch(objs[j].type)
        {
            case AUTO_EVENT:
                hndl = PAL_absclass::PAL_createEvent( objs[j].objName, FALSE, FALSE, NULL);
                break;
            case PULSE_EVENT:
            case MANUAL_EVENT:
                hndl = PAL_absclass::PAL_createEvent( objs[j].objName, TRUE, FALSE, NULL);
                break;
            case MUTEX:
                hndl = PAL_absclass::PAL_createMutex(NULL, NULL, objs[j].objName);
                break;
            case SEMAPHORE:
                hndl = PAL_absclass::PAL_createSemaphore( 
                        objs[j].objName, 
                        objs[j].semValues.initialSet, 
                        threadCnt, 
                        NULL);
                break;
            case CS:
                PAL_absclass::PAL_initializeCriticalSection(&cs);
                hndl = cs;
                break;
            default:
                break;
        }

        StartThreads(hndl, &objs[j], threadCnt, iterationsToPerform);

        // main processing during thread run
        switch(objs[j].type)
        {
            case MANUAL_EVENT:
            case AUTO_EVENT:
                while (gThreadsOut != threadCnt)
                {
                    // we pause here to allow the threads to 
                    // reach a common place
                    PAL_absclass::PAL_sleep(15);
                    DBG_PRINT("calling setEvent() %d %d\n", gThreadsOut, threadCnt);
                    PAL_absclass::PAL_setEvent(hndl);
                    PAL_absclass::PAL_sleep(15);
                }
                break;
            case PULSE_EVENT:
                // ensure that at least one thread is waiting
                // to test pulse properly
                while (gThreadsIn==0)
                {
                    // sleep 1 ms seems quite a long time but I don't
                    // know how to yield the CPU under WIN32. We should
                    // implement a PAL_yield() function soon
                    PAL_absclass::PAL_sleep(1);
                    //sched_yield();
                }
                while (gThreadsOut != threadCnt)
                {
                    DBG_PRINT("calling pulseEvent()\n");
                    PAL_absclass::PAL_pulseEvent(hndl);
                    // we pause after the pulse so that we signal
                    // as many early threads as possible
                    PAL_absclass::PAL_sleep(15);
                }
                break;
            case CS:
                break;
            case MUTEX:
                break;
            case SEMAPHORE:
                // if the threads aren't going to wait on the semaphore
                // lets wait until all threads are complete so that the
                // test executes in a deterministic time frame
                if (objs[j].waitTime.type == NO_DELAY)
                {
                    while (gThreadsOut != threadCnt)
                        PAL_absclass::PAL_sleep(1);
                }
                else
                {
                    PAL_absclass::PAL_sleep(15);
                }

                if ((threadCnt > objs[j].semValues.initialSet) &&
                        (threadCnt > gThreadsFailures))
                {
                    semsLeft = threadCnt-objs[j].semValues.initialSet;

                    // don't wait for threads that have already returned an error
                    semsLeft -= gThreadsFailures;

                    while (semsLeft)
                    {
                        // USE_MAX_THREADS denotes signalling setting the semaphore to threadCnt
                        if (objs[j].semValues.semInc == USE_MAX_THREADS)
                            objs[j].semValues.semInc = threadCnt;

                        // do not signal more than needed
                        objs[j].semValues.semInc = _min(objs[j].semValues.semInc, semsLeft);

                        DBG_PRINT("incSemaphore by %d\n", objs[j].semValues.semInc);

                        PAL_absclass::PAL_incSemaphore(hndl, objs[j].semValues.semInc, NULL);

                        // wait for some work to simulate 
                        PAL_absclass::PAL_sleep(15);

                        semsLeft = semsLeft - objs[j].semValues.semInc;
                    }
                }
                break;
            default:
                break;
        }

        StopThreads(threadCnt);

        // post processing
        switch(objs[j].type)
        {
            case INCREMENT:
            case DECREMENT:
                printf("Starting value %ld\n", testValue);
                printf("Final value    %ld\n", passedValue);
                break;
            case EXCHANGE:
                printf("Starting value 1 %ld Final value 1  %ld\n", testValue , passedValue);
                printf("Starting value 2 %ld Final value 2  %ld\n", testValue2 , rndValue);
                break;
            default:
                break;
        }
        // close out the handle object
        if (objs[j].bReleaseObj)
        {
            PAL_absclass::PAL_closeHandle(hndl);
        }
    }

    return 0;
}
