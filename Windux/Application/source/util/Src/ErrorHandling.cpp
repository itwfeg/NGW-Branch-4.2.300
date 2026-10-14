// Header file included by ANTLR UniLexer
#include <PAL_platformheader.h>

#if defined ( _WIN32 )
#include <tchar.h>
#endif
#include "ErrorHandling.h"
#include "CSmartPtr.h"
#include "utilunicode.h"
#include "OutputLogger.h"
#include <stdio.h>
#include <stdlib.h>
//#include "Util_EventPackets.h"

void Util::LogMessage(const TCHAR* i_file, int i_line, const TCHAR* msg) throw()
{
	_log(i_file, i_line, msg);
}

void Util::LogException(const TCHAR *i_file, int i_line) throw()
{
	_log(i_file, i_line, L"Deprecated. Use LogMessage() instead.");
}

void Util::_log(const TCHAR* i_file, int i_line, const TCHAR* msg) throw()
{
	// CJC       8/10/00 2:53:34 PM Changed Format to include time stamp
	//  const TCHAR templatestring[] = _T("Exception log: %s, line %d");

	const TCHAR templatestring[] = _T(", line %d");
	const int max_line_number_digits = 11;      // Enough for the largest number (including - sign)
	TCHAR wstring[_MAX_PATH + sizeof(templatestring) / sizeof(TCHAR) + max_line_number_digits];

	swprintf(wstring, sizeof(wstring) / sizeof(TCHAR), templatestring, i_line);
	std::wstring    out = L"";
	out += Util::CMSGTime::GetTheCurrentSTTime().GetSQLDATETIME().c_str();
	out += std::wstring(L" : ");
	out += i_file;
	out += wstring;
#ifdef CHECK_FT
	COutputLogger::GetErrorLogger().WriteWideStr(out);
#else
	if (NULL != msg)
	{
		out += L", ";
		out += msg;
	}
	COutputLogger::GetErrorLogger().WriteWideStr(out, true);
#endif
	//  Event::SoftwareExceptionTrace (std::string (i_file), i_line);
}

void Util::SoftwareException(const std::wstring& i_file, unsigned long i_line, unsigned long i_ErrorCode, const std::wstring& i_ErrorMsg)
{
	//  CJC Temporarily do something similar to that done in "LogException"

	// CJC       8/10/00 2:56:00 PM Changed format to include timestamp instead of "Exception Log"
	//  const TCHAR templatestring[] = _T("Exception log: %s, line %d Code = %d Message = %s");
	const TCHAR templatestring[] = _T(", line %d. Code = %d, Message = ");
	const int max_line_number_digits = 11;      // Enough for the largest number (including - sign)

	TCHAR wbuff[_MAX_PATH + sizeof(templatestring) / sizeof(TCHAR) + max_line_number_digits];
	swprintf(wbuff, sizeof(wbuff) / sizeof(TCHAR), templatestring, i_line, i_ErrorCode);
	std::wstring    out = L"";
	out += Util::CMSGTime::GetTheCurrentSTTime().GetSQLDATETIME().c_str();
	out += std::wstring(L" : ");
	out += i_file;
	out += wbuff;
	out += i_ErrorMsg;

	COutputLogger::GetErrorLogger().WriteWideStr(out);
	//  Event::SoftwareException (i_file, i_line, i_ErrorCode, i_ErrorMsg);
}


std::wstring Util::CBaseException::ExceptionMsg() const throw()
{
	return _T("Unnamed exception");
}

DWORD Util::CBaseException::ErrorCode() const throw()
{
	return 0;
}

std::wstring Util::CBaseException::DetailedReportMsg() const throw()
{
	static const TCHAR templatestring[] = _T(", line %d. Code = %d, Message = ");

	TCHAR wbuff[_MAX_PATH];
	swprintf(wbuff, sizeof(wbuff) / sizeof(TCHAR), templatestring, Line(), ErrorCode());

	const Util::CMSGTime now(Util::CMSGTime::GetTheCurrentSTTime());

	std::wstring out;
	out += now.GetSQLDATETIME();
	out += L" : ";
	out += File();
	out += wbuff;
	out += ExceptionMsg();
	out += L" -- ";
	out += Description();

	return out;
}


#if defined(PAL_WIN32_RUN)  // CNTException class implementation (real CPlatformException for Win32)
Util::CNTException::CNTException() throw()
{
	m_ErrorCode = PAL_absclass::PAL_getLastError();
}

#if 0   // To enable this function, implement /PAL/PAL_linux/PAL_errno.cpp::PAL_mapErrnoWin322Linux() first.
Util::CNTException::CNTException(const DWORD &i_ErrorCode) throw()
{
	m_ErrorCode = i_ErrorCode;
}
#endif

#if 0   // Not in used.
void Util::WindowsErrorDescription(const DWORD iErrorCode, std::wstring & iDescr)
{
	const size_t l_max_size(511);
	iDescr.clear();
	iDescr.resize(l_max_size + 1);
	iDescr[511] = 0;

	DWORD l_ret =
		PAL_absclass::PAL_formatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			iErrorCode,
			0,
			(wchar_t *)iDescr.data(),
			l_max_size,
			NULL
		);
	if (l_ret == 0)
		iDescr.clear();
}
#endif

std::wstring Util::CNTException::ExceptionMsg() const throw ()
{
	TCHAR strErrorCode[64];
	std::wstring ErrorMsg;
	TCHAR message[e_MAX_ERROR_MSG_SIZE + 1];
	DWORD ret;

	_stprintf(strErrorCode, _T("%d"), m_ErrorCode);

	// Get the related NT error message
	ret = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		m_ErrorCode,
		0,
		message,
		e_MAX_ERROR_MSG_SIZE,
		NULL);

	// If 'FormatMessage' has nothing to say, then make one up.
	if (ret == 0)
	{
		ErrorMsg = _T("GetLastError () Return Code: ") + std::wstring(strErrorCode);
	}
	else
	{
		message[e_MAX_ERROR_MSG_SIZE] = '\0';
		ErrorMsg = _T("Error Code: ") + std::wstring(strErrorCode) + _T(" Error Msg: ") + message;
	}

	return ErrorMsg;
}

DWORD Util::CNTException::ErrorCode() const throw ()
{
	return m_ErrorCode;
}
#endif  // PAL_WIN32_RUN (CNTException class implementation)

#if defined(__linux__)  // CLinuxException class implementation (real CPlatformException for Linux)
Util::CLinuxException::CLinuxException() throw()
{
	m_ErrorNo = errno;
}

#if 0   // To enable this function, implement /PAL/PAL_linux/PAL_errno.cpp::PAL_mapErrnoWin322Linux() first.
Util::CLinuxException::CLinuxException(const DWORD &i_ErrorCode) throw()
{
	// The i_ErrorCode is Win32 error code, we need to map it into Linux error code first
	m_ErrorNo = PAL_absclass::PAL_mapErrnoWin322Linux(i_ErrorCode); // Implement /PAL/PAL_linux/PAL_errno.cpp::PAL_mapErrnoWin322Linux() first.
}
#endif

std::wstring Util::CLinuxException::ExceptionMsg() const throw ()
{
	char *str_p;
	char    str_buf[8192];
	wchar_t wchar_str[8192];

	str_p = strerror_r(m_ErrorNo, str_buf, sizeof(str_buf));

	if ((str_p == NULL) || (mbstowcs(wchar_str, str_p, 8192) == (size_t)-1))
	{
		return _T("(unable to get error string)");
	}

	return std::wstring(wchar_str);
}

DWORD Util::CLinuxException::ErrorCode() const throw ()
{
	return m_ErrorNo;
}
#endif  // __linux__ (CLinuxException class implementation)

std::wstring Util::CTimeoutException::ExceptionMsg() const throw ()
{
	return _T("Timeout");
}


Util::CCLibException::CCLibException() throw ()
{
	m_ErrNo = errno;
}

std::wstring Util::CCLibException::ExceptionMsg() const throw()
{
	// Get the related C Library error message



#ifdef UNICODE
	// CJC       6/8/00 12:07:04 PM
	// We need to do a generic converions of strerror( ) 
	// as this function has no "generic" equivalent.
	Util::CSmartPtr<wchar_t> wstrerr = Util::chartowideptr(strerror(m_ErrNo));
	return (_T("Errno= (") + std::wstring(wstrerr) + _T(")"));
#else
	return (_T("Errno= (") + std::wstring(strerror(m_ErrNo)) + _T(")"));
#endif
}

DWORD Util::CCLibException::ErrorCode() const throw()
{
	return m_ErrNo;
}

std::wstring Util::CBadObject::ExceptionMsg() const throw()
{
	return _T("Bad Object");
}

std::wstring Util::CDataNotAvailable::ExceptionMsg() const throw()
{
	return _T("Data Not Available");
}


std::wstring Util::CDynamicCastException::ExceptionMsg() const throw()
{
	return _T("Bad Dynamic Cast on supplied object");
}

std::wstring Util::CIncompatibleNetworkPrototype::ExceptionMsg() const throw()
{
	return _T("Attempted to use and Internal object on an External data flow or Visa/Vesa.");
}

std::wstring Util::NotEnoughPeekData::ExceptionMsg() const throw()
{
	return _T("Not enough Data to Peek");
}

// ftp exception class 

Util::CFTPException::CFTPException(EFTPError i_error) throw () :CBaseException(), m_ErrorType(i_error)
{
	switch (m_ErrorType)
	{
	case e_Connect:
		m_Errormessage = _T("Couldn't connect to server");
		break;
	case e_RemoteDir:
		m_Errormessage = _T("Invalid directory on FTP Server");
		break;
	case e_Send:
		m_Errormessage = _T("Couldn't send file to server");
		break;
	case e_Transferring:
		m_Errormessage = _T("Transfer is active");
		break;
	}
}

std::wstring Util::CFTPException::ExceptionMsg() const throw ()
{
	return m_Errormessage;
}


std::wstring Util::CInvalidMethodCall::ExceptionMsg() const throw ()
{
	return _T("A method has been called that should not have been.");
}

std::wstring Util::CInvalidEnum::ExceptionMsg() const throw()
{
	return _T("An Invalid Enum was encountered");
}

std::wstring Util::CTerminateNow::ExceptionMsg() const throw ()
{
	return _T("Shutdown of thread requested");
}

DWORD Util::CTerminateNow::ErrorCode() const throw ()
{
	return 1;
}
