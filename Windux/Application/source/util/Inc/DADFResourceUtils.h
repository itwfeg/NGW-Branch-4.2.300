// Header file included by ANTLR UniLexer


#ifdef PAL_WIN32_RUN
#include <tchar.h>
#endif

#ifndef Util_DADFResourceUtils_h
#define Util_DADFResourceUtils_h


/**************************************************************************
****  DADF MODULE HEADER   ***


TYPE:			Functions
		

PURPOSE:		To be able to wait easily and efficiently on object(s)


FUNCTION:		see above


INTERFACES:		"See 'INTERFACES:' in the module declaration below"
	

RESOURCES:	

REFERENCES:		


PROCESSING:		See Above


DATA:			See 'DATA:' in the functions below.

****  END MODULE HEADER   ***
**************************************************************************/
#ifdef PAL_WIN32_RUN
#pragma  warning(disable: 4786)	
#endif


#include <vector>
#include <deque>

#include "SmartHandle.h"



// Some DADF_Global definitions....
enum E_GLOBAL_DEFS
{
	DADF_TERM_TIMEOUT = 30000
};

namespace Util
{
	// Description
	//		Resource claim routines that use vectors of handles
	// Arguments
	//		i_Resources		The resources to wait for
	//		i_Timeout		The timeout to return after if the resources
    //						have not become signalled
	DWORD MSGWaitForOneOfMultipleObjects(std::vector<HANDLE>& i_Resources,
										 DWORD i_Timeout = INFINITE);

	// Description
	//		Resource claim routines that use a handle
	// Arguments
	//		i_Resources		The resources to wait for
	//		i_Timeout		The timeout to return after if the resources
    //						have not become signalled
	DWORD MSGWaitForSingleObject(HANDLE i_Handle, 
								 DWORD i_Timeout= INFINITE);

	// Description
	//		Resource claim routines that use vectors of handles
	// Arguments
	//		i_Resources		The resources to wait for
	//		i_Timeout		The timeout to return after if the resources
    //						have not become signalled
	DWORD MSGWaitForMultipleObjects(std::vector<HANDLE>& i_Handles, 
									DWORD i_Timeout = INFINITE);



	// using namespace Util;

/*	// Description
	//		Wait for all the objects to be signalled
	//  Parameters
	//		i_Resources			The Resources to wait for...
	//		i_Timeout			The time to wait for the signalling before returning	
	inline DWORD MSGWaitForMultipleObjects(std::deque<Util::CSmartHandle>& i_Resources, 
										   DWORD i_Timeout= INFINITE )
	{
		// typedef std::deque<Util::CSmartHandle> container;
		std::deque<CSmartHandle>::iterator l_it;
		std::vector<HANDLE> l_VectorOfHandles;
		
		// Get the SmartHandles into the correct format.
		for (l_it = i_Resources.begin() ; l_it != i_Resources.end() ; l_it++)
		{
			l_VectorOfHandles.push_back((*l_it).GetHandle());
		}
		return MSGWaitForMultipleObjects(l_VectorOfHandles,
										 i_Timeout);
	}

	
	// Description
	//		Wait for all the objects to be signalled
	//  Parameters
	//		i_Resources			The Resources to wait for...
	//		i_Timeout			The time to wait for the signalling before returning	
	inline DWORD MSGWaitForOneOfMultipleObjects(std::deque<Util::CSmartHandle>& i_Resources, 
												DWORD i_Timeout= INFINITE)
	{
		//typedef std::deque<Util::CSmartHandle> container;
		std::deque<CSmartHandle>::iterator l_it;
		std::vector<HANDLE> l_VectorOfHandles;
		
		// Get the SmartHandles into the correct format.
		for (l_it = i_Resources.begin() ; l_it != i_Resources.end() ; l_it++)
		{
			l_VectorOfHandles.push_back(((CSmartHandle)(*l_it)).GetHandle());
		}
		return MSGWaitForOneOfMultipleObjects(l_VectorOfHandles,
											  i_Timeout);
	}
*/

	// Description
	//			Wait for of the the list of objects to be signalled
	//  Parameters
	//			i_Resource			The object to wait for
	//			i_Timeout			The time to wait for the signalling before returning	
	inline DWORD MSGWaitForSingleObject(CSmartHandle& i_Resource,DWORD i_Timeout= INFINITE)
	{
		return MSGWaitForSingleObject(i_Resource.get(),i_Timeout);
	}

} // end of the namespace

#endif

