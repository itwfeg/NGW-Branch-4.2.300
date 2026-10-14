// Header file included by ANTLR UniLexer


#ifndef REM_LINUX_ERR
//#include <tchar.h>
#endif


#ifndef ErrorHandling_included
#define ErrorHandling_included

/*******************************************************************************

TYPE:			Exception Classes

PURPOSE:		This is a set of global functions, macros and exceptions
covering the exception and error handling.

FUNCTION:		Every SI needs methods to handle their errors. The C++
language delivers the exception handling (see [Strous]).
This enables one to define blocks where to catch errors.
The project specific handling is done by the exception
handling and the exception classes. The error processing
when an error occurs is done by the functions and macros.

INTERFACES:		Global functions:
LogException		: Logs an exception.

Macros:
Assert				: Raises an exception if the boolean
expression returns false
LogCatch			: calls Util::LogException
LogCatchAndThrow	: calls Util::LogException and raises
the exception again

Exception classes:
CBaseException		: the base class for the exceptions
CNTException		: covers all NT specific exceptions
CTimeoutException	: is raised after a timeout if it
needs to
CNamedException		: raise a exception with a name
CMMIInformation		: show a dialog box to the user with information
CParamException		: exception in case of wrong paramters
CCLibException		: exception of a library component
CBadObject			: exception of an unrecognized object
CDataNotAvailable	: is raised if data is not available
CFTPException		: exception of the FTP client  classes

RESOURCES:		DADF Libraries: Util

REFERENCES:		none

PROCESSING:		none

DATA:			See 'DATA:' in the class header below.

*******************************************************************************/

#include "PAL_platformheader.h"
#include <string>


namespace Util
{
#if defined(PAL_WIN32_RUN)
#define PCSTR_NEXT_LINE             "\r\n"
#define PTSTR_NEXT_LINE             L"\r\n"

#elif   defined(__linux__)
#define PCSTR_NEXT_LINE             "\n"
#define PTSTR_NEXT_LINE             L"\n"

#else
#error  "define related VARIABLE"
#endif

#define PCSTR_NEW_LINE              "\n"
#define PTSTR_NEW_LINE              L"\n"

	class CBaseException;

	void SoftwareException(
		const std::wstring &i_file,
		unsigned long i_line,
		unsigned long i_ErrorCode,
		const std::wstring &i_ErrorMsg);


	void LogMessage(
		const TCHAR *i_file,			// buffer where to write
		int i_line,						// amount of information
		const TCHAR *msg) throw();


	// Description:	This function logs an exception.
	// Returns:		Nothing.
	void LogException(
		const TCHAR *i_file,			// buffer where to write
		int i_line) throw();			// amount of information


										// called by logMessage and LogException for reuse
	void _log(
		const TCHAR* i_file,
		int i_line,
		const TCHAR* msg) throw();

	// Description:	Used by the Assert macro, below.  Logs and throws the exception.
	//				Do not use this, otherwise.
	// Returns:		Nothing.
	template <class E>
	void LogAndThrow(
		E &e,							// A reference to the exception to be thrown.  This is a
										// templated parameter because using Util::CBaseException&
										// means that only that kind of exception can be caught.
										// i.e.  try { Assert( false, CNTException() ); } catch (CNTException&e) {}
										// would not catch the exception.
		const TCHAR *i_description,		// A description for LogReason
		const TCHAR *i_file,			// The name of the file (filled in by the macro)
		int	i_line)					// The line in the file (filled in by the macro)
	{
		e.SetDescription(i_description);
		e.SetFile(i_file);
		e.SetLine(i_line);

		SoftwareException(i_file, i_line, e.ErrorCode(), e.ExceptionMsg());

		throw e;
	}


	// Description:	This macro raises an exception if the condition a is false.
	// Arguments:	a	condition
	//				e	exception
	// Returns:		Nothing
#define Assert( a, e ) if (!(a)) { auto obj = e; Util::LogAndThrow( obj, _T(#a), _T(__FILE__), __LINE__ ); }

	// Description:	This macro logs an exception.
	// Arguments:	__FILE__	buffer where to write
	//				__LINE__	amount of information
	//				s			extra text to put in log
	// Returns:		Nothing.
#define Log(s) Util::LogMessage(_T(__FILE__), __LINE__, (s) );

	// Description:	This macro logs an exception.
	// Arguments:	__FILE__	buffer where to write
	//				__LINE__	amount of information				
	// Returns:		Nothing.
#define LogCatch			Util::LogException(_T(__FILE__), __LINE__ );
	//#define LogCatch			Util::LogException((__FILE__), __LINE__ );


	// Description:	This macro logs an exception and raises it again.
	// Arguments:	__FILE__	buffer where to write
	//				__LINE__	amount of information				
	// Returns:		Nothing.
#define LogCatchAndThrow	{ Util::LogException(_T(__FILE__), __LINE__); throw; }


	// This condition should have been true before the method was called
#define PRECONDITION(p) Assert(p, Util::CNamedException( _T(#p) ));


	// This condition should have been made true by the method
#define POSTCONDITION(p) Assert(p, Util::CNamedException( _T(#p) ));


	// CJC	 8/10/00 10:19:24 PM
	// This macro simply reports the contents of an exception to the error log.
#define UTIL_REPORT_EXCEPTION(e) Util::SoftwareException(_T(__FILE__),__LINE__,e.ErrorCode(),\
        e.ExceptionMsg());






	class CBaseException
	{
	public:
		//	INTERFACES:
		virtual ~CBaseException() {}

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();

		//	Description:	This function returns the Exception code.
		//	Returns:		Exception code.
		virtual DWORD ErrorCode() const throw();

		virtual std::wstring Description() const throw() { return m_Description; }
		virtual void SetDescription(const std::wstring &value) throw() { m_Description = value; }

		virtual std::wstring File() const throw() { return m_File; }
		virtual void SetFile(const std::wstring &value) throw() { m_File = value; }

		virtual int Line() const throw() { return m_Line; }
		virtual void SetLine(int value) throw() { m_Line = value; }

		//	Description:	Returns the Formatted message text.
		//	Returns:		Formatted message.
		virtual std::wstring DetailedReportMsg() const throw();

	protected:

		std::wstring m_Description;
		std::wstring m_File;
		int m_Line;
	};

#if defined(PAL_WIN32_RUN)  // CNTException class (real CPlatformException for Win32)
	class CNTException : public Util::CBaseException
	{
	public:
		//	INTERFACES:
		CNTException() throw();
		//            CNTException(const DWORD &i_ErrorCode) throw();

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();

		//	Description:	Returns the Exception code.
		//	Returns:		Exception code
		virtual DWORD ErrorCode() const throw();

	protected:
		//	DATA:
		// list of error codes to access them via enum names 
		enum { e_MAX_ERROR_MSG_SIZE = 511 };

	protected:
		//	DATA:
		// the error created by the exception
		DWORD	m_ErrorCode;
	};
#endif  // PAL_WIN32_RUN

#if defined(__linux__)  // CLinuxException class (real CPlatformException for Linux)
	class CLinuxException : public Util::CBaseException
	{
	public:
		//	INTERFACES:
		CLinuxException() throw();
		//            CLinuxException(const DWORD &i_ErrorCode) throw();

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();

		//	Description:	Returns the Exception code.
		//	Returns:		Exception code
		virtual DWORD ErrorCode() const throw();

	protected:
		//	DATA:
		// the error created by the exception
		DWORD	m_ErrorNo;
	};
#endif  // __linux__

	class CTimeoutException : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class CNamedException : public Util::CBaseException
	{
	public:
		//	Description:			
		//		This constructor takes the text which should appear in case
		//		of an error.
		//	Arguments:				
		//		name				message that should appear
		//	Returns:				none
		CNamedException(const TCHAR* 	    i_Name) throw() : m_name(i_Name) {}
		CNamedException(const std::wstring& i_Name) throw() : m_name(i_Name) {}

		// Description:	Destructor of the class.  Added to match 
		//		base classes throw
		// Arguments:	None
		// Returns:	Nothing.
		~CNamedException() {};

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw() { return m_name; }

	private:
		//	DATA:
		// the error text
#ifdef PAL_WIN32_RUN
#pragma warning( disable : 4251 )
#endif

		std::wstring m_name;

#ifdef PAL_WIN32_RUN
#pragma warning( default : 4251 )
#endif
	};


	class CMMIInformation : public Util::CBaseException
	{
	public:
		//	Description:			
		//		This constructor takes the text which should appear in case
		//		of an error.
		//	Arguments:				
		//		name				message that should appear
		//	Returns:				none
		CMMIInformation(const TCHAR*        i_Name)                     throw() : m_name(i_Name) {}
		CMMIInformation(const std::wstring& i_Name = _T("Software Anomaly")) throw() : m_name(i_Name) {}

		// Description:	Destructor of the class.  Added to match 
		//		base classes throw
		// Arguments:	None
		// Returns:	Nothing.
		~CMMIInformation() {};

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw() { return m_name; }

	private:
		//	DATA:
		// the error text
#ifdef PAL_WIN32_RUN
#pragma warning( disable : 4251 )
#endif

		std::wstring m_name;

#ifdef PAL_WIN32_RUN
#pragma warning( default : 4251 )
#endif
	};


	class CParamException : public Util::CBaseException
	{
	public:
		CParamException() throw() {}

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw() { return _T("Invalid Parameter"); }
	};


	class CCLibException : public Util::CBaseException
	{

	public:

		CCLibException() throw();

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();

		//	Description:	Returns the error number.
		//	Returns:		Error number.
		virtual DWORD ErrorCode() const throw();

	protected:
		//	DATA:
		// the error number
		int		m_ErrNo;

	};


	class CBadObject : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class CDataNotAvailable : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class CDynamicCastException : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class CIncompatibleNetworkPrototype : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class NotEnoughPeekData : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class CFTPException : public Util::CBaseException
	{
	public:
		// error type
		enum EFTPError
		{
			e_Connect,
			e_RemoteDir,
			e_Send,
			e_Transferring
		};

		// Description:	Constructor of the FTP exception class
		// Arguments:	Type of error
		// Returns:		Nothing.
		CFTPException(EFTPError i_error) throw();

		// Description:	Destructor of the FTP exception class.  Added to match 
		//		base classes throw
		// Arguments:	None
		// Returns:	Nothing.
		~CFTPException() {};

		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();

	private:
		// error type
		EFTPError	m_ErrorType;
		// human readable error message
		std::wstring m_Errormessage;
	};


	class CInvalidMethodCall : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	class CInvalidEnum : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();
	};


	// This class indicates that the current thread is to be terminated by
	// a shutdown request.
	class CTerminateNow : public Util::CBaseException
	{
	public:
		//	Description:	Returns the Exception text.
		//	Returns:		Exception text.
		virtual std::wstring ExceptionMsg() const throw();

		virtual DWORD ErrorCode() const throw();
	};




	// Constructed and thrown when a Win32 (C Structured) Exception occurs.
	class CSEException : public Util::CBaseException
	{

	protected:

		// DATA:

		// The exception code of the Win32 (C Structured) Exception.
		unsigned int m_ExceptionCode;

	public:

		// INTERFACES:

		// Description:	Constructor. Sets the exception code.
		// Returns:		Nothing.
		CSEException
		(
			unsigned int i_ExceptionCode = 0
		)
			: m_ExceptionCode(i_ExceptionCode)
		{
		}

		// Description:	Destructor of the class.  Added to match 
		//		base classes throw
		// Arguments:	None
		// Returns:	Nothing.
		~CSEException() {};

		// Description:	Returns the exception message text.
		// Returns:		The exception message text.
		virtual std::wstring ExceptionMsg() const throw()
		{
			TCHAR message[1000];

			swprintf(message, sizeof(message) / sizeof(TCHAR),
				_T("Win32 (C Structured) Exception occurred, exception code = %08x"), m_ExceptionCode);

			std::wstring name;
			switch (m_ExceptionCode)
			{
			case EXCEPTION_ACCESS_VIOLATION:        name = _T(" (EXCEPTION_ACCESS_VIOLATION)");            break;
			case EXCEPTION_BREAKPOINT:              name = _T(" (EXCEPTION_BREAKPOINT)");                  break;
			case EXCEPTION_DATATYPE_MISALIGNMENT:   name = _T(" (EXCEPTION_DATATYPE_MISALIGNMENT)");       break;
			case EXCEPTION_SINGLE_STEP:             name = _T(" (EXCEPTION_SINGLE_STEP)");                 break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:   name = _T(" (EXCEPTION_ARRAY_BOUNDS_EXCEEDED)");       break;
			case EXCEPTION_FLT_DENORMAL_OPERAND:    name = _T(" (EXCEPTION_FLT_DENORMAL_OPERAND)");        break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:      name = _T(" (EXCEPTION_FLT_DIVIDE_BY_ZERO)");          break;
			case EXCEPTION_FLT_INEXACT_RESULT:      name = _T(" (EXCEPTION_FLT_INEXACT_RESULT)");          break;
			case EXCEPTION_FLT_INVALID_OPERATION:   name = _T(" (EXCEPTION_FLT_INVALID_OPERATION)");       break;
			case EXCEPTION_FLT_OVERFLOW:            name = _T(" (EXCEPTION_FLT_OVERFLOW)");                break;
			case EXCEPTION_FLT_STACK_CHECK:         name = _T(" (EXCEPTION_FLT_STACK_CHECK)");             break;
			case EXCEPTION_FLT_UNDERFLOW:           name = _T(" (EXCEPTION_FLT_UNDERFLOW)");               break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO:      name = _T(" (EXCEPTION_INT_DIVIDE_BY_ZERO)");          break;
			case EXCEPTION_INT_OVERFLOW:            name = _T(" (EXCEPTION_INT_OVERFLOW)");                break;
			case EXCEPTION_PRIV_INSTRUCTION:        name = _T(" (EXCEPTION_PRIV_INSTRUCTION)");            break;
			case EXCEPTION_NONCONTINUABLE_EXCEPTION: name = _T(" (EXCEPTION_NONCONTINUABLE_EXCEPTION)");   break;
			}

			return std::wstring(message) + name;
		}

		// Description:	Returns the error number.
		// Returns:		The error number.
		virtual DWORD ErrorCode() const throw()
		{
			return m_ExceptionCode;
		}

	};

#if !defined ( __linux__ )
	// Description:     Translates a Win32 (C Structured) Exception into a C++ exception class.
	//                  To be registered with _set_se_translator() at the beginning of the program.
	// Returns:         Nothing.
	inline void TranslateSE
	(
		unsigned int           i_ExceptionCode,        // As returned by GetExceptionCode().
		EXCEPTION_POINTERS*    /*i_ExceptionPointers*/ // As returned by GetExceptionInformation().
	)
	{
		throw Util::CSEException(i_ExceptionCode);
	}
#endif  // __linux__

} // end namespace


#endif
