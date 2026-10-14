//
// File : PAL_absclass.h
//
// Holds the declaration of linux abstraction layer APIs.
//
/// @file PAL_absclass.h
/// Holds the declaration of linux abstraction layer APIs.

#ifndef _PAL_ABSCLASS_H_
#define _PAL_ABSCLASS_H_

//#undef USING_SELECT_FOR_WAITING
#define USING_SELECT_FOR_WAITING

#include <list>
#include <vector>

#if defined( __linux__ )
#include <mutex>
#endif

#include "string.h"
#include "assert.h"

#define PAL_INTER_PROCESS_SEM           0
#define PAL_CURR_PROCESS_SEM            0

#define PAL_SEM_OPEN_FAILED             SEM_FAILED
#define PAL_OPEN_EXISTING_SEM           0

//#define PAL_PTHREAD_INFINITE          0xFFFFFFFFFFFFFFFF
#define PAL_PTHREAD_INFINITE            0xFFFFFFFF

//#define PAL_MAX_SEM_VAL               SEM_VALUE_MAX
#define PAL_SEM_MAX_VAL                 200

////////////////////////////
/// @name Log Defines
//@{
#define PAL_PRINT_LOG(z, s)             if(z & PAL_absclass::PAL_logging) PAL_absclass::PAL_log(s)
#define PAL_PRINT_FORMAT_LOG1(z, fmt, arg1) \
    if (z & PAL_absclass::PAL_logging) PAL_absclass::PAL_log_printf(fmt, arg1)
#define PAL_PRINT_FORMAT_LOG2(z, fmt, arg1, arg2) \
    if (z & PAL_absclass::PAL_logging) PAL_absclass::PAL_log_printf(fmt, arg1, arg2)
#define PAL_PRINT_FORMAT_LOG3(z, fmt, arg1, arg2, arg3) \
    if (z & PAL_absclass::PAL_logging) PAL_absclass::PAL_log_printf(fmt, arg1, arg2, arg3)
#define PAL_PRINT_FORMAT_LOG4(z, fmt, arg1, arg2, arg3, arg4) \
    if (z & PAL_absclass::PAL_logging) PAL_absclass::PAL_log_printf(fmt, arg1, arg2, arg3, arg4)
#define PAL_LOG_ACCESS_MODE             std::wfstream::in|std::wfstream::out|std::wfstream::app
#if defined ( __linux__ )
#define PAL_LOG_FILE_PATH               "/home/Hobart/AccessScale/PAL_log.log"	//"/var/log/hobart/PAL_log.log"
#else
#define PAL_LOG_FILE_PATH               "PAL_log.log"
#endif
#define PAL_VERBOSE                     0XFFFFFFFF
#define PAL_NO_LOG                      0x00000000
#define PAL_WHERE_IS_LOG                0x00000001
#define PAL_INFO_LOG                    0x00000002
#define PAL_WARN_LOG                    0x00000004
#define PAL_ERR_LOG                     0x00000008
#define PAL_FUNC_LOG                    0x00000010      // Function Entry/Leave
//@}

/// The maximum number of objs that can wait for something
#define PAL_MAXIMUM_WAIT_OBJECTS        64

enum PAL_EV_CALLER_TYPE
{
    PAL_EV_CALLER_WAIT=0
    ,PAL_EV_CALLER_RESET
};

/// Windows equivalent functionality. Implements windows equivalent API
/// routine of the WIN32 functions used by the HLX suite.
class PAL_absclass
{
    public:

#if 0
        /// Structure for getting file parameters
        typedef struct _PAL_FILE_PARAM_
        {
#if defined(PAL_WIN32_RUN)
            //HANDLE    win_handle;             // return value of CreateFile()

            LPCTSTR     win_lpFileName;
            DWORD       win_dwDesireAccess;
            DWORD       win_dwShareMode;
            LPSECURITY_ATTRIBUTES   win_lpSecurityAttributes;
            DWORD       win_dwCreationDisposition;
            DWORD       win_dwFlagsAndAttributes;
            HANDLE      win_hTemplateFile;

            DWORD       win_dwByteOfReadWrite;
#elif   defined( __linux__ )
            //int       linux_fdesc;            // return value of open()
            /// The path for the file.
            LPCTSTR     linux_pathname;
            /// The flags for the file.
            int         linux_flags;
            /// The permission mode for the file.
            mode_t      linux_mode;
            /// Bytes.
            ssize_t     linux_byteOfReadWrite;
#else
#error  "Define FILE structure for the system"
#endif
        } PAL_FILE_PARAM;
#endif

        /// @name Exception
        /// Exception functionality.
        //@{
        /// Raises an exception in the calling thread.
        static void PAL_API PAL_raiseException(
                /// [in] An user defined exception code of the exception being raised.
                DWORD dwExceptionCode,
                /// [in] The exception flags.
                DWORD dwExceptionFlags,
                /// [in] The number of arguments in the lpArguments array.
                DWORD nNumberOfArguments,
                /// [in] An array of arguments, can be NULL.
                const ULONG_PTR *lpArguments
                );
        //@}

        /// @name Log File
        /// PAL log file related.
        //@{
        /// Write the string to the log
        static void PAL_log(
                /// [in] The string to place in the log
                const char *str
                );
        /// Write the wide string to the log
        static void PAL_log(
                /// [in] The wchar_t string to place in the log
                const wchar_t *str
                );
        /// Write the given formated parameters to the log
        static void PAL_log_printf(
                /// [in] The template string to format string
                const char *format,
                /// [in] Argument list
                ...
                );
        /// Write the given formated parameters to the log at the specified level
        static void PAL_PrintFormatLog(
                /// [in] Debugging Zone
                DWORD log_zone,
                /// [in] The template string to format string
                const char *format,
                /// [in] Argument list
                ...
                );
        //@}

        /// @name Threading
        /// Thread related functions
        //@{
        /// Sets the physical processor that this thread should run on.
        static DWORD PAL_API PAL_setThreadIdealProcessor(
                /// [in] The handle of the thread to set
                HANDLE hThread,
                /// [in] The number of the preferred processor for the thread.
                DWORD dwIdealProcessor
                );

        /// Resumes a paused thread.
        static DWORD PAL_API PAL_resumeThread(
                /// The handle of the thread to resume.
                HANDLE hThread
                );

        /// Sets a thread's priority.
        /// @return TRUE on success.
        static BOOL PAL_API PAL_setThreadPriority(
                /// The handle of the thread to set.
                HANDLE hThread,
                /// The new priority of the thread.
                int nPriority
                );

        /// Creates a thread.
        static HANDLE PAL_createThread(
#if defined ( __linux__ )
                /// [in] UNUSED-A pointer to a SECURITY_ATTRIBUTES structure.
#else
                /// [in] A pointer to a SECURITY_ATTRIBUTES structure that
                /// determines whether the returned handle can be inherited
                /// by child processes, can be NULL.
#endif
                PAL_PTR_SEC_ATTRIBS lpThreadAttributes,
#if defined ( __linux__ )
                /// [in] UNUSED-The initial size of the stack, in bytes.
#else
                /// The initial size of the stack, in bytes.
#endif
                SIZE_T dwStackSize,
                /// [in] A pointer to the function to be executed by the thread.
                PAL_THREAD_START lpStartAddress,
                /// [in] A pointer to a variable to be passed to the thread.
                LPVOID lpParameter,
#if defined ( __linux__ )
                /// [in] UNUSED-The flags that control the creation of the thread.
#else
                /// [in] The flags that control the creation of the thread.
#endif
                DWORD dwCreationFlags,
#if defined ( __linux__ )
                /// [out] UNUSED-A pointer to a variable that receives the thread identifier.
#else
                /// [out] A pointer to a variable that receives the thread identifier.
#endif
                LPDWORD lpThreadId);

        /// Creates a thread.
        static uintptr_t PAL_beginthreadex(
                /// [in] A pointer to a SECURITY_ATTRIBUTES structure that
                /// determines whether the returned handle can be inherited
                /// by child processes, can be NULL.
                VOID *security,
                /// The initial size of the stack, in bytes.
                UINT stack_size,
                /// [in] A pointer to the function to be executed by the thread.
                UINT (__stdcall * start_address)(void *),
                /// [in] Argument list to be passed to a new thread or NULL.
                VOID *arg_list,
                /// [in] Initial state of a new thread, running or suspended.
                UINT init_flag,
                /// [out] Points to a 32-bit variable that receives the thread identifier.
                UINT *thread_addr
                );

        /// Terminates a thread created by PAL_beginthreadex.
        static void PAL_endthreadex(
                /// [in] Thread exit code.
                unsigned retval
                );

        /// Retrieves the termination status of the specified thread.
        static BOOL PAL_API PAL_getExitCodeThread(
                /// [in] A handle to the thread
                HANDLE hThread,
                /// [out] A pointer to a variable to receive the thread termination status.
                LPDWORD lpExitCode
                );

        /// Retrieves timing information for the specified thread.
        static BOOL PAL_API PAL_getThreadTimes(
                /// [in] A handle to the thread
                HANDLE hThread,
                /// [out]
                PAL_LPFILETIME lpCreationTime,
                /// [out]
                PAL_LPFILETIME lpExitTime,
                /// [out]
                PAL_LPFILETIME lpKernelTime,
                /// [out]
                PAL_LPFILETIME lpUserTime
                );

        /// Terminates a thread created by PAL_createThread.
        static BOOL PAL_API PAL_terminateThread(
                /// [in] A handle to the thread
                HANDLE hThread,
                /// [in] Thread exit code.
                DWORD dwExitCode
                );
        //@}

        /// @name Atomic
        /// Atomic functionality
        //@{
        /// Wrapper used to provide atomic incremenet on the linux platform.
        /// @return Incremented value
        static long PAL_InterlockedIncrement(
                long *val       /// [in,out] Pointer to value to increment atomically
                );
        /// Wrapper used to provide atomic decrement on the linux platform.
        /// @return Decremented value
        static long PAL_InterlockedDecrement(
                long *val       /// [in,out] Pointer to value to decrement atomically
                );
        //The function sets this variable to val, and returns its prior value.
        /// Wrapper used to provide atomic exchange on the linux platform.
        /// @return Original value of dest
        static long PAL_InterlockedExchange(
                long *dest,     ///< [in,out] A pointer to the value to be exchanged.
                long val        ///< [in] The value to be exchanged with the value pointed to by dest.
                );
        //@}

        /// @name Process
        /// Process functionality
        //@{
        /// Retrieves and returns the handle to the current process.
        /// @return Handle to the PAL_PtrMan class.
        static HANDLE PAL_getCurrProc(void);

        /// Retrieves the process affinity mask for the specified process
        /// and the system affinity mask for the system.
        /// @return TRUE on success.
        static BOOL PAL_API PAL_getProcessAffinityMask(
                /// [in] A handle to the process whose affinity mask is requested.
                HANDLE hProcess,
                /// [out] A pointer that receives the affinity mask for the specified process.
                PDWORD_PTR lpProcessAffinityMask,
                /// [out] A pointer that receives the affinity mask for the system.
                PDWORD_PTR lpSystemAffinityMask
                );

        /// Terminates the specified process and all of its threads.
        /// @return TRUE on success
        static BOOL PAL_API PAL_terminateProcess(
                /// [in] A handle to the process to be terminated.
                HANDLE hProcess,
                /// [in] The exit code to be used by the process and threads terminated as a result of this call.
                UINT uExitCode
                );
        //@}

        /// @name Critical Section
        /// Critical Section functionality.
        //@{
        /// Initializes a critical section object.
        static void PAL_API PAL_initializeCriticalSection(
                /// [out] the pointer to the new CS.
                PAL_LPCRITICAL_SECTION lpCriticalSection);
        /// Waits for the specified critical section object and then returns.
        static void PAL_API PAL_enterCriticalSection(
                /// [in] the pointer to the CS to wait on.
                PAL_LPCRITICAL_SECTION lpCriticalSection);
        /// Releases the specified critical section.
        static void PAL_API PAL_leaveCriticalSection(
                /// [in] the pointer to the CS to wait on.
                PAL_LPCRITICAL_SECTION lpCriticalSection);
        /// Frees resources used by the specified critical section
        static void PAL_API PAL_deleteCriticalSection(
                /// [in] the pointer to the CS to remove
                PAL_LPCRITICAL_SECTION lpCriticalSection);
        //@}

        /// Waits until the specified object is in the signaled state
        /// or the time-out interval elapses.
        /// @return If the function succeeds, the return value indicates
        /// the handle that caused the function to return.
        static DWORD PAL_waitForSingleObj(
                /// [in] A handle to the object.
                HANDLE hndl,
                /// [in] The time-out interval, in milliseconds.
                DWORD time);

        /// Used by PAL_waitForMultipleObjs.  Acts as an independent
        /// thread for each handle that PAL_waitForMultipleObjs attempts
        /// to wait on.  Only one thread per handle is maintained by
        /// updating appropriate variables in the PtrMan handle.
        /// Method calls waitForSingleObj internally and touches the
        /// fifo upon return.
        /// @return 1 upon success and 0 upon failure.
        static DWORD PAL_processThrdHndl(
                /// [in] Handle to be used with call to PAL_waitForSingleObj
                HANDLE hndl
                );
        /// Waits until one or all of the specified objects are in the
        /// signaled state or the time-out interval elapses.
        /// @return If the function succeeds, the return value indicates
        /// the handle(s) that caused the function to return.
        static DWORD PAL_waitForMultipleObjs(
                /// [in] The number of object handles in the array pointed
                /// to by lpHandles.
                DWORD nCount,
                /// [in] An array of object handles.
                const HANDLE *lpHandles,
                /// [in] If this parameter is TRUE, the function returns
                /// when the state of all objects in the lpHandles array
                /// is signaled.
                BOOL bWaitAll,
                /// [in] The time-out interval, in milliseconds.
                DWORD dwMilliseconds
                );
#ifdef USING_SELECT_FOR_WAITING

       /// Used by PAL_processSelect.  Searchs the PtrMan handle list 
		/// for a given file descriptor.
        /// @return Handle containing the fd upon success or NULL upon failure.
		static HANDLE PAL_findPtrForfd(
				/// [in] handle list which needs to be searched for the given fd
				const HANDLE *hndl, 
				/// [in] Size of handle list
				const int32_t hndl_cnt, 
				/// [in] fd being searched. 
				const int32_t fd_2_find,
				/// [in,out] index into the hndl array corresponding to the fd 
				/// being searched
				int32_t &idx);

        ///We get called when we have one or more active fds for the thread.  We search for
        ///the active fds and call PAL_waitForSingleObject for the PtrMan.  Return value
        ///depends on the wait_all flag.
        //
        ///[in] rd_fds read fds set.
        ///[in] hndls Pointers to hanldes that the thread intends to wait on
        ///[in] act_fd_cnt return value of the select.  At this point it is
        ///                      guarntied to be active fd count.
        ///@param[in,out] max_fd the maximum value of the fd in this fd set.
        ///@param[in,out] fds_in_set Count of the handles in the fd set.
        ///@param[in] b_wait_all wheather the thread intends to wait for all the handle
        ///                      or just the first one that is available.
        ///@param[out] val_to_ret holds WAIT_OBJECT_0+active handle, WAIT_FAILED on error,
        ///                      WAIT_TIMEOUT on time out or n_cnt if b_wait_all is set and all
        ///                      handles are available.
        ///@param[out] tmp_act_lst list of active fds removed from fd_set when wait_all is set
        ///@param[in] n_cnt Count of the handles the thread intends to wait on
        //
        ///@return TRUE if the value in val_to_ret should be simply returned back to the
        ///              the caller, FALSE if we are not doen waiting.
        static bool PAL_processSelectRetVal(fd_set &rd_fds, const HANDLE *hndls,
                const int32_t max_fd, DWORD &fds_in_set, const bool b_wait_all, 
				DWORD &val_to_ret, std::vector<int32_t> &tmp_act_lst, 
				std::vector<int32_t> &tmp_inact_lst);

		///Initializes the inactive fd lists with fds belonging to all handles passed in.
        //
        ///[in] hndls Pointers to hanldes that the thread intends to wait on
        ///[in,out] max_fd value of the largest fd in the set.
        ///[in,out] rd_fds read fds set.
        ///@param[in] n_cnt Count of the handles the thread intends to wait on
        static DWORD PAL_initMultiObj(const HANDLE *hndls, const DWORD n_cnt, 
												std::vector<int32_t> &inact_lst);

		///Initializes the fd set based on the current contents of the dynamic array.
		//
		///@param[in] inactive_hndls inactive fds. Used while waiting on all handles sent
		///                          to waitformultiobj
		///@param[in,out] max_fd value of the largest fd in the set.
		///@param[in,out] rd_fds read fds set.
		static DWORD PAL_initfdSet(std::vector<int32_t> &inactive_hndls,
								        		int32_t &max_fd, fd_set &rd_fds);
#endif


        /// @name Mutex
        /// Mutex functionality
        //@{
        /// Creates a Mutex object
        /// @return A handle to the new Mutex object
        static HANDLE PAL_createMutex(
#if defined ( __linux__ )
                /// [in] UNUSED-A pointer to Attributes.
#else
                /// [in] A pointer to Attributes.
#endif
                PAL_PMUTEX_ATTRIB attrib,
#if defined ( __linux__ )
                /// [in] UNUSED-TRUE Specifies the caller is the initial owner of the thread.
#else
                /// [in] TRUE Specifies the caller is the initial owner of the thread.
#endif
                BOOL owner,
                /// [in] The name of the mutex, can be ""
                LPCTSTR mutex_name
                );
        /// Releases the mutex object.
        /// @return TRUE if success.
        static BOOL PAL_releaseMutex(
                /// [in] Handle to the mutex.
                HANDLE hndl
                );
        //@}

        /// @name Semaphore
        /// Semaphore functionality.
        //@{
        /// Creates a named semaphore.
#if defined ( __linux__ )
        /// There is no wide char equivalent
        /// semphore API available so there is a conversion problem that
        /// we face while using semaphore name.
#endif      // __linux__
        /// @return a handle to the semaphore object
        static HANDLE PAL_createSemaphore(
                /// [in] The name of the new semaphore.
                LPCTSTR name,
                /// [in] The initial value of the new semaphore.
                LONG init_cnt,
                /// [in] The maximum value of the new semaphore.
#if defined ( __linux__ )
                /// Only used for parameter verification in linux.
#endif      // __linux__
                LONG max_cnt,
                /// [in] The attributes for the new semaphore.
#if defined ( __linux__ )
                /// Ignored in linux
#endif      // __linux__
                PAL_PSEM_ATTRIB attrib
                );
        /// Increments a semaphore by ammount.
        /// @return BOOL TRUE if success else FALSE.
        static BOOL PAL_incSemaphore(
                /// [in] The handle of the semaphore
                HANDLE hndl,
                /// [in] Ammount to increment
                LONG cnt,
                /// [in] Previous value of the semaphore
                LPLONG prev_cnt
                );
        //@}

        /// @name Event
        /// Event functionality.
        //@{
        /// Initializes an event object.
#if defined ( __linux__ )
        /// In linux we emulate this by creating a semaphore.
#endif      // __linux__
        /// @return Handle to the event or NULL upon error.
        static HANDLE PAL_createEvent(
                /// [in] Event name.
                LPCTSTR l_name,
                /// [in] FALSE indicates request for event that auto
                /// resets. Manual resets are not supported.
                BOOL reset_type,
                /// [in] TRUE indicates non-blocking
#if defined ( __linux__ )
                /// or semaphore value of 0 for the event.
#endif      // __linux__
                BOOL init_state,
                /// [in] Is always NULL
                PAL_PEVENT_ATTRIB attrib
                );
        /// Singals the event in order to release obj(s) waiting on the
        /// the event.
        /// @return TRUE if success else failure.
        static BOOL PAL_setEvent(
                /// [in] Handle containing the event
                HANDLE hndl
                );
        /// Resets the event to a non-signaled state.
        /// @return TRUE if success else failure.
        static BOOL PAL_resetEvent(
                /// [in] Handle containing the event handle.
                HANDLE hndl);

        /// Pulses the event by signaling the event object and the reseting
        /// it to non-signal state after releasing appropriate number of
        /// waiting threads. If no threads are waiting then this routine has
        /// no effect.
        ///
        /// If no thread is waiting the event is simply reset back.
        ///
        /// Manual event releases all waiting threads only.  Threads
        /// arriving later are bloecked.
        ///
        /// Auto event releases only one thread.
        ///
        /// @return TRUE if success else failure.
        static BOOL PAL_pulseEvent(
                /// [in] Handle containing the event handle.
                HANDLE hndl);
        //@}

        /// @name Handle
        /// Handle specific routines.
        //@{
        /// Duplicates an object handle.
        //
        /// @return FALSE on failure.
        static BOOL PAL_dupHandle(
                /// [in] Process to which orig handle belongs.
                HANDLE src_proc,
                /// [in] Handle to copy.
                HANDLE src_hndl,
                /// [in] The proc receiving the duplicate handle.
                HANDLE dest_proc,
                /// [out] A pointer to a variable that receives the duplicate handle.
                LPHANDLE dest_hndl,
                /// [in] UNUSED-The access requested for the new handle.
                DWORD desired_access,
                /// [in] UNUSED-A variable that indicates whether the handle is inheritable.
                BOOL inherit_hndl,
                /// [in] UNUSED-Optional actions, can be 0.
                DWORD opt
                );
        /// Closes open handle by deleting it. PAL_PtrMan destructor
        /// handles the details of destruction and clean-up based on
        /// handle type.
        /// @return TRUE if success else failure.
        static BOOL PAL_closeHandle(
                /// [in] Handle to close.
                HANDLE hndl);
        //@}

        /// @name Debug
        /// Debug output related 
        //@{
        /// Used to write an ASCII string message to the PAL Log
        static void PAL_OutputDbgStrA(const
                /// [in] The ASCII string to write to the PAL LOG
                char *str
                );

        /// Used to write a message to the PAL Log
        static void PAL_OutputDbgStr(
                /// [in] The string to write to the PAL LOG
                const TCHAR *str
                );

        /// Used to write a message to the PAL Log
        static void PAL_OutputDbgStr(
                /// [in] The string to write to the PAL LOG
                const std::string &str
                );
        //@}

        /// @name File
        /// File functionality.
        //@{
        /// Get file size
        /// @return TRUE on Success, otherwise FALSE.
        static BOOL     PAL_getFileSize(
                /// [in] The filename to retreive the size of
                const char *name,
                /// [in] a pointer to an int to hold the filesize
                int *fileSize
                );

        /// Creates or opens a file or I/O device.
        /// @return A handle to the file or NULL if error.
        static HANDLE   PAL_createFile(
                /// [in] The name of the file or device to be created or opened.
                LPCSTR name,
                /// [in] The requested access to the file or device.
                DWORD access_right,
                /// [in] The requested sharing mode of the file or device.
                DWORD share_mode,
#if defined ( __linux__ )
                /// [in] UNUSED-A pointer to a SECURITY_ATTRIBUTES structure.
#else
                /// [in] A pointer to a SECURITY_ATTRIBUTES structure.
#endif
                LPSECURITY_ATTRIBUTES secure_attr_p,
                /// [in] An action to take on a file or device that exists or does not exist.
                DWORD disposition,
                /// [in] The file or device attributes and flags.
                DWORD flag_and_attribute,
#if defined ( __linux__ )
                /// [in] UNUSED-A valid handle to a template file with the GENERIC_READ access right.
#else
                /// [in] A valid handle to a template file with the GENERIC_READ access right.
#endif
                /// The template file supplies file attributes and extended attributes
                /// for the file that is being created.
                HANDLE template_file
                    );

        /// Creates or opens a file or I/O device.
#if defined ( __linux__ )
        /// This function is a wrapper for the other PAL_createFile, existing only
        /// to convert LCTSTR to std:string for LINUX
#endif
        /// @return A handle to the file or NULL if error.
        static HANDLE   PAL_createFile(
                /// [in] The name of the file or device to be created or opened.
                LPCTSTR name,
                /// [in] The requested access to the file or device.
                DWORD access_right,
                /// [in] The requested sharing mode of the file or device.
                DWORD share_mode,
#if defined ( __linux__ )
                /// [in] UNUSED-A pointer to a SECURITY_ATTRIBUTES structure.
#else
                /// [in] A pointer to a SECURITY_ATTRIBUTES structure.
#endif
                LPSECURITY_ATTRIBUTES secure_attr_p,
                /// [in] An action to take on a file or device that exists or does not exist.
                DWORD disposition,
                /// [in] The file or device attributes and flags.
                DWORD flag_and_attribute,
#if defined ( __linux__ )
                /// [in] UNUSED-A valid handle to a template file with the GENERIC_READ access right.
#else
                /// [in] A valid handle to a template file with the GENERIC_READ access right.
#endif
                /// The template file supplies file attributes and extended attributes
                /// for the file that is being created.
                HANDLE template_file
                    );

        /// Read data from file
        /// @return: TRUE on Success, otherwise FALSE.
        static BOOL PAL_readFile(
                /// [in] File handle.
                HANDLE hndl,
                /// [in] Pointer to buffer receiving the data read from a file.
                LPVOID buf_p,
                /// [in] Number of bytes to be read.
                DWORD bytes_to_read,
                /// [out] Pointer to variable receiving the number of bytes read (Synchronous read).
                LPDWORD bytes_read_p,
#if defined ( __linux__ )
                /// UNUSED-A pointer to an OVERLAPPED structure
                void *p
#else
                /// A pointer to an OVERLAPPED structure is required if the hFile parameter was opened with FILE_FLAG_OVERLAPPED, otherwise it can be NULL.
                PAL_POVERLAPPED overlap_p
#endif
                );
        /// Write data to file
        /// @return: TRUE if success else failure.
        static BOOL PAL_writeFile(
                /// [in] File handle.
                HANDLE hndl,
                /// [in] Pointer to buffer containing the data to write to a file.
                LPCVOID buf_p,
                /// [in] Number of bytes to be written.
                DWORD bytes_to_write,
                /// [out] Pointer to variable receiving the number of bytes written (Synchronous write)
                LPDWORD bytes_written_p,
#if defined ( __linux__ )
                /// UNUSED-A pointer to an OVERLAPPED structure
                void *p
#else
                /// A pointer to an OVERLAPPED structure is required if the hFile parameter was opened with FILE_FLAG_OVERLAPPED, otherwise it can be NULL.
                PAL_POVERLAPPED overlap_p
#endif
                );

        /// Determines whether the operating system can retrieve version
        /// information for a specified file.
        /// @return If the function succeeds, the return value is the size,
        /// in bytes, of the file's version information.
        static DWORD PAL_getFileVersionInfoSize (
                /// [in] Pointer to a null-terminated string that specifies
                /// the name of the file of interest.
                LPCTSTR lptstrFilename,
                /// [out] Pointer to a variable that the function sets to zero.
                LPDWORD lpdwHandle
                );

        /// Retrieves version information for the specified file.
        /// @return If the function succeeds, the return value is nonzero.
        static BOOL PAL_getFileVersionInfo(
                /// [in] Pointer to a null-terminated string that specifies
                /// the name of the file of interest.
                LPCTSTR lptstrFilename,
                /// This parameter is ignored.
                DWORD dwHandle,
                /// [in] Specifies the size, in bytes, of the buffer pointed
                /// to by the lpData parameter.
                DWORD dwLen,
                /// [out] Pointer to a buffer that receives the file-version
                /// information.
                LPVOID lpData
                );

        /// Retrieves the size of the specified file, in bytes.
        /// @return If the function succeeds, the return value is the
        /// low-order doubleword of the file size
        static DWORD PAL_API PAL_getFileSize(
                /// [in] A handle to the file.
                HANDLE hFile,
                /// [out] A pointer to the variable where the high-order
                /// doubleword of the file size is returned.
                LPDWORD lpFileSizeHigh
                );

        /// Moves the file pointer of the specified file.
        /// @return If the function succeeds and lpDistanceToMoveHigh is NULL,
        /// the return value is the low-order DWORD of the new file pointer.
        static DWORD PAL_API PAL_setFilePointer(
                /// [in] A handle to the file.
                HANDLE hFile,
                /// [in] The low order 32-bits of a signed value that
                /// specifies the number of bytes to move the file pointer.
                LONG lDistanceToMove,
                /// [in,out] A pointer to the high order 32-bits of the
                /// signed 64-bit distance to move.
                PLONG lpDistanceToMoveHigh,
                /// [in] The starting point for the file pointer move.
                DWORD dwMoveMethod
                );

        /// Update the access and modification times of the specified file.
        /// @return TRUE if success else FALSE.
        static BOOL PAL_API PAL_touch(
                HANDLE hFile
                );

        /// Lock the file according to the comman flag command
        /// @return 0 on success -1 on error
        static int PAL_API PAL_lockF(
                HANDLE hFile,
                int command,
                int offset);

        /// Unlock the file according to the comman flag command
        /// @return 0 on success -1 on error
        static int PAL_API PAL_unlockF(
                HANDLE hFile,
                int command,
                int offset);


        /// Cancel all pending I/O operation that are issued by the calling thread.
        /// @return: TRUE if success else failure.
        static BOOL PAL_cancelIo(
                /// Pointer to PAL_PtrMan handle.
                HANDLE hndl
                );

        /// Deletes an existing file.
        /// @return If the function succeeds, the return value is nonzero.
        static BOOL PAL_API PAL_deleteFile(
                /// [in] The name of the file to be deleted.
                LPCTSTR lpFileName);

        /// @name Directory
        /// Directory functionality.
        //@{
        ///
#if defined ( __linux__ )   // only needed on linux
        /// Find the actual path and filename for the given symbolic link
        /// @return The physical path and filename the given link pointed to
        static char *PAL_readLinkMalloc (
                const char *file_name       ///< a string of a symbolic link name to dereference
				);
#endif      // __linux__
        /// List all files in a directory
        /// @return A vector of strings of all the filenames in the given directory
        static std::vector<std::string> PAL_API PAL_getDirectoryList(
                ///
                const char *path);
        /// Set the current working directory to the given wide string
        static BOOL PAL_API PAL_setCurrentDirectoryW(
                ///
                LPCTSTR lpPathName);
        /// Set the current working directory to the given string
        static BOOL PAL_API PAL_setCurrentDirectoryA(
                ///
                LPCSTR lpPathName);
        /// Retrieves the current directory for the current process.
        static DWORD PAL_API PAL_getCurrentDirectoryW(
                /// [in] The length of the buffer for the current
                /// directory string, in TCHARs.
                DWORD nBufferLength,
                /// [out] A pointer to the buffer that receives the current
                /// directory string. This null-terminated string specifies
                /// the absolute path to the current directory.
                LPTSTR lpBuffer
                );
        /// Retrieves the current directory for the current process.
        static DWORD PAL_API PAL_getCurrentDirectoryA(
                /// [in] The length of the buffer for the current
                /// directory string, in TCHARs.
                DWORD nBufferLength,
                /// [out] A pointer to the buffer that receives the current
                /// directory string. This null-terminated string specifies
                /// the absolute path to the current directory.
                LPSTR lpBuffer
                );
#ifndef PAL_WIN32_RUN   // we don't need this on Windows
        /// Breaks a given filename down to it's path, base filename and extension
        static void PAL_API PAL_wsplitpath(
                /// [in] The full path to break down.
                const wchar_t *inpath,
                /// [out] Optional drive letter (this doesn't make sense on Linux).
                wchar_t *drv,
                /// [out] Optional directory path, including trailing slash.
                wchar_t *dir,
                /// [out] Base filename (no extension).
                wchar_t *fname,
                /// [out] Optional filename extension, including leading period (.)
                wchar_t *ext );
#endif      // PAL_WIN32_RUN
        //@}

        /// @name Timer
        /// Timer functionality.
        //@{
        /// Creates or opens a waitable timer object.
        /// @return If the function succeeds, the return value is a handle
        /// to the timer object.
        static HANDLE PAL_API PAL_createWaitableTimer(
                /// [in] A pointer to a SECURITY_ATTRIBUTES structure that
                /// specifies a security descriptor for the new timer object
                /// and determines whether child processes can inherit the
                /// returned handle.
                LPSECURITY_ATTRIBUTES lpTimerAttributes,
                /// [in] If this parameter is TRUE, the timer is a
                /// manual-reset notification timer.
                BOOL bManualReset,
                /// [in] The name of the timer object. Can be NULL.
                LPCTSTR lpTimerName
                );

        /// Sets the specified waitable timer to the inactive state.
        /// @return If the function succeeds, the return value is nonzero.
        static BOOL PAL_API PAL_cancelWaitableTimer(
                /// [in] A handle to the timer object.
                HANDLE hTimer);

        /// Activates the specified waitable timer.
        /// @return If the function succeeds, the return value is nonzero.
        static BOOL PAL_API PAL_setWaitableTimer(
                /// [in] A handle to the timer object.
                HANDLE hTimer,
                /// [in] The time after which the state of the timer is to
                /// be set to signaled, in 100 nanosecond intervals.
                const LARGE_INTEGER *pDueTime,
                /// [in] The period of the timer, in milliseconds.
                LONG lPeriod,
#if defined ( __linux__ )
                /// [in] A pointer to an optional completion routine.
                void *pfnCompletionRoutine,
#else
                /// [in] A pointer to an optional completion routine.
                PAL_PTIMERAPCROUTINE pfnCompletionRoutine,
#endif
                /// [in] A pointer to a structure that is passed to the
                /// completion routine.
                LPVOID lpArgToCompletionRoutine,
                /// [in] If this parameter is TRUE, restores a system in
                /// suspended power conservation mode when the timer state
                /// is set to signaled.
                BOOL fResume
                    );
        //@}

        /// @name Module
        /// Module functionality.
        //@{
        /// Retrieves a module handle for the specified module.
        /// @return If the function succeeds, the return value is a handle
        /// to the specified module.
        static PAL_HMODULE PAL_API PAL_getModuleHandle(
                /// [in] The name of the loaded module.
                LPCTSTR lpModuleName);

        /// Retrieves the fully-qualified path for the file that contains
        /// the specified module.
        /// @return If the function succeeds, the return value is the length
        /// of the string that is copied to the buffer, in characters, not
        /// including the terminating null character.
        static DWORD PAL_API PAL_getModuleFileName(
                /// [in] A handle to the loaded module whose path is being requested.
                PAL_HMODULE hModule,
                /// [out] A pointer to a buffer that receives the
                /// fully-qualified path of the module.
                LPTSTR lpFilename,
                /// [in] The size of the lpFilename buffer, in TCHARs.
                DWORD nSize);
        //@}

        /// @name Device
        /// Device functionality
        //@{
        /// Registers the device or type of device for which a window will
        /// receive notifications.
        /// @return If the function succeeds, the return value is a device
        /// notification handle.
        static PAL_HDEVNOTIFY PAL_API PAL_registerDeviceNotification(
                /// [in] A handle to the window or service that will receive
                /// device events for the devices specified in the
                /// NotificationFilter parameter.
                HANDLE hRecipient,
                /// A pointer to a block of data that specifies the type of
                /// device for which notifications should be sent.
                LPVOID NotificationFilter,
                /// This parameter can be one of several values.
                DWORD Flags
                );

        /// Closes the specified device notification handle.
        /// @return If the function succeeds, the return value is nonzero.
        static BOOL PAL_API PAL_unregisterDeviceNotification(
                /// [in] Device notification handle returned by the
                /// RegisterDeviceNotification function.
                PAL_HDEVNOTIFY Handle
                );
        //@}

        /// @name Error
        /// Error handling routines.
        //@{
        /// Returns the calling thread's last-error code.
        /// @return Last error code recorded for the thread.
        static DWORD PAL_getLastError(void);
        /// Generates simple tones on the speaker.
        /// @return FALSE on failure.
        static BOOL PAL_beep(
                /// [in] The frequency of the sound, in hertz.
                DWORD freq,
                /// [in] The duration of the sound, in milliseconds.
                DWORD duration
                );
        /// Plays a simple beep. (If the sound card it not available, the sound is generated using the speaker)
        static BOOL PAL_API PAL_simpleBeep();
        //@}
        //

        /// @name Time
        /// Time related functions.
        //@{
        /// Suspends the execution of the current thread until the time-out interval elapses.
        static VOID PAL_sleep(
                /// [in] time to sleep in milliseconds
                DWORD dwMilliseconds
                );
        /// Returns the number of cpu cycles since boot-up.
#if defined ( __linux__ )
        // Y2K28/Y2038 problem (http://en.wikipedia.org/wiki/Year_2038_problem)
        // -2147483648 ~ 2147483647. Change int to uint push the problem to 2106.
#endif
        /// @return Number of milliseconds that have elapsed since
        /// the system was started, up to 49.7 days.
        static DWORD PAL_getTickCount();
        /// Sets the current local time and date.
        /// @return TRUE (non-zero) on success.
        static BOOL PAL_setLocalTime(
                /// [in] A pointer to a PAL_PTIME_STRUCT structure that contains the date and time.
                PAL_PTIME_STRUCT time_struct_p
                );
        /// Converts a system time to file time format.
        /// The system time is expressed in Coordinated Universal Time (UTC).
        static BOOL PAL_API PAL_systemTimeToFileTime(
                const PAL_TIME_STRUCT *lpSystemTime,
                PAL_LPFILETIME lpFileTime
                );
        /// Converts a file time to a local file time.
        static BOOL PAL_API PAL_fileTimeToLocalFileTime(
                const PAL_FILETIME *lpFileTime,
                PAL_LPFILETIME lpLocalFileTime
                );
        /// Converts a file time to system time format.
        /// The system time is expressed in Coordinated Universal Time (UTC).
        static BOOL PAL_API PAL_fileTimeToSystemTime(
                const PAL_FILETIME *lpFileTime,
                PAL_PTIME_STRUCT lpSystemTime
                );
        /// Retrieves the current local date and time.
        static void PAL_API PAL_getLocalTime(
                PAL_PTIME_STRUCT lpSystemTime);

        /// Retrieves the current system date and time.
        /// The system time is expressed in Coordinated Universal Time (UTC).
        static void PAL_API PAL_getSystemTime(
                PAL_PTIME_STRUCT lpSystemTime);

		/// Retrieves the current time zone settings.
		/// These settings control the translations between Coordinated Universal Time (UTC) and local time.
		static DWORD PAL_API PAL_getTimeZoneInformation(
			PAL_PTIME_ZONE_STRUCT pTimeZone);

        /// Formats time as a time string for a locale specified
        /// by identifier. 
        /// The function formats either a specified time or the
        /// local system time.
        /// @return The number of TCHAR values placed in the buffer
        static int PAL_getTimeFormat(
                LCID Locale,                    ///< [in] Specifies the locale
                DWORD dwFlags,                  ///< [in] Specifies options
                const PAL_TIME_STRUCT *lpTime,  ///< [in] Optional pointer to 
                LPCTSTR lpFormat,               ///< [in] Optional pointer to formatting
                LPTSTR lpTimeStr,               ///< [out] Optional pointer to buffer
                int cchTime                     ///< [in] Size of lpTimeStr buffer, can be zero
                );

        /// Formats a date as a date string for a locale
        /// specified by the locale identifier. The function formats either
        /// a specified date or the local system date.
        static int PAL_getDateFormat(
                LCID Locale,
                DWORD dwFlags,
                const PAL_TIME_STRUCT *lpDate,
                LPCTSTR lpFormat,
                LPTSTR lpDateStr,
                int cchDate
                );
        /// Converts the elapsed time, in seconds, since the beginning of 1970 to an absolute system time value.
        static void PAL_API PAL_rtlSecondsSince1970ToTime(
                DWORD Seconds,      ///< [in] Number of seconds since Mignight 12/31/1969
                LARGE_INTEGER *Time ///< [out] Pointer to receiving variable
                );
        /// Converts the specified 64-bit system time to the number of seconds
        /// since the beginning of January 1, 1970.
        /// @return TRUE if successful
        static BOOL PAL_API PAL_rtlTimeToSecondsSince1970(
                /// [in] A pointer to a LARGE_INTEGER structure that specifies
                /// the system time.
                const LARGE_INTEGER *Time,
                /// [out] A pointer to a variable that receives the number of
                /// seconds.
                LPDWORD Seconds );
        /// Converts a file time to MS-DOS date and time values.
        /// @return TRUE if successful
        static BOOL PAL_API PAL_fileTimeToDosDateTime(
                /// [in] A pointer to a FILETIME structure containing the file
                /// time to convert to MS-DOS date and time format.
                const PAL_LPFILETIME lpFiletime,
                /// [out] A pointer to a variable to receive the MS-DOS date.
                LPWORD lpFatDate,
                /// [out] A pointer to a variable to receive the MS-DOS time.
                LPWORD lpFatTime);
        /// Converts MS-DOS date and time values to a file time.
        /// @return TRUE on success
        static BOOL PAL_API PAL_dosDateTimeToFileTime(
                WORD fatdate,       ///< [in] The MS-DOS date.
                WORD fattime,       ///< [in] The MS-DOS time.
                PAL_LPFILETIME ft   ///< [out] a PAL_LPFILETIME structure that recieves the converted time.
                ); 
        //@}

        /// @name Locale
        /// Locale related functions
        //@{
        /// Creates a locale object.
        /// @return The specified locale settings as a PAL_LOCALE_TYPE object.
        static PAL_LOCALE_TYPE PAL_createLocale(
                /// [in] The locale category caller is interested in.
                int32_t category,
                /// [in] The name of the locale to use for the category.
                const char *localeName
                );
        /// Frees a locale object.
        static void PAL_freeLocale(
                /// locale object to free.
                PAL_LOCALE_TYPE locale_to_free
                );
        //@}

        /// @name Text
        /// Text related functions
        //@{
#ifndef PAL_WIN32_RUN   // we don't need this on Windows
        /// Convert unsigned long to wide char string
        /// @return a pointer to the string
        static LPWSTR PAL_API PAL_ultow(
                /// [in] Value to be converted
                ULONG value,
                /// [out] Destination for the converted value
                LPWSTR str,
                /// [in] Number base for conversion
                int radix);
#endif
        /// Convert a multi-byte string to a wide string
        static int PAL_MB2WC(
                UINT CodePage,
                DWORD dwFlags,
                LPCSTR lpMultiByteStr,
                int cbMultiByte,
                LPWSTR lpWideCharStr,
                int cchWideChar
                );
        /// Convert a wide string to a multi-byte string
        static int PAL_WC2MB(
                UINT CodePage,
                DWORD dwFlags,
                LPCWSTR lpWideCharStr,
                int cchWideChar,
                LPSTR lpMultiByteStr,
                int cbMultiByte,
                LPCSTR lpDefaultChar,
                LPBOOL lpUsedDefaultChar
                );
        //@}

        /// @name Winsock Networking
        /// Network (Winsock) related functions
        //@{
        /// Returns the error status for the last socket operation that failed.
        /// @return The error status for the last socket operation that failed.
        static int PAL_wsaGetLastError(void);
        /// controls the I/O mode of a socket.
        /// @return 0 on success.
        static int PAL_ioctlsocket(
                /// [in] A socket descriptor.
                PAL_SOCKET s,
                /// [in] A command to perform.
                long cmd,
                /// A pointer to a parameter for cmd.
                u_long *argp
                );
        /// Closes an existing socket.
        /// @return 0 on success.
        static int PAL_closesocket(
                /// A socket descriptor.
                PAL_SOCKET s
                );
        /// Determines whether or not a local system is connected to a network
        /// @return TRUE on success
        static BOOL PAL_isNetworkAlive(
                /// [out] A pointer to the network type.
                LPDWORD lpdwFlags
                );
        /// Retrieves adapter information.
        /// @return ERROR_SUCCESS on success.
        static DWORD PAL_getAdaptersInfo (
                /// [out] A pointer to a buffer that receives a linked list of
                /// IP_ADAPTER_INFO structures.
                PIP_ADAPTER_INFO pAdapterInfo,
                /// [in] A pointer to a ULONG variable that specifies the size
                /// of the buffer pointed to by the pAdapterInfo parameter
                PULONG pOutBufLen
                );

        /// Activates networking. Under linux this opens the CURL library
        /// and saves the networking parameters in a global structure.
        static PAL_INET_HANDLE PAL_internetOpen(
                PAL_INET_STR lpszAgent,         ///< A string containing the agent name to use
                DWORD dwAccessType,             ///< The type of access. UNUSED in linux.
                PAL_INET_STR lpszProxyName,
                PAL_INET_STR lpszProxyBypass,
                DWORD dwFlags
                );

        /// gets a dir listing and stores it for later use
        static PAL_INET_HANDLE PAL_internetConnect(
                PAL_INET_HANDLE hInternet,
                PAL_INET_STR lpszServerNameAndPath,
                INTERNET_PORT nServerPort,
                PAL_INET_STR lpszUsername,
                PAL_INET_STR lpszPassword,
                DWORD dwService,
                DWORD dwFlags,
                DWORD_PTR dwContext
                );

#if defined ( PAL_WIN32_RUN )
        bool PAL_internetFindNextFile(
                PAL_INET_HANDLE inet_handle,
                PVOID find_data
                );
#endif      // PAL_WIN32_RUN
#if defined ( __linux__ )
        /// Finds all the files with the given extension
        /// @return Returns a vector of strings of filenames ending in the given extension. 
        static std::vector<struct ftpFileInfo> PAL_ftpFindFilesByExtension(
				/// [in] handle for ftp internet session
				PAL_INET_HANDLE hFtp, 
                /// [in] The file extension to search for
                std::string extStr
                );
#endif      // __linux__

		static char* extendedErrorMessageFormat;
		static unsigned long extendedError;

		// extended error info for windows. Returns any FTP/cURL errors to caller
		static void setExtendedError( 
				unsigned long err 
				);
		static unsigned long getExtendedError( 
				);
		static bool PAL_InternetGetLastResponseInfo(
				unsigned long* pdwError, 
				char* data, 
				unsigned long* dwErrorBufLength
				);
		
		/// Retrieves a file from the FTP server and stores it under the 
        /// specified file name, creating a new local file in the process.
        /// @return TRUE if successful
        static bool PAL_ftpGetFile(
                PAL_INET_HANDLE inet_handle,    ///< [in] Handle to a PAL_INET
                PAL_INET_STR remote_file,       ///< [in] The name of the file to get
                PAL_INET_STR new_file,          ///< [in] The name of the file to create
                BOOL fail_if_exists,            ///< [in] if set fail if the local file already exists
                DWORD flags_and_attributes,     ///< [in] File attributes for the created file
                DWORD flags,                    ///< [in] Download control
                DWORD_PTR context               ///< [in] Pointer to a variable that contains the application-defined value that associates this search with any application data.
				);
		static bool PAL_ftpPutFile(
				PAL_INET_HANDLE inet_handle,
				PAL_INET_STR new_file,
				PAL_INET_STR remote_file,
				DWORD flags,
				DWORD_PTR context
				);
		static bool PAL_ftpCreateDirectory(
				PAL_INET_HANDLE inet_handle,
				PAL_INET_STR remote_directory
			);
        /// Closes the current internet session. The hInternet parameter is UNUSED 
        /// under linux because the global ftp structure tracks it for us
        static BOOL PAL_internetCloseHandle( 
                PAL_INET_HANDLE hInternet       ///< [in] Handle to PAL_INET
                );
        //@}

        /// @name Vars
        /// vars
        //@{
        /// The current output log verbosity level
        static DWORD PAL_logging;
        //@}

        /// @name Graphics
        /// Graphic related functions
        //@{
        /// Deletes a font object
        /// @return TRUE on success
        static BOOL PAL_deleteFont(
                /// [in] A handle to a font obj to delete.
                HANDLE font_handle);
        //@}

        /// @name Thread Data
        /// Thread specific data handlers
        //@{
        /// Thread local storage allocation
        static DWORD PAL_API PAL_tlsAlloc(void);
        /// Free thread local storage at given index. Indexing is not used now.
        /// @return TRUE
        static BOOL PAL_API PAL_tlsFree( 
                DWORD dwTlsIndex        ///< [in] UNUSED The index to free
                );
        /// Retrieve thread local storage at given index. Indexing is not used now.
        /// @return A void pointer to the data stored at the given index
        static LPVOID PAL_tlsGetValue(
                DWORD dwTlsIndex        ///< [in] UNUSED The index to retrieve from
                );
        /// Save value in thread local storage at given index. Indexing is not used now.
        /// @return TRUE if successful
        static BOOL PAL_API PAL_tlsSetValue( 
                DWORD dwTlsIndex,       ///< [in] UNUSED The index to save to
                LPVOID lpTlsValue       ///< [in] A pointer to the value to save
                );
        //@}

        /// @name Misc functions
        /// Functions that belong to no other grouping
        //@{
        /// reports the available memory resources
        static void PAL_API PAL_globalMemoryStatus(
                /// A pointer to a buffer.
                PAL_LPMEMORYSTATUS lpBuffer
                );
        /// Calls /usr/bin/www_cmd.sh to launch a browser window
        static void PAL_API PAL_launchBrowswer(void);
        /// Does the window named "title" exist?
        /// @return TRUE if the window with the given title is in the window manager list
        static bool PAL_API PAL_findWindow(
                const char *title       ///< [in] A case insensitive string to search for in the list of window titles
                );
        /// Closes the given window (soft-close)
        static void PAL_API PAL_closeWindow(
                const char *title       ///< [in] A case insensitive string to search for in the list of window titles
                );
        /// Hides the given window
        static void PAL_API PAL_hideWindow(
                const char *title       ///< [in] A case insensitive string to search for in the list of window titles
                );
        /// Brings the given window to the top of the main desktop
        static void PAL_API PAL_showWindow(
                const char *title       ///< [in] A case insensitive string to search for in the list of window titles
                );
		/// Play a video in the video player
		static void PAL_API PAL_showVideoPlayer(
				const char *title
				);


		/// Execute an external program and get the results is writes to stdout
		static int PAL_API PAL_executeWithResults(
				const std::string &command, std::string &commandOutput
				//const char *command, char *commandOutput
				);


        //@}

#ifdef USING_SELECT_FOR_WAITING
        //static HANDLE                 select_mutx;
        static PAL_LPCRITICAL_SECTION   select_mutx;
        static HANDLE                   sel_ev;
#endif
    private:
        /// Class constructor
        PAL_absclass();
        /// Class destructor
        ~PAL_absclass();

        /// Save the given error and msg to the log
        static inline void PAL_ErrorSave(
                PAL_ERRNO_TYPE err,
                const char *msg
                );

        /// If the test fails log the error and msg
        static inline bool PAL_ErrorChk(
                bool test,
                PAL_ERRNO_TYPE err,
                const char *msg
                );

#if defined( __linux__ )
        static LPTSTR pal_getDateFormat_FillingPattern(
                PAL_DATETIME_FORMAT_PATTERN pattern,
                int pattern_match_count,
                const PAL_TIME_STRUCT *lpDate,
                LPTSTR date_str_p,
                int &date_str_size,
                LPTSTR lpDateStr
                );

        static LPTSTR pal_getTimeFormat_FillingPattern(
                PAL_DATETIME_FORMAT_PATTERN pattern,
                int pattern_match_count,
                const PAL_TIME_STRUCT *lpTime,
                LPTSTR time_str_p,
                int &time_str_size,
                LPTSTR lpTimeStr
                );
#endif

#if defined( __linux__ )
        static DWORD pal_waitForMutexObj(
                HANDLE hndl,
                DWORD time,
                PAL_PMUTEX inMutex
                );
        static DWORD pal_waitForSemaphoreObj(HANDLE hndl, DWORD time);

        ///Implements logic to wait for events.
        //@param hndl - Event handle of PtrMan type.
        //@param time - Time routine should wait.
        //@param type - Caller, could be PAL_resetEvent or a thread.
        //
        //@return WAIT_OBJECT, WAIT_FAILED or WAIT_TIMEDOUT
        static DWORD PAL_waitForEventObj(HANDLE hndl, DWORD time, 
                PAL_EV_CALLER_TYPE type);

        /// @name Thread specific data handlers
        //@{

        /// Creates thread specific data key.  Routine is called from
        /// pthread_once in order to maintain only one key per process.
        static void PAL_initThreadOnce(void);

        /// Creates the thread specific data key and associates thread data
        /// with the value of this key (if these steps have not already
        /// completed for this thread)
        static HANDLE PAL_initThrdData(void);

        /// Frees the memory associated with the thread specific data. Routine
        /// is automagically called at thread exit.
        static void PAL_delThrdData(
                /// [in] Pointer to thread specific data.
                HANDLE dat
                );

        /// Wrapped thread cleanup function.
        static void PAL_WrapperThread_cleanup(void *arg);

        /// Wraps threads.
        /// Instead of creating the thread by pthread_create() directly
        /// we insert a PAL_WrapperThread so that we could emulate CREATE_SUSPENDED
        /// and retrieve thread exit code, and do others things.
        static void *PAL_WrapperThread(
                /// [in] a pointer to a void casted PAL_PWRAPPER_THREAD_INFO structure
                void *arg
                );
        //@}

        //
        //vars
        //

        enum {LOG_OUT_SIZE=8192};

        /// enables a pthread_once function to be executed only once
        static pthread_once_t PAL_kinit_once;
        /// thread-specific data key visible to all threads in the process
        static pthread_key_t PAL_key;
        /// Used during FTP transactions
        static struct ftpStruct ftp;
        /// Networking opened?
        static bool bPalInternetOpened;

        /// Used to protect logging output
        static std::mutex PAL_global_log_mutex;
        static char out[LOG_OUT_SIZE];

        /// a pointer the PAL_GLOBAL_THREAD_DATA structure
        typedef std::list<PAL_PGLOBAL_THREAD_DATA> gt_data_t;
        static gt_data_t PAL_global_thread_data;
        /// Used to protect global thread data
        static std::mutex PAL_global_thread_data_mutex;

		///critical section handle used in close and dupHandle
		///calls
		static PAL_LPCRITICAL_SECTION hndl_crit;
#endif      // __linux__

        /// the wfstream coresponding to the output log file
        static std::wfstream PAL_log_file;
        //
        //macros
        //

};//PAL_absclass

/// The different types of pointers PAL_PtrMan can hold
enum PAL_TYPE_OF_PTR
{
    UNASSIGNED=0,       ///< A pointer that has yet to be assigned
    MUTEX_TYPE,         ///< A pointer of MUTEX type
    SEM_TYPE,           ///< A pointer of Semaphore type
    CS_TYPE,            ///< A pointer of CriticalSection type
    AUTO_EVENT_TYPE,    ///< A pointer of Auto reseting Event type
    MANUAL_EVENT_TYPE,  ///< A pointer of Manual reseting Event type
    SYSTEM_FILE_TYPE,   ///< A pointer of a system file type
    STDIO_FILE_TYPE,    ///< A pointer of a standard file type
    PROCESS_ID_TYPE,    ///< A pointer of a Process Identification type
    THREAD_TYPE         ///< A pointer of a Thread type
};

/// Acts as generic holder of different pointer types
class PAL_PtrMan
{
    private:
        /// Assignment operator
        PAL_PtrMan & operator=(const PAL_PtrMan&);

        /// Copy constructor
        PAL_PtrMan(const PAL_PtrMan &ptr_man):
            hndl(ptr_man.hndl),
            ptr_type(ptr_man.ptr_type),
            id(ptr_man.id),
            cs_count(ptr_man.cs_count), 
#ifdef USING_SELECT_FOR_WAITING
//            wait_state(ptr_man.wait_state),
            thrd_cnt(ptr_man.thrd_cnt),
            wait_ret_val(ptr_man.wait_ret_val),
            thrd_id(ptr_man.thrd_id)
#endif
		{wait_state[0]=ptr_man.wait_state[0];wait_state[1]=ptr_man.wait_state[1];}
        /// Instance of the handle
        HANDLE hndl;

        /// Type of object represented by this handle
        PAL_TYPE_OF_PTR ptr_type;

        /// Identifying number of this handle
        DWORD id;

        /// Critical section count
        int cs_count;
#ifdef USING_SELECT_FOR_WAITING

        ///used in select calls to resume when status of a handle has changed.
        int32_t wait_state[2];

        ///count of threads waiting for this handle in waitForMultiple
        int32_t thrd_cnt;

        ///return value for waitForMultiple
        int32_t wait_ret_val;

        ///Thread id for the waitForMultiple's thread.
        HANDLE thrd_id;

        /// The filename of the fifo
        char fifoName[256];
#endif
//////
//new add
		/// Ref count for the object.  Object not deleted until ref count==0
//////
		int32_t ref_cnt;
    public:
        /// PAL_PtrMan constructor
#ifdef USING_SELECT_FOR_WAITING
        PAL_PtrMan(HANDLE p=NULL, PAL_TYPE_OF_PTR t=UNASSIGNED, DWORD new_id=0):
            hndl(p),
            ptr_type(t),
            id(new_id),
            cs_count(0),
            thrd_cnt(0),
            wait_ret_val(0),
            thrd_id(0),
//////
//new add
			ref_cnt(1) {wait_state[0]=-1;wait_state[1]=-1;}
///////
#else
        PAL_PtrMan(HANDLE p=NULL, PAL_TYPE_OF_PTR t=UNASSIGNED, DWORD new_id=0):
            hndl(p),
            ptr_type(t),
            id(new_id),
            cs_count(0),
//////
//new add
			ref_cnt(1) {wait_state[0]=-1;wait_state[1]=-1;}
///////
#endif
        /// PAL_PtrMan destructor
#ifdef PAL_WIN32_RUN
        ~PAL_PtrMan(){}
#else
        ~PAL_PtrMan();
#endif

        /// Saves the handles passed in the class member
        void PAL_setHndl(
                /// [in] Handle to be saved.
                HANDLE new_hndl
                )
        {
            hndl = new_hndl;
        }

        /// Assign new handle to class
        /// @return *this
        PAL_PtrMan &operator = (
                /// [in] Handle to be assigned.
                HANDLE new_hndl
                )
        {
            hndl = new_hndl;
            return *this;
        }

        /// Access routines returns the handle saved with the this instance.
        /// @return hndl - Handle saved in the instance of class.
        HANDLE PAL_getHndl(void) const {return hndl;}

        /// Saves the type of the handles represented by this class instance.
        void PAL_setType(
                /// [in] Type of the Handle saved in this instance.
                PAL_TYPE_OF_PTR type
                )
        {
            ptr_type = type;
        }

        /// Retrieves the type of the handle represented by this class instance.
        /// @return ptr_type - Type of handle saved in the instance of class.
        PAL_TYPE_OF_PTR PAL_getType(void) const
        {
            return ptr_type;
        }

#if defined( __linux__ )
        /// Set the identifying number for this handle
        void PAL_setID(
			/// [in] The new identifying number to set this handle to
			pthread_t new_id )
        {
            id = new_id;
        }

        /// Get the identifying number for this handle
        /// @return id The identifying number for this handle
        pthread_t PAL_getID(void) const {return id;}

        /// Get the CriticalSection count for this handle
        /// @return cs_count The CriticalSection count for this handle
        int PAL_getCSCount(void) const {return cs_count;}

        /// Increase the CriticalSection count for this handle
        int32_t PAL_incCSCount(void)
        {
        	if (1 != cs_count+1)
        	{
        		__sync_add_and_fetch(&cs_count,1);
				return 0;
        	}
        	return -1;
        }

		/// Decrease the CriticalSection count for this handle
		int32_t PAL_decCSCount(void)
		{
			if (0 < cs_count)
			{
				__sync_sub_and_fetch(&cs_count,1);
				return 0;
			}
			return -1;
		}

		/// Increase the object reference count for this handle
		int32_t PAL_incObjCnt(void)
		{
			if (-1 != ref_cnt+1)
			{
				__sync_add_and_fetch(&ref_cnt,1);
				return 0;
			}
			return -1;
		}

		/// Decrease the object reference count for this handle. Object will
		/// be deleted when the count goes to 0.
		int32_t PAL_decObjCnt(void)
		{
			if (0 < ref_cnt)
			{
				__sync_sub_and_fetch(&ref_cnt,1);
				return 0;
			}
			return -1;
		}

		/// Tests if the ref count has reached 0.
		BOOL PAL_isObjCntZero(void){return(0==ref_cnt);}

#ifdef USING_SELECT_FOR_WAITING
		//////////////////////////////////////////////////
		//Access routines returns the fd used with wait
		//routines.
		//
		//@return wait_state - fd used in select call to wait
		//
		//void PAL_setFd(int32_t fd){wait_state = fd;}
		int32_t *PAL_getFd(void) const {return (const_cast<int32_t *> (wait_state));}

		int32_t PAL_initfd(void);

		//
		///Reads the contents of the buffer deactivates the handle
		//
		int32_t PAL_clearfd(void)
		{
			char buf[2]={"\0"};
			
			if ((-1 == wait_state[0]) || (-1 == wait_state[1]))
			{
				PAL_PRINT_LOG(PAL_ERR_LOG, __FUNCTION__);
				PAL_PRINT_LOG(PAL_ERR_LOG, ": Invalid fd.");
				errno = -EINVAL;
				return -1;
			}

			int32_t ret = read(wait_state[0], buf , sizeof(buf));
			if (-1 == ret)
			{
				PAL_PRINT_LOG(PAL_ERR_LOG, __FUNCTION__);
				PAL_PRINT_LOG(PAL_ERR_LOG, ": Read failed");
				return(ret);
			}
			return(ret);
		}
		
		//
		///Write to the buffer, activating the handle
		//
		int32_t PAL_touchfd(void)
		{
			if ((-1 == wait_state[0]) || (-1 == wait_state[1]))
			{
				PAL_PRINT_LOG(PAL_ERR_LOG, __FUNCTION__);
				PAL_PRINT_LOG(PAL_ERR_LOG, ": Invalid fd.");
				errno = -EINVAL;
				return -1;
			}

			int32_t ret = write(wait_state[1], "up", 2);
			if (-1 == ret)
			{
				PAL_PRINT_LOG(PAL_ERR_LOG, __FUNCTION__);
				PAL_PRINT_LOG(PAL_ERR_LOG, ": Write failed");
				return(ret);
			}
			return(ret);
		}

		//
		///Closes the read and write fds
		//
		void PAL_closefd(void)
		{
			if (0 < wait_state[0])
			{
				PAL_PRINT_LOG(PAL_ERR_LOG, __FUNCTION__);
				PAL_PRINT_LOG(PAL_ERR_LOG, ": Closing read fd");
				close(wait_state[0]);
				wait_state[0] = -1;
			}
			if (0 < wait_state[1])
			{
				PAL_PRINT_LOG(PAL_ERR_LOG, __FUNCTION__);
				PAL_PRINT_LOG(PAL_ERR_LOG, ": Closing write fd");
				close(wait_state[1]);
				wait_state[1] = -1;
			}
		}

#endif//USING_SELECT_FOR_WAITING
#endif

};


#if defined( __linux__ )
///////////////////////////////////////////////////
//Semaphore structure used to hold the max value
//passed by the caller.  This is need in order to
//emulate Microsoft semaphore APIs.
//
typedef struct PAL_SyncObj
{
    public:
        PAL_SyncObj():
            sem(NULL),
            sem_name(NULL),
            max_cnt(PAL_SEM_MAX_VAL),
			bHasMutex(FALSE),
            mutex(),
            thrd_cnt(0),
            sync_type(SEM_TYPE),
            state(NON_SIGNALED),
            bIsNamed(FALSE){}
        ~PAL_SyncObj();

        PAL_PSEM        sem;
        char*           sem_name;
        uint32_t        max_cnt;

        BOOL					bHasMutex;
        PAL_CRITICAL_SECTION	mutex;

        uint32_t        thrd_cnt;
        uint32_t        sync_type;
        BOOL            state;
        BOOL            bIsNamed;

        enum SEM_STATE
        {
            NON_SIGNALED=0,
            SIGNALED
        };
    private:
        PAL_SyncObj & operator=(const PAL_SyncObj&);
        PAL_SyncObj(const PAL_SyncObj &);
}PAL_SYNC_DAT, *PAL_PSYNC_DAT;

#endif      // __linux__
#endif      //_PAL_ABSCLASS_H_
