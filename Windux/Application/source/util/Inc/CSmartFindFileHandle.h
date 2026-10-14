// Header file included by ANTLR UniLexer
#include <tchar.h>
#ifndef CSmartFindFileHandle_h
#define CSmartFindFileHandle_h

namespace Util
{

class CSmartFindFileHandle
{
public:
	CSmartFindFileHandle(){m_Handle=NULL;};
	CSmartFindFileHandle(HANDLE i_Handle){m_Handle=i_Handle;};

	~CSmartFindFileHandle()
	{
		FindClose(m_Handle);
	};
	

	operator HANDLE ()
			{return m_Handle;};

	CSmartFindFileHandle& operator = (HANDLE rhs) 
		 {
			if (m_Handle != NULL)
			{
				FindClose(m_Handle);
			}
			m_Handle = rhs;
			return *this;
		 }

	const bool operator == (HANDLE i_Handle) const 
			{return m_Handle == i_Handle;}

		
	const bool operator != (HANDLE i_Handle) const 
			{return !(m_Handle == i_Handle);}
private:
	HANDLE m_Handle;
};

}; // end namespace

#endif