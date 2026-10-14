//
// File : PAL_absclass.cpp
//
// Implemetns the linux handle abstraction.  Handle allows PAL to cleanly manage
// pointers of various type without the application suite having to worry about
// creation and cleanup.
//
#include"PAL_absclass.h"


///////////////////////////////////////////////////////
//Destructor:
//destroy the handle and based on the type of handle
//call linux specific API for clean up.  Doing it here
//allows us to cleanup as we go out of scope.
//
PAL_PtrMan::~PAL_PtrMan()
{
    if (NULL != hndl)
    {
        switch(ptr_type)
        {
            case(MUTEX_TYPE):
                pthread_mutex_destroy((PAL_PMUTEX) hndl);
                delete((PAL_PMUTEX) hndl);
                hndl = NULL;
                break;
            case(SEM_TYPE):
                sem_close((PAL_PSEM) hndl);
                delete((PAL_PSEM) hndl);
                hndl = NULL;
                break;
            case(UNASSIGNED):
            case(EVENT_TYPE):
                break;
        }
    }
}
