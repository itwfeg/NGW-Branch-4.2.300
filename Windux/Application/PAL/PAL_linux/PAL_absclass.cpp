//
// File : PAL_absclass.cpp
//
// Implements the linux abstraction layer for APIs which are non portable.
//
/// @file PAL_absclass.cpp
/// Implements the linux abstraction layer for APIs which are non portable.

#include"../PAL_platformheader.h"

#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/times.h>
#include <vector>
#include <ifaddrs.h>
#include <sys/wait.h>
#include "../../source/util/Debug.h"
#include "../../source/HFC/HobString.h"
#include "../../source/util/Bool.h"
#include "../../source/util/Integer.h"
#include "../../source/util/Long.h"

#include <iostream>
#include <sstream>


#undef OLD_LOCK

using namespace std;


//***************************************************************/
#ifdef USING_SELECT_FOR_WAITING
HANDLE PAL_absclass::select_mutx;
HANDLE PAL_absclass::sel_ev;
#endif

PAL_LPCRITICAL_SECTION PAL_absclass::hndl_crit=NULL;
pthread_key_t PAL_absclass::PAL_key;
pthread_once_t PAL_absclass::PAL_kinit_once;
DWORD PAL_absclass::PAL_logging;
std::wfstream PAL_absclass::PAL_log_file;

class criticalSection
{
public:
  criticalSection() { PAL_absclass::PAL_initializeCriticalSection(&cs); }
  ~criticalSection() { PAL_absclass::PAL_deleteCriticalSection(&cs); }
  operator PAL_CRITICAL_SECTION*() { return &cs; }
private:
  PAL_CRITICAL_SECTION cs;
};

static int internetSessionCounter = 0;
static criticalSection internetSessionCounterCs;

char PAL_absclass::out[LOG_OUT_SIZE];

char strMoveMethod[][9]=
  {
    "SEEK_SET",
    "SEEK_CUR",
    "SEEK_END"
  };

class PAL_PtrMan;
int32_t PAL_PtrMan::PAL_initfd(void)
{
  int32_t ret = pipe(PAL_PtrMan::wait_state);
  int32_t flgs=0;
  if (-1 == ret)
    return ret;

  if (-1 == (flgs = fcntl(wait_state[0], F_GETFL, 0)))
    flgs = 0;
  fcntl(wait_state[0], F_SETFL, flgs|O_NONBLOCK);

  if (-1 == (flgs = fcntl(wait_state[1], F_GETFL, 0)))
    flgs = 0;
  fcntl(wait_state[1], F_SETFL, flgs|O_NONBLOCK);

  return ret;
}

// for PAL_getDateFormat()
PAL_DATETIME_DATA_STRU  PAL_long_monthname[] =
  {
    {   PAL_LOCALE_LONG_MONTHNAME_1,    sizeof(PAL_LOCALE_LONG_MONTHNAME_1)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_2,    sizeof(PAL_LOCALE_LONG_MONTHNAME_2)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_3,    sizeof(PAL_LOCALE_LONG_MONTHNAME_3)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_4,    sizeof(PAL_LOCALE_LONG_MONTHNAME_4)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_5,    sizeof(PAL_LOCALE_LONG_MONTHNAME_5)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_6,    sizeof(PAL_LOCALE_LONG_MONTHNAME_6)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_7,    sizeof(PAL_LOCALE_LONG_MONTHNAME_7)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_8,    sizeof(PAL_LOCALE_LONG_MONTHNAME_8)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_9,    sizeof(PAL_LOCALE_LONG_MONTHNAME_9)   -1},
    {   PAL_LOCALE_LONG_MONTHNAME_10,   sizeof(PAL_LOCALE_LONG_MONTHNAME_10)  -1},
    {   PAL_LOCALE_LONG_MONTHNAME_11,   sizeof(PAL_LOCALE_LONG_MONTHNAME_11)  -1},
    {   PAL_LOCALE_LONG_MONTHNAME_12,   sizeof(PAL_LOCALE_LONG_MONTHNAME_12)  -1},
  };

PAL_DATETIME_DATA_STRU  PAL_short_monthname[] =
  {
    {   PAL_LOCALE_SHORT_MONTHNAME_1,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_1)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_2,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_2)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_3,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_3)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_4,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_4)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_5,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_5)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_6,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_6)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_7,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_7)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_8,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_8)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_9,   sizeof(PAL_LOCALE_SHORT_MONTHNAME_9)  -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_10,  sizeof(PAL_LOCALE_SHORT_MONTHNAME_10) -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_11,  sizeof(PAL_LOCALE_SHORT_MONTHNAME_11) -1},
    {   PAL_LOCALE_SHORT_MONTHNAME_12,  sizeof(PAL_LOCALE_SHORT_MONTHNAME_12) -1},
  };

PAL_DATETIME_DATA_STRU  PAL_long_dayname[] =
  {
    {   PAL_LOCALE_LONG_DAYNAME_0,      sizeof(PAL_LOCALE_LONG_DAYNAME_0)     -1},
    {   PAL_LOCALE_LONG_DAYNAME_1,      sizeof(PAL_LOCALE_LONG_DAYNAME_1)     -1},
    {   PAL_LOCALE_LONG_DAYNAME_2,      sizeof(PAL_LOCALE_LONG_DAYNAME_2)     -1},
    {   PAL_LOCALE_LONG_DAYNAME_3,      sizeof(PAL_LOCALE_LONG_DAYNAME_3)     -1},
    {   PAL_LOCALE_LONG_DAYNAME_4,      sizeof(PAL_LOCALE_LONG_DAYNAME_4)     -1},
    {   PAL_LOCALE_LONG_DAYNAME_5,      sizeof(PAL_LOCALE_LONG_DAYNAME_5)     -1},
    {   PAL_LOCALE_LONG_DAYNAME_6,      sizeof(PAL_LOCALE_LONG_DAYNAME_6)     -1},
  };

PAL_DATETIME_DATA_STRU  PAL_short_dayname[] =
  {
    {   PAL_LOCALE_SHORT_DAYNAME_0,     sizeof(PAL_LOCALE_SHORT_DAYNAME_0)    -1},
    {   PAL_LOCALE_SHORT_DAYNAME_1,     sizeof(PAL_LOCALE_SHORT_DAYNAME_1)    -1},
    {   PAL_LOCALE_SHORT_DAYNAME_2,     sizeof(PAL_LOCALE_SHORT_DAYNAME_2)    -1},
    {   PAL_LOCALE_SHORT_DAYNAME_3,     sizeof(PAL_LOCALE_SHORT_DAYNAME_3)    -1},
    {   PAL_LOCALE_SHORT_DAYNAME_4,     sizeof(PAL_LOCALE_SHORT_DAYNAME_4)    -1},
    {   PAL_LOCALE_SHORT_DAYNAME_5,     sizeof(PAL_LOCALE_SHORT_DAYNAME_5)    -1},
    {   PAL_LOCALE_SHORT_DAYNAME_6,     sizeof(PAL_LOCALE_SHORT_DAYNAME_6)    -1},
  };

std::list<PAL_PGLOBAL_THREAD_DATA> PAL_absclass::PAL_global_thread_data;
std::mutex PAL_absclass::PAL_global_thread_data_mutex;

std::mutex PAL_absclass::PAL_global_log_mutex;

void timespec_add_ms(struct timespec *ts, uint64_t msecs)
{
  ts->tv_sec = ts->tv_sec + (long)(msecs / 1000);
  ts->tv_nsec = (ts->tv_nsec + ((long)msecs % 1000) * 1000000);
  if (ts->tv_nsec >= 1000000000) {
    ts->tv_nsec -= 1000000000;
    ts->tv_sec++;
  }
}

void addDWORDToOutput(char *in, int maxlen, DWORD ptr)
{
  char add[64];
  sprintf(add, "%u-", ptr);
  int curlen = strlen(in);
  strncat(in, add, (maxlen-curlen-1));
}

void addPtrToOutput(char *in, int maxlen, void *ptr)
{
  char add[64];
  sprintf(add, "%p-", ptr);
  int curlen = strlen(in);
  strncat(in, add, (maxlen-curlen-1));
}

void addStrToOutput(char *in, int maxlen, const char *msg)
{
  int curlen = strlen(in);
  strncat(in, msg, (maxlen-curlen-1));
}

//***************************************************************/
// PAL_ErrorSave(PAL_ERRNO_TYPE err, const char *msg)
void PAL_absclass::PAL_ErrorSave(PAL_ERRNO_TYPE err, const char *msg)
{
  std::string err_str;
  PAL_ERRNO_TYPE l_errc = err;
  char    err_buf[LOG_OUT_SIZE];
  char    *err_message;

  // Retrieve error message
  err_message = strerror_r(l_errc, err_buf, sizeof(err_buf));

  if (msg)
    {
      err_str += std::string(msg);
      err_str += std::string(" ");
    }

  // format error message
  char    out_buf[LOG_OUT_SIZE];
	sprintf(out_buf, "Code = 0x%08x, Message = ", l_errc);

  // Convert to std::string so that we could operate string easily
  err_str += std::string(out_buf);
  err_str += err_message;

  PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());

  PAL_CThreadData *myThrd = static_cast<PAL_CThreadData *> (PAL_initThrdData());

  if (NULL != myThrd)
    myThrd->m_errno = l_errc;
}

//***************************************************************/
// PAL_ErrorChk(bool test, PAL_ERRNO_TYPE err, const char *msg)
bool PAL_absclass::PAL_ErrorChk(bool test, PAL_ERRNO_TYPE err, const char *msg)
{
  // have we passed the 'test'?
  if (test)
    {
      // log the error and save the error code
      PAL_ErrorSave(err, msg);
    }
  return test;
}

//***************************************************************/
// PAL_log(const wchar_t *log_str)
void PAL_absclass::PAL_log( const wchar_t *log_str )
{
	std::lock_guard<std::mutex> g( PAL_global_log_mutex );

	// check if the log has been started
	if (!PAL_log_file.is_open())
	{
		// start the log
		PAL_log_file.open( PAL_LOG_FILE_PATH, PAL_LOG_ACCESS_MODE );
	}
    
	/* Print today's date e.g. "January 31, 2001".  */
	struct timeval tv;
	gettimeofday( &tv, NULL );

	time_t mytime( time( NULL ) );

	int count( sprintf( PAL_absclass::out, "%06u:", (DWORD)tv.tv_usec ) );
	strftime( &PAL_absclass::out[count], 100, "%T:%m:%d:%Y-w-", localtime( &mytime ) );

	count = strlen( out );
	sprintf( &PAL_absclass::out[count], "0x%08x-", (DWORD)pthread_self() );

	PAL_log_file << PAL_absclass::out << log_str << std::endl;
	PAL_log_file.flush();
}

//***************************************************************/
// PAL_log(const char *log_str)
void PAL_absclass::PAL_log( const char *log_str )
{
	std::lock_guard<std::mutex> g( PAL_global_log_mutex );

	// check if the log has been started
	if (!PAL_log_file.is_open())
	{
		// start the log
		PAL_log_file.open( PAL_LOG_FILE_PATH, PAL_LOG_ACCESS_MODE );
	}

	/* Print today's date e.g. "January 31, 2001".  */
	struct timeval tv;
	gettimeofday( &tv, NULL );

	time_t mytime( time( NULL ) );

	int count( sprintf( PAL_absclass::out, "%06u:", (DWORD)tv.tv_usec ) );
	strftime( &PAL_absclass::out[count], 100, "%T:%m:%d:%Y- -", localtime( &mytime ) );

	count = strlen( out );
	sprintf( &PAL_absclass::out[count], "0x%08x-", (DWORD)pthread_self() );

	PAL_log_file << PAL_absclass::out << log_str << std::endl;
	PAL_log_file.flush();
}

//***************************************************************/
// PAL_log_printf(const char *format, ...)
void PAL_absclass::PAL_log_printf( const char *format, ... )
{
  va_list arg_list;
  char    strbuf[LOG_OUT_SIZE];

  va_start(arg_list, format);
  vsnprintf(strbuf, LOG_OUT_SIZE, format, arg_list);
  va_end(arg_list);

  PAL_log(strbuf);
}

//***************************************************************/
// PAL_PrintFormatLog(const char *format, ...)
void PAL_absclass::PAL_PrintFormatLog(DWORD log_zone, const char *format, ...)
{
  if ( !(log_zone & PAL_logging) )    return;

  va_list arg_list;
  char    strbuf[LOG_OUT_SIZE];

  va_start(arg_list, format);
  vsnprintf(strbuf, LOG_OUT_SIZE, format, arg_list);
  va_end(arg_list);

  PAL_log(strbuf);
}

//***************************************************************/
// PAL_OutputDbgStrA(const char *str)
void PAL_absclass::PAL_OutputDbgStrA(const char *str)
{
  PAL_log(str);
}

//***************************************************************/
// PAL_OutputDbgStr(const TCHAR *str)
void PAL_absclass::PAL_OutputDbgStr(const TCHAR *str)
{
  PAL_log(str);
}

//***************************************************************/
// PAL_OutputDbgStr(const std::string &str)
void PAL_absclass::PAL_OutputDbgStr(const std::string &str)
{
  PAL_log(str.c_str());
}

//***************************************************************/
// PAL_InterlockedIncrement(long *val)
long PAL_absclass::PAL_InterlockedIncrement(long *val)
{
  char outline[256];
  sprintf(outline, "PAL_InterlockedIncrement(%ld):", *val);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  return __sync_add_and_fetch(val, 1);
}

//***************************************************************/
// PAL_InterlockedDecrement(long *val)
long PAL_absclass::PAL_InterlockedDecrement(long *val)
{
  char outline[256];
  sprintf(outline, "PAL_InterlockedDecrement(%ld):", *val);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  return __sync_sub_and_fetch(val, 1);
}

//***************************************************************/
// PAL_InterlockedExchange(long *dest, long val)
long PAL_absclass::PAL_InterlockedExchange(long *dest, long val)
{
  long old_v = *dest;
  char outline[256];
  sprintf(outline, "PAL_InterlockedExchange(%ld, %ld):", *dest, val);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  __sync_lock_test_and_set(dest, val);

  return old_v;
}

//***************************************************************/
// PAL_getCurrProc(void)	fixed memory leak here in CurrProc -- kh/ats
PAL_PtrMan *pCurrProc = NULL;
HANDLE PAL_absclass::PAL_getCurrProc(void)
{
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_getCurrProc(void)");

	if(pCurrProc == NULL)
	{
		pCurrProc = new (PAL_PtrMan);

		// Check that we got the memory we requested
		if( NULL == pCurrProc )
		{
			PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for handle (pCurrProc).");
			return NULL;
		}

		PAL_PPROC_HNDL pid = new (PAL_PROC_HNDL);

		if (NULL == pid)
		{
			PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for pid. Delete pCurrProc.");
			delete(pCurrProc);
			pCurrProc = NULL;
			return NULL;
		}

		*pid = getpid();

		pCurrProc->PAL_setHndl((HANDLE) pid);
		pCurrProc->PAL_setType(PROCESS_ID_TYPE);
	}
	return((HANDLE) pCurrProc);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD PAL_API PAL_absclass::PAL_setThreadIdealProcessor(
	HANDLE thrd,
	DWORD dwIdealProcessor )
{
	char outline[256];
	sprintf( outline, "PAL_setThreadIdealProcessor(%p, %u)-incomplete:", thrd, dwIdealProcessor );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	if ( thrd )
	{
		PAL_PtrMan *e_hndl( static_cast<PAL_PtrMan*>( thrd ) );
		PAL_PPROC_HNDL pid( static_cast<PAL_PPROC_HNDL>( e_hndl->PAL_getHndl() ) );

		if ( pid )
		{
			cpu_set_t cpu_set;
			CPU_SET( dwIdealProcessor, &cpu_set );

			// set the affinity for the given thread/process
			if ( sched_setaffinity( *pid, sizeof(cpu_set_t), &cpu_set ) )
			{
				PAL_ErrorSave( errno, NULL );
				return -1;
			}
		}
	}

	// I'm supposed to return the previous IdealProcessor, but I don't know
	// this at the moment though there is a cli command
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD PAL_API PAL_absclass::PAL_resumeThread(
	HANDLE hThread )
{
	char outline[256];
	sprintf( outline, "PAL_resumeThread(%p):", hThread );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	// Search linked-list to find the matched handle
	if ( hThread )
	{
		std::lock_guard<std::mutex> g( PAL_global_thread_data_mutex );

		for ( auto& gt_data : PAL_global_thread_data )
			if ( hThread == gt_data->hThread )
			{
				// Found the thread data, trigger it
				pthread_mutex_lock  ( &gt_data->CREATE_SUSPENDED.mutex );
				pthread_cond_signal ( &gt_data->CREATE_SUSPENDED.cond );
				pthread_mutex_unlock( &gt_data->CREATE_SUSPENDED.mutex );
				
				return 1; // ***** WARNING: Previous suspend count value in Win32
			}
	}

	if( hThread == NULL )
		Debug::trace("PALThread", "PAL_resumeThread() hThread is NULL!!!" );
	else
		Debug::trace("PALThread", "PAL_resumeThread() Could not find handle in global list!!!" );

	errno = ESRCH;
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PAL_API PAL_absclass::PAL_setThreadPriority(
	HANDLE hThread,
	int priority )
{
	// This causes system crashes under linux. Investigate.
	char outline[256];
	sprintf(outline, "PAL_setThreadPriority(%p, %d):", hThread, priority);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

#ifdef SETTHREADPRIORITY_WORKS
	PAL_PtrMan *e_hndl( static_cast<PAL_PtrMan*>( hThread ) );
	PAL_PTHREAD t_id( static_cast<PAL_PTHREAD>( e_hndl->PAL_getHndl() ) );
	struct sched_param param;
	int policy;

	memset(&param, 0, sizeof(param));
	if (pthread_getschedparam(*t_id, &policy, &param) != 0)
	{
		PAL_ErrorSave(errno, "get thread priority");
		return FALSE;
	}

	int prio_min = sched_get_priority_min(policy);
	int prio_max = sched_get_priority_max(policy);

	if (priority > prio_max)
		priority = prio_max;
	if (priority < prio_min)
		priority = prio_min;

	param.sched_priority = priority;
	if (pthread_setschedparam(*t_id, policy, &param) != 0)      // or pthread_setschedprio(hThread, priority)
	{
		PAL_ErrorSave(errno, "set thread priority");
		return FALSE;
	}
#endif      //  SETTHREADPRIORITY_WORKS

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uintptr_t PAL_absclass::PAL_beginthreadex(
	VOID * UNUSED(security),
	UINT stack_size,
	UINT (__stdcall * start_address)(void *),
	VOID *arg_list,
	UINT UNUSED(init_flag),
	UINT *thread_addr )
{
	static const char *err_str("PAL_beginthreadex: ");
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

	// call normal createThread
	HANDLE ret = PAL_createThread( NULL, stack_size, start_address, arg_list, 0, thread_addr);

	return (uintptr_t)ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PAL_absclass::PAL_endthreadex( unsigned retval )
{
	static const char *err_str( "PAL_endthreadex: " );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, err_str );

	const pthread_t thrd_id( pthread_self() );

	std::lock_guard<std::mutex> g( PAL_global_thread_data_mutex );

	// find our thread
	for ( auto& gt_data : PAL_global_thread_data )
	{
		PAL_PtrMan *ptr_man( static_cast<PAL_PtrMan*>( gt_data->hThread ) );
		HANDLE h;

		if (( ptr_man ) && ( (h = ptr_man->PAL_getHndl()) ))
		{
			pthread_t thrd( *(static_cast<PAL_PTHREAD>( h )) );

			if ( thrd_id == thrd )
			{
				gt_data->setExitCode( retval );
				break;
			}
		}
	}

	// cleanup thread resources
	pthread_detach( thrd_id );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PAL_API PAL_absclass::PAL_getExitCodeThread(
	HANDLE hThread,
	LPDWORD lpExitCode )
{
	char outline[256];
	sprintf( outline, "PAL_getExitCodeThread(%p, %p):", hThread, lpExitCode );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	if ( hThread )
    {
		std::lock_guard<std::mutex> g( PAL_global_thread_data_mutex );

		for ( auto& gt_data : PAL_global_thread_data )
			if ( hThread == gt_data->hThread )
			{
				// Found the thread data
				*lpExitCode = gt_data->m_exitcode;
				return TRUE;
			}
	}

	errno = ESRCH;
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PAL_API PAL_absclass::PAL_getThreadTimes(
	HANDLE UNUSED(thrd),
	PAL_LPFILETIME UNUSED(creation_time),
	PAL_LPFILETIME UNUSED(exit_time),
	PAL_LPFILETIME UNUSED(kernel_time),
	PAL_LPFILETIME UNUSED(user_time) )
{
	static const char *err_str("PAL_getThreadTimes: (DEAD CODE)");
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PAL_API PAL_absclass::PAL_terminateThread(
	HANDLE hThread,
	DWORD dwExitCode )
{
	char outline[256];
	sprintf( outline, "PAL_terminateThread(%p, %u):", hThread, dwExitCode );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	if ( hThread )
    {
		PAL_PtrMan *e_hndl( static_cast<PAL_PtrMan*>( hThread ) );
		PAL_PTHREAD t_id( static_cast<PAL_PTHREAD>( e_hndl->PAL_getHndl() ) );

		if ( t_id )
		{
			std::lock_guard<std::mutex> g( PAL_global_thread_data_mutex );

			for ( auto& gt_data : PAL_global_thread_data )
				if ( hThread == gt_data->hThread )
				{
					// Found the thread data, cancel it
					const BOOL result( 0 == pthread_cancel( *t_id ) );

					gt_data->setExitCode( dwExitCode );

#ifdef USING_SELECT_FOR_WAITING
					e_hndl->PAL_touchfd();
#endif

					return result;
				}
		}
    }

	// according to 'man' pthread_cancel only has one error
	PAL_ErrorSave( (errno = ESRCH), " No Thread with given t_id found");
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PAL_API PAL_absclass::PAL_getProcessAffinityMask(
	HANDLE proc,
	PDWORD_PTR proc_affinity_mask,
	PDWORD_PTR sys_affinity_mask )
{
	static const char *err_str("PAL_getProcessAffinityMask: ");
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

	PAL_PtrMan *e_hndl( static_cast<PAL_PtrMan*>( proc ) );
	PAL_PPROC_HNDL pid( static_cast<PAL_PPROC_HNDL>( e_hndl->PAL_getHndl() ) );
	cpu_set_t cpuset;

	if ( sched_getaffinity(*pid, sizeof(cpu_set_t), &cpuset) )
		return FALSE;

	*proc_affinity_mask = 0;
	*sys_affinity_mask = 0;      // TODO: I don't know how to get this

	// assumes little endian. Also, it's possible that there are more
	// than 32 processors but then WIN32 would have had to use
	// getProcessAffinityMaskEx to get groupings.

	// turn a cpu_set_t into a bitmask
	for (unsigned int i=0;i<sizeof(DWORD);i++)
		if (CPU_ISSET(i,&cpuset))
			*proc_affinity_mask |= ( 1 << i );

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PAL_API PAL_absclass::PAL_terminateProcess(
	HANDLE hProcess,
	UINT uExitCode )
{
	char outline[256];
	sprintf(outline, "PAL_terminateProcess(%p, %u):", hProcess, uExitCode);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	PAL_PtrMan *e_hndl( static_cast<PAL_PtrMan*>( hProcess ) );

	if ( !e_hndl )
		return FALSE;

	PAL_PPROC_HNDL pid( static_cast<PAL_PPROC_HNDL>( e_hndl->PAL_getHndl() ) );

	// kill (terminate) the given process
	if (( pid ) && ( kill( *pid, SIGTERM ) ))
	{
		PAL_ErrorSave( errno, NULL );
		return FALSE;
	}

	// get thread specific data
	PAL_CThreadData *myThrd( static_cast<PAL_CThreadData*>( PAL_initThrdData() ) );
	if ( myThrd )
		myThrd->m_exitcode = uExitCode;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HANDLE PAL_absclass::PAL_createThread(
	PAL_PTR_SEC_ATTRIBS thrd_attrib,
	SIZE_T stack_size,
	PAL_THREAD_START start_addr,
	LPVOID param,
	DWORD creation_flags,
	LPDWORD thrd_id )
{
	char outline[1024];
	sprintf(outline, "PAL_createThread(%p, %zd, %p, %p, 0x%08x, %p):",
		thrd_attrib,
		stack_size,
		start_addr,
		param,
		creation_flags,
		thrd_id);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	PAL_PtrMan *ptr_man = new (PAL_PtrMan);
	if ( NULL == ptr_man )
	{
		Debug::trace("PALThread", "PAL_createThread() ptr_man is NULL!!!" );
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for handle (PAL_PtrMan)");

		return NULL;
	}

	PAL_PTHREAD thread = new PAL_THREAD;
	if ( NULL == thread )
    {
		Debug::trace("PALThread", "PAL_createThread() thread is NULL!!!" );
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for thread. Delete ptr_man.");

		delete(ptr_man);
		ptr_man = NULL;

		return NULL;
    }

	ptr_man->PAL_setHndl((HANDLE) thread);
	ptr_man->PAL_setType(THREAD_TYPE);

	// we keep a copy of this in PAL_GLOBAL_THREAD_DATA so increment our reference count
	ptr_man->PAL_incObjCnt();

	// setup the thread attributes
	pthread_attr_t gPthread_attr;
	pthread_attr_init(&gPthread_attr);

	// Memory will be released later in the Wrapper thread.
	PAL_PWRAPPER_THREAD_INFO wt_info = new PAL_WRAPPER_THREAD_INFO;
	if (wt_info == NULL)
    {
		Debug::trace("PALThread", "PAL_createThread() wt_info is NULL!!!" );
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for wt_info. Delete ptr_man.");

		delete(ptr_man);
		ptr_man = NULL;

		return NULL;
    }
	memset(wt_info, 0, sizeof(PAL_WRAPPER_THREAD_INFO));

	PAL_PGLOBAL_THREAD_DATA gt_data = new PAL_GLOBAL_THREAD_DATA;
	if (gt_data == NULL)
    {
		Debug::trace("PALThread", "PAL_createThread() gt_data Global thread data is NULL!!!" );
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for gt_data. Delete wt_info, delete ptr_man.");

		delete(wt_info);
		wt_info = NULL;

		delete(ptr_man);
		ptr_man = NULL;

		return NULL;
    }
	memset(gt_data, 0, sizeof(PAL_GLOBAL_THREAD_DATA));

#ifdef USING_SELECT_FOR_WAITING
	ptr_man->PAL_initfd();
#endif

	wt_info->pGlobalData = gt_data;     // Bind Wrapper info to its Global thread data

	// With the benefit of static initialization of mutex, we could use global data here.
	// Link current gt_data to head of global thread data
	PAL_global_thread_data_mutex.lock();
	PAL_global_thread_data.push_front( gt_data );

	// Cleanup old thread data
	std::list<PAL_PGLOBAL_THREAD_DATA> doomed;

	unsigned int running( 0 );

	// create list of inactive threads
	for ( auto& gt_data_tmp : PAL_global_thread_data )
	{
		if ( gt_data_tmp->isRunning() )
			++running;
		else if ( 30000 <= gt_data_tmp->millisecondsSinceExit() )
			doomed.push_back( gt_data_tmp );
	}

	sprintf(outline, "PAL_createThread() threads %d (running %d, complete %d), doomed %d",
		PAL_global_thread_data.size(),
		running,
		PAL_global_thread_data.size() - running,
		doomed.size());
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	if ( Debug::isRunning() )
		Debug::trace("PALThread", outline );

	// delete out old threads
	for ( auto& gt_data_tmp : doomed )
	{
		PAL_absclass::PAL_closeHandle( gt_data_tmp->hThread );

		PAL_global_thread_data.remove( gt_data_tmp );

		delete gt_data_tmp;
	}

	PAL_global_thread_data_mutex.unlock();

	wt_info->creation_flags = creation_flags;
	wt_info->start_addr     = start_addr;
	wt_info->param          = param;
	/******************************************************************************/
	//for every ptr_man copy we must call dupHandle this would prevent any deletion
	//of the ptr_man object when user calls closeHandle on it!!!
	/******************************************************************************/
	wt_info->ptr            = ptr_man;

	wt_info->pGlobalData->hThread = (HANDLE)ptr_man;

	// TODO: not 64bit safe
	if (NULL != thrd_id)
		*thrd_id = (DWORD)ptr_man;
	
	int ret = pthread_create(
			   thread,
			   &gPthread_attr,
			   PAL_WrapperThread,
			   wt_info);

	// check for create thread errors
	if (0 != ret)
    {
		PAL_global_thread_data_mutex.lock();
		PAL_global_thread_data.remove( gt_data );
		PAL_global_thread_data_mutex.unlock();

		delete(gt_data);
		gt_data = NULL;

		delete(wt_info);
		wt_info = NULL;

		delete(ptr_man);
		ptr_man = NULL;

		switch (ret)
        {
		case EAGAIN:
			Debug::trace("PALThread", "PAL_createThread(): The system lacked the necessary resources to create another thread.");
			PAL_ErrorSave(
				ret,
				"The system lacked the necessary resources to create "\
				"another thread, or the system-imposed limit on the "\
				"total number of threads in a process "\
				"{PTHREAD_THREADS_MAX} would be exceeded." );

			break;
		case EINVAL:
			Debug::trace("PALThread", "PAL_createThread(): The value specified by attr is invalid." );

			PAL_ErrorSave(
				ret,
				"The value specified by attr is invalid." );

			break;
		case EPERM:
			Debug::trace("PALThread", "PAL_createThread(): The caller does not have appropriate permission to set "\
			             "the required scheduling parameters or scheduling policy" );
			PAL_ErrorSave(
				ret,
				"The caller does not have appropriate permission to set \
				the required scheduling parameters or scheduling policy." );

			break;
		default:
			Debug::trace("PALThread", "PAL_createThread(): unknown error: " + Integer::toString(ret));
			PAL_ErrorSave( ret, " unknown error" );

			break;
        }
    }

	pthread_attr_destroy(&gPthread_attr);

	sprintf(outline, "PAL_createThread(%p, %zd, %p, %p, 0x%08x, %p):  Returning ",
			thrd_attrib,
			stack_size,
			start_addr,
			param,
			creation_flags,
			thrd_id);

	addPtrToOutput(outline, 256, ptr_man);
	PAL_PRINT_LOG(PAL_FUNC_LOG, outline);
	return((HANDLE) ptr_man);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void PAL_absclass::PAL_WrapperThread_cleanup(void *arg)
{
	PAL_PWRAPPER_THREAD_INFO wt_info( (PAL_PWRAPPER_THREAD_INFO)arg );
	PAL_PGLOBAL_THREAD_DATA gt_data( wt_info->pGlobalData );

	char outline[256];
	sprintf(outline, "PAL_WrapperThread_cleanup(%p)", arg);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	Debug::trace("PALThread", "PAL_WrapperThread_cleanup() Destroy thread resources." );

	// Destroy CREATE_SUSPENDED mutex & condition
	if ( (wt_info->creation_flags & PAL_THREAD_CREATE_SUSPENDED) )
	{
		pthread_mutex_destroy( &(gt_data->CREATE_SUSPENDED.mutex) );
		pthread_cond_destroy( &(gt_data->CREATE_SUSPENDED.cond) );
	}

#ifdef USING_SELECT_FOR_WAITING
	PAL_PtrMan *ptr( (PAL_PtrMan *)wt_info->ptr );
	ptr->PAL_touchfd();
#endif

	delete wt_info; // Free wt_info allocalted in PAL_createThread()

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void *PAL_absclass::PAL_WrapperThread(void *arg)
{
	PAL_PWRAPPER_THREAD_INFO wt_info( (PAL_PWRAPPER_THREAD_INFO)arg );
	PAL_PGLOBAL_THREAD_DATA gt_data( wt_info->pGlobalData );

	char outline[256];
	sprintf( outline, "PAL_WrapperThread(%p)", arg );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	int32_t old_cncl( 0 );

	if ( 0 != pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, &old_cncl ) )
	{
		PAL_ASSERT( 1 );
		PAL_PRINT_LOG( PAL_ERR_LOG, "Unable to set thead cancel state" );
	}

	if ( 0 != pthread_setcanceltype( PTHREAD_CANCEL_ASYNCHRONOUS, &old_cncl ) )
	{
		PAL_ASSERT( 1 );
		PAL_PRINT_LOG( PAL_ERR_LOG, "Unable to set thead cancel type" );
	}

	// Wait for mutex to emulate Win32 CREATE_SUSPENDED flags
	if ( (wt_info->creation_flags & PAL_THREAD_CREATE_SUSPENDED) )
	{
		pthread_mutex_lock  ( &(gt_data->CREATE_SUSPENDED.mutex) );
		pthread_cond_wait   ( &(gt_data->CREATE_SUSPENDED.cond), &(gt_data->CREATE_SUSPENDED.mutex) );
		pthread_mutex_unlock( &(gt_data->CREATE_SUSPENDED.mutex) );
	}

	// We use cancellation function to release resources.
	// It could handle both cancellation exit (pthread_cancel)& normal exit (pthread_exi)
	pthread_cleanup_push( PAL_WrapperThread_cleanup, wt_info );

	gt_data->m_exitcode = STILL_ACTIVE;

	// Call to real thread funtion
	DWORD thrd_exit_code( wt_info->start_addr( wt_info->param ) );

	// ***** WARNING: code below this point will NOT be executed
	// when thread is canceled by pthread_cancel(), aka PAL_terminateThread()

	gt_data->setExitCode( thrd_exit_code );

	pthread_cleanup_pop(1);

	return NULL;
}

#ifdef USING_SELECT_FOR_WAITING

/////////////////////////////////////////////////////////////////////////////////////
//Initializes the fd set based on the current contents of the dynamic array.
//
//@param[in] inactive_hndls inactive fds. Used while waiting on all handles sent
//							to waitformultiobj
//@param[in,out] max_fd value of the largest fd in the set.
//@param[in,out] rd_fds read fds set.
//
//
DWORD PAL_absclass::PAL_initfdSet(std::vector<int32_t> &inactive_hndls,
				  int32_t &max_fd, fd_set &rd_fds)
{
  char outline[256];

  sprintf(outline, "PAL_initfdSet()");

  //
  //reinitialize the fd set
  //
  FD_ZERO(&rd_fds);
  max_fd = 0;

  std::vector<int32_t>::size_type n_cnt = inactive_hndls.size();
  for(DWORD i=0;i<n_cnt;i++)
    {
      max_fd = (max_fd < inactive_hndls[i]) ? inactive_hndls[i] : max_fd;
      FD_SET(inactive_hndls[i], &rd_fds);
    }//for

  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  return WAIT_OBJECT_0;
}
/////////////////////////////////////////////////////////////////////////////////////
//Initializes the inactive fd lists with fds belonging to all handles passed in.
//
//@param[in] hndls Pointers to hanldes that the thread intends to wait on
//@param[in] n_cnt Count of the handles the thread intends to wait on
//@param[in] inact_lst list of inactive fds used to reintialized the fdset
//						through the main loop in waitformulitobj
//
//
DWORD PAL_absclass::PAL_initMultiObj(
	const HANDLE *hndls,
	const DWORD n_cnt,
	std::vector<int32_t> &inact_lst )
{
	char outline[256];

	sprintf(outline, "PAL_initMultiObj()");
	//
	//clear the set and load all handles into
	//the fd_set to be monitored.  Return
	//EINVAL if we find a NULL pointer while
	//still within the valid pointer count.
	//
	//.    FD_ZERO(&rd_fds);

	//
	//retrieve the fds from PtrMan & build
	//fdset.
	//
	for ( DWORD i( n_cnt ); i--; )
	{
		PAL_PtrMan *ptr;

		if ( !(ptr = static_cast<PAL_PtrMan *>( hndls[i] )) )
		{
			errno = -EINVAL;
			return WAIT_FAILED;
		}

		const PAL_TYPE_OF_PTR ptr_type( ptr->PAL_getType() );

		// call the handler for the different types
		switch ( ptr_type )
		{
		case CS_TYPE:
			addStrToOutput( outline, 256, "CS " );
			break;
		case MUTEX_TYPE:
			addStrToOutput( outline, 256, "mutex " );
			break;
		case SEM_TYPE:
			addStrToOutput( outline, 256, "semaphore " );
			break;
		case MANUAL_EVENT_TYPE:
			addStrToOutput( outline, 256, "manual " );
			break;
		case AUTO_EVENT_TYPE:
			addStrToOutput( outline, 256, "auto " );
			break;
		case THREAD_TYPE:
			addStrToOutput( outline, 256, "thread " );
			break;
		default:
			addStrToOutput( outline, 256, "Waiting on Invalid type:" );
			addDWORDToOutput( outline, 256, ptr_type );
			PAL_PRINT_LOG( PAL_ERR_LOG, outline );
			break;
		}

		const int32_t *fd( ptr->PAL_getFd() );

		if ( 0 <= fd[0] )
			inact_lst.push_back( fd[0] );

	}//for

	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	return WAIT_OBJECT_0;
}

/////////////////////////////////////////////////////////////////////////////////////
//PAL_findPtrForfd
//Find the ptr matching the fd passed.
//
//@param[in] hndl Pointers to all hanldes which may potentially contain the fd. 
//@param[in] total_fds Size of the hndl array
//@param[in] fd_2_find Fd that needs to be found.
//@param[in,out] idx index into the array of handles corresponding to the found fd.
//
//@return NULL or ptr_man containing the fd_2_find.
//
HANDLE PAL_absclass::PAL_findPtrForfd(const HANDLE *hndl, const int32_t hndl_cnt, const int32_t fd_2_find, int32_t &idx)
{
  //
  //find the ptr hndl with active fd
  //
  HANDLE ptr_man = NULL;

  for(idx=0;idx<hndl_cnt;idx++)
    {
      if (static_cast<PAL_PtrMan *> (hndl[idx])->PAL_getFd()[0] == fd_2_find)
	{
	  ptr_man = hndl[idx];
	  break;
	}
    }
  return ptr_man;
}
/////////////////////////////////////////////////////////////////////////////////////
//PAL_processSelectRetVal()
//We get called when we have one or more active fds for the thread.  We search for
//the active fds and call PAL_waitForSingleObject for the PtrMan.  Return value
//depends on the wait_all flag.
//
//@param[in] rd_fds read fds set.
//@param[in] hndls Pointers to hanldes that the thread intends to wait on
//@param[in] act_fd_cnt return value of the select.  At this point it is
//           guaranteed to be active fd count.
//@param[in,out] max_fd the maximum value of the fd in this fd set.
//@param[in,out] fds_in_set Count of the handles in the fd set.
//@param[in] b_wait_all wheather the thread intends to wait for all the handle
//           or just the first one that is available.
//@param[out] val_to_ret WAIT_OBJECT_0+active handle, WAIT_FAILED on error,
//            WAIT_TIMEOUT on time out or fd_cnt if b_wait_all is set and all
//            handles are available.
//@param[out] tmp_act_lst list of active fds removed from fd_set when wait_all is set
//@param[in] n_cnt Count of the handles the thread intends to wait on
//
//@return TRUE if the value in val_to_ret should be simply returned back to the
//        the caller, FALSE if we are not doen waiting.
//
bool PAL_absclass::PAL_processSelectRetVal(
					   fd_set &rd_fds,
					   const HANDLE *hndls,
					   const int32_t max_fd,
					   DWORD &fds_in_set,
					   const bool b_wait_all,
					   DWORD &val_to_ret,
					   std::vector<int32_t> &tmp_act_lst,
					   std::vector<int32_t> &tmp_inact_lst)
{
  DWORD fd_cnt=0;

  bool all_active=TRUE;
  const DWORD orig_fd_cnt=fds_in_set;
  fd_set r_fds;
  const int32_t orig_max_fd=max_fd;
  std::vector<int32_t>::size_type list_sz = tmp_act_lst.size();
  DWORD act_idx = static_cast<DWORD> (list_sz);

  //
  //seems like one of the fds was activated
  //if b_wait_all is true then we should return
  //success only if all fds have been signaled. Else
  //we return WAIT_OBJECT_0+first_activated_fd
  //
  for(int32_t i=0;i<=orig_max_fd;i++)
    {
      if (FD_ISSET(i, &rd_fds))
        {
	  int32_t idx;
	  PAL_PtrMan *ptr_man;

	  //.			ptr_man = static_cast <PAL_PtrMan *> (PAL_findPtrForfd(hndls, max_fd, i, idx));
	  ptr_man = static_cast <PAL_PtrMan *> (PAL_findPtrForfd(hndls, orig_fd_cnt, i, idx));
	  if (NULL == ptr_man)
	    {
	      errno = -EINVAL;
	      val_to_ret = WAIT_FAILED;
	      goto EXIT_PROCSELECT;
	    }
	  //.			fd_cnt++;

	  //
	  //we call the waitforsingleobj method to
	  //block threads that call any of the waitforXXobj
	  //methods.  The fd is cleared in this method also.
	  //
	  switch(PAL_waitForSingleObj(ptr_man, 0))
            {
	      //
	      //there is no reason why we should not get
	      //success here.  If we dont then something
	      //bad happened
	      //
	    case(WAIT_FAILED):
	      val_to_ret = WAIT_FAILED;
	      if (TRUE != b_wait_all)
		return FALSE;
	      goto EXIT_PROCSELECT;
	    case(WAIT_TIMEOUT):
	      val_to_ret = WAIT_TIMEOUT;
	      //we should try to check other handles before we give up if  b_wait_all is not true;
	      ///
	      //. 	 	       		if (TRUE != b_wait_all)
	      //.	                    return FALSE;
	      //.					goto EXIT_PROCSELECT;
	      if (idx == (int32_t)orig_fd_cnt)	
		{
		  if (TRUE != b_wait_all)
		    return FALSE;
		  goto EXIT_PROCSELECT;
		}
	      else
		continue;
	      ///
	    case(WAIT_OBJECT_0):
	      fd_cnt++;
	      if (TRUE != b_wait_all)
		{
		  val_to_ret = WAIT_OBJECT_0 + idx;
		  return TRUE;
		}
	      break;
	    default:
	      val_to_ret = WAIT_FAILED;
	      goto EXIT_PROCSELECT;
            }// switch(PAL_waitForSingleObj(ptr_man, 0))
	  //
	  //check if wait_all flag is set
	  //if set only return if all objs are signaled
	  //else return the first active obj
	  //

	  fds_in_set--;
	  //
	  //we are done if all sync obj are
	  //available.  ret_val of 0 tells us wheather
	  //an error occured in one of the objects
	  //tested.
	  //
	  if (fd_cnt == orig_fd_cnt)
            {
	      //
	      //if we are unable to acquire all synch objs
	      //while b_wait_all is set we move the active_fds
	      //to a dynamic array so that select call blocks.
	      //we also check the elements already in the dynamic
	      //array to see if some fds here are inactive if so
	      //then we move all of these back to fd set and return
	      //this way fd_set always has inactive fds causing select
	      //to block and the dynamic array always contains active
	      //fds of the time.
	      //

	      //.	            std::vector<int32_t>::size_type list_sz = tmp_act_lst.size();
	      std::vector<int32_t>::iterator it;
	      std::vector<int32_t>::iterator lst_beginning=tmp_act_lst.begin();
	      for(it=lst_beginning;it<lst_beginning+list_sz;)
                {
		  //
		  //we use select with timeout=0 to see if
		  //the fd is active or not
		  //

		  int32_t fd = *it;
		  struct timeval t_out;
						
		  memset(&t_out, 0, sizeof(t_out));

		  FD_ZERO(&r_fds);
		  FD_SET(fd, &r_fds);

		  int32_t act_fds = select(fd+1, &r_fds, NULL, NULL, &t_out);
		  if (0 == act_fds)
                    {
		      tmp_inact_lst.push_back(*it);
		      tmp_act_lst.erase(it);
		      all_active = FALSE;
		      fds_in_set++;
		      act_idx--;
		      list_sz--;
		      continue;
                    }
		  //                    else
		  it++;
                }
	      //
	      //this thread cnt would be used by the waitforsingleobj
	      //thread to figure out when all threads are done
	      //and it can quite
	      //
	      if (TRUE == all_active)
		{
		  val_to_ret = WAIT_OBJECT_0 + tmp_act_lst.size()+1;
		  return all_active;
		}
            }//if (fd_cnt == orig_fd_cnt)

	  tmp_act_lst.push_back(i);
	  //			for(int32_t loop_cnt=0;loop_cnt<inact_lst_sz;loop_cnt++)
	  //			int32_t inact_lst_sz = tmp_inact_lst.size();
	  std::vector<int32_t>::iterator it;
	  for(it=tmp_inact_lst.begin();it<tmp_inact_lst.end();it++)
	    {
	      if (i == *it)
		{
		  tmp_inact_lst.erase(it);
		  break;
		}
	    }
        }//if fd_isset
    }//for

 EXIT_PROCSELECT:
  char outline[256];
  int32_t idx;
  //
  //Only used for logging purposes.
  //
  DWORD retVal = WAIT_FAILED;

  if (TRUE == b_wait_all)
    {
      //		std::vector<int32_t>::size_type list_sz = tmp_act_lst.size();
      //		for(;act_idx<list_sz;act_idx++)
      //
      //we intialize act_idx with the size of dynamic array when
      //first get called.  This way only newly added fds would
      //be released and old (already released) items are left
      //alone.
      //
		std::vector<int32_t>::iterator it;
		for(it=tmp_act_lst.begin()+act_idx;it<tmp_act_lst.end();it++)
		{
			//
			//we use select with timeout=0 to see if
			//the fd is active or not
			//

			PAL_PtrMan *ptr_man = static_cast <PAL_PtrMan *> (PAL_findPtrForfd(hndls, max_fd, tmp_act_lst[act_idx], idx));
			if (NULL != ptr_man)
			{
				PAL_TYPE_OF_PTR ptr_type = ptr_man->PAL_getType();		//	CCD: We should set this after we check for ptr_man not Null!
				switch(ptr_type)
				{
					case CS_TYPE:
						addStrToOutput(outline, 256, "CS");
						PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
						retVal = PAL_releaseMutex(ptr_man);
						break;
					case MUTEX_TYPE:
						addStrToOutput(outline, 256, "mutex");
						PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
						retVal = PAL_releaseMutex(ptr_man);
						break;
					case SEM_TYPE:
						addStrToOutput(outline, 256, "semaphore");
						PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
						retVal = PAL_incSemaphore(ptr_man, 1, NULL);
						break;					
					case MANUAL_EVENT_TYPE:
						//
						//no need to do anything here as this 
						//is an event and waitforevent logic updates
						//the semaphore accordingly.
						//
						addStrToOutput(outline, 256, "manual");
						PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
//						retVal = PAL_incSemaphore(ptr_man, time, NULL);
						break;
					case AUTO_EVENT_TYPE:
						addStrToOutput(outline, 256, "auto");
						PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
//						retVal = PAL_incSemaphore(ptr_man, time, NULL);
						if (PAL_setEvent(ptr_man))
							retVal = WAIT_FAILED;
						else
							retVal = WAIT_OBJECT_0;
						break;
					case THREAD_TYPE:
						addStrToOutput(outline, 256, "thread");
						PAL_PRINT_LOG(PAL_ERR_LOG, outline);
						break;
					default:
						addStrToOutput(outline, 256, "Waiting on Invalid type:");
						addDWORDToOutput(outline, 256, ptr_type);
						PAL_PRINT_LOG(PAL_ERR_LOG, outline);
						break;
				}//switch
			}//if ptr_man != NULL
		}//for
	}//if TRUE == b_wait_all	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////
//PAL_waitForMultipleObjs
//
//@param[in] n_cnt Count of the handles the thread intends to wait on
//@param[in] hndls Pointers to hanldes that the thread intends to wait on
//@param[in] b_wait_all wheather the thread intends to wait for all the handle
//           or just the first one that is available.
//@param[in] dw_ms NULL or INFINITE later specifies wait until a handle is
//           available
//
//@return WAIT_OBJECT_0+active handle, WAIT_FAILED on error, WAIT_TIMEOUT on
//        time out or n_cnt if b_wait_all is set and all handles are
//        available.
//
DWORD PAL_absclass::PAL_waitForMultipleObjs(
	DWORD n_cnt,
	const HANDLE *hndls,
	BOOL b_wait_all,
	DWORD dw_ms )
{
	if (( n_cnt <= 0 ) || ( !hndls ))
	{
		errno = -EINVAL;
		return WAIT_FAILED;
	}
	else if ( 1 == n_cnt )
	{
		PAL_PtrMan *ptr;

		if ( !(ptr = static_cast<PAL_PtrMan*>( hndls[0] )) )
		{
			errno = -EINVAL;
			return WAIT_FAILED;
		}

		return PAL_waitForSingleObj( ptr, dw_ms );
	}

	char outline[256];

	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	//
	//retrieve all fds from the handles and save them in 
	//inactive fd list.
	//
	std::vector<int32_t> tmp_act_lst;
	tmp_act_lst.reserve( n_cnt );

	std::vector<int32_t> tmp_inact_lst;
	tmp_inact_lst.reserve( n_cnt );

	if ( WAIT_FAILED == PAL_initMultiObj( hndls, n_cnt, tmp_inact_lst ) )
		return WAIT_FAILED;

	DWORD fds_in_set( tmp_inact_lst.size() );

	for ( ;; )
	{
		//
		//update the fd set based on the dynamic array.
		//
		fd_set rd_fds;
		int32_t max_fd( 0 );

		if ( WAIT_FAILED == PAL_initfdSet( tmp_inact_lst, max_fd, rd_fds ) )
			return WAIT_FAILED;

		//
		//setup timeout value
		//
		struct timeval time_out;
		struct timeval *t_out( NULL );

		if ( INFINITE == dw_ms )
			sprintf( outline, "PAL_waitForMultipleObjs(%u, %p, %s, INF):(fd):", n_cnt, hndls, (b_wait_all ? "ALL" : "SOME") );
		else
		{
			sprintf( outline, "PAL_waitForMultipleObjs(%u, %p, %s, %u):(fd):", n_cnt, hndls, (b_wait_all ? "ALL" : "SOME"), dw_ms );

			//
			//we want to make sure we try to accomdate the time in milisec
			//the best we can.  If the number passed in is too large we
			//we divide and turn it into seconds instead of multiplying.
			//
			memset( &time_out, 0, sizeof(time_out) );
			time_out.tv_sec = dw_ms / 1000;
			time_out.tv_usec = 1000000 * (dw_ms % 1000);

			t_out = &time_out;
		}

		//
		//this event is set by the thread which has locked the
		//select_mutx.  Prevents threads waiting on the same
		//handle as the locking thread from spinning.
		//
		//.        PAL_waitForEventObj(sel_ev, INFINITE, PAL_EV_CALLER_WAIT);//Do we really needs this the logic is not good!!!
		DWORD active_fds( select( max_fd+1, &rd_fds, NULL, NULL, t_out ) );

	//
	//we got here either because of error
	//or because one or more fd_set was activated
	//or because a timeout occured. We'll try to
	//acquire mutex before any further processing
	//to allow serialization between threads,
	//as select releases all threads at the same time.
	//This way only one thread would check status of
	//it's handles and all the rest would return to
	//the select statement, once the first thread is
	//finished it would release the mutex and any
	//thread waiting on a handle other than the ones
	//already acquired by the first thread would now
	//be able to acquire mutex and continue their
	//processing.
	//

	//
	//try to lock the mutex(trylock)
	//************************************************888
	//####################
	//switch(PAL_waitForMutexObj(select_mutx, 0, NULL)) SHOULD BE MUTEX NOT CRIT_SEC
	//.        PAL_enterCriticalSection(select_mutx);

	//
	//we have not cleared the fd of the handle that caused
	//us to get here, resetting this event would cause any
	//other thread waiting for the select_mutx to get blocked
	//without reaching select call until we are done. Thus no
	//spinning.
	//
	//.       PAL_resetEvent(sel_ev);
	//
	//for all synch object that this
	//thread is waiting on
	//
	switch(active_fds)
	{
		//
		//error value returned
		//
		case(-1):
		{
			//.                    //PAL_releaseMutex(select_mutx);
			//.                    PAL_leaveCriticalSection(select_mutx);
			//.                    PAL_setEvent(sel_ev);
			if (-EINTR == errno)
				continue;
			else
				return WAIT_FAILED;
		}//case(-1)

		//
		//no active fds
		//
		case(0):
		{
			//PAL_releaseMutex(select_mutx);
			//.                 PAL_leaveCriticalSection(select_mutx);
			//.                 PAL_setEvent(sel_ev);
			//
			//did we specify timeout?
			//
			//  if (NULL == t_out)
			/*
			// Randy QMX DEMO
			// modified to return TIMEOUT... time_out may or may not be modified... POSIX.1-2001 permits either
			// behavior... regardless we cannot base logic on what only sometimes happens.
			if (0 == time_out.tv_usec)
			{
				//
				//no timeout specified.  Something
				//is wrong.
				//
				errno = -EIO;
				return WAIT_FAILED;
			}
			//it may be better to return timeout under "default case" see msdn
			else
	    */
			return WAIT_TIMEOUT;
		}//case (0)

		default:
			DWORD val_to_ret;
			bool ret_val = PAL_processSelectRetVal(
					rd_fds,
					hndls,
					max_fd,
					fds_in_set,
					b_wait_all,
					val_to_ret,
					tmp_act_lst,
					tmp_inact_lst);
			//PAL_releaseMutex(select_mutx);
			//.                PAL_leaveCriticalSection(select_mutx);
			//.                PAL_setEvent(sel_ev);
			if (TRUE == ret_val)
				return val_to_ret;
			else if (WAIT_FAILED == val_to_ret)
				return val_to_ret;
	}// switch
	//
	//we failed to satisfy our need.
	//release the mutex and proceed to
	//select statement.  If reach here
	//and wait_all is not set then we
	//have a problem.  case(0) continue
	//prevents us from reaching here in
	//valid cases.
	//
	//        if (TRUE != b_wait_all)
	//       {
	//          errno = -EINVAL;
	//         return WAIT_FAILED;
	//    }
	//
		//allow the thread that's got mutex to
		//finish processing. Avoid others from
		//spinning
		//
		sleep(1);
	}//while forever
}
#endif	//USING_SELECT_FOR_WAITING

//***************************************************************/
// PAL_createMutex(
HANDLE PAL_absclass::PAL_createMutex(
	PAL_PMUTEX_ATTRIB UNUSED(attrib),
	BOOL UNUSED(owner),
	LPCTSTR mutex_name)
{
	char outline[256];
	sprintf( outline, "PAL_createMutex:(%ls)", mutex_name );

	PAL_ERRNO_TYPE l_errc=PAL_SUCCESS;

	// Uses semaphore based events if named mutex is required
	if (( NULL != mutex_name ) && ( 0 != PAL_StrLen( mutex_name ) ) )
		return ( PAL_createEvent( mutex_name, FALSE, TRUE, NULL ) );

	// Use posix mutex interface if named mutex is not required.
	PAL_PtrMan *ptr_man = new (PAL_PtrMan);
	if ( NULL == ptr_man )
	{
		PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for handle (PAL_PtrMan)");
		return NULL;
	}

	PAL_PMUTEX  mutex = new PAL_MUTEX;
	if ( NULL == mutex )
	{
		PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for mutex. Delete ptr_man.");

		delete(ptr_man);
		ptr_man = NULL;

		return NULL;
	}

	// Initialize the mutex and create it.
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );

	// FT-634: switch mutex to recursive type so OS can handle locking on same thread instead of
	// using ptr_man and handling it ourselves (which has timing issues)...
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );

	l_errc = pthread_mutex_init( mutex, &attr );
	if ( l_errc != PAL_SUCCESS )
	{
		PAL_ErrorSave(l_errc, " Unable to initialize mutex. Delete ptr_man, delete mutex.");

		delete(ptr_man);
		ptr_man = NULL;

		delete(mutex);
		mutex = NULL;

		return NULL;
	}

	ptr_man->PAL_setHndl( (HANDLE) mutex );
	ptr_man->PAL_setType( MUTEX_TYPE );

#ifdef USING_SELECT_FOR_WAITING
	ptr_man->PAL_initfd();
	ptr_man->PAL_touchfd();
#endif

	pthread_mutexattr_destroy( &attr );

	addPtrToOutput( outline, 256, ptr_man );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	return ((HANDLE) ptr_man);
}

//***************************************************************/
// PAL_releaseMutex(HANDLE hndl)
BOOL PAL_absclass::PAL_releaseMutex( HANDLE hndl )
{
	static const char *err_str( "PAL_releaseMutex: " );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, err_str );

	PAL_ERRNO_TYPE l_errc = PAL_SUCCESS;

	// check for valid passed handle
	if (( NULL == hndl ) || ( NULL == static_cast<PAL_PtrMan *> (hndl)->PAL_getHndl() ) )
	{
		PAL_ErrorSave( PAL_INVALID_VAL, " Invalid handle passed." );
		return FALSE;
	}

	PAL_PtrMan *e_hndl = static_cast <PAL_PtrMan *> (hndl);

	if (AUTO_EVENT_TYPE == e_hndl->PAL_getType())
		return (PAL_setEvent(e_hndl));

	PAL_PMUTEX mutex = static_cast <PAL_PMUTEX> (e_hndl->PAL_getHndl());

	l_errc = pthread_mutex_unlock(mutex);

	// handle errors
	if ( PAL_PTHREAD_SUCCESS != l_errc )
	{
		PAL_ErrorSave( l_errc, " Unable to unlock mutex.");
		return FALSE;
	}

#ifdef USING_SELECT_FOR_WAITING
	e_hndl->PAL_touchfd();
#endif

	return TRUE;
}

//***************************************************************/
// pal_waitForMutexObj(HANDLE hndl, DWORD time, PAL_PMUTEX inMutex)
DWORD PAL_absclass::pal_waitForMutexObj( HANDLE hndl, DWORD time, PAL_PMUTEX inMutex )
{
	DWORD retVal = WAIT_FAILED;
	PAL_PMUTEX mutx = NULL;
	struct timespec abs_time;

	char outline[256];
	sprintf( outline, "pal_waitForMutexObj(%p, %u, %p):", hndl, time, inMutex );

	// use handle or pointer to mutex?
	if ( NULL != hndl )
	{
		PAL_PtrMan *handle = static_cast <PAL_PtrMan *> (hndl);
		mutx = static_cast<PAL_PMUTEX> (handle->PAL_getHndl());
	}
	else if ( NULL != inMutex)
	{
		mutx = inMutex;
	}

	// make sure we have a valid mutex
	if ( NULL == mutx )
	{
		addStrToOutput( outline, 256, "No valid mutex or handle given" );
	}
	else
	{
		// functionality based on passed time value
		if ( 0 == time )
		{
			// do not wait at all
			switch(pthread_mutex_trylock(mutx))
			{
			case 0:
				// success
				addStrToOutput(outline, 256, "success");
				retVal = WAIT_OBJECT_0;
				break;
			case EBUSY:
				// mutex locked
				addStrToOutput(outline, 256, "locked");
				retVal = WAIT_TIMEOUT;
				break;
			case EINVAL:
				// failure
				addStrToOutput(outline, 256, "invalid mutex");
				retVal = WAIT_FAILED;
				break;
			}
		}
		else if (INFINITE == time)
		{
			// blast over the last outline, faster than setting up
			// a whole slew of if clasues
			sprintf( outline, "pal_waitForMutexObj(%p, INF, %p):", hndl, mutx );

			// wait forever
			switch(pthread_mutex_lock(mutx))
			{
			case 0:
				// success
				addStrToOutput(outline, 256, "success");
				retVal = WAIT_OBJECT_0;
				break;
			case EINVAL:
				// failure
				addStrToOutput(outline, 256, "invalid mutex");
				retVal = WAIT_FAILED;
				break;
			case EDEADLK:
				// this thread already has the mutex
				addStrToOutput(outline, 256, "deadlocked mutex");
				retVal = WAIT_FAILED;
				break;
			}
		}
		else
		{
			// milliseconds to wait is given
			clock_gettime(CLOCK_REALTIME, &abs_time);
			timespec_add_ms(&abs_time, time);

			switch(pthread_mutex_timedlock(mutx, &abs_time))
			{
			case 0:
				// success
				addStrToOutput(outline, 256, "success");
				retVal = WAIT_OBJECT_0;
				break;
			case ETIMEDOUT:
				// the timer expired
				addStrToOutput(outline, 256, "timeout");
				retVal = WAIT_TIMEOUT;
				break;
			case EINVAL:
				// failure
				addStrToOutput(outline, 256, "invalid mutex");
				retVal = WAIT_FAILED;
				break;
			case EDEADLK:
				// this thread already has the mutex
				PAL_ErrorSave( PAL_INVALID_VAL, "invalid mutex");
				retVal = WAIT_FAILED;
				break;
			}
        }
    }

	if (retVal == WAIT_FAILED)
	{
		PAL_PRINT_LOG( PAL_ERR_LOG, outline );
	}
	else
	{
#ifdef USING_SELECT_FOR_WAITING
		if ((NULL != hndl) && (WAIT_OBJECT_0 == retVal))
			static_cast <PAL_PtrMan *> (hndl)->PAL_clearfd();
#endif
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
	}

	return retVal;
}

//////////////////////////////////////////////////////////////////////
//PAL_waitForEventObj
DWORD PAL_absclass::PAL_waitForEventObj(HANDLE hndl, DWORD time, PAL_EV_CALLER_TYPE type)
{
  char outline[256];
  sprintf(outline, "pal_waitForEventObj(%p, 0x%x)", hndl, time);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  struct timespec abs_time;

  PAL_PtrMan *handle = static_cast <PAL_PtrMan *> (hndl);
  PAL_PSYNC_DAT sem_dat = static_cast<PAL_PSYNC_DAT> (handle->PAL_getHndl());
  PAL_TYPE_OF_PTR ptr_type = handle->PAL_getType();

  // have to set the timeout before the while in case we're continued
  clock_gettime(CLOCK_REALTIME, &abs_time);
  timespec_add_ms(&abs_time, time);

  PAL_enterCriticalSection(sem_dat->mutex);

  while(1)
    {
      int ret;

      if (PAL_EV_CALLER_WAIT == type)
        {
	  if (PAL_SyncObj::NON_SIGNALED == sem_dat->state)
            {
	      sem_dat->thrd_cnt++;
	      switch(time)
                {
		case 0:
		  ret = sem_trywait(sem_dat->sem);
		  break;
		case INFINITE:
		  PAL_leaveCriticalSection(sem_dat->mutex);
		  ret = sem_wait(sem_dat->sem);
		  PAL_enterCriticalSection(sem_dat->mutex);
		  break;
		default:
		  PAL_leaveCriticalSection(sem_dat->mutex);
		  ret = sem_timedwait(sem_dat->sem, &abs_time);
		  PAL_enterCriticalSection(sem_dat->mutex);
		  break;
                }
	      sem_dat->thrd_cnt--;
            }
	  else
            {
	      PAL_leaveCriticalSection(sem_dat->mutex);
	      return WAIT_OBJECT_0;
            }

        }
      else
        {
	  switch(time)
            {
	    case 0:
	      ret = sem_trywait(sem_dat->sem);
	      break;
	    case INFINITE:
	      ret = sem_wait(sem_dat->sem);
	      break;
	    default:
	      ret = sem_timedwait(sem_dat->sem, &abs_time);
	      break;
            }
        }

      if (PAL_PTHREAD_SUCCESS == ret)
        {
	  //
	  //we never want to go to zero count on the semaphore 
	  //by simply waiting.  Events dont work this way.  We 
	  //get around this problem by posting as soon as we get 
	  //done. 
	  //
	  if ((PAL_EV_CALLER_WAIT == type) && (MANUAL_EVENT_TYPE == ptr_type))
	    PAL_ErrorChk((sem_post(sem_dat->sem)), errno, 
			 "PAL_waitForEventObj: sem_post() failed.");

	  //
	  //clear the signaled state for auto events
	  //
	  int32_t curr_cnt=0;

	  if (0 > sem_getvalue(sem_dat->sem, &curr_cnt))
            {
	      PAL_leaveCriticalSection(sem_dat->mutex);
	      return WAIT_FAILED;
            }
#ifdef USING_SELECT_FOR_WAITING
	  if ((AUTO_EVENT_TYPE == ptr_type) || (PAL_EV_CALLER_RESET == type))
	    handle->PAL_clearfd();
#endif
	  if (0 >= curr_cnt)
	    sem_dat->state = PAL_SyncObj::NON_SIGNALED;

	  PAL_leaveCriticalSection(sem_dat->mutex);

	  return WAIT_OBJECT_0;
        }

      switch(errno)
        {
	case(EINTR):
	  // we got interrupted - continue on
	  continue;
	case(EAGAIN):
	  // only for sem_trywait
	  // sem_trywait() returns immediately, EAGAIN is the case that it can not decrease semaphore.
	  //return WAIT_FAILED;
	  PAL_leaveCriticalSection(sem_dat->mutex);
	  return WAIT_TIMEOUT;
	case(ETIMEDOUT):
	  // only for sem_timedwait
	  PAL_leaveCriticalSection(sem_dat->mutex);
	  return WAIT_TIMEOUT;
	default:
	  PAL_leaveCriticalSection(sem_dat->mutex);
	  PAL_PRINT_LOG(PAL_ERR_LOG, "sem_XXXwait failed");
	  return WAIT_FAILED;
        }
    }       //while forever

  //
  //we should never get here
  //
  PAL_leaveCriticalSection(sem_dat->mutex);
  return WAIT_FAILED;
}

//////////////////////////////////////////////////////////////////////
//pal_waitForSemaphoreObj
DWORD PAL_absclass::pal_waitForSemaphoreObj(HANDLE hndl, DWORD time)
{
  char outline[256];
  sprintf(outline, "pal_waitForSemaphoreObj(%p, 0x%x)", hndl, time);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  struct timespec abs_time;

  PAL_PtrMan *handle = static_cast <PAL_PtrMan *> (hndl);
  PAL_PSYNC_DAT sem_dat = static_cast<PAL_PSYNC_DAT> (handle->PAL_getHndl());

  // have to set the timeout before the while in case we're continued
  clock_gettime(CLOCK_REALTIME, &abs_time);
  timespec_add_ms(&abs_time, time);

  while(1)
    {
      // named semaphores
      int32_t ret;

      switch(time)
        {
	case 0:
	  ret = sem_trywait(sem_dat->sem);
	  break;
	case INFINITE:
	  ret = sem_wait(sem_dat->sem);
	  break;
	default:
	  ret = sem_timedwait(sem_dat->sem, &abs_time);
	  break;
        }

      //
      // decrement the use count regardless of success or failure
      // as we are not waiting for the sem any more in either
      // condition.
      //
      if (PAL_PTHREAD_SUCCESS == ret)
        {
#ifdef USING_SELECT_FOR_WAITING
	  handle->PAL_clearfd();
#endif
	  return WAIT_OBJECT_0;
        }

      switch(errno)
        {
	case(EINTR):
	  // we got interrupted - continue on
	  continue;
	case(EAGAIN):
	  // only for sem_trywait
	  // sem_trywait() returns immediately, EAGAIN is the case that it can not decrease semaphore.
	  return WAIT_TIMEOUT;
	case(ETIMEDOUT):
	  // only for sem_timedwait
	  return WAIT_TIMEOUT;
	default:
	  PAL_PRINT_LOG(PAL_ERR_LOG, "sem_XXXwait failed");
	  return WAIT_FAILED;
        }
    } //while forever
  //
  // we should never get here.
  //
  return WAIT_FAILED;
}

//***************************************************************/
// PAL_waitForSingleObj(HANDLE hndl, DWORD time)
DWORD PAL_absclass::PAL_waitForSingleObj(HANDLE hndl, DWORD time)
{
	DWORD retVal( WAIT_FAILED );

	char outline[256];
	if ( INFINITE == time )
		sprintf(outline, "PAL_waitForSingleObj(%p, INF):", hndl);
	else
		sprintf(outline, "PAL_waitForSingleObj(%p, %u):", hndl, time);

	if (( NULL == hndl ) || ( INVALID_HANDLE_VALUE == hndl ))
	{
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		PAL_ErrorSave( PAL_INVALID_VAL, "Invalid handle passed");
		return retVal;
	}

	PAL_PtrMan *handle( static_cast<PAL_PtrMan*>( hndl ) );

	// call the handler for the different types
	const PAL_TYPE_OF_PTR ptr_type( handle->PAL_getType() );

	switch ( ptr_type )
	{
	case CS_TYPE:
		addStrToOutput(outline, 256, "CS");
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		retVal = pal_waitForMutexObj(hndl, time, NULL);
		break;
	case MUTEX_TYPE:
		addStrToOutput(outline, 256, "mutex");
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		retVal = pal_waitForMutexObj(hndl, time, NULL);
		break;
	case SEM_TYPE:
		addStrToOutput(outline, 256, "semaphore");
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		retVal = pal_waitForSemaphoreObj(hndl, time);
		break;
	case MANUAL_EVENT_TYPE:
		addStrToOutput(outline, 256, "manual");
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		retVal = PAL_waitForEventObj(hndl, time, PAL_EV_CALLER_WAIT);
		break;
	case AUTO_EVENT_TYPE:
		addStrToOutput(outline, 256, "auto");
		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
		retVal = PAL_waitForEventObj(hndl, time, PAL_EV_CALLER_WAIT);
		break;
	case THREAD_TYPE:
		addStrToOutput(outline, 256, "thread");
		PAL_PRINT_LOG(PAL_ERR_LOG, outline);

		// TODO: Gotta do something there $$TRS
		// probably can pthread_join but we'll need
		// timeout handlers and such
		if ( INFINITE == time )
		{
			PAL_PTHREAD t_id( static_cast<PAL_PTHREAD>( handle->PAL_getHndl() ) );
			if ( pthread_join( *t_id, NULL ) == 0 )
				retVal = WAIT_OBJECT_0;
		}

		break;
	default:
		addStrToOutput(outline, 256, "Waiting on Invalid type:");
		addDWORDToOutput(outline, 256, ptr_type);
		PAL_PRINT_LOG(PAL_ERR_LOG, outline);
		break;
	}

	if (retVal != WAIT_FAILED)
	{
		if ( INFINITE == time )
			sprintf(outline, "PAL_waitForSingleObj(%p, INF):%d", hndl, retVal);
		else
			sprintf(outline, "PAL_waitForSingleObj(%p, %u):%d", hndl, time, retVal);

		PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
	}

	return retVal;
}

//***************************************************************/
// PAL_initializeCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
void PAL_API PAL_absclass::PAL_initializeCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection )
{
	char outline[256];
	sprintf( outline, "PAL_initializeCriticalSection(%p):", lpCriticalSection );

	PAL_ERRNO_TYPE l_errc = PAL_SUCCESS;

	// Use posix mutex interface
	PAL_PtrMan *ptr_man = new (PAL_PtrMan);
	if ( NULL == ptr_man )
	{
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for handle (PAL_PtrMan). lpCriticalSection = NULL");

		lpCriticalSection = NULL;
		return;
	}

	PAL_PMUTEX mutex = new PAL_MUTEX;
	if ( NULL == mutex )
	{
		PAL_ErrorSave(PAL_OUT_OF_MEM, " Unable to allocate memory for mutex. Delete ptr_man. lpCriticalSection = NULL");

		delete(ptr_man);
		ptr_man = NULL;

		lpCriticalSection = NULL;
		return;
	}

	// Initialize the mutex and create it.
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );

	// FT-634: switch mutex to recursive type so OS can handle locking on same thread instead of
	// using ptr_man and handling it ourselves (which has timing issues)...
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );

	l_errc = pthread_mutex_init( mutex, &attr );
	if ( l_errc != PAL_SUCCESS )
	{
		PAL_ErrorSave(l_errc, " Unable to initialize mutex. Delete ptr_man, delete mutex, lpCriticalSection = NULL.");

		delete(ptr_man);
		ptr_man = NULL;

		delete(mutex);
		mutex = NULL;

		lpCriticalSection = NULL;
		return;
	}

	ptr_man->PAL_setHndl( (HANDLE) mutex );
	ptr_man->PAL_setType( CS_TYPE );
	ptr_man->PAL_setID( 0 );

	pthread_mutexattr_destroy( &attr );

	addPtrToOutput( outline, 256, ptr_man );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	*(HANDLE *)lpCriticalSection = (HANDLE) ptr_man;
}

//***************************************************************/
// PAL_enterCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
void PAL_API PAL_absclass::PAL_enterCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection )
{
	char outline[256];
	sprintf( outline, "PAL_enterCriticalSection(%p):", lpCriticalSection );

	HANDLE hndl = static_cast<HANDLE> (lpCriticalSection);

	// check for valid passed handle
	if (( NULL == hndl ) || (NULL == static_cast<PAL_PtrMan *> (hndl)->PAL_getHndl()))
	{
		PAL_ErrorSave( PAL_INVALID_VAL, " Invalid handle passed." );
		return;
	}

	PAL_PtrMan *e_hndl = static_cast <PAL_PtrMan *> (hndl);
	PAL_PMUTEX mutx = static_cast<PAL_PMUTEX> (e_hndl->PAL_getHndl());

	addDWORDToOutput( outline, 256, e_hndl->PAL_getCSCount() );

	DWORD ret = pthread_mutex_trylock( mutx );

	// FT-634: switch mutex to recursive type so OS can handle locking on same thread instead of
	// using ptr_man and handling it ourselves (which has timing issues)...
	switch ( ret )
	{
    case EBUSY:
    	// if we can't and we're not the same thread, then wait
		addStrToOutput( outline, 256, "-diff thread req." );
		PAL_PRINT_LOG( PAL_NO_LOG, outline );

		if ( 0 != pthread_mutex_lock( mutx ) )
		{
			addStrToOutput( outline, 256, "-Error locking" );
			PAL_PRINT_LOG( PAL_NO_LOG, outline );
			break;
		}

		// fall through to next case!

    case 0:
		// we got the CS and we're first
		e_hndl->PAL_setID( pthread_self() );
		e_hndl->PAL_incCSCount();

		addStrToOutput( outline, 256, "-first thread req." );
		PAL_PRINT_LOG( PAL_NO_LOG, outline );
		break;

    default:
		addStrToOutput( outline, 256, "-Error" );
		PAL_PRINT_LOG(PAL_ERR_LOG, outline);
		break;
    }
}

//***************************************************************/
// PAL_leaveCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
void PAL_API PAL_absclass::PAL_leaveCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection )
{
	char outline[256];
	sprintf( outline, "PAL_leaveCriticalSection(%p):", lpCriticalSection );

	PAL_ERRNO_TYPE l_errc=PAL_SUCCESS;
	HANDLE hndl = static_cast<HANDLE> (lpCriticalSection);

	// check for valid passed handle
	if (( NULL == hndl ) || (NULL == static_cast<PAL_PtrMan *> (hndl)->PAL_getHndl()))
	{
		PAL_ErrorSave( PAL_INVALID_VAL, " Invalid handle passed.");
		return;
	}

	PAL_PtrMan *e_hndl = static_cast <PAL_PtrMan *> (hndl);
	PAL_PMUTEX mutex = static_cast <PAL_PMUTEX> (e_hndl->PAL_getHndl());

	e_hndl->PAL_decCSCount();

	addDWORDToOutput( outline, 256, e_hndl->PAL_getCSCount() );

	// FT-634: switch mutex to recursive type so OS can handle locking on same thread instead of
	// using ptr_man and handling it ourselves (which has timing issues)...
	l_errc = pthread_mutex_unlock( mutex );

	// handle errors
	if ( PAL_PTHREAD_SUCCESS != l_errc )
		PAL_ErrorSave( l_errc, " Unable to unlock mutex." );

	PAL_PRINT_LOG( PAL_NO_LOG, outline );
	return;
}

//***************************************************************/
// PAL_deleteCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection)
void PAL_API PAL_absclass::PAL_deleteCriticalSection( PAL_LPCRITICAL_SECTION lpCriticalSection )
{
	char outline[256];
	sprintf( outline, "PAL_deleteCriticalSection(%p):", lpCriticalSection );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, outline );

	//
	//we may delete CS while someone
	//is using it. But MS docs leave this
	//situation undefined.
	//
	PAL_absclass::PAL_closeHandle( static_cast<HANDLE> (lpCriticalSection) );
}

//***************************************************************/
// PAL_createSemaphore
HANDLE PAL_absclass::PAL_createSemaphore(
	LPCTSTR name,
	LONG init_cnt,
	LONG max_cnt,
	PAL_PSEM_ATTRIB UNUSED(unused_attrib) )
{
	char outline[256];
	sprintf(outline, "PAL_createSemaphore(%ls, %ld, %ld):",
			name, init_cnt, max_cnt);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	if ((init_cnt > max_cnt) || (0 == max_cnt))
	{
		PAL_ErrorSave( PAL_INVALID_VAL, "Invalid incremental count specified.");
		return NULL;
	}

	PAL_PtrMan *ptr_man = new (PAL_PtrMan);
	if (PAL_ErrorChk((NULL == ptr_man), PAL_OUT_OF_MEM, "Unable to allocate memory for handle."))
		return NULL;

	//
	//allocate memory for the semaphore and init it
	//
	PAL_PSYNC_DAT sem_dat = new PAL_SYNC_DAT;
	if(PAL_ErrorChk((NULL == sem_dat), PAL_OUT_OF_MEM, "Unable to allocate memory for semaphore."))
	{
		delete(ptr_man);
		ptr_man = NULL;

		return NULL;
	}

	// set the max value for the semaphore
	sem_dat->max_cnt = max_cnt;

	//
	//make sure the name string is not empty
	//we need to create a non-named sem if so
	//
	if ((NULL != name) && (0 != PAL_StrLen(name)))
	{
		sem_dat->bIsNamed = TRUE;

		//
		// we need '/' infront of the name
		//
		// **************warning**************************
		// no wide char API for semaphore available, using
		// char instead of TCHAR
		// ***********************************************

		unsigned int i=0;
		char *tmp_l_name;
		tmp_l_name = sem_dat->sem_name = (char *)calloc(1, NAME_MAX-4);

		if ('/' != name[0])
		{
			sem_dat->sem_name[0]='/';
			tmp_l_name = &sem_dat->sem_name[1];
		}

#ifdef UNICODE
		// convert wide chars to char
		for(;i<wcsnlen(name, NAME_MAX-1);i++)
			tmp_l_name[i] = char(name[i]);
#endif

		sem_dat->sem = sem_open(
			sem_dat->sem_name,
			O_CREAT|O_EXCL, //S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH,
			0666,
			init_cnt);

		if (PAL_SEM_OPEN_FAILED == sem_dat->sem)
		{
			// errno is thread safe in Linux so this is ok
			switch(errno)
			{
			//
			//if the sem exists, attempt to open it
			//
			case EEXIST:
				addStrToOutput(outline, 256, "Open existing");
				PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

				sem_dat->sem = sem_open(sem_dat->sem_name, PAL_OPEN_EXISTING_SEM);
				if (PAL_ErrorChk(
						(PAL_SEM_OPEN_FAILED == sem_dat->sem),
						errno,
						"PAL_createSemaphore: sem_open (existing) failed. Delete ptr_man, delete sem_dat."))
				{
					delete(ptr_man);
					ptr_man = NULL;

					delete(sem_dat);
					sem_dat = NULL;
				}

				break;

			default:
				PAL_ErrorChk(true, errno, "PAL_createSemaphore: sem_open failed. Delete ptr_man, delete sem_dat.");

				delete(ptr_man);
				ptr_man = NULL;

				delete(sem_dat);
				sem_dat = NULL;

				break;
            }
        }
	}//if named sem required

	else
	{
        sem_dat->bIsNamed = FALSE;
		sem_dat->sem = new(PAL_SEM);

		if(PAL_ErrorChk(
				(NULL == sem_dat->sem),
				PAL_OUT_OF_MEM,
				"Unable to allocate memory for semaphore. Delete ptr_man, delete sem_dat."))
		{
			delete(ptr_man);
			ptr_man = NULL;

			delete(sem_dat);
			sem_dat = NULL;
		}
		else if (PAL_ErrorChk(
				sem_init(sem_dat->sem, PAL_INTER_PROCESS_SEM, init_cnt),
				errno,
				"Unable to initialize semaphore. Delete ptr_man, delete sem_dat."))
		{
			delete(ptr_man);
			ptr_man = NULL;

			delete(sem_dat);
			sem_dat = NULL;
		}
    }

	if (ptr_man)
	{
		ptr_man->PAL_setHndl(sem_dat);
		ptr_man->PAL_setType(SEM_TYPE);

#ifdef USING_SELECT_FOR_WAITING
		//
		//write as many bytes as the init count
		//so that appropriate number of threads
		//can proceed without blocking
		//
		ptr_man->PAL_initfd();
		for (LONG l_cnt=0;l_cnt<init_cnt;l_cnt++)
			ptr_man->PAL_touchfd();
#endif
	}

	sprintf(outline, "PAL_createSemaphore(%ls, %ld, %ld):%p",
			name, init_cnt, max_cnt, ptr_man);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	return((HANDLE) ptr_man);
}

//***************************************************************/
// PAL_incSemaphore(HANDLE hndl, LONG cnt, LPLONG prev_cnt)
BOOL PAL_absclass::PAL_incSemaphore(HANDLE hndl, LONG cnt, LPLONG prev_cnt)
{
  char outline[256];
  sprintf(outline, "PAL_incSemaphore(%p, %ld, %p)", hndl, cnt, prev_cnt);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  if (PAL_ErrorChk((NULL == hndl) || (NULL == (static_cast <PAL_PSYNC_DAT> (hndl))->sem),
		   PAL_INVALID_VAL,
		   "Invalid parameter passed."))
    return FALSE;
  //
  //retrieve the current value. Save it if required
  //and make sure we do not exceed the max value.
  //
  int old_cnt;
  PAL_PtrMan *e_hndl = static_cast <PAL_PtrMan *> (hndl);
  PAL_PSYNC_DAT sem_dat = static_cast<PAL_PSYNC_DAT> (e_hndl->PAL_getHndl());

  sem_getvalue(sem_dat->sem, &old_cnt);

  if (NULL != prev_cnt)
    *prev_cnt = old_cnt;

  if (cnt > (LONG)sem_dat->max_cnt)
    cnt = sem_dat->max_cnt;
  //
  //windows allows caller to specify increment count
  //we achieve this in linux by posting in a loop
  //
  for(int32_t i=0;i<cnt;i++)
    {
      if (PAL_ErrorChk((sem_post(sem_dat->sem)),
		       errno,
		       "PAL_incSemaphore: sem_post() failed."))
	return FALSE;
#ifdef USING_SELECT_FOR_WAITING
      e_hndl->PAL_touchfd();
#endif
    }
  return TRUE;
}

//***************************************************************/
// PAL_createEvent(
HANDLE PAL_absclass::PAL_createEvent(
				     LPCTSTR l_name,
				     BOOL reset_type,
				     BOOL init_state,
				     PAL_PEVENT_ATTRIB UNUSED(attrib))
{
  static const char *err_str("PAL_createEvent:");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  //
  //we need both semaphore and mutex to emulate
  //manual event.  Semaphore would serve as the
  //event whereas mutex would protect the thread
  //count.  Thread cout is used to inc/post semphore
  //appropriate number of times in order to release
  //only the threads which were already suspended
  //at the time event was signaled/set.
  //

  PAL_PtrMan *e_hndl;
  int sem_state = 0;  //signaled or not
  int sem_max = 1;    //auto or manual

  if (TRUE == init_state)
    {
      if (FALSE == reset_type)
	sem_state = 1;
      else
        {
	  sem_state = PAL_SEM_MAX_VAL;
	  sem_max = PAL_SEM_MAX_VAL;
        }
    }
  else
    sem_max = PAL_SEM_MAX_VAL;

  e_hndl = static_cast<PAL_PtrMan *>
    (PAL_createSemaphore(l_name,    //name if used.
			 sem_state, //zero or max number of threads possible.
			 sem_max,   //limit set to max thread count possible
			 NULL));

  if (NULL == e_hndl) {
	  Debug::trace("PALThread", "PAL_createSemaphore() Failed." );
	  return NULL;                    //error code/str saved by
  }
  PAL_PSYNC_DAT e_sem = static_cast <PAL_PSYNC_DAT> (e_hndl->PAL_getHndl());

  PAL_initializeCriticalSection(&e_sem->mutex);
  e_sem->bHasMutex = TRUE;

  if (0 == sem_state)
    e_sem->state = PAL_SyncObj::NON_SIGNALED;
  else
    e_sem->state = PAL_SyncObj::SIGNALED;

  //
  //change the handle type to event typeL
  //
  if (FALSE == reset_type)
    e_hndl->PAL_setType(AUTO_EVENT_TYPE);
  else
    e_hndl->PAL_setType(MANUAL_EVENT_TYPE);

  return e_hndl;
}

//***************************************************************/
// PAL_setEvent(HANDLE hndl)
BOOL PAL_absclass::PAL_setEvent(HANDLE hndl)
{
	static const char *err_str( "PAL_setEvent:" );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, err_str );

	PAL_PtrMan *e_hndl( static_cast <PAL_PtrMan*>( hndl ) );

	if ( PAL_ErrorChk( (!e_hndl), PAL_INVALID_VAL, "PAL_releaseMutex: Invalid handle passed." ) )
		return FALSE;

	PAL_PSYNC_DAT sem( static_cast<PAL_PSYNC_DAT>( e_hndl->PAL_getHndl() ) );

	if ( PAL_ErrorChk( (!sem), PAL_INVALID_VAL, "PAL_releaseMutex: Bad handle value." ) )
		return FALSE;

	uint32_t inc_cnt( 1 ); //we assume auto event at this point

	PAL_enterCriticalSection( sem->mutex );

	//
	//if we are already in signaled state then
	//no processing is required.  Any attempt to
	//post/release semaphore would cause erroneous
	//behavior particularly in case of auto event.
	//
	if ( PAL_SyncObj::SIGNALED == sem->state )
	{
		PAL_leaveCriticalSection( sem->mutex );
		return TRUE;
	}

	//
	//Auto event only allows one thread to proceed
	//if no thread is waiting then the first one
	//initiating acquisition statisfies the condition
	//
	//manual event allows all threads already
	//waiting to be released.  This includes all
	//threads up until the event is reset.
	//
	if ( MANUAL_EVENT_TYPE == e_hndl->PAL_getType() )
		inc_cnt = PAL_SEM_MAX_VAL;

	for ( uint32_t i( 0 ); i < inc_cnt; i++ )
	{
		if ( PAL_ErrorChk( (sem_post(sem->sem)), errno, "PAL_setEvent: sem_post() failed." ) )
        {
			if ( 0 < i )
			{
				sem->state = PAL_SyncObj::SIGNALED;

#ifdef USING_SELECT_FOR_WAITING
				e_hndl->PAL_touchfd();
#endif
			}

			PAL_leaveCriticalSection( sem->mutex );
			return FALSE;
		}

#ifdef USING_SELECT_FOR_WAITING
		e_hndl->PAL_touchfd();
#endif
	}

	sem->state = PAL_SyncObj::SIGNALED;

	PAL_leaveCriticalSection( sem->mutex );
	return TRUE;
}

//***************************************************************/
// PAL_resetEvent(HANDLE hndl)
BOOL PAL_absclass::PAL_resetEvent(HANDLE hndl)
{
  static const char *err_str("PAL_resetEvent:");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  if ((NULL == hndl) ||
      (NULL == static_cast<PAL_PtrMan *> (hndl)->PAL_getHndl()))
    {
      PAL_ErrorSave(PAL_INVALID_VAL, "The handle passed is not valid");
      return FALSE;
    }

  PAL_PtrMan *e_hndl = static_cast<PAL_PtrMan *> (hndl);
  PAL_PSYNC_DAT sem = static_cast<PAL_PSYNC_DAT> (e_hndl->PAL_getHndl());

  PAL_enterCriticalSection(sem->mutex);
  //
  //if we are already in non-signaled state then
  //no processing is required.
  //
  if (PAL_SyncObj::NON_SIGNALED == sem->state)
    {
      PAL_leaveCriticalSection(sem->mutex);
      return TRUE;
    }
  //
  //if we are an auto event then we cann't really
  //do anything here
  //
  if (AUTO_EVENT_TYPE == sem->sync_type)
    {
      PAL_leaveCriticalSection(sem->mutex);
      PAL_absclass::PAL_ErrorSave(PAL_INVALID_VAL, "Attempt to reset auto event failed");
      return FALSE;
    }

  //
  //Timeout tells us that semaphore count is now
  //zero.
  //
  int32_t wait_ret=WAIT_OBJECT_0;
  do
    {
      //
      //loop until semaphore count = 0
      //
      wait_ret = PAL_waitForEventObj(e_hndl, 0, PAL_EV_CALLER_RESET);
    }
  while (WAIT_OBJECT_0 == wait_ret);


  sem->state = PAL_SyncObj::NON_SIGNALED;

  PAL_leaveCriticalSection(sem->mutex);

  return TRUE;    // ***** WARNING: are we already done (to return TRUE) ?
}

//***************************************************************/
// PAL_pulseEvent(HANDLE hndl)
BOOL PAL_absclass::PAL_pulseEvent(HANDLE hndl)
{
  static const char *err_str("PAL_pluseEvent:");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  if (PAL_ErrorChk( ((NULL == hndl) || (NULL == static_cast<PAL_PtrMan *> (hndl)->PAL_getHndl())),
		    PAL_INVALID_VAL,
		    "PAL_pulseMutex: Invalid handle passed."))
    return FALSE;

  PAL_PtrMan *e_hndl = static_cast<PAL_PtrMan *> (hndl);
  PAL_PSYNC_DAT sem = static_cast<PAL_PSYNC_DAT> (e_hndl->PAL_getHndl());
  BOOL ret;
  uint32_t inc_cnt=1;             //we assume auto event at this piont


  PAL_enterCriticalSection(sem->mutex);

  //
  //Auto event only allows one thread to proceed
  //if no thread is waiting then event is simply
  //reset
  //manual event allows all threads already
  //waiting to be released.  Event is simply
  //reset if no threads are already waiting.
  //
  if (0 != sem->thrd_cnt)
    {
      if (MANUAL_EVENT_TYPE == e_hndl->PAL_getType())
	inc_cnt = sem->thrd_cnt;

      ret = PAL_incSemaphore(e_hndl, inc_cnt, NULL);
    }

  int old_cnt=0;

  sem_getvalue(sem->sem, &old_cnt);

  sem->thrd_cnt++;

  while(old_cnt)
    {
      sched_yield();
      sem_getvalue(sem->sem, &old_cnt);
    };

  sem->thrd_cnt--;

  //
  //update the state just in case we were called
  //while the state was already signaled
  //
  sem->state = PAL_SyncObj::NON_SIGNALED;

  PAL_leaveCriticalSection(sem->mutex);

  return TRUE;
}

//***************************************************************/
// PAL_closeHandle(HANDLE hndl)
BOOL PAL_absclass::PAL_closeHandle(HANDLE hndl)
{
	char outline[256];
	sprintf(outline, "PAL_closeHandle(%p):", hndl);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	BOOL ret( TRUE );

	if (NULL == hndl_crit)
		PAL_initializeCriticalSection(&hndl_crit);
	if ( INVALID_HANDLE_VALUE == hndl )
	{
		PAL_ErrorSave( PAL_INVALID_VAL, " Handle already closed.");
		// I don't know what to return here. Hopefully TRUE
		return TRUE;
	}

	PAL_enterCriticalSection(hndl_crit);

	// check for valid passed handle
	PAL_PtrMan *ptr_man( static_cast<PAL_PtrMan*>( hndl ) );

	if (( NULL == ptr_man ) ||  (NULL == ptr_man->PAL_getHndl() ))
	{
		errno = EINVAL;
		PAL_ErrorSave( PAL_INVALID_VAL, " Invalid handle passed.");
		ret = FALSE;
		goto EXIT_DONE;
	}

	// Do NOT handle resource releasing stuff here, it will be processed in PAL_PtrMan()::~PAL_PtrMan()
	if (-1 == ptr_man->PAL_decObjCnt())
	{
		errno = ECANCELED;
		PAL_ErrorSave( PAL_INVALID_VAL, " Count already zero.");
		ret = FALSE;
		goto EXIT_DONE;
	}

	if ( ptr_man->PAL_isObjCntZero() )
	{
		switch ( ptr_man->PAL_getType() )
		{
		case UNASSIGNED:
			break;
		case MUTEX_TYPE:
			ptr_man->PAL_closefd();
			break;
		case CS_TYPE:
			break;
		case SEM_TYPE:
			ptr_man->PAL_closefd();
			break;
		case AUTO_EVENT_TYPE:
			ptr_man->PAL_closefd();
			break;
		case MANUAL_EVENT_TYPE:
			ptr_man->PAL_closefd();
			break;
		case SYSTEM_FILE_TYPE:
			break;
		case STDIO_FILE_TYPE:
			break;
		case PROCESS_ID_TYPE:
			break;
		case THREAD_TYPE:
			ptr_man->PAL_closefd();
			break;
		default:
			break;
		}				

		// delete ptr man
		delete ptr_man;
    }

EXIT_DONE:

	PAL_leaveCriticalSection( hndl_crit );
	return ret;
}

//***************************************************************/
// PAL_dupHandle(
BOOL PAL_absclass::PAL_dupHandle(
				 HANDLE src_proc,
				 HANDLE src_hndl,
				 HANDLE dest_proc,
				 LPHANDLE dest_hndl,
				 DWORD UNUSED(desired_access),
				 BOOL UNUSED(inherit_hndl),
				 DWORD UNUSED(opt))
{
  static const char *err_str("PAL_dupHandle:");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  BOOL ret=TRUE;

  /////////
  //new add
  ////////
  if (NULL == hndl_crit)
    PAL_initializeCriticalSection(&hndl_crit);
  //////////
  if ((NULL == src_hndl)  || (NULL == src_proc) ||
      (NULL == dest_hndl) || (NULL == dest_proc))
    return FALSE;
  PAL_PtrMan *pal_src_proc    = static_cast <PAL_PtrMan *> (src_proc);
  PAL_PtrMan *pal_dest_proc   = static_cast <PAL_PtrMan *> (dest_proc);
  PAL_PPROC_HNDL  src_pid_p   = static_cast <PAL_PPROC_HNDL>( pal_src_proc->PAL_getHndl() );
  PAL_PPROC_HNDL  dest_pid_p  = static_cast <PAL_PPROC_HNDL>( pal_dest_proc->PAL_getHndl());

  /////////
  //new add
  ////////
  PAL_enterCriticalSection(hndl_crit);
  //////////
	

  if (*src_pid_p == *dest_pid_p)
    {
      *dest_hndl = src_hndl;
      /////////
      //new add
      PAL_PtrMan *ptr_man = static_cast<PAL_PtrMan *> (src_hndl);
      if (-1 == ptr_man->PAL_incObjCnt())
	{
	  errno = ECANCELED;
	  PAL_ErrorSave( PAL_INVALID_VAL, " Count already maximum.");
	  ret = FALSE;
	  goto EXIT_DUP_HNDL;
	  //.			return FALSE;
	}
      ////////
    }
  else
    {
      /////////
      //new add
      errno = ECANCELED;
      ////////
      PAL_ErrorSave( PAL_INVALID_VAL, "Interprocess duplication not supported");
      PAL_PRINT_LOG(PAL_INFO_LOG, err_str);

      ret = FALSE;
      goto EXIT_DUP_HNDL;
      //.        return FALSE;
    }

 EXIT_DUP_HNDL:
  PAL_leaveCriticalSection(hndl_crit);
  return ret;
}

BOOL PAL_absclass::PAL_getFileSize(const char *name, int *fileSize)
{
  struct stat fstat;

  if (stat(name, &fstat) == 0)
    {
      *fileSize = fstat.st_size;
      return TRUE;
    }
  return FALSE;
}

//***************************************************************/
// PAL_createFile(
HANDLE PAL_absclass::PAL_createFile(
				    LPCSTR name,
				    DWORD access_right,
				    DWORD share_mode,
				    LPSECURITY_ATTRIBUTES secure_attr_p,
				    DWORD disposition,
				    DWORD flag_and_attribute,
				    HANDLE template_file)
{
  int     open_flags = 0;
  mode_t  create_mode = 0;    // When create
  char    outline[256];

  sprintf(outline, "PAL_createFile(%s, %o, %d, 0x%p, %d, %d, 0x%p):",
	  name, access_right, share_mode, secure_attr_p, disposition, flag_and_attribute, template_file);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  PAL_ERRNO_TYPE l_errc=PAL_SUCCESS;

  // Windows GENERAL_xxx flags
  if ((access_right & PAL_FILE_GEN_RD) && (access_right & PAL_FILE_GEN_WR)) {
    open_flags |= O_RDWR;
  } else if (access_right & PAL_FILE_GEN_RD) {
    open_flags |= O_RDONLY;
  } else if (access_right & PAL_FILE_GEN_WR) {
    open_flags |= O_WRONLY;
  }
  if (access_right & PAL_FILE_GEN_EXE) {
    // ***** WARNING: What is it in Linux ?
  }
  if (access_right & PAL_FILE_GEN_ALL) {
    open_flags |= O_RDWR;
    // ***** WARNING: need to add "EXE" flag
  }

  // Windows FILE_SHARE_xxx flags
  if (share_mode & PAL_FILE_SHARE_RD) {
    // ***** WARNING: What flag for Linux ?
  }
  if (share_mode & PAL_FILE_SHARE_WR) {
    // ***** WARNING: What flag for Linux ?
  }
  if (share_mode & PAL_FILE_SHARE_RM) {
    // ***** WARNING: What flag for Linux ?
  }

  /// Windows disposition flasg
  switch (disposition)
    {
    case PAL_FILE_CREATE:                    // Create a new file, only if it doesn not already exist.
      open_flags |= O_CREAT | O_EXCL;
      break;

    case PAL_FILE_CREATE_FORCE:              // Always creates a new file
      open_flags |= O_CREAT | O_TRUNC;
      break;

    case PAL_FILE_OPEN:                      // Open only if it exists
      // ***** WARNING: Seems nothing to do in Linux
      break;

    case PAL_FILE_OPEN_CREATE:               // If the file does not exist, create it.
      open_flags |= O_CREAT;
      break;

    case PAL_FILE_TRUNC:                     // Opens file and truncates it so that its size is zero bytes.
      open_flags |= O_TRUNC;
      break;

    default:
      // Nothing. We should already handle all cases above.
      break;
    }

  // Windows "flags and attributes" flags
#if 0   // Not supported yet
  if (flag_and_attribute & PAL_FILE_FLAG_WRITE_THROUGH) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
#endif
  if (flag_and_attribute & PAL_FILE_FLAG_OVERLAPPED) {        // Windows uses it for Asynchronize I/O
    open_flags |= O_ASYNC;
    return INVALID_HANDLE_VALUE;
  }
#if 0   // Not supported yet
  if (flag_and_attribute & PAL_FILE_FLAG_NO_BUFFERING) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_FLAG_RANDOM_ACCESS) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
#endif
  if (flag_and_attribute & PAL_FILE_FLAG_SEQUENTIAL_SCAN) {
    PAL_OutputDbgStr("File Sequential Scan is not supported!");
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
#if 0   // Not supported yet
  if (flag_and_attribute & PAL_FILE_FLAG_DELETE_ON_CLOSE) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_FLAG_BACKUP_SEMANTICS) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_FLAG_POSIX_SEMANTICS) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_FLAG_OPEN_REPARSE_POINT) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_FLAG_OPEN_NO_RECALL) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_FLAG_FIRST_PIPE_INSTANCE) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
#endif
#if 0   // Not supported yet
  if (flag_and_attribute & PAL_FILE_ATTR_RDONLY) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_HIDDEN) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_SYSTEM) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_DIR) {
    open_flags |= O_DIRECTORY;
    return INVALID_HANDLE_VALUE;
  }
#endif
  if (flag_and_attribute & PAL_FILE_ATTR_ARCHIVE) {
    PAL_OutputDbgStr("File ARCHIVE attribute is not supported!");
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
#if 0   // Not supported yet
  if (flag_and_attribute & PAL_FILE_ATTR_DEVICE) {
    // ***** WARNING: device is no difference in Linux, it's always file.
  }
#endif
  if (flag_and_attribute & PAL_FILE_ATTR_NORMAL) {
    create_mode |= S_IRWXU | S_IRGRP | S_IROTH;     // -rwx-r--r--
  }
#if 0   // Not supported yet
  if (flag_and_attribute & PAL_FILE_ATTR_TEMP) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_SPARSE) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_REPARSE) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_COMPRESS) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_OFFLINE) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_INDEX) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
  if (flag_and_attribute & PAL_FILE_ATTR_ENCRYPT) {
    // ***** WARNING: Un-supported yet.
    return INVALID_HANDLE_VALUE;
  }
#endif

  // Open the file
  int *fdesc_p = new (int);
  if (fdesc_p == NULL) {
    std::string err_str("PAL_createFile: ");
    l_errc = PAL_OUT_OF_MEM;
    err_str += l_errc;
		err_str += " Unable to allocate memory for file descriptor.";

    PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());
    return INVALID_HANDLE_VALUE;
  }
  *fdesc_p = open(name, open_flags, create_mode);

  if (*fdesc_p == -1) {
    delete fdesc_p;
    return INVALID_HANDLE_VALUE;
  }

  // Acquire HANDLE
  PAL_PtrMan *ptr_man = new (PAL_PtrMan);
  if (NULL == ptr_man)
    {
      std::string err_str("PAL_createFile: ");
      l_errc = PAL_OUT_OF_MEM;
      err_str += l_errc;
		err_str += " Unable to allocate memory for handle (PAL_PtrMan).";

      PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());
      return INVALID_HANDLE_VALUE;
    }

  // Assign HANDLE
  ptr_man->PAL_setHndl((HANDLE)fdesc_p);
  ptr_man->PAL_setType(SYSTEM_FILE_TYPE);

  return (HANDLE)(ptr_man);
}

//***************************************************************/
// PAL_createFile(
HANDLE PAL_absclass::PAL_createFile(
				    LPCTSTR name,
				    DWORD access_right,
				    DWORD share_mode,
				    LPSECURITY_ATTRIBUTES secure_attr_p,
				    DWORD disposition,
				    DWORD flag_and_attribute,
				    HANDLE template_file)
{
  HANDLE retVal = NULL;
  char str[1024];
  sprintf(str, "PAL_createFile(%ls, %d, %d, 0x%p, %d, %d, 0x%p)-wide:",
	  name, access_right, share_mode, secure_attr_p, disposition, flag_and_attribute, template_file);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, str);

#ifdef  UNICODE
  // Convert WCHAR to CHAR
  size_t len = wcslen(name) + 1;
  char *char_name = new char [len];
  wcstombs(char_name, name, len);     // Convert WCHAR to CHAR
#else
  char *char_name = name;
#endif

  retVal = PAL_createFile(char_name, access_right, share_mode, secure_attr_p, disposition, flag_and_attribute, template_file);

#ifdef  UNICODE
  delete [] char_name;
#endif

  return retVal;
}

/// LINUX STUB  // ***** NTService related (version)
//***************************************************************/
// PAL_getFileVersionInfoSize (
DWORD PAL_absclass::PAL_getFileVersionInfoSize (
						LPCTSTR UNUSED(lptstrFilename),
						LPDWORD UNUSED(lpdwHandle)
						)
{
  return 0;
}

/// LINUX STUB  // ***** NTService related (version)
//***************************************************************/
// PAL_getFileVersionInfo(
BOOL PAL_absclass::PAL_getFileVersionInfo(
					  LPCTSTR UNUSED(lptstrFilename),
					  DWORD UNUSED(dwHandle),
					  DWORD UNUSED(dwLen),
					  LPVOID UNUSED(lpData)
					  )
{
  return FALSE;
}

//***************************************************************/
// PAL_getFileSize( HANDLE hFile, LPDWORD UNUSED(lpFileSizeHigh))
DWORD PAL_API PAL_absclass::PAL_getFileSize( HANDLE hFile, LPDWORD UNUSED(lpFileSizeHigh))
{
  static const char *err_str("PAL_getFileSize: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  struct stat statBuf;

  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hFile);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());
  if ( fstat(*linux_fdesc_p, &statBuf) )
    {
      // an error has occured
      PAL_ErrorSave( errno, NULL);
      return PAL_INVALID_FILE_SIZE;
    }
  return (DWORD) statBuf.st_size;
}

int PAL_API PAL_absclass::PAL_unlockF( HANDLE hFile, int command, int offset)
{
  static const char *err_str("PAL_getFileSize: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  //struct stat statBuf;
  int retval = 0;

  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hFile);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());
  retval = lockf(*linux_fdesc_p, command, offset);
  return retval;
}

int PAL_API PAL_absclass::PAL_lockF( HANDLE hFile, int command, int offset)
{
  static const char *err_str("PAL_getFileSize: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  //struct stat statBuf;
  int retval = 0;

  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hFile);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());
  retval = lockf(*linux_fdesc_p, command, offset);
  return retval;
}

//***************************************************************/
// ::PAL_setFilePointer(HANDLE hFile,
DWORD PAL_API PAL_absclass::PAL_setFilePointer(HANDLE hFile,
					       LONG lDistanceToMove,
					       PLONG lpDistanceToMoveHigh,
					       DWORD dwMoveMethod)
{
  char outline[256];
  sprintf(outline, "PAL_setFilePointer(0x%p, %ld, 0x%p, %s):",
	  hFile,
	  lDistanceToMove,
	  lpDistanceToMoveHigh,
	  strMoveMethod[dwMoveMethod]);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor
  off_t       dist_moved;

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hFile);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());

  dist_moved = lseek(*linux_fdesc_p, lDistanceToMove, dwMoveMethod);
  if (dist_moved == (off_t)-1)
    {
      PAL_REPORT_ERROR;
      return PAL_INVALID_SET_FILE_POINTER;
    }

  return dist_moved;
}

//***************************************************************/
// PAL_touch( HANDLE hFile )
BOOL PAL_API PAL_absclass::PAL_touch(HANDLE hFile)
{
  BOOL bRc = FALSE;

  static const char *err_str("PAL_touch: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hFile);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());
  if ( futimes(*linux_fdesc_p, NULL) == 0)
    {
      bRc = TRUE;
    }

  return bRc;
}

/// LINUX STUB  // ***** NTService related (print module pathname for Version)
//***************************************************************/
// PAL_getModuleHandle(LPCTSTR UNUSED(lpModuleName))
PAL_HMODULE PAL_API PAL_absclass::PAL_getModuleHandle(LPCTSTR UNUSED(lpModuleName))
{
  static const char *err_str("PAL_getModuleHandle: <STUB>");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);
  //PAL_ERRNO_TYPE l_errc=PAL_SUCCESS;
  // TODO implement linux funcionality
  return NULL;
}

/// LINUX STUB  // ***** NTService related (print module pathname for Version/Uninstall)
//***************************************************************/
// PAL_getModuleFileName(
DWORD PAL_API PAL_absclass::PAL_getModuleFileName(
						  PAL_HMODULE UNUSED(hModule),
						  LPTSTR UNUSED(lpFilename),
						  DWORD UNUSED(nSize))
{
  static const char *err_str("PAL_getModuleFileName: <STUB>");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);
  //PAL_ERRNO_TYPE l_errc=PAL_SUCCESS;
  // TODO implement linux funcionality
  return 0;
}

//***************************************************************/
// PAL_getCurrentDirectory(
DWORD PAL_API PAL_absclass::PAL_getCurrentDirectoryW(
						     DWORD nBufferLength, LPTSTR lpBuffer)
{
  DWORD retVal = nBufferLength;

  char *buf = (char *)calloc(sizeof(char),nBufferLength);
  char outline[256];
  sprintf(outline, "PAL_getCurrentDirectoryW:(%u, %p)", nBufferLength, lpBuffer);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  if ((nBufferLength == 0) && (lpBuffer == NULL))
    {
      // requesting buffer size to hold current directory
      if (getcwd(buf, MAX_PATH) != NULL)
        {
	  retVal = 1 + strlen(buf);
        }
      else
        {
	  // could not get current directory
	  retVal = 0;
        }
    }
  else
    {
      buf[nBufferLength-1]=0;

      if (getcwd(buf, nBufferLength) != NULL)
	mbstowcs(lpBuffer, buf, nBufferLength);
      else
	retVal = 0;
    }
  free (buf);
  return retVal;
}

//***************************************************************/
// PAL_getCurrentDirectoryA(
DWORD PAL_API PAL_absclass::PAL_getCurrentDirectoryA(
						     DWORD nBufferLength, LPSTR lpBuffer)
{
  DWORD retVal = nBufferLength;

  char *buf = (char *)calloc(sizeof(char),nBufferLength);
  char outline[256];
  sprintf(outline, "PAL_getCurrentDirectoryA:(%u, %p)", nBufferLength, lpBuffer);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  if ((nBufferLength == 0) && (lpBuffer == NULL))
    {
      // requesting buffer size to hold current directory
      if (getcwd(buf, MAX_PATH) != NULL)
        {
	  retVal = 1 + strlen(buf);
        }
      else
        {
	  retVal = 0;
        }
    }
  else
    {
      buf[nBufferLength-1]=0;

      if (getcwd(buf, nBufferLength) != NULL)
	memcpy(lpBuffer, buf, nBufferLength);
      else	
	retVal = 0;
    }
  free (buf);
  return retVal;
}

//***************************************************************/
// PAL_setCurrentDirectoryW(
BOOL PAL_API PAL_absclass::PAL_setCurrentDirectoryW(
						    LPCTSTR lpPathName)
{
  char *buf = (char *)calloc(sizeof(char),MAX_PATH);
  static const char *err_str("PAL_setCurrentDirectoryW: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);
  wcstombs(buf, lpPathName, MAX_PATH);
  if (chdir(buf))
    {
      PAL_ErrorSave(errno, NULL);
      free(buf);
      return FALSE;
    }
  free(buf);
  return TRUE;
}

//***************************************************************/
// PAL_setCurrentDirectoryA(
BOOL PAL_API PAL_absclass::PAL_setCurrentDirectoryA(
						    LPCSTR lpPathName)
{
  static const char *err_str("PAL_setCurrentDirectoryA: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);
  if (chdir(lpPathName))
    {
      PAL_ErrorSave(errno, NULL);
      return FALSE;
    }
  return TRUE;
}


//***************************************************************/
// PAL_readFile(
BOOL PAL_absclass::PAL_readFile(
				HANDLE hndl,
				LPVOID buf_p,
				DWORD bytes_to_read,
				LPDWORD bytes_read_p,
				void *)
{
  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor
  ssize_t bytes_read;
  BOOL    bRc = TRUE;

  if ((hndl == NULL) || (buf_p == NULL))
    {
      // ***** WARNING: We need to Sync all PAL_ErrorSave() the error code to save.
      //PAL_ErrorSave(INVALID PARAMETER, " Invalid Parameter");
      return FALSE;
    }

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hndl);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());

  // Read from file
  bytes_read = read( *linux_fdesc_p, buf_p, bytes_to_read);

  if (bytes_read < 0)
    {
      // ***** WARNING: We need to Sync all PAL_ErrorSave() the error code to save.
      //PAL_ErrorSave(READ ERROR, " File read error");
      bRc = FALSE;
    }
  else if (bytes_read_p)
    {
      *bytes_read_p = bytes_read;
    }

  return bRc;
}

//***************************************************************/
// PAL_writeFile(HANDLE hndl,
BOOL PAL_absclass::PAL_writeFile(HANDLE hndl,
				 LPCVOID buf_p,
				 DWORD bytes_to_write,
				 LPDWORD bytes_written_p,
				 void *)
{
  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor
  ssize_t bytes_written;
  BOOL    bRc = TRUE;

  if ((hndl == NULL) || (buf_p == NULL)) {
    // ***** WARNING: We need to Sync all PAL_ErrorSave() the error code to save.
    //PAL_ErrorSave(INVALID PARAMETER, " Invalid Parameter");
    return FALSE;
  }

  // Get the real file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hndl);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());

  // Write to file
  bytes_written = write( *linux_fdesc_p, buf_p, bytes_to_write);

  if (bytes_written < 0)
    {
      // ***** WARNING: We need to Sync all PAL_ErrorSave() the error code to save.
      //PAL_ErrorSave(FILE WRITE ERROR, " File write error");
      bRc = FALSE;
    }
  else if (bytes_written_p)
    {
      *bytes_written_p = bytes_written;
    }

  return bRc;
}

//***************************************************************/
// PAL_deleteFile(LPCTSTR lpFileName)
BOOL PAL_API PAL_absclass::PAL_deleteFile(LPCTSTR lpFileName)
{
  BOOL retVal = TRUE;
  int name_len = wcslen(lpFileName);
  char *pathname = (char *) malloc(name_len+1);
  if (pathname == NULL)
    {
      retVal = FALSE;
      return retVal;
    }
  pathname[name_len]=0;

  // 1. Convert wchar_t to char_t
  wcstombs(pathname, lpFileName, name_len);

  if (unlink(pathname))
    {
      PAL_ErrorSave(errno, NULL);
      retVal = FALSE;
    }

  free(pathname);

  return retVal;
}

//***************************************************************/
// PAL_cancelIo(HANDLE hndl)
BOOL PAL_absclass::PAL_cancelIo(HANDLE hndl)
{
  PAL_PtrMan  *ptr_man;               // HANDLE of Win
  int *linux_fdesc_p;                 // Linux file descriptor
  ptr_man = static_cast<PAL_PtrMan *>(hndl);
  linux_fdesc_p = static_cast<int *>(ptr_man->PAL_getHndl());
  FILE *fp = fdopen(*linux_fdesc_p, "r");
  __fpurge(fp);
  return TRUE;
}

//***************************************************************/
// PAL_sleep(DWORD dw_msec)
VOID PAL_absclass::PAL_sleep(DWORD dw_msec)
{
  std::stringstream ss;
  ss << dw_msec;

  Debug::trace("PALThread.sleep", "PAL_sleep(): dw_msec=" + ss.str());

  if (dw_msec == 0)
    sched_yield();
  else
    usleep( dw_msec * 1000 );
}

//***************************************************************/
// PAL_getTickCount()
DWORD PAL_absclass::PAL_getTickCount()
{
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  // Randy QMX DEMO
  // return (ts.tv_sec << 10) + (ts.tv_nsec >> 22);
  return (ts.tv_sec *1000) + (ts.tv_nsec / 1000000);
}

//***************************************************************/
// PAL_setLocalTime(PAL_PTIME_STRUCT time_struct_p)
BOOL PAL_absclass::PAL_setLocalTime(PAL_PTIME_STRUCT time_struct_p)
{
  struct tm thetime;
  struct timeval tv;

  // we need to get if DST is set
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &thetime);

  thetime.tm_year = time_struct_p->wYear-1900;
  thetime.tm_mon = time_struct_p->wMonth-1;
  thetime.tm_mday = time_struct_p->wDay;
  thetime.tm_hour = time_struct_p->wHour;
  thetime.tm_min = time_struct_p->wMinute;
  thetime.tm_sec = time_struct_p->wSecond;
  tv.tv_sec = mktime(&thetime);
  tv.tv_usec = time_struct_p->wMilliseconds * 1000;

  if ( settimeofday(&tv, NULL) )
    {
      PAL_ErrorSave( errno, NULL);
      return FALSE;
    }
  return TRUE;
}

PAL_LOCALE_TYPE PAL_absclass::PAL_createLocale(int32_t category, const char *locale_name)
{
  /// Emulates windows _create_locale routine by calling newlocale.
  /// Side effect is the creation of local object which needs to
  /// be freed by calling PAL_freeLocale.

  static const char *err_str("PAL_createLocale: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str);

  //
  //translate windows constants to linux
  //
  int l_cat= LC_ALL_MASK;
  switch(category)
    {
    case(PAL_LC_ALL):
      PAL_PRINT_LOG(PAL_INFO_LOG, "LC_ALL mask");
      l_cat = LC_ALL_MASK;
      break;
    case(PAL_LC_COLLATE):
      PAL_PRINT_LOG(PAL_INFO_LOG, "LC_COLLATE mask");
      l_cat = LC_COLLATE_MASK;
      break;
    case(PAL_LC_CTYPE):
      PAL_PRINT_LOG(PAL_INFO_LOG, "LC_CTYPE mask");
      l_cat = LC_CTYPE_MASK;
      break;
    case(PAL_LC_MONETARY):
      PAL_PRINT_LOG(PAL_INFO_LOG, "LC_MONETARY mask");
      l_cat = LC_MONETARY_MASK;
      break;
    case(PAL_LC_NUMERIC):
      PAL_PRINT_LOG(PAL_INFO_LOG, "LC_NUMERIC mask");
      l_cat = LC_NUMERIC_MASK;
      break;
    case(PAL_LC_TIME):
      PAL_PRINT_LOG(PAL_INFO_LOG, "LC_TIME mask");
      l_cat = LC_TIME_MASK;
      break;
    default:
      PAL_ErrorSave( PAL_INVALID_VAL, "Unknown mask specified");
      l_cat = LC_TIME_MASK;
      break;

    }

  return (newlocale(l_cat, locale_name, NULL));
  /*
  //  return (querylocale(l_cat, locale_name));
  //
  //-retrieve the current locale string
  //-set the caller specified locale
  //-retrieve the needed cat string
  //-reset the locale to the orig
  //
  char *cur_locale = setlocale(l_cat, NULL);
  setlocale(l_cat, locale_name);
  char *new_locale = setlocale(l_cat, NULL);
  setlocale(l_cat, cur_locale);

  return new_locale;
  */

}

//***************************************************************/
// PAL_freeLocale(PAL_LOCALE_TYPE locale_to_free)
void PAL_absclass::PAL_freeLocale(PAL_LOCALE_TYPE locale_to_free)
{
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_freeLocale:");
  freelocale(locale_to_free);
}

//***************************************************************/
// PAL_wsaGetLastError(void)
int PAL_absclass::PAL_wsaGetLastError(void)
{
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_wsaGetLastError:");

  // We'll use our thread safe error capture for socket errors
  // too. TODO: maybe we should add a int to the struct?
  // ***** WARNING: PAL_getLastError() returns DWORD, be aware the type conversion.
  return (int)PAL_getLastError();
}

//***************************************************************/
// PAL_ioctlsocket( PAL_SOCKET s, long cmd, u_long *argp)
int PAL_absclass::PAL_ioctlsocket( PAL_SOCKET s, long cmd, u_long *argp)
{
  int retVal = 0;
  struct timeval tv;
  fd_set readfds;

  tv.tv_sec = 0;
  tv.tv_usec = 0;

  FD_ZERO(&readfds);

  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_ioctlsocket:");

  if (argp == NULL)
    {
      PAL_ErrorChk( true, PAL_INVALID_VAL, " ioctlsocket bad args");
      retVal = PAL_SOCKET_ERROR;
    }
  else
    {
      switch(cmd)
        {
	case PAL_FIONREAD:
	  /*************** 
                  temp remove
                  FD_SET(s, &readfds);
                  if ( -1 == select(s+1, &readfds, NULL, NULL, &tv))
                  {
                  PAL_ErrorChk( true, errno, " select failed");
		  *argp = -1;
                 break;
                 }

                 if (FD_ISSET(s, &readfds))
	  ***********************/
	  {

	    const int32_t buf_size=512;
	    char buf[buf_size];
	    *argp = 0;
	    *argp = recv(s, buf, buf_size, MSG_PEEK);
	    if (static_cast<u_long>(-1) == *argp)
	      {
		//PAL_ErrorChk( true, errno, strerror(errno));
		//perror("PAL_ioctlsocket");
		retVal = PAL_SOCKET_ERROR;
		*argp = 0;
	      }


	  }
	  break;
	case PAL_FIONBIO:
	  // get the current settings
	  retVal = fcntl(s, F_GETFL, 0);
	  if (*argp == TRUE)
	    {
	      // add non-blocking
	      fcntl(s, F_SETFL, retVal | FNDELAY);
	    }
	  else
	    {
	      // add non-blocking
	      fcntl(s, F_SETFL, retVal & ~FNDELAY);
	    }

	  break;
	default:
	  PAL_ErrorChk( true, PAL_INVALID_VAL, " ioctlsocket bad cmd");
	  retVal = PAL_SOCKET_ERROR;
	  break;
        }
    }
  return retVal;
}

//***************************************************************/
// PAL_closesocket( PAL_SOCKET s)
int PAL_absclass::PAL_closesocket( PAL_SOCKET s)
{
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, "PAL_closesocket:" );

  shutdown( s, SHUT_RDWR );
  int rc = close( s );

  if ( PAL_ErrorChk( 0 != rc, errno, NULL ) )
    return PAL_SOCKET_ERROR;

  return PAL_SOCKET_SUCCESS;
}

//***************************************************************/
// PAL_isNetworkAlive( LPDWORD lpdwFlags)
// Removed unused parameter PTR 621
BOOL PAL_absclass::PAL_isNetworkAlive( LPDWORD )
{
  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifAS=NULL; 
  bool returnVal = false;
	
  //call function to get IP address 
  getifaddrs(&ifAddrStruct); 

  ifAS = ifAddrStruct; 
	
  while (ifAS!=NULL) { 
    if (ifAS->ifa_addr && ifAS->ifa_addr->sa_family==AF_INET && strcmp(ifAS->ifa_name, "lo")!=0) // check it is IP4 and not local host
      {			
	// is a valid IP4 Address, are we online? 
	if ( ifAS->ifa_flags & IFF_RUNNING)
	  {			
	    //We are online, so return true				
	    returnVal = true;
	    break; 
	  }
      } 
    ifAS=ifAS->ifa_next; 
  }    

  //clean up after ourselves
  freeifaddrs(ifAddrStruct);

  return returnVal;
}

#define BUFSIZE 4096
int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
  struct nlmsghdr *nlHdr;
  int readLen = 0, msgLen = 0;

  do
    {
      /* Recieve response from the kernel */
      if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
        {
	  perror("SOCK READ: ");
	  return -1;
        }
      nlHdr = (struct nlmsghdr *)bufPtr;

      /* Check if the header is valid */
      if((NLMSG_OK(nlHdr, (unsigned) readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
        {
	  perror("Error in recieved packet");
	  return -1;
        }

      /* Check if the its the last message */
      if(nlHdr->nlmsg_type == NLMSG_DONE)
        {
	  break;
        }
      else
        {
	  /* Else move the pointer to buffer appropriately */
	  bufPtr += readLen;
	  msgLen += readLen;
        }

      /* Check if its a multi part message */
      if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
        {
	  /* return if its not */
	  break;
        }
    } while((nlHdr->nlmsg_seq != (unsigned)seqNum) || (nlHdr->nlmsg_pid != (unsigned)pId));

  return msgLen;
}

/* parse the route info returned */
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo)
{
  struct rtmsg *rtMsg;
  struct rtattr *rtAttr;
  int rtLen;

  rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

  /* If the route is not for AF_INET or does not belong to main routing table then return. */
  if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
    return;

  /* get the rtattr field */
  rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
  rtLen = RTM_PAYLOAD(nlHdr);

  for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen))
    {
      switch(rtAttr->rta_type)
        {
	case RTA_OIF:
	  if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
	  break;

	case RTA_GATEWAY:
	  memcpy(&rtInfo->gateWay, RTA_DATA(rtAttr), sizeof(rtInfo->gateWay));
	  break;

	case RTA_PREFSRC:
	  memcpy(&rtInfo->srcAddr, RTA_DATA(rtAttr), sizeof(rtInfo->srcAddr));
	  break;

	case RTA_DST:
	  memcpy(&rtInfo->dstAddr, RTA_DATA(rtAttr), sizeof(rtInfo->dstAddr));
	  break;
        }
    }

  return;
}

int get_gatewayip(char *gatewayip, socklen_t size)
{
  int found_gatewayip = 0;

  struct nlmsghdr *nlMsg;
  struct rtmsg *rtMsg;
  struct route_info *rtInfo;
  char *msgBuf;

  int sock, len, msgSeq = 0;

  /* Create Socket */
  if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0)
    {
      perror("Socket Creation: ");
      return(-1);
    }

  /* Initialize the buffer */
  msgBuf = (char *)malloc(BUFSIZE);
  if (msgBuf == NULL)
    {
      close(sock);
      return (-1);
    }
  memset(msgBuf, 0, BUFSIZE);

  /* point the header and the msg structure pointers into the buffer */
  nlMsg = (struct nlmsghdr *)msgBuf;
  rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);

  /* Fill in the nlmsg header*/
  nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
  nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

  nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
  nlMsg->nlmsg_seq = msgSeq++; // Sequence of the message packet.
  nlMsg->nlmsg_pid = getpid(); // PID of process sending the request.

  /* Send the request */
  if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0)
    {
      fprintf(stderr, "Write To Socket Failed...\n");
      return -1;
    }

  /* Read the response */
  if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0)
    {
      fprintf(stderr, "Read From Socket Failed...\n");
      free(msgBuf);
      return -1;
    }

  /* Parse and print the response */
  rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
  if (rtInfo == NULL)
    {
      free(msgBuf);
      close(sock);
      return -1;
    }

#define PAL_NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
			       (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
			       (int)(nlh)->nlmsg_len <= (len))
  for(;PAL_NLMSG_OK(nlMsg,len); nlMsg = NLMSG_NEXT(nlMsg,len))
    {
      memset(rtInfo, 0, sizeof(struct route_info));
      parseRoutes(nlMsg, rtInfo);

      // Check if default gateway
      if (strstr((char *)inet_ntoa(rtInfo->dstAddr), "0.0.0.0"))
        {
	  // copy it over
	  inet_ntop(AF_INET, &rtInfo->gateWay, gatewayip, size);
	  found_gatewayip = 1;
	  break;
        }
    }
  free(msgBuf);
  free(rtInfo);
  close(sock);
  return found_gatewayip;
}

DWORD PAL_absclass::PAL_getAdaptersInfo( PIP_ADAPTER_INFO pAdapterInfo, PULONG UNUSED(pOutBufLen))
{
  /* This is the structure we need to fill in
     typedef struct _IP_ADAPTER_INFO {
     struct _IP_ADAPTER_INFO *Next;
     DWORD                   ComboIndex;
     char                    AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
     char                    Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
     UINT                    AddressLength;
     BYTE                    Address[MAX_ADAPTER_ADDRESS_LENGTH];
     DWORD                   Index;
     UINT                    Type;
     UINT                    DhcpEnabled;
     PIP_ADDR_STRING         CurrentIpAddress;
     IP_ADDR_STRING          IpAddressList;
     IP_ADDR_STRING          GatewayList;
     IP_ADDR_STRING          DhcpServer;
     BOOL                    HaveWins;
     IP_ADDR_STRING          PrimaryWinsServer;
     IP_ADDR_STRING          SecondaryWinsServer;
     time_t                  LeaseObtained;
     time_t                  LeaseExpires;
     }

     Right now the Hobart code only ever accesses GatewayList so
     for the time being that is all I will fill in

  */
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_getAdaptersInfo:");

  struct ifaddrs * ifAddrStruct=NULL;
  struct ifaddrs * ifAS=NULL; 
  void * tmpAddrPtr=NULL; 
  //int i=0; 
  char addressBuffer[16];
  memset(addressBuffer, 0, 16);
 
  getifaddrs(&ifAddrStruct); 

  ifAS = ifAddrStruct; 
	
  while (ifAS!=NULL) { 
    if (ifAS->ifa_addr && ifAS->ifa_addr->sa_family==AF_INET && strcmp(ifAS->ifa_name, "lo")!=0) // check it is IP4 and not local host
      {			
	// is a valid IP4 Address 
	tmpAddrPtr=&((struct sockaddr_in *)ifAS->ifa_addr)->sin_addr; 
	inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, 16);
      } 
    ifAS=ifAS->ifa_next; 
  }    

  //clean up after ourselves
  freeifaddrs(ifAddrStruct);

  char gatewayip[512];
  if (get_gatewayip(gatewayip, 512) != -1)
    {
      PIP_ADAPTER_INFO pTmp = new IP_ADAPTER_INFO;
      if (pTmp != NULL)
        {
	  memset(pTmp, 0, sizeof(IP_ADAPTER_INFO));
	  memcpy((void *)pTmp->GatewayList.IpAddress.String, (void *)gatewayip, 16);
	  memcpy((void *)pTmp->IpAddressList.IpAddress.String, (void *)addressBuffer, 16);
	  memcpy(pAdapterInfo, pTmp, sizeof(IP_ADAPTER_INFO));
					
	  delete pTmp;
	  return ERROR_SUCCESS;
        }
    }

  return -1;
}


//***************************************************************/
// PAL_ultow(
LPWSTR PAL_API PAL_absclass::PAL_ultow(
				       ULONG value,
				       LPWSTR str,
				       int radix)
{
  if (str != NULL)
    {
      WCHAR buffer[33];
      PWCHAR pos;
      WCHAR digit;

      pos = &buffer[32];
      *pos = '\0';

      do
        {
	  digit = value % radix;
	  value = value / radix;
	  if (digit < 10)
	    *--pos = '0' + digit;
	  else
	    *--pos = 'a' + digit - 10;
        }
      while (value != 0L);

      memcpy(str, pos, (&buffer[32] - pos + 1) * sizeof(WCHAR));
    }
  return str;
}

//***************************************************************/
// ::PAL_wsplitpath(
void PAL_API PAL_absclass::PAL_wsplitpath(
					  const wchar_t *inpath,
					  wchar_t *drv,
					  wchar_t *dir,
					  wchar_t *fname,
					  wchar_t *ext )
{
  const wchar_t *p, *end;

  if (inpath[0] && inpath[1] == ':')
    {
      if (drv)
        {
	  drv[0] = inpath[0];
	  drv[1] = inpath[1];
	  drv[2] = 0;
        }
      inpath += 2;
    }
  else if (drv) drv[0] = 0;

  /* look for end of directory part */
  end = NULL;
  for (p = inpath; *p; p++) if (*p == '/' || *p == '\\') end = p + 1;

  if (end)  /* got a directory */
    {
      if (dir)
        {
	  memcpy( dir, inpath, (end - inpath) * sizeof(wchar_t) );
	  dir[end - inpath] = 0;
        }
      inpath = end;
    }
  else if (dir) dir[0] = 0;

  /* look for extension: what's after the last dot */
  end = NULL;
  for (p = inpath; *p; p++) if (*p == '.') end = p;

  if (!end) end = p; /* there's no extension */

  if (fname)
    {
      memcpy( fname, inpath, (end - inpath) * sizeof(wchar_t) );
      fname[end - inpath] = 0;
    }
  if (ext) wcscpy( ext, end );
}

static void *myrealloc(void *ptr, size_t size);

static void *myrealloc(void *ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
   *      NULL pointers, so we take care of it here */
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

char* PAL_absclass::extendedErrorMessageFormat = "cURL FTP error code is %ld";
unsigned long PAL_absclass::extendedError=0;

void PAL_API PAL_absclass::setExtendedError( unsigned long err )
{
  extendedError = err;
}

unsigned long PAL_API PAL_absclass::getExtendedError()
{
  return( extendedError );
}

bool PAL_API PAL_absclass::PAL_InternetGetLastResponseInfo(unsigned long* pdwError, char* data, unsigned long* dwErrorBufLength)
{
  // TODO
  return false;
}

/*****************************************************************
 **
 ** FTP related functions
 **
 *****************************************************************/

static size_t ftpWriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  ftpSession *mem = (ftpSession *)data;

  mem->memory = (char*)myrealloc((void *)mem->memory, mem->size + realsize + 1);
  if (mem->memory)
    {
      memcpy(&(mem->memory[mem->size]), ptr, realsize);
      mem->size += realsize;
      mem->memory[mem->size] = 0;
    }
  return realsize;
}

//***************************************************************/
// PAL_internetOpen(
PAL_INET_HANDLE PAL_absclass::PAL_internetOpen(
					       PAL_INET_STR lpszAgent,
					       DWORD UNUSED(dwAccessType),
					       PAL_INET_STR lpszProxyName,
					       PAL_INET_STR lpszProxyBypass,
					       DWORD UNUSED(dwFlags))
{
  internetHandle *inetHandle = new internetHandle;

  inetHandle->dwSize = sizeof(internetSession);
  internetSession* inet = new internetSession; 
  inetHandle->inet = inet;
  inet->lpszAgent = (lpszAgent == 0) ? "" : lpszAgent;
  inet->lpszProxyName = (lpszProxyName == 0) ? "" : lpszProxyName;
  inet->lpszProxyBypass = (lpszProxyBypass == 0) ? "" : lpszProxyBypass;

  PAL_enterCriticalSection(&internetSessionCounterCs);
  ++ internetSessionCounter;
  if (1 == internetSessionCounter)
    {
      curl_global_init(CURL_GLOBAL_DEFAULT);
    }
  PAL_leaveCriticalSection(&internetSessionCounterCs);
  return (PAL_INET_HANDLE)inetHandle;
}

//***************************************************************/
// PAL_ftpFindFilesByExtension(string extStr)
vector<struct ftpFileInfo> PAL_absclass::PAL_ftpFindFilesByExtension(PAL_INET_HANDLE hFtp, std::string extStr)
{
  vector<ftpFileInfo> v;

  internetHandle *ftpHandle = (internetHandle*)hFtp;
  if (ftpHandle && ftpHandle->dwSize == sizeof(ftpSession) && ftpHandle->ftp->memory)
    {
      string inStr(ftpHandle->ftp->memory);

      istringstream ss(inStr);
      string s;
      while (ss>>s)
        {
	  struct ftpFileInfo f;

	  if (!extStr.empty())
            {
	      if (s.find(extStr) != string::npos)
                {
		  //cout << "found" << s << endl;
		  f.name = s;
		  v.push_back(f);
                }
            }
	  else
            {
	      //cout << "found" << s << endl;
	      f.name = s;
	      v.push_back(f);
            }
        }
    }
  return v;
}

//***************************************************************/
// PAL_internetConnect(
PAL_INET_HANDLE PAL_absclass::PAL_internetConnect(
						  PAL_INET_HANDLE hInternet,
						  PAL_INET_STR lpszServerNameAndPath,
						  INTERNET_PORT UNUSED(nServerPort),
						  PAL_INET_STR lpszUsername,
						  PAL_INET_STR lpszPassword,
						  DWORD UNUSED(dwService),
						  DWORD UNUSED(dwFlags),
						  DWORD_PTR UNUSED(dwContext))
{
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_internetConnect:");

  internetHandle *ftpHandle = NULL;
  internetHandle *inetHandle = (internetHandle*)hInternet;
  if (inetHandle && inetHandle->dwSize == sizeof(internetSession))
    {
      internetSession* inet = inetHandle->inet;
      ftpHandle = new internetHandle;
      ftpHandle->dwSize = sizeof(ftpSession);
      ftpSession *ftp = new ftpSession;
      ftpHandle->ftp = ftp;
      ftp->curl = curl_easy_init();
      CURL *curl = ftp->curl;
      if (curl)
        {
	  ftp->lpszSiteAddress = lpszServerNameAndPath;

	  // turn off output
	  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1 ) ;
	  curl_easy_setopt(curl, CURLOPT_PROXY, inet->lpszProxyName.c_str());
	  curl_easy_setopt(curl, CURLOPT_NOPROXY, inet->lpszProxyBypass.c_str());

	  CURLcode res;

	  if (ftp->memory)
            {
	      free(ftp->memory);
	      ftp->memory = NULL;
            }
	  ftp->size = 0;

	  curl_easy_setopt(curl, CURLOPT_URL, lpszServerNameAndPath);
	  curl_easy_setopt(curl, CURLOPT_USERNAME, lpszUsername);
	  curl_easy_setopt(curl, CURLOPT_PASSWORD, lpszPassword);
	  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ftpWriteMemoryCallback);
	  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)ftp);
	  curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 10);            
	  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	  curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 1);
	  res = curl_easy_perform(curl);
	  if (res != CURLE_OK)
            {
	      PAL_internetCloseHandle( (PAL_INET_HANDLE) curl );
	      curl = NULL;
            }
        }
    }

  return (PAL_INET_HANDLE)ftpHandle;
}

//***************************************************************/
// PAL_ftpGetFile(
bool PAL_absclass::PAL_ftpGetFile(
				  PAL_INET_HANDLE hFtp,
				  PAL_INET_STR remote_file,
				  PAL_INET_STR new_file,
				  BOOL UNUSED(fail_if_exists),
				  DWORD UNUSED(flags_and_attributes),
				  DWORD UNUSED(flags),
				  DWORD_PTR UNUSED(context))
{
  CURLcode res;
  bool retVal = false;

  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_ftpGetFile:");

  internetHandle *ftpHandle = (internetHandle*)hFtp;
  if (ftpHandle && ftpHandle->dwSize == sizeof(ftpSession) && ftpHandle->ftp)
    {
      ftpSession *ftp = ftpHandle->ftp;
      CURL *curl = ftp->curl;
      if(curl)
        {
	  /*  Not necessary, and wasn't added by BSquare, but could be used in the future,
	      so I'm leaving the example of setting up a buffer that curllib will fill with
	      a more informative error message should an error occur.

	      char errbuff[CURL_ERROR_SIZE];
	      memset(errbuff, 0, CURL_ERROR_SIZE);
	      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuff);
	  */

	  string fullRemoteFile = ftp->lpszSiteAddress;
	  fullRemoteFile.append(remote_file);
	  curl_easy_setopt(curl, CURLOPT_URL, fullRemoteFile.c_str());
	  FILE *pf = fopen(new_file, "w");
	  if (pf != NULL)
            {
	      curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 0);
	      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	      curl_easy_setopt(curl, CURLOPT_WRITEDATA, pf);
	      res = curl_easy_perform(curl);
	      fclose(pf);
	      if (res == CURLE_OK)
		retVal = true;
            }
        }
    }
  return retVal;
}

//***************************************************************/
// PAL_ftpPutFile(
bool PAL_absclass::PAL_ftpPutFile(
				  PAL_INET_HANDLE inet_handle,
				  PAL_INET_STR new_file,
				  PAL_INET_STR remote_file,
				  DWORD flags,
				  DWORD_PTR context)
{
  CURLcode res;
  bool retVal = false;

  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_ftpPutFile:");

  internetHandle *ftpHandle = (internetHandle*)inet_handle;
  if (ftpHandle && ftpHandle->dwSize == sizeof(ftpSession) && ftpHandle->ftp)
    {
      ftpSession *ftp = ftpHandle->ftp;
      CURL *curl = ftp->curl;
      if(curl)
        {
	  /*  Not necessary, and wasn't added by BSquare, but could be used in the future,
	      so I'm leaving the example of setting up a buffer that curllib will fill with
	      a more informative error message should an error occur.

	      char errbuff[CURL_ERROR_SIZE];
	      memset(errbuff, 0, CURL_ERROR_SIZE);
	      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuff);
	  */

	  string fullRemoteFile = ftp->lpszSiteAddress;
	  fullRemoteFile.append(remote_file);
	  curl_easy_setopt(curl, CURLOPT_URL, fullRemoteFile.c_str());
	  FILE *pf = fopen(new_file, "r");
	  if (pf != NULL)
            {
	      curl_easy_setopt(curl, CURLOPT_DIRLISTONLY, 0);
	      curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	      curl_easy_setopt(curl, CURLOPT_READDATA, pf);
	      res = curl_easy_perform(curl);
	      fclose(pf);
	      if (res == CURLE_OK)
		retVal = true;
            }
        }
    }
  return retVal;
}

bool PAL_absclass::PAL_ftpCreateDirectory(
					  PAL_INET_HANDLE inet_handle,
					  PAL_INET_STR remote_directory
					  )
{
  CURLcode res;
  bool retVal = false;

  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_ftpPutFile:");

  internetHandle *ftpHandle = (internetHandle*)inet_handle;
  if (ftpHandle && ftpHandle->dwSize == sizeof(ftpSession) && ftpHandle->ftp)
    {
      ftpSession *ftp = ftpHandle->ftp;
      CURL *curl = ftp->curl;
      if(curl)
        {
	  /*  Not necessary, and wasn't added by BSquare, but could be used in the future,
	      so I'm leaving the example of setting up a buffer that curllib will fill with
	      a more informative error message should an error occur.

	      char errbuff[CURL_ERROR_SIZE];
	      memset(errbuff, 0, CURL_ERROR_SIZE);
	      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuff);
	  */

	  string fullRemoteFile = ftp->lpszSiteAddress;
	  fullRemoteFile.append(remote_directory);
	  curl_easy_setopt(curl, CURLOPT_URL, fullRemoteFile.c_str());
	  curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1);
	  res = curl_easy_perform(curl);
	  if (res == CURLE_OK)
	    retVal = true;
        }
    }
  return retVal;
}

//***************************************************************/
// PAL_internetCloseHandle( PAL_INET_HANDLE UNUSED(hInternet))
BOOL PAL_absclass::PAL_internetCloseHandle(PAL_INET_HANDLE hInternet)
{
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "PAL_internetCloseHandle:");

  internetHandle *inetHandle = (internetHandle*)hInternet;
  if (inetHandle)
    {
      if (sizeof(internetSession) == inetHandle->dwSize)
	{
	  if (inetHandle->ftp)
	    {
	      delete inetHandle->ftp;
	      inetHandle->ftp = NULL;
	    }
	  PAL_enterCriticalSection(&internetSessionCounterCs);
	  if (internetSessionCounter > 0)
	    {
	      -- internetSessionCounter;
	      if (0 == internetSessionCounter)
		{
		  curl_global_cleanup();
		}
	    }
	  PAL_leaveCriticalSection(&internetSessionCounterCs);
	}
      else if (sizeof(ftpSession) == inetHandle->dwSize)
	{
	  if (inetHandle->ftp->curl)
	    {
	      curl_easy_cleanup(inetHandle->ftp->curl);
	      inetHandle->ftp->curl = NULL;
	    }
	  if (inetHandle->ftp->memory)
	    {
	      free(inetHandle->ftp->memory);
	      inetHandle->ftp->memory = NULL;
	    }
	  if (inetHandle->ftp)
	    {
	      delete inetHandle->ftp;
	      inetHandle->ftp = NULL;
	    }
	}
      else
	{
	  return FALSE;
	}

      inetHandle->dwSize = 0;
      inetHandle->inet = NULL;

      delete inetHandle;
    }
  return TRUE;
}

/*****************************************************************
 **
 ** end of FTP related functions
 **
 *****************************************************************/


//***************************************************************/
// PAL_tlsAlloc(void)
DWORD PAL_API PAL_absclass::PAL_tlsAlloc(void)
{
  return reinterpret_cast<DWORD>(PAL_initThrdData());
}

//***************************************************************/
// PAL_tlsFree( DWORD dwTlsIndex)
BOOL PAL_API PAL_absclass::PAL_tlsFree( DWORD dwTlsIndex)
{
  char outline[256];
  sprintf(outline, "PAL_tlsFree(%u):", dwTlsIndex);
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  PAL_delThrdData(reinterpret_cast<HANDLE>(dwTlsIndex));
  return TRUE;
}

//***************************************************************/
// PAL_delThrdData(HANDLE dat)
void PAL_absclass::PAL_delThrdData(HANDLE dat)
{
	char outline[256];
	sprintf(outline, "PAL_delThrdData(%p):", dat);
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

	PAL_CThreadData *myThrd( static_cast<PAL_CThreadData*>( dat ) );

	if ( myThrd )
	{
		delete myThrd;
	}
}

//***************************************************************/
// PAL_initThreadOnce(void)
void PAL_absclass::PAL_initThreadOnce(void)
{
	std::string err_str="PAL_initThreadOnce";
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

	int32_t l_err = pthread_key_create(&PAL_key, PAL_delThrdData);

	if (0 != l_err)
	{
		err_str += ": Unable to create key for error handling. ";
		err_str += l_err;
		PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());
	}
}

//***************************************************************/
HANDLE PAL_absclass::PAL_initThrdData(void)
{
	std::string err_str = "PAL_initThrdData:";
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

	//
	//call PAL_initThreadOnce method if it has not been already
	//called.  Determine if this calling thread has a value
	//already associated with the key.  If not, allocate
	//memory and associate it with the key.
	//
	int32_t l_err = pthread_once( &PAL_kinit_once, &PAL_absclass::PAL_initThreadOnce );

	if (0 != l_err)
	{
		err_str += l_err;
		err_str += "pthread_once call failed while creating key";
		Debug::trace("PALThread", "pthread_once call failed while creating key");
		PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());
	}

	HANDLE l_thrd_dat;
	if (NULL == (l_thrd_dat = pthread_getspecific(PAL_key) ))
	{
		err_str="PAL_initThrdData: Allocating memory for thread data.";
		PAL_PRINT_LOG(PAL_INFO_LOG, err_str.c_str());

		l_thrd_dat = static_cast<HANDLE> (new(PAL_CThreadData));
		if (NULL == l_thrd_dat)
        {
			err_str = "PAL_initThrdData:";
			err_str += " Unable to allocate memory for thread specific data (l_thrd_dat).";
			Debug::trace("PALThread", "Unable to allocate memory for thread specific data (l_thrd_dat).");
			PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());
			return NULL;
        }

		l_err = pthread_setspecific( PAL_key, l_thrd_dat );

		if (0 != l_err)
		{
			err_str = "PAL_initThrdData:";
			err_str += " Unable to set thread specific data. ";
			Debug::trace("PALThread", "Unable to set thread specific data.");
			err_str += l_err;
			PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());
		}
	}

	return(l_thrd_dat);
}

//***************************************************************/
// PAL_tlsGetValue(DWORD UNUSED(tlsIndex))
LPVOID PAL_absclass::PAL_tlsGetValue(DWORD UNUSED(tlsIndex))
{
	PAL_CThreadData *myThrdData( static_cast<PAL_CThreadData*>( PAL_absclass::PAL_initThrdData() ) );

	return (LPVOID) (myThrdData->m_uservalue);
}

//***************************************************************/
// PAL_tlsSetValue( DWORD UNUSED(dwTlsIndex), LPVOID lpTlsValue)
BOOL PAL_API PAL_absclass::PAL_tlsSetValue( DWORD UNUSED(dwTlsIndex), LPVOID lpTlsValue)
{
	PAL_CThreadData *myThrdData( static_cast<PAL_CThreadData*>( PAL_absclass::PAL_initThrdData() ) );

	if ( myThrdData )
	{
		myThrdData->m_uservalue = (DWORD) lpTlsValue;
		return TRUE;
	}

    return FALSE;
}

// TODO:Linux returns int while Win returns DWORD
//***************************************************************/
// PAL_getLastError(void)
DWORD PAL_absclass::PAL_getLastError(void)
{
	// We return mapped 'errno' in this function.
	// Do not call any API to prevent it from changing 'errno' (or save it for later use)
#if 0
	std::string err_str="PAL_getLastError: ";
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

	HANDLE ptr=NULL;

	if (NULL != (ptr = pthread_getspecific(PAL_key)) )
	{
		//return *static_cast<PAL_CThreadData> (ptr);
		PAL_CThreadData *myThrdData = static_cast<PAL_CThreadData *> (ptr);
		return myThrdData->m_errno;
	}

	err_str += " Unable to retrieve thread specific error.";
	PAL_PRINT_LOG(PAL_ERR_LOG, err_str.c_str());

	return errno;
#endif

	return PAL_mapErrnoLinux2Win32( errno );
}

/// LINUX STUB  // Sound related
//***************************************************************/
// PAL_beep(DWORD UNUSED(freq), DWORD UNUSED(duration))
BOOL PAL_absclass::PAL_beep(DWORD UNUSED(freq), DWORD UNUSED(duration))
{
  printf("\x7");                      // Simple beep
  return FALSE;
}

//***************************************************************/
// PAL_simpleBeep()
BOOL PAL_API PAL_absclass::PAL_simpleBeep()
{
  printf("\x7");                      // Simple beep
  return TRUE;
}

/// LINUX STUB  // Graphic related (Font)
//***************************************************************/
// PAL_deleteFont(HANDLE UNUSED(font_handle))
BOOL PAL_absclass::PAL_deleteFont(HANDLE UNUSED(font_handle))
{
  return FALSE;
}

#if 0   // Alamy: Remove this function
/// LINUX STUB      // Function removed
//***************************************************************/
// PAL_formatMessage(
DWORD PAL_API PAL_absclass::PAL_formatMessage(
					      DWORD UNUSED(dwFlags),
					      LPCVOID UNUSED(lpSource),
					      DWORD UNUSED(dwMessageId),
					      DWORD UNUSED(dwLanguageId),
					      LPTSTR UNUSED(lpBuffer),
					      DWORD UNUSED(nSize),
					      va_list *UNUSED(Arguments))
{
  std::string err_str("PAL_formatMessage: <STUB>");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());
  //PAL_ERRNO_TYPE l_errc=PAL_SUCCESS;
  // TODO implement linux funcionality
  // ***** WARNING: add code for linux
  return 0;
}
#endif

#if 0
typedef int64_t filetime_t;

/* Convert a UNIX time_t into a Windows filetime_t */
filetime_t unixTimeToFileTime(time_t utime) {
  int64_t tconv = ((int64_t)utime * RATE_DIFF) + EPOCH_DIFF;
  return tconv;
}

/* Convert a Windows filetime_t into a UNIX time_t */
time_t fileTimeToUnixTime(filetime_t ftime) {
  int64_t tconv = (ftime - EPOCH_DIFF) / RATE_DIFF;
  return (time_t)tconv;
}
#endif

//***************************************************************/
//
BOOL PAL_API PAL_absclass::PAL_dosDateTimeToFileTime( WORD fatdate, WORD fattime, PAL_LPFILETIME ft)
{
  struct tm newtm;
#ifndef HAVE_TIMEGM
  struct tm *gtm;
  time_t time1, time2;
#endif

  newtm.tm_sec  = (fattime & 0x1f) * 2;
  newtm.tm_min  = (fattime >> 5) & 0x3f;
  newtm.tm_hour = (fattime >> 11);
  newtm.tm_mday = (fatdate & 0x1f);
  newtm.tm_mon  = ((fatdate >> 5) & 0x0f) - 1;
  newtm.tm_year = (fatdate >> 9) + 80;
  newtm.tm_isdst = -1;
#ifdef HAVE_TIMEGM
  PAL_rtlSecondsSince1970ToTime( timegm(&newtm), (LARGE_INTEGER *)ft );
#else
  time1 = mktime(&newtm);
  gtm = gmtime(&time1);
  time2 = mktime(gtm);
  PAL_rtlSecondsSince1970ToTime( 2*time1-time2, (LARGE_INTEGER *)ft );
#endif
  return TRUE;
}

//***************************************************************/
//
void PAL_API PAL_absclass::PAL_rtlSecondsSince1970ToTime(
							 DWORD Seconds,
							 LARGE_INTEGER *Time )
{
  ULONGLONG secs = Seconds * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
  Time->u.LowPart  = (DWORD)secs;
  Time->u.HighPart = (DWORD)(secs >> 32);
}

//***************************************************************/
//
BOOL PAL_API PAL_absclass::PAL_rtlTimeToSecondsSince1970(
							 const LARGE_INTEGER *Time,
							 LPDWORD Seconds )
{
  ULONGLONG tmp = ((ULONGLONG)Time->u.HighPart << 32) | Time->u.LowPart;
  tmp = tmp / TICKSPERSEC - SECS_1601_TO_1970;
  if (tmp > 0xffffffff) return FALSE;
  *Seconds = (DWORD)tmp;
  return TRUE;
}

//***************************************************************/
//
BOOL PAL_API PAL_absclass::PAL_fileTimeToDosDateTime(
						     const PAL_LPFILETIME lpFiletime,
						     LPWORD lpFatDate,
						     LPWORD lpFatTime)
{
  LARGE_INTEGER       li;
  DWORD               t = 0;
  time_t              unixtime;
  struct tm*          tm;

  li.u.LowPart = lpFiletime->dwLowDateTime;
  li.u.HighPart = lpFiletime->dwHighDateTime;
  PAL_rtlTimeToSecondsSince1970( &li, &t );
  unixtime = t;
  tm = gmtime( &unixtime );
  if (lpFatTime)
    *lpFatTime = (tm->tm_hour << 11) + (tm->tm_min << 5) + (tm->tm_sec / 2);
  if (lpFatDate)
    *lpFatDate = ((tm->tm_year - 80) << 9) + ((tm->tm_mon + 1) << 5)
      + tm->tm_mday;
  return TRUE;

}
//***************************************************************/
// PAL_systemTimeToFileTime(
BOOL PAL_API PAL_absclass::PAL_systemTimeToFileTime(
						    const PAL_TIME_STRUCT *lpSystemTime,
						    PAL_LPFILETIME lpFileTime)
{
  std::string err_str("PAL_systemTimeToFileTime: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

  struct tm temptime;
  struct tm thetime;
  struct timeval tv;

  // we need to get if DST is set
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &thetime);

  temptime.tm_isdst = thetime.tm_isdst;

  temptime.tm_year = lpSystemTime->wYear - 1900;
  temptime.tm_mon = lpSystemTime->wMonth - 1;
  temptime.tm_mday = lpSystemTime->wDay;
  temptime.tm_hour = lpSystemTime->wHour;
  temptime.tm_min = lpSystemTime->wMinute;
  temptime.tm_sec = lpSystemTime->wSecond;

  time_t sec = mktime(&temptime);
  if ( -1 == sec)
    {
      PAL_ErrorSave(
		    PAL_INVALID_VAL,
		    " Could not change TIME_STRUCT to FILETIME"
		    );
      return FALSE;
    }

  // subtract the local time difference from UTC
  sec -= thetime.tm_gmtoff;

  //	Include the milliseconds in the FileTime value [CCD June 2011].
  int64_t msec = ((int64_t) sec * 1000) + lpSystemTime->wMilliseconds;
  int64_t time = ((RATE_DIFF / 1000) * msec) + EPOCH_DIFF;

  lpFileTime->dwHighDateTime = time >> 32;
  lpFileTime->dwLowDateTime = (DWORD)time;

  return TRUE;
}

//***************************************************************/
// PAL_fileTimeToLocalFileTime(
BOOL PAL_API PAL_absclass::PAL_fileTimeToLocalFileTime(
						       const PAL_FILETIME * lpFileTime,
						       PAL_LPFILETIME lpLocalFileTime)
{
  std::string err_str("PAL_fileTimeToLocalFileTime: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

  int64_t time = (int64_t)(lpFileTime->dwHighDateTime)<<32;
  time |= lpFileTime->dwLowDateTime;

  // covert to unix seconds
  time = (time - EPOCH_DIFF) / RATE_DIFF;

  struct tm thetime;
  struct timeval tv;

  // we need to get if DST is set
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &thetime);

  // add the time zone
  time = time + thetime.tm_gmtoff;

  // convert back to windows
  time = (time * RATE_DIFF) + EPOCH_DIFF;

  lpLocalFileTime->dwLowDateTime = (DWORD) time;
  lpLocalFileTime->dwHighDateTime = time >> 32;

  return TRUE;
}

//***************************************************************/
// PAL_fileTimeToSystemTime(
BOOL PAL_API PAL_absclass::PAL_fileTimeToSystemTime(
						    const PAL_FILETIME *lpFileTime,
						    PAL_PTIME_STRUCT lpSystemTime)
{
  std::string err_str("PAL_fileTimeToSystemTime: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

  int64_t time = (int64_t)(lpFileTime->dwHighDateTime)<<32;
  time |= lpFileTime->dwLowDateTime;

  // covert to unix seconds
  time = (time - EPOCH_DIFF) / RATE_DIFF;

  struct tm thetime;
  struct timeval tv;

  // we need to get if DST is set
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &thetime);

  // add the time zone
  time = time + thetime.tm_gmtoff;

  gmtime_r((time_t *)&time, &thetime);
  lpSystemTime->wYear = thetime.tm_year+1900;
  lpSystemTime->wMonth = thetime.tm_mon+1;
  lpSystemTime->wDayOfWeek = thetime.tm_wday;
  lpSystemTime->wDay = thetime.tm_mday;
  lpSystemTime->wHour = thetime.tm_hour;
  lpSystemTime->wMinute = thetime.tm_min;
  lpSystemTime->wSecond = thetime.tm_sec;
  lpSystemTime->wMilliseconds = 0;

  return TRUE;
}

//***************************************************************/
// pal_getTimeFormat_FillingPattern(
LPTSTR PAL_absclass::pal_getTimeFormat_FillingPattern(
						      PAL_DATETIME_FORMAT_PATTERN pattern,
						      int pattern_match_count,
						      const PAL_TIME_STRUCT *lpTime,
						      LPTSTR time_str_p,
						      int &time_str_size,
						      LPTSTR lpTimeStr                // lpTimeStr is used to detect if we need to fill the string
						      )
{
  // Normalize pattern size
  if (pattern_match_count > 2)
    {
      pattern_match_count = 2;
    }
  int filling_string_size = pattern_match_count;  // size may change later

  switch (pattern)
    {
    case PAL_DATETIME_FORMAT_HOUR12:
      if (pattern_match_count >= 2)           // hh
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%02d", lpTime->wHour % 12);
	}
      else if (pattern_match_count == 1)      // h
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%d", lpTime->wHour % 12);
	  if (lpTime->wHour >= 10)    filling_string_size = 2;
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    case PAL_DATETIME_FORMAT_HOUR24:
      if (pattern_match_count >= 2)           // HH
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%02d", lpTime->wHour);
	}
      else if (pattern_match_count == 1)      // H
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%d", lpTime->wHour);
	  if (lpTime->wHour >= 10)    filling_string_size = 2;
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    case PAL_DATETIME_FORMAT_MINUTE:
      if (pattern_match_count >= 2)           // mm
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%02d", lpTime->wMinute);
	}
      else if (pattern_match_count == 1)      // m
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%d", lpTime->wMinute);
	  if (lpTime->wMinute >= 10)    filling_string_size = 2;
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    case PAL_DATETIME_FORMAT_SECOND:
      if (pattern_match_count >= 2)           // ss
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%02d", lpTime->wSecond);
	}
      else if (pattern_match_count == 1)      // s
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, L"%d", lpTime->wSecond);
	  if (lpTime->wSecond >= 10)    filling_string_size = 2;
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    case PAL_DATETIME_FORMAT_AMPM:
      if (pattern_match_count >= 2)           // tt
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, ((lpTime->wHour >= 12) ? L"PM" : L"AM"));
	}
      else if (pattern_match_count == 1)      // t
	{
	  if (lpTimeStr)  swprintf(time_str_p, time_str_size, ((lpTime->wHour >= 12) ? L"P" : L"A"));
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    default:
      // ***** ERROR: This should never happen
      break;
    }

  // swprintf() will write at most 'time_str_size' characters into 'time_str_p'
  if (
      (lpTimeStr)
      && (time_str_size < filling_string_size)
      ) {
    filling_string_size = time_str_size;    // Exactly how many buffer we used
    time_str_size -= filling_string_size;   // How many buffer we still have
  }

  return (time_str_p + filling_string_size);  // Move string pointer
}

// http://msdn.microsoft.com/en-us/library/dd318130%28v=VS.85%29.aspx
//***************************************************************/
// PAL_getTimeFormat(
int PAL_absclass::PAL_getTimeFormat(
				    LCID Locale,
				    DWORD dwFlags,
				    const PAL_TIME_STRUCT *lpTime,
				    LPCTSTR lpFormat,
				    LPTSTR lpTimeStr,
				    int cchTime)
{
  PAL_DATETIME_FORMAT_PATTERN pattern_curr, pattern_prev;
  BOOL    pattern_found_flag;         // With this variable, save time from calling function to detect matching pattern
  int     pattern_match_count;
  LPTSTR  time_str_p;                 // Keep original pointer, lpDateStr, to calculate string size
  int     time_str_size;              // How many buffer we could still use

  char outline[256];
  sprintf(outline, "PAL_getTimeFormat(%x, %x, %p, %ls, %ls, %d):",
	  (DWORD)Locale,
	  dwFlags,
	  lpTime,
	  lpFormat,
	  lpTimeStr,
	  cchTime
	  );
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);

  // Support only what current Hobart code is using
  if (
      (Locale != PAL_LOCALE_SYS_DEFAULT)
      || (dwFlags != 0)
      ) {
    goto Error_NotSupported;
  }

  // Check parameter
  if (
      (lpTime == NULL)
      || (lpFormat == NULL)
      || ((lpTimeStr == NULL) && (cchTime != 0))
      ) {
    goto Error_NotSupported;
  }

  // Initiate variables
  pattern_prev = PAL_DATETIME_FORMAT_NULL;
  pattern_curr = PAL_DATETIME_FORMAT_NULL;
  pattern_found_flag = FALSE;
  pattern_match_count = 0;
  time_str_p = lpTimeStr;
  time_str_size = cchTime;

  if (cchTime == 0)                   // return the required buffer size
    {
      lpTimeStr = NULL;               // (lpTimeStr == NULL) represents (cchTime == 0) now.
      // That means: if (lpTimeStr == NULL) { calcualte required buffer size }
    }

  while (TRUE)
    {
      // Parsing format (http://msdn.microsoft.com/en-us/library/dd318131%28v=VS.85%29.aspx)
      switch (*lpFormat)
        {
	case 'h':   pattern_curr = PAL_DATETIME_FORMAT_HOUR12;
	  pattern_found_flag = TRUE;
	  break;
	case 'H':   pattern_curr = PAL_DATETIME_FORMAT_HOUR24;
	  pattern_found_flag = TRUE;
	  break;

	case 'm':   pattern_curr = PAL_DATETIME_FORMAT_MINUTE;
	  pattern_found_flag = TRUE;
	  break;

	case 's':   pattern_curr = PAL_DATETIME_FORMAT_SECOND;
	  pattern_found_flag = TRUE;
	  break;

	case 't':   pattern_curr = PAL_DATETIME_FORMAT_AMPM;
	  pattern_found_flag = TRUE;
	  break;

	default:    pattern_curr = PAL_DATETIME_FORMAT_NULL;
	  break;
        }   // End of switch (*lpFormat)

      if (pattern_curr != pattern_prev)
        {
	  if (
	      (pattern_found_flag == TRUE)
	      && (pattern_prev != PAL_DATETIME_FORMAT_NULL)
	      ) { // pattern found! fill string and reset matching variables.
	    time_str_p = pal_getTimeFormat_FillingPattern(
							  pattern_prev, pattern_match_count,
							  lpTime,
							  time_str_p, time_str_size,
							  lpTimeStr
							  );
	    pattern_found_flag = FALSE;
	    pattern_match_count = 0;
	  }

	  if (pattern_curr != PAL_DATETIME_FORMAT_NULL)
            {   // match a new pattern
	      pattern_match_count = 1;
            }
	  else
            {   // Not a pattern, copy it to target
	      if (lpTimeStr)
                {
		  *time_str_p = *lpFormat;
		  time_str_size--;
                }
	      time_str_p++;
            }
        }
      else
        {
	  if (pattern_curr != PAL_DATETIME_FORMAT_NULL)
            {   // continue matching the same pattern
	      pattern_match_count++;
            }
	  else
            {
	      // Not a pattern, copy it to target
	      if (lpTimeStr)
                {
		  *time_str_p = *lpFormat;
		  time_str_size--;
                }
	      time_str_p++;
            }
        }   // End of if (pattern_curr != pattern_prev)

      // End of string
      if (*lpFormat == '\0')
        {
	  break;
        }

      // Step to next format character
      pattern_prev = pattern_curr;
      lpFormat++;
    }   // End of while (TRUE)

  return (time_str_p - lpTimeStr);

 Error_NotSupported:
  errno = EINVAL;
  return 0;
}

//***************************************************************/
// pal_getDateFormat_FillingPattern(
LPTSTR PAL_absclass::pal_getDateFormat_FillingPattern(
						      PAL_DATETIME_FORMAT_PATTERN pattern,
						      int pattern_match_count,
						      const PAL_TIME_STRUCT *lpDate,
						      LPTSTR date_str_p,
						      int &date_str_size,
						      LPTSTR lpDateStr                // lpDateStr is used to detect if we need to fill the string
						      )
{
  int filling_string_size = pattern_match_count;      // size may change later

  switch (pattern)
    {
    case PAL_DATETIME_FORMAT_YEAR:
      if (pattern_match_count >= 3)           // yyyy
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%04d", lpDate->wYear % 10000);
	  filling_string_size = 4;
	}
      else if (pattern_match_count == 2)      // yy
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%02d", lpDate->wYear % 100);
	}
      else if (pattern_match_count == 1)      // y
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size,   L"%d", lpDate->wYear % 10);
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    case PAL_DATETIME_FORMAT_MONTH:
      if (pattern_match_count >= 4)           // MMMM
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%s", PAL_long_monthname[lpDate->wMonth - 1].str_date);
	  filling_string_size = PAL_long_monthname[lpDate->wMonth - 1].str_size;
	}
      else if (pattern_match_count == 3)      // MMM
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%s", PAL_short_monthname[lpDate->wMonth - 1].str_date);
	  filling_string_size = PAL_short_monthname[lpDate->wMonth - 1].str_size;
	}
      else if (pattern_match_count == 2)      // MM
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%02d", lpDate->wMonth);
	}
      else if (pattern_match_count == 1)      // M
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%d", lpDate->wMonth);
	  if (lpDate->wMonth >= 10) {
	    filling_string_size = 2;
	  }
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    case PAL_DATETIME_FORMAT_DAY:
      if (pattern_match_count >= 4)           // dddd
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%s", PAL_long_dayname[lpDate->wDayOfWeek].str_date);
	  filling_string_size = PAL_long_dayname[lpDate->wDayOfWeek].str_size;
	}
      else if (pattern_match_count == 3)      // ddd
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%s", PAL_short_dayname[lpDate->wDayOfWeek].str_date);
	  filling_string_size = PAL_short_dayname[lpDate->wDayOfWeek].str_size;
	}
      else if (pattern_match_count == 2)      // dd
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%02d", lpDate->wDay);
	}
      else if (pattern_match_count == 1)      // d
	{
	  if (lpDateStr)  swprintf(date_str_p, date_str_size, L"%d", lpDate->wDay);
	  if (lpDate->wDay >= 10)
	    {
	      filling_string_size = 2;
	    }
	}
      else
	{
	  // ***** ERROR: This should never happen
	}
      break;

    default:
      // ***** ERROR: This should never happen
      break;
    }

  // swprintf() will write at most 'date_str_size' characters into 'date_str_p'
  if (
      (lpDateStr)
      && (date_str_size < filling_string_size)
      ) {
    filling_string_size = date_str_size;    // Exactly how many buffer we used
    date_str_size -= filling_string_size;   // How many buffer we still have
  }

  return (date_str_p + filling_string_size);  // Move string pointer
}

// http://msdn.microsoft.com/en-us/library/dd318086%28VS.85%29.aspx
//***************************************************************/
// PAL_getDateFormat(
int PAL_absclass::PAL_getDateFormat(
				    LCID Locale,
				    DWORD dwFlags,
				    const PAL_TIME_STRUCT *lpDate,
				    LPCTSTR lpFormat,
				    LPTSTR lpDateStr,
				    int cchDate)
{
  PAL_DATETIME_FORMAT_PATTERN pattern_curr, pattern_prev;
  BOOL    pattern_found_flag;         // With this variable, save time from calling function to detect matching pattern
  int     pattern_match_count;
  LPTSTR  date_str_p;                 // Keep original pointer, lpDateStr, to calculate string size
  int     date_str_size;              // How many buffer we could still use

  std::string err_str("PAL_getDateFormat: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());

  // Support only what current Hobart code is using
  if (
      (Locale != PAL_LOCALE_SYS_DEFAULT)
      || (dwFlags != 0)
      ) {
    goto Error_NotSupported;
  }

  // Check parameter
  if (
      (lpDate == NULL)
      || (lpFormat == NULL)
      || ((lpDateStr == NULL) && (cchDate != 0))
      ) {
    goto Error_NotSupported;
  }

  // Initiate variables
  pattern_prev = PAL_DATETIME_FORMAT_NULL;
  pattern_curr = PAL_DATETIME_FORMAT_NULL;
  pattern_found_flag = FALSE;
  pattern_match_count = 0;
  date_str_p = lpDateStr;
  date_str_size = cchDate;

  if (cchDate == 0)                   // return the required buffer size
    {
      lpDateStr = NULL;               // (lpDateStr == NULL) represents (cchDate == 0) now.
      // That means: if (lpDateStr == NULL) { calcualte required buffer size }
    }

  while (TRUE)
    {
      // Parsing format (http://msdn.microsoft.com/en-us/library/dd317787%28v=VS.85%29.aspx)
      switch (*lpFormat)
        {
	case 'y':   pattern_curr = PAL_DATETIME_FORMAT_YEAR;
	  pattern_found_flag = TRUE;
	  break;

	case 'M':   pattern_curr = PAL_DATETIME_FORMAT_MONTH;
	  pattern_found_flag = TRUE;
	  break;

	case 'd':   pattern_curr = PAL_DATETIME_FORMAT_DAY;
	  pattern_found_flag = TRUE;
	  break;

	default:    pattern_curr = PAL_DATETIME_FORMAT_NULL;
	  break;
        }   // End of switch (*lpFormat)

      if (pattern_curr != pattern_prev)
        {
	  if (
	      (pattern_found_flag == TRUE)
	      && (pattern_prev != PAL_DATETIME_FORMAT_NULL)
	      ) { // pattern found! fill string and reset matching variables.
	    date_str_p = pal_getDateFormat_FillingPattern(
							  pattern_prev, pattern_match_count,
							  lpDate,
							  date_str_p, date_str_size,
							  lpDateStr
							  );
	    pattern_found_flag = FALSE;
	    pattern_match_count = 0;
	  }

	  if (pattern_curr != PAL_DATETIME_FORMAT_NULL)
            {   // match a new pattern
	      pattern_match_count = 1;
            }
	  else
            {   // Not a pattern, copy it to target
	      if (lpDateStr)
                {
		  *date_str_p = *lpFormat;
		  date_str_size--;
                }
	      date_str_p++;
            }
        }
      else
        {
	  if (pattern_curr != PAL_DATETIME_FORMAT_NULL)
            {   // continue matching the same pattern
	      pattern_match_count++;
            }
	  else
            {
	      // Not a pattern, copy it to target
	      if (lpDateStr)
                {
		  *date_str_p = *lpFormat;
		  date_str_size--;
                }
	      date_str_p++;
            }
        }   // End of if (pattern_curr != pattern_prev)

      // End of string
      if (*lpFormat == '\0')
        {
	  break;
        }

      // Step to next format character
      pattern_prev = pattern_curr;
      lpFormat++;
    }   // End of while (TRUE)

  return (date_str_p - lpDateStr);

 Error_NotSupported:
  errno = EINVAL;
  return 0;
}

//***************************************************************/
// PAL_getLocalTime(
void PAL_API PAL_absclass::PAL_getLocalTime(
					    PAL_PTIME_STRUCT lpSystemTime)
{
  std::string err_str("PAL_getLocalTime: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());
  struct timeval tv;
  if (gettimeofday(&tv, NULL) == 0)
    {
      struct tm thetime;
      localtime_r(&tv.tv_sec, &thetime);
      lpSystemTime->wYear = thetime.tm_year+1900;
      lpSystemTime->wMonth = thetime.tm_mon+1;
      lpSystemTime->wDayOfWeek = thetime.tm_wday;
      lpSystemTime->wDay = thetime.tm_mday;
      lpSystemTime->wHour = thetime.tm_hour;
      lpSystemTime->wMinute = thetime.tm_min;
      lpSystemTime->wSecond = thetime.tm_sec;
      lpSystemTime->wMilliseconds = (tv.tv_usec/1000)%1000;
    }
}

//***************************************************************/
// PAL_getSystemTime(
void PAL_API PAL_absclass::PAL_getSystemTime(
					     PAL_PTIME_STRUCT lpSystemTime)
{
  std::string err_str("PAL_getSystemTime: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());
  struct timeval tv;
  if (gettimeofday(&tv, NULL) == 0)
    {
      struct tm thetime;
      gmtime_r(&tv.tv_sec, &thetime);
      lpSystemTime->wYear = thetime.tm_year+1900;
      lpSystemTime->wMonth = thetime.tm_mon+1;
      lpSystemTime->wDayOfWeek = thetime.tm_wday;
      lpSystemTime->wDay = thetime.tm_mday;
      lpSystemTime->wHour = thetime.tm_hour;
      lpSystemTime->wMinute = thetime.tm_min;
      lpSystemTime->wSecond = thetime.tm_sec;
      lpSystemTime->wMilliseconds = (tv.tv_usec/1000)%1000;
    }
}

//***************************************************************/
// PAL_getTimeZoneInformation(
DWORD PAL_API PAL_absclass::PAL_getTimeZoneInformation(
						       PAL_PTIME_ZONE_STRUCT pTimeZone)
{
  memset(pTimeZone, 0, sizeof(PAL_TIME_ZONE_STRUCT));

  tzset();

  pTimeZone->Bias = timezone / 60;

  PAL_MB2WC(0, 0, tzname[0], strlen(tzname[0]) + 1, pTimeZone->StandardName, (sizeof(PAL_TIME_ZONE_STRUCT::StandardName) / sizeof(WCHAR)));

  if (daylight)
    {
      PAL_MB2WC(0, 0, tzname[1], strlen(tzname[1]) + 1, pTimeZone->DaylightName, (sizeof(PAL_TIME_ZONE_STRUCT::DaylightName) / sizeof(WCHAR)));
      pTimeZone->DaylightBias = -60;
    }

  struct timeval timeOfDay;
  gettimeofday(&timeOfDay, NULL);
  struct tm localTime;
  localtime_r(&timeOfDay.tv_sec, &localTime);
  if (localTime.tm_isdst > 0)
    {
      pTimeZone->DaylightBias = (localTime.tm_gmtoff / -60) - pTimeZone->Bias;
    }

  return 0;
}

//***************************************************************/
// PAL_MB2WC(
int PAL_absclass::PAL_MB2WC(
			    UINT UNUSED(CodePage),
			    DWORD UNUSED(dwFlags),
			    LPCSTR lpMultiByteStr,
			    int cbMultiByte,
			    LPWSTR lpWideCharStr,
			    int cchWideChar)
{
  std::string err_str("PAL_MB2WC: ");
  PAL_PRINT_LOG(PAL_WHERE_IS_LOG, err_str.c_str());
  if ( -1 == cbMultiByte )
    {
      mbstate_t mbstate;
      size_t count = mbsrtowcs(lpWideCharStr, &lpMultiByteStr, cchWideChar, &mbstate);
      if ( (size_t)-1 == count )
        {
	  // error during conversion
	  PAL_ErrorSave(errno, NULL);
	  return 0;
        }
      else
        {
	  return count;
        }
    }

  PAL_ErrorSave(
		PAL_INVALID_VAL,
		" invalid parameter");

  return 0;
}

//***************************************************************/
// PAL_WC2MB(
int PAL_absclass::PAL_WC2MB(
			    UINT UNUSED(CodePage),
			    DWORD UNUSED(dwFlags),
			    LPCWSTR lpWideCharStr,
			    int cchWideChar,
			    LPSTR lpMultiByteStr,
			    int cbMultiByte,
			    LPCSTR UNUSED(lpDefaultChar),
			    LPBOOL UNUSED(lpUsedDefaultChar))
{
  /*
    char outline[1024*4];
    sprintf(outline, "PAL_WC2MB(%ls, %d, %s, %d):",
    lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte);
    PAL_PRINT_LOG(PAL_WHERE_IS_LOG, outline);
  */
  if ( -1 == cchWideChar )
    {
      size_t count = wcstombs(lpMultiByteStr, lpWideCharStr, cbMultiByte);
      if ( (size_t)-1 == count )
        {
	  // error during conversion
	  PAL_ErrorSave(errno, NULL);
	  return 0;
        }
      else
        {
	  return count;
        }
    }

  PAL_ErrorSave(
		PAL_INVALID_VAL,
		" invalid parameter");

  return 0;
}

/// LINUX STUB  // ***** NTService related - USB device notification - Run()
//***************************************************************/
// PAL_registerDeviceNotification(
PAL_HDEVNOTIFY PAL_API PAL_absclass::PAL_registerDeviceNotification(
								    HANDLE UNUSED(hRecipient),
								    LPVOID UNUSED(NotificationFilter),
								    DWORD UNUSED(Flags)
								    )
{
  return NULL;
}

/// LINUX STUB  // ***** NTService related - USB device notification - Run(), OnStop()
//***************************************************************/
// PAL_unregisterDeviceNotification(
BOOL PAL_API PAL_absclass::PAL_unregisterDeviceNotification(
							    PAL_HDEVNOTIFY UNUSED(Handle)
							    )
{
  return FALSE;
}

/*
  Timer related function (CreateWaitableTimer(), CancelWaitableTimer(), and SetWaitableTimer()
  are only used by CRelativeTimer class (source/util/Inc/CRelativeTimer.h)

  CRelativeTimer is only used in CServiceKiller::Run() function.
  It creates a CRelativeTimer, l_timer, and wait for its signal to terminate itself
  by TerminateProcess(GetCurrProc())

  CServiceKiller is created in CNTService::HandlerEx() with i_Opcode == SERVICE_CONTROL_STOP
  g_the_timebomb is a static CServiceKiller varaible in source/util/Src/CNTService.cpp.
  CServiceKiller class is created but never run (g_the_timebomb->Run()).


  The design logic should be
  SERVICE_CONTROL_STOP --> Start CServiceKiller time bomb to detonate in 2 minutes --> Terminate current process.

*/

/// LINUX STUB
//***************************************************************/
// PAL_createWaitableTimer(
HANDLE PAL_API PAL_absclass::PAL_createWaitableTimer(
						     LPSECURITY_ATTRIBUTES UNUSED(lpTimerAttributes),
						     BOOL UNUSED(bManualReset),
						     LPCTSTR UNUSED(lpTimerName)
						     )
{
  return 0;
}

/// LINUX STUB
//***************************************************************/
// PAL_cancelWaitableTimer(
BOOL PAL_API PAL_absclass::PAL_cancelWaitableTimer(
						   HANDLE UNUSED(hTimer)
						   )
{
  return FALSE;
}

/// LINUX STUB
//***************************************************************/
// PAL_setWaitableTimer(
BOOL PAL_API PAL_absclass::PAL_setWaitableTimer(
						HANDLE UNUSED(hTimer),
						const LARGE_INTEGER *UNUSED(pDueTime),
						LONG UNUSED(lPeriod),
						void *UNUSED(pfnCompletionRoutine),
						LPVOID UNUSED(lpArgToCompletionRoutine),
						BOOL UNUSED(fResume)
						)
{
  return FALSE;
}

//***************************************************************/
// PAL_globalMemoryStatus(
void PAL_API PAL_absclass::PAL_globalMemoryStatus(
						  PAL_LPMEMORYSTATUS lpBuffer
						  )
{
	struct sysinfo si;
	sysinfo(&si);

#if 1
	//	Linux uses 'Free' RAM for caching and such, to improve the speed.
	//	The MemFree in SysInfo only indicates what Linux and the Application are not using.
	//	Linux can free the RAM it has in use when needed.
	//	Essentially, sysinfo is showing us less Free RAM than really exists.
	//	The true 'free' RAM is the total of (free + buffers + cached) in /proc/meminfo.
	//	That is what we will now return to give a more accurate representation of what exists
	//	and not freak people out because they think we are running out of RAM. [CCD 08/2014]

	std::fstream memStream;
	memStream.open("/proc/meminfo", ios_base::in);
	unsigned int i;
	long miMemTotal = 0;
	long miMemFree = 0;
	long miBuffers = 0;
	long miCached = 0;
	for (i = 0; !memStream.fail() && !memStream.eof(); ++ i)
	{
		char lineBuffer[1024] = { 0 };
		memStream.getline(lineBuffer, sizeof(lineBuffer));

		// We only need to look at the first 4 lines of this file.
		if (i < 4)
		{
			//	Parse out the name
			std::string line = lineBuffer;
			size_t nonEmptyPos = line.find_first_not_of(" \t\n");
			size_t colonPos = line.find(':');
			size_t kBPos = line.find("kB");
			std::string memName, kBytesStr;
			if (nonEmptyPos < colonPos)
			{
				memName = line.substr(nonEmptyPos, colonPos - nonEmptyPos);
				if (colonPos < kBPos)
				{
					std::string temp = line.substr((colonPos + 1), (kBPos - colonPos - 2));
					size_t nonSpacePos = temp.find_first_not_of(" ");
					if (nonSpacePos != std::string::npos)
						kBytesStr = temp.substr(nonSpacePos);
				}
			}
			if (memName == "MemTotal")
				miMemTotal = Long::valueOf(kBytesStr) * 1024;	//	To get a value in bytes.
			else if (memName == "MemFree")
				miMemFree = Long::valueOf(kBytesStr) * 1024;
			else if (memName == "Buffers")
				miBuffers = Long::valueOf(kBytesStr) * 1024;
			else if (memName == "Cached")
				miCached = Long::valueOf(kBytesStr) * 1024;
		}
	}
	memStream.close();

	lpBuffer->dwLength = sizeof(PAL_MEMORYSTATUS);
	lpBuffer->dwMemoryLoad = (si.freeram/si.totalram)*100;
	lpBuffer->dwTotalPhys = miMemTotal;
	lpBuffer->dwAvailPhys = miMemFree + miBuffers + miCached;	//	Since Linux uses 'available' RAM to run faster.
	lpBuffer->dwTotalPageFile = si.totalswap;
	lpBuffer->dwAvailPageFile = si.freeswap;
	lpBuffer->dwTotalVirtual = si.totalram + si.totalswap;
	lpBuffer->dwAvailVirtual = si.freeram + si.freeswap;
	if (Debug::isTracing("MonitorMemory"))
	{
		Debug::trace("MonitorMemory", ("********        PAL_globalMemoryStatus()        ********"));
		Debug::trace("MonitorMemory", ("miMemTotal: ") + Long::toString(miMemTotal));
		Debug::trace("MonitorMemory", (" miMemFree: ") + Long::toString(miMemFree));
		Debug::trace("MonitorMemory", (" miBuffers: ") + Long::toString(miBuffers));
		Debug::trace("MonitorMemory", ("  miCached: ") + Long::toString(miCached));
		Debug::trace("MonitorMemory", ("  totalram: ") + Long::toString(miMemTotal));
		Debug::trace("MonitorMemory", ("   freeram: ") + Long::toString((miMemFree + miBuffers + miCached)));
	}
#else
	lpBuffer->dwLength = sizeof(PAL_MEMORYSTATUS);
	lpBuffer->dwMemoryLoad = (si.freeram/si.totalram)*100;
	lpBuffer->dwTotalPhys = si.totalram;
	lpBuffer->dwAvailPhys = si.freeram;
	lpBuffer->dwTotalPageFile = si.totalswap;
	lpBuffer->dwAvailPageFile = si.freeswap;
	lpBuffer->dwTotalVirtual = si.totalram + si.totalswap;
	lpBuffer->dwAvailVirtual = si.freeram + si.freeswap;
	if (Debug::isTracing("MonitorMemory"))
	{
		Debug::trace("MonitorMemory", ("********        PAL_globalMemoryStatus()        ********"));
		Debug::trace("MonitorMemory", ("   uptime: ") + Long::toString(si.uptime));
		Debug::trace("MonitorMemory", (" totalram: ") + Long::toString(si.totalram));
		Debug::trace("MonitorMemory", ("  freeram: ") + Long::toString(si.freeram));
		Debug::trace("MonitorMemory", ("sharedram: ") + Long::toString(si.sharedram));
		Debug::trace("MonitorMemory", ("bufferram: ") + Long::toString(si.bufferram));
		Debug::trace("MonitorMemory", ("totalswap: ") + Long::toString(si.totalswap));
		Debug::trace("MonitorMemory", (" freeswap: ") + Long::toString(si.freeswap));
		Debug::trace("MonitorMemory", ("    procs: ") + Integer::toString(si.procs));
		Debug::trace("MonitorMemory", ("totalhigh: ") + Long::toString(si.totalhigh));
		Debug::trace("MonitorMemory", (" freehigh: ") + Long::toString(si.freehigh));
		Debug::trace("MonitorMemory", (" mem_unit: ") + Integer::toString(si.mem_unit));
	}
#endif
}

///////////////////////////////////////////////////
//Retrieves the file name pointed to by the symbolic
//
//@param[in] file_name name of the symbolic link
//
//@return buffer pointing to the file path.
//              caller must free this buffer.
//

char *PAL_absclass::PAL_readLinkMalloc (const char *file_name)
{
  int size = 100;
  char *buffer = NULL;

  //
  //Attempt to read the path of the file the
  //symbolic link points to if path size is
  //found to be less than the passed buffer
  //size then new buffer is allocated and
  //another attempt to read teh path is
  //made
  //
  while (1)
    {
      buffer = static_cast<char *> (realloc(buffer, size));
      memset(buffer,0,size);
      int nchars = readlink(file_name, buffer, size);
      if (nchars < 0)
        {
	  PAL_absclass::PAL_OutputDbgStr
	    ("Failed to read the symbolic link\n");

	  free (buffer);
	  return NULL;
        }
      if (nchars < size)
	return buffer;
      size *= 2;
    }
}

vector<string> PAL_API PAL_absclass::PAL_getDirectoryList(const char *path)
{
  vector<string> v;
  DIR *dh;                //directory handle
  struct dirent *file;    //a 'directory entity' AKA file
  struct stat info;       //info about the file.
  std::string err_str="PAL_getDirectoryList: ";
  err_str += path;
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, err_str.c_str() );

  dh=opendir(path);
  if (dh == NULL)
    {
      PAL_REPORT_ERROR;
    }
  else
    {
      while(( file = readdir(dh) ))
        {
	  if(file->d_name[0]!='.')
            {
	      stat(file->d_name,&info);
	      if(!S_ISDIR(info.st_mode))
                {
		  //printf("%s\n",file->d_name);
		  v.push_back(file->d_name);
                }
            }
        }
      closedir(dh);
    }

  return v;
}

void PAL_API PAL_absclass::PAL_launchBrowswer(void)
{
  char out[256];
  sprintf(out, "PAL_launchBrowswer()");
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, out);
  sprintf(out, "/usr/bin/www_cmd.sh");
  if (system(out) == -1)
    {
      sprintf(out, "PAL_launchBrowswer() Error %m");
      PAL_PRINT_LOG( PAL_ERR_LOG, out);
    }
}

bool PAL_API PAL_absclass::PAL_findWindow(const char *title)
{
  char out[256];
  sprintf(out, "PAL_findWindow(%s)", title);
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, out);
  sprintf(out, "/usr/bin/win_cmd.sh find %s", title);
  if (system(out) == -1)
    {
      sprintf(out, "PAL_findWindow(%s) Error %m", title);
      PAL_PRINT_LOG( PAL_ERR_LOG, out);
    }
  sprintf(out, "/tmp/%s", title);
  ifstream ifile(out);
  if (ifile)
    return true;
  else
    return false;
}

void PAL_API PAL_absclass::PAL_showVideoPlayer(const char *title)
{
  char out[256];
  sprintf(out, "PAL_showVideoPlayer(%s)", title);
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, out);
  sprintf(out, "/usr/bin/videoPlayer %s", title);
  if (system(out) == -1)
    {
      sprintf(out, "PAL_showVideoPlayer(%s) Error %m", title);
      PAL_PRINT_LOG( PAL_ERR_LOG, out);
    }
}

void PAL_API PAL_absclass::PAL_showWindow(const char *title)
{
  char out[256];
  sprintf(out, "PAL_showWindow(%s)", title);
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, out);
  sprintf(out, "/usr/bin/win_cmd.sh show %s", title);
  if (system(out) == -1)
    {
      sprintf(out, "PAL_showWindow(%s) Error %m", title);
      PAL_PRINT_LOG( PAL_ERR_LOG, out);
    }
}

void PAL_API PAL_absclass::PAL_hideWindow(const char *title)
{
  char out[256];
  sprintf(out, "PAL_hideWindow(%s)", title);
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, out);
  sprintf(out, "/usr/bin/win_cmd.sh hide %s", title);
  if (system(out) == -1)
    {
      sprintf(out, "PAL_hideWindow(%s) Error %m", title);
      PAL_PRINT_LOG( PAL_ERR_LOG, out);
    }
}

void PAL_API PAL_absclass::PAL_closeWindow(const char *title)
{
  char out[256];
  sprintf(out, "PAL_closeWindow(%s)", title);
  PAL_PRINT_LOG( PAL_WHERE_IS_LOG, out);
  sprintf(out, "/usr/bin/win_cmd.sh close %s", title);
  if (system(out) == -1)
    {
      sprintf(out, "PAL_closeWindow(%s) Error %m", title);
      PAL_PRINT_LOG( PAL_ERR_LOG, out);
    }
}

/***************************************************/
/********************PAL_PtrMan*********************/
/***************************************************/

/////////////////////////////////////////////////////
//Destroy the handle and based on the type of handle
//call linux specific API for clean up.  Doing it here
//allows us to cleanup as we go out of scope.
//
PAL_PtrMan::~PAL_PtrMan()
{
	std::string err_str( "~PAL_PtrMan: " );
	PAL_PRINT_LOG( PAL_WHERE_IS_LOG, err_str.c_str() );

	if (NULL != hndl)
	{

#if 0   // For Debugging
		err_str += " Destroying ";
		switch(ptr_type)
		{
			case UNASSIGNED:        err_str += "Unassigned";        break;
			case MUTEX_TYPE:        err_str += "Mutex";             break;
			case CS_TYPE:           err_str += "CritSec";           break;
			case SEM_TYPE:          err_str += "Sem";               break;
			case AUTO_EVENT_TYPE:   err_str += "Auto event";        break;
			case MANUAL_EVENT_TYPE: err_str += "Manual event";      break;
			case SYSTEM_FILE_TYPE:  err_str += "System file";       break;
			case STDIO_FILE_TYPE:   err_str += "Stdio file";        break;
			case PROCESS_ID_TYPE:   err_str += "Process ID";        break;
			case THREAD_TYPE:       err_str += "Thread";            break;
			default:                err_str += "Unknown";           break;
		}

		PAL_PRINT_LOG( PAL_INFO_LOG, err_str.c_str() );
#endif  // For Debugging
		
		//clean up file descriptors
		PAL_closefd();

		switch(ptr_type)
		{
		case CS_TYPE:
			// fall through
		case(MUTEX_TYPE):
			pthread_mutex_destroy(static_cast <PAL_PMUTEX> (hndl));
			delete(static_cast<PAL_PMUTEX> (hndl));
			break;

		case THREAD_TYPE:
			delete(static_cast<PAL_PTHREAD>(hndl));
			break;

		case(AUTO_EVENT_TYPE):
			//fall through
		case(MANUAL_EVENT_TYPE):
			//fall through
		case(SEM_TYPE): {
			PAL_PSYNC_DAT sem_dat( static_cast<PAL_PSYNC_DAT>(hndl) );

			if (( sem_dat ) && ( sem_dat->bHasMutex ))
				PAL_absclass::PAL_deleteCriticalSection( sem_dat->mutex );

			delete sem_dat;
			break;
		}

		case SYSTEM_FILE_TYPE: {
			int *fdesc_p( static_cast<int*>(hndl) );
			PAL_ASSERT( close(*fdesc_p) == 0 );
			free(fdesc_p);
			break;
		}

		case(PROCESS_ID_TYPE):
			delete (static_cast<PAL_PPROC_HNDL>(hndl));
			break;

		case(UNASSIGNED):
			break;

		default:
			PAL_absclass::PAL_PrintFormatLog(PAL_ERR_LOG, "WARN: ~PAL_PtrMan - Unknown handle type: %d", ptr_type);
			break;
		} //switch
	}

	hndl = NULL;
}

/***************************************************/
/********************PAL_SyncObj******************/
/***************************************************/

/////////////////////////////////////////////////////
//Destroy the handles and calls linux specific API
//for clean up.  Doing it in the destructor allows us
//to cleanup as we go out of scope.
//
PAL_SyncObj::~PAL_SyncObj()
{
	std::string err_str = "~PAL_SyncObj: ";

	if (NULL != sem)
	{
		PAL_PSEM psem( static_cast<PAL_PSEM>( sem ) );

		if (bIsNamed)
		{
			err_str += " Closing named object.";
			err_str += sem_name;
			PAL_PRINT_LOG(PAL_INFO_LOG, err_str.c_str());

			sem_close(psem);

			sem_unlink(sem_name);
			free(sem_name);
		}
		else
		{
			err_str += " Closing unamed object.";
			PAL_PRINT_LOG(PAL_INFO_LOG, err_str.c_str());

			sem_destroy(psem);

			delete psem;
			psem = NULL;
		}

		sem = NULL;
    }
}
#ifdef STANDALONE_EXE
/**********************************************/
int main(int argc, char *argv[])
{
  printf("PAL main\n");
	PAL_absclass::PAL_logging = PAL_NO_LOG;
//	PAL_absclass::PAL_logging = PAL_WHERE_IS_LOG|PAL_ERR_LOG|PAL_INFO_LOG;
	if (PAL_VERBOSE & PAL_absclass::PAL_logging)
		printf("Verbose on\n");
	PAL_PRINT_LOG(PAL_VERBOSE, "verbose test");
	PAL_PRINT_LOG(PAL_WARN_LOG, "warn test");
	PAL_PRINT_LOG(PAL_ERR_LOG, "errro test");
	PAL_PRINT_LOG(PAL_WHERE_IS_LOG, "where is test");

  /****************
      int *p;

      p = static_cast<int*> (PAL_absclass::PAL_initThrdDat());
      *p = 5;

     std::string err_s;


     int *kp = static_cast<int*> (PAL_absclass::PAL_initThrdDat());

     std::cout<< "sfsdfdsfdssdfdsf " << *kp << std::endl;

     *kp = 8;
     int *s = static_cast<int*> (PAL_absclass::PAL_initThrdDat());
     std::cout<< "sfsdfdsfdssdfdsf " << *s << std::endl;

     err_s += *p;
     err_s += " is the errror";
     PAL_PRINT_LOG(PAL_ERR_LOG, err_s.c_str());
  ******************/
  return 1;
}
/**********************************************/
#endif  // STANDALONE_EXE


/// Execute an external program and get the results is writes to stdout
int PAL_API PAL_absclass::PAL_executeWithResults(const std::string &command, std::string &commandOutput)
{
	int procStatus( 0 );
	FILE *fp;

	// open process
	if ( NULL != (fp = popen( command.c_str(), "r" )) )
    {
		char path[PATH_MAX];
		int status;

		while ( NULL != fgets( path, PATH_MAX, fp ) )
			commandOutput.append( path );

		status = pclose( fp );

		// Error reported by pclose()
		if ( -1 == status )
			procStatus = status;
		else
		{
			// Use macros described under wait() to inspect `status' in order
			// to determine success/failure of command executed by popen()
			if ( WIFEXITED(status) )
				procStatus = WEXITSTATUS(status);
			else
				procStatus = -1;
		}
    }

	return procStatus;
}
