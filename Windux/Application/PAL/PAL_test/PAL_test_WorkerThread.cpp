//
// File : PAL_test_WorkerThread.cpp
//
// the source of the test thread code
//
#include "PAL_platformheader.h"

#include "PAL_test.h"

// the common parts of all test threads
// has a first half and second half
// first half runs before the wait or math
// second half runs after the wait or math
void pal_Main(ThreadTracker *th, DWORD retVal, BOOL bFirstHalf)
{
    if (bFirstHalf)
    {
        // if we need an initial delay
        if (th->startTime)
            //usleep(th->startTime);
            PAL_absclass::PAL_sleep(th->startTime/1000);

        // increase the ready thread count
        if (th->threadIngress)
            *th->threadIngress += 1;

        DBG_PRINT("%d Thread %d about to wait\n", getpid(),th->threadNumber);

        // allow all threads to start for mutex's 
        if (th->waitAtStart)
        {
            // make sure we can count number of threads
            if (th->threadIngress)
            {
                while (*th->threadIngress != th->threadTotal)
                {
                    //sched_yield();
                    PAL_absclass::PAL_sleep(1);
                }
            }
            else
            {
                DBG_PRINT("Cannot wait at start unless Ingress counter is given\n");
            }
        }
    }
    else
    {
        if (retVal == WAIT_OBJECT_0)
        {
            DBG_PRINT(" Thread %d got it\n", th->threadNumber);

            // simulate doing some work
            if (th->workTime)
                //usleep(th->workTime);
                PAL_absclass::PAL_sleep(th->workTime/1000);

            if (th->forMutex)
                // release the mutex to allow another thread to do work
                PAL_absclass::PAL_releaseMutex(th->objHandle);
        }
        else if (retVal == WAIT_TIMEOUT)
        {
            DBG_PRINT(" Thread %d timed out\n", th->threadNumber);
            gThreadsFailures++;
        }
        else
        {
            // we failed to get the object
            DBG_PRINT(" Thread %d retVal == %d\n", th->threadNumber, retVal );
            gThreadsFailures++;
        }

        // increase the ready exit count
        if (th->threadEgress)
            *th->threadEgress += 1;

        DBG_PRINT(" Thread %d exit\n", th->threadNumber);
    }
}

// thread to do Interlocked Exchange of two values
DWORD PAL_API pal_IE(void *inArg)
{
    ThreadTracker *thisThread = (ThreadTracker *)inArg;

    pal_Main(thisThread, 0, TRUE);
    for (unsigned int i=0;i<thisThread->iterations;i++)
        *thisThread->valueToModify2=PAL_absclass::PAL_InterlockedExchange(thisThread->valueToModify, *thisThread->valueToModify2);
    pal_Main(thisThread, WAIT_OBJECT_0, FALSE);
    return(NULL);
}

// thread to do Interlocked Decrement of a value
DWORD PAL_API pal_ID(void *inArg)
{
    ThreadTracker *thisThread = (ThreadTracker *)inArg;

    pal_Main(thisThread, 0, TRUE);
    for (unsigned int i=0;i<thisThread->iterations;i++)
        PAL_absclass::PAL_InterlockedDecrement(thisThread->valueToModify);
    pal_Main(thisThread, WAIT_OBJECT_0, FALSE);
    return(NULL);
}

// thread to do Interlocked Increment of a value
DWORD PAL_API pal_II(void *inArg)
{
    ThreadTracker *thisThread = (ThreadTracker *)inArg;

    pal_Main(thisThread, 0, TRUE);
    for (unsigned int i=0;i<thisThread->iterations;i++)
        PAL_absclass::PAL_InterlockedIncrement(thisThread->valueToModify);
    pal_Main(thisThread, WAIT_OBJECT_0, FALSE);
    return(NULL);
}

// thread to wait on a single object
DWORD PAL_API pal_workerThread(void *inArg)
{
    DWORD retVal;
    ThreadTracker *thisThread = (ThreadTracker *)inArg;

    pal_Main(thisThread, 0, TRUE);
    retVal = PAL_absclass::PAL_waitForSingleObj(thisThread->objHandle, thisThread->waitTime);
    pal_Main(thisThread, retVal, FALSE);

    return(NULL);
}


// thread to wait on a single object
DWORD PAL_API pal_csWorkerThread(void *inArg)
{
    DWORD retVal = WAIT_OBJECT_0;
    ThreadTracker *thisThread = (ThreadTracker *)inArg;

    pal_Main(thisThread, 0, TRUE);
    DBG_PRINT(" %d Getting 1st CS\n", thisThread->threadNumber);
    PAL_absclass::PAL_enterCriticalSection(thisThread->objHandle);
    PAL_absclass::PAL_sleep(10);
    DBG_PRINT(" %d Getting 2nd CS\n", thisThread->threadNumber);
    PAL_absclass::PAL_enterCriticalSection(thisThread->objHandle);
    PAL_absclass::PAL_sleep(10);
    PAL_absclass::PAL_leaveCriticalSection(thisThread->objHandle);
    PAL_absclass::PAL_sleep(10);
    PAL_absclass::PAL_leaveCriticalSection(thisThread->objHandle);
    pal_Main(thisThread, retVal, FALSE);
    return(NULL);
}
