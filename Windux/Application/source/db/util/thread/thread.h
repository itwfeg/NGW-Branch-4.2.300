/*
 * The thread class is meant to be subclassed in order to be used to run code in
 * a thread of control "separate" from the thread of control used to create the
 * thread object. The code that the "new" thread runs belongs to the subclass in
 * its "doRun" method.
 * This approach, however, presents a critical problem if a "new" thread is still
 * running when the object of the subclass is deleted (either its destructor is
 * called or it comes out of scope): the object's destructor will be called before
 * the thread's destructor, which implies that the "new" thread is still executing
 * the subclass' code (doRun method) after its associated object has been "deleted"
 * (its destructor has been called, although its memory has not been freed).
 *
 * In order to avoid this potential problem, the following measures will be 
 * implemented:
 * - Thread's constructor is protected so this class can only be instanciated as
 * part of a subclass.
 * - Thread's destructor is private so that no object of any subclass can be
 * destroyed (the destructor cannot be implemented). A consequence of this is that
 * an object of the subclass can only be instanciated using the operator new (it
 * cannot be a data member of any other class or be instanciated in the stack).
 * - Thread's destructor is virtual so that when an object of type Thread is
 * destroyed, the whole object (from the superclass' level) is destroyed.
 * - Thread has a public, non-const method 'end' that can be called at the 
 * superclass' level which ends the thread of control and destroys the object
 * (in that order to prevent the problem described above).
 */



#ifndef THREAD_H
#define THREAD_H



#include <string>


//#ifdef VXWORKS
//#include "vxworkstasks.h"
//#else
//#include "win32threads.h"
//#endif
#if defined( WIN32 )
    #include    "win32threads.h"

#elif   defined( __linux__ )
    #include    "linuxthreads.h"

#elif   defined( SOLARIS )
    #error "Not support yet"

#elif   defined( VXWORKS )
    #include    "vxworkstasks.h"

#else
    #error "Unknown system"

#endif



#ifndef NO_NAMESPACES
namespace util_thread
{
#endif



//#ifdef VXWORKS
//template <class ThreadingModel = VxWorksTasks>
//#else
//template <class ThreadingModel = Win32Threads>
//#endif
#ifdef WIN32_OS
template <class ThreadingModel = Win32Threads>
#endif
#ifdef VXWORKS
template <class ThreadingModel = VxWorksTasks>
#endif
#ifdef  __linux__
template <class ThreadingModel = LinuxThreads>
#endif
class Thread
{
	public:
		// --- Types and subtypes ---
		typedef typename ThreadingModel::ThreadIDType ThreadIDType;
		typedef typename ThreadingModel::ThreadHandle ThreadHandle;
		typedef typename ThreadingModel::ThreadExitCodeType ThreadExitCodeType;



		// --- Getters and setters ---
		int getPriority() const;
		void setPriority(int p);
		ThreadIDType getThreadID() const { return id_; }
		ThreadExitCodeType getExitCode() const { return exitCode_; }
		bool isRunning() const;

		static ThreadIDType getCurrentThreadID() { return ThreadingModel::currentThreadID(); }
		static int getCurrentThreadPriority() { return ThreadingModel::currentThreadPriority(); }
		static int setCurrentThreadPriority(int p) { return ThreadingModel::currentThreadPriority(p); }
		static int incrementCurrentThreadPriority(int p = 1)
				{ return ThreadingModel::incrementCurrentThreadPriority(p); }
		static int decrementCurrentThreadPriority(int p = 1)
				{ return ThreadingModel::decrementCurrentThreadPriority(p); }



		// --- Processing methods ---
		void start();
		bool sleep(int milliseconds);
		void suspend();
		void resume();
		void stop();
		void endThread();          // This method is equivalent to calling the delete operator


		// --- Static Processing methods ---
		static bool sleepCurrentThread(int milliseconds);

		static void startThreadSwitchTracking() { ThreadingModel::registerSwitchTaskHook(); }
		static void reportThreadSwitch()
				{ ThreadingModel::unregisterSwitchTaskHook(); ThreadingModel::reportSwitchTaskInfo();}


	protected:
		// The constructor is protected so that this class has to be instantiated
#ifdef VXWORKS
		Thread(const std::string& name, int priority = _DefaultPriority_, size_t stackSize = _DefaultStackSize_);
#else
		Thread(const std::string& name);
#endif
		// The destructor is private so that no subclass' destructor can be implemented or called
		virtual ~Thread();


		// The 'run' and 'doRun' methods constitute a Template Method Pattern (GoF, p.325)
		virtual int doRun() { return 0; };


	private:
		// --- Data members ---
		std::string name_;
		ThreadIDType id_;
		bool started_;
		bool finished_;
		ThreadExitCodeType exitCode_;
#ifdef WIN32_OS
		ThreadHandle handle_;
#endif
#ifdef VXWORKS
		WIND_TCB tcb_;
		char* stack_;
#endif


//#ifdef VXWORKS
#if defined(VXWORKS)
		static int run(Thread<ThreadingModel>* t, ...);
#elif   defined( __linux__ )
		static void *run(void *void_p);
#else
		static DWORD WINAPI run(LPVOID p);
#endif


#ifdef VXWORKS
		static const int _DefaultPriority_ = 100;
		static const size_t _DefaultStackSize_ = 48*1024;
#endif

#if defined( __linux__ )
        // Linux has no function for Suspend/Resume.
        pthread_mutex_t mutex_suspend_resume;
        pthread_cond_t  cond_suspend_resume;
        bool            suspend_flag;
#endif
};



#ifndef NO_NAMESPACES
} // namespace util_thread
#endif



#include "../platform/platform.h"



#ifndef NO_NAMESPACES
namespace util_thread
{
#endif



template <class ThreadingModel>
#ifdef VXWORKS
Thread<ThreadingModel>::Thread(const std::string& name, int priority, size_t stackSize)
		: name_(name), id_(0), started_(false), finished_(false), stack_(new char[stackSize])
#else
/* (SJJC port 6/19/06) */
Thread<ThreadingModel>::Thread(const std::string& name) : name_(name), id_(0), started_(false), finished_(false)
#endif
{
#ifdef WIN32_OS
	handle_ = CreateThread(NULL, 0, &(Thread::run), this, CREATE_SUSPENDED, &id_);
#endif
#ifdef VXWORKS
	if ( taskInit(&tcb_, const_cast<char *> (name_.c_str()), priority, 0, stack_ + (stackSize - 1), stackSize,
			(FUNCPTR)&(Thread::run), (int)this, 0, 0, 0, 0, 0, 0, 0, 0, 0) == OK )
	{
		id_ = taskNameToId(const_cast<char *> (name_.c_str()));
	}
	else
		id_ = 0;
#endif
#if defined( __linux__ )

    suspend_flag = true;                // create as suspended
    pthread_mutex_init(&mutex_suspend_resume, NULL);
    pthread_cond_init(&cond_suspend_resume, NULL);

    if (    pthread_create(
                &id_,                   // Thrad ID returned
                NULL,                   // Thread attributes. (NULL: use default)
                (Thread::run),          // Thread function
                this                    // Argument to pass to thread function
            ) != 0)
    {
        id_ = 0;
    }
#endif
}


template <class ThreadingModel>
Thread<ThreadingModel>::~Thread()
{
#ifdef WIN32_OS
	if ( handle_ != NULL )
	{
		TerminateThread(handle_, 0);
	}
#endif

#ifdef VXWORKS
	if ( id_ )
	{
		if ( !finished_ )
			taskDelete(id_);
	}

	delete[] stack_;
#endif

#if defined( __linux__ )
    if ( id_ )
    {
        if (pthread_cancel( id_ ) != 0)
        {
            // Function call failed. Do something
        }
        if (pthread_join( id_, NULL ) != 0)
        {
            // Function call failed. Do something
        }
    }
#endif
}


#ifdef VXWORKS
template <class ThreadingModel>
void Thread<ThreadingModel>::setPriority(int p)
{
	taskPrioritySet(id_, p);
}


template <class ThreadingModel>
int Thread<ThreadingModel>::getPriority() const
{
	int p;

	taskPriorityGet(id_, &p);

	return p;
}
#endif


template <class ThreadingModel>
bool Thread<ThreadingModel>::isRunning() const
{
	if ( started_ && !finished_ )
	{
#ifdef WIN32_OS	  /* (SJJC port 6/19/06) */
		if ( GetExitCodeThread(handle_, &exitCode_) )
			return true;
		else
		{
			if ( GetLastError() == STILL_ACTIVE )
				return false;
		}
#endif

#ifdef VXWORKS
		if ( taskIdVerify(id_) == OK )
			return (taskIsReady(id_) == OK);
#endif

#if defined( __linux__ )
        // ***** WARNING: We don't have code in Linux to detect thread condition.
#endif
	}

	return false;
}


template <class ThreadingModel>
void Thread<ThreadingModel>::start()
{
	if ( !started_ )
	{
		started_ = true;

#ifdef WIN32_OS
		ResumeThread(handle_);
#endif
#ifdef VXWORKS
		taskActivate(id_);
#endif
#if defined( __linux__ )
        // This simple implementation is to resume the thread suspended on creating.
        // There is no Suspend/Reusme equivlent implementation.
        //
        // To have really similiar Suspend/Resume functions,
        // we need to move the pthread_cond_wait() into the while() loop in the derived class
        // That's, currently, the driver2.cpp (Feb-04/2010)
        pthread_mutex_lock(&mutex_suspend_resume);
        suspend_flag = false;
        pthread_cond_signal(&cond_suspend_resume);
        pthread_mutex_unlock(&mutex_suspend_resume);
#endif
	}
}


template <class ThreadingModel>
bool Thread<ThreadingModel>::sleep(int milliseconds)
{
	if ( started_ && !finished_ )
    {
    #if defined(LINUX_FIX)
		return util_platform::Platform::getInstance().delay(milliseconds);
    #else
		return Platform::getInstance().delay(milliseconds);
    #endif
    }

	return false;
}


template <class ThreadingModel>
bool Thread<ThreadingModel>::sleepCurrentThread(int milliseconds)
{
	return util_platform::Platform::getInstance().delay(milliseconds);
}


template <class ThreadingModel>
void Thread<ThreadingModel>::endThread()
{
	if ( started_ && !finished_ )
	{
#ifdef WIN32_OS
		if ( handle_ != NULL )
		{
			TerminateThread(handle_, 0);
			finished_ = true;
		}
#endif

#ifdef VXWORKS
		if ( id_ )
		{
			if ( !finished_ )
			{
				taskDelete(id_);
				finished_ = true;
			}
		}
#endif

#if defined( __linux__ )
        if ( id_ )
        {
            if ( (pthread_cancel( id_ ) == 0) && (pthread_join( id_, NULL) ==0) )
            {
                finished_ = true;
            }
        }
#endif
	}

	delete this;
}


#ifdef WIN32_OS
template <class ThreadingModel>
DWORD WINAPI Thread<ThreadingModel>::run(LPVOID p)
{
	Thread<ThreadingModel>* t = (Thread<ThreadingModel>*)p;

	DWORD ret = t->doRun();
	t->finished_ = true;

	return ret;
}
#endif
#ifdef VXWORKS
template <class ThreadingModel>
int Thread<ThreadingModel>::run(Thread<ThreadingModel>* t, ...)
{
	int ret = t->doRun();
	t->finished_ = true;

	return ret;
}
#endif
#if defined( __linux__ )
template <class ThreadingModel>
void *Thread<ThreadingModel>::run(void *void_p)
{
	Thread<ThreadingModel>* t = (Thread<ThreadingModel>*)void_p;
	static int ret;

    // Simulate simple Suspend / Resume in Linux
    pthread_mutex_lock( &(t->mutex_suspend_resume) );
    while (t->suspend_flag == true)
    {
        pthread_cond_wait( &(t->cond_suspend_resume), &(t->mutex_suspend_resume) );
    }
    pthread_mutex_unlock( &(t->mutex_suspend_resume) );

    ret = t->doRun();
	t->finished_ = true;

	return (void *)&ret;
}
#endif


#ifndef NO_NAMESPACES
} // namespace util_thread
#endif



#endif // THREAD_H

