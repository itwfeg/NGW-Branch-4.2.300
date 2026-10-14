//
// File : PAL_test.h
//
// This is the main header file for the PAL layer test
//

#ifndef __PAL_TEST_H
#define __PAL_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

// Project wide configurations

// Project wide defines
#define THREADS 1
#define ITERATIONS 5
#ifndef NAME_MAX
#define NAME_MAX 256
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif
#define _max(x,y) ((x) > (y) ? (x) : (y))
#define _min(x,y) ((x) < (y) ? (x) : (y))

#define USE_MAX_THREADS ((unsigned int)-1)

// to see the test threads in action define the following
#define DEBUG

#ifdef DEBUG
#if defined(__linux__)
#define DBG_PRINT(fmt, args...) printf(fmt,##args)
#else
#define DBG_PRINT(fmt,...) printf(fmt,__VA_ARGS__)
#endif
#else
#if defined(__linux__)
#define DBG_PRINT(fmt, args...)
#else
#define DBG_PRINT(fmt,...)
#endif
#endif

// an enumeration of the different tests we can currently run
typedef enum
{
    CS,
    INCREMENT,
    DECREMENT,
    EXCHANGE,
    MUTEX,
    AUTO_EVENT,
    MANUAL_EVENT,
    PULSE_EVENT,
    SEMAPHORE,
    SKIP
} eTest;

// an enumeration of the different time values we can calculate
typedef enum
{
    NO_DELAY,       // Immediate
    RANDOM,         // a value between 0 and 200000
    RANDOM_INC,     // an increasing value added to a random value
    RANDOM_MIN,     // a random value not less than the given value
    RANDOM_MAX,     // a random value not more than the given value
    INCREASING,     // an increasing value (starts at 1) 
    VALUE,          // set to the given value
    FOREVER         // without end
} eTime;

// Project wide typedefs/structs

// this is the structure passed to the created threads
typedef struct
{
    unsigned int iterations;        // how many iterations to run (atomic math functions)
    unsigned int threadTotal;       // how many total threads are going to be created
    unsigned int threadNumber;      // this thread's unique number
    unsigned int startTime;         // how long to wait to start
    unsigned int workTime;          // how long to simulate working
    unsigned int waitTime;          // how long to wait on the object (non atomic math functions)
    unsigned int *threadIngress;    // a pointer to count threads entered
    unsigned int *threadEgress;     // a pointer to count threads exited
    unsigned int *threadFailures;   // a pointer to count thread failures
    long *valueToModify;            // a pointer to a value to modify (atomic math functions)
    long *valueToModify2;           // another pointer to a value to modify
    BOOL forMutex;                  // is this thread for testing Mutex's?
    BOOL waitAtStart;               // wait for all threads to initialize before starting?
    HANDLE objHandle;               // a handle to the object to wait on (non atomic math functions)
} ThreadTracker;

// this is a structure used to define an initial set of values for a
// semaphore
typedef struct
{
    unsigned int initialSet;        // the initial value of the semaphore
    unsigned int semInc;            // the value to increment the semaphore each post
} semStruct;

// a structure to hold to pointers to longs (for atomic math functions)
typedef struct
{
    long *ptr1;
    long *ptr2;
} intStruct;

// a structure to be used to generate a time value
typedef struct
{
    eTime type;
    unsigned int time;
} timeStruct;

// the actual test struture
typedef struct
{
    eTest type;
    char name[NAME_MAX];            // the name of the test being run
    CONST TCHAR objName[NAME_MAX];  // the object name to create (can be \0)
    semStruct semValues;
    timeStruct startTime;
    timeStruct waitTime;            // object wait time
    timeStruct workTime;            // work thread delay after event wait
    BOOL waitAtStart;               // wait on all thread creation before continuing
    BOOL bReleaseObj;               // clean object up after finished (used to create error conditions)
    PAL_THREAD_START func;          // a pointer to thread function
    intStruct intValues;
} objStruct;

// Project wide functions
void StartThreads(HANDLE hndl, objStruct *obj, int threadCnt, int iterationsToPerform);
void StopThreads(int threadCnt);

// Project wide variables
extern unsigned int gThreadsIn;
extern unsigned int gThreadsOut;
extern unsigned int gThreadsFailures;

#ifdef __cplusplus
}
#endif
#endif  // __PAL_TEST_H
