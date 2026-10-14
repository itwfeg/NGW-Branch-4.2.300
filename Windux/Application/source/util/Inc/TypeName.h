// Header file included by ANTLR UniLexer
#ifndef TYPENAME_H
#define TYPENAME_H

#if defined ( _WIN32 )
#include <tchar.h>
#endif
#include "ErrorHandling.h"
#include "CCriticalSection.h"
#include "utilunicode.h"
namespace Util
{
extern short_to_wide_string_map gwTypeIdMap;// global giving "wide" typename 

	class TypeNameCSContainer
	{
	public:
	static Util::CSmartCriticalSection sm_TypeNameCS;		// Declared in CTLSThread.cpp so that no new .cpp file needed.
	};

	template <class C>
	const char *TypeName( const C& r )
	{
		try
		{
			Util::CLockCriticalSection l( TypeNameCSContainer::sm_TypeNameCS);
#ifdef UNICODEAAAAAA

// CJC	 6/9/00 5:37:57 PM typeid has no Unicode equivalent.
// store the typeid names in a string -> wstring map on an
// "as required" basis

			std::string typeName( typeid( r ).name() );
			Util::short_to_wide_string_map::iterator iter;
			iter = Util::gwTypeIdMap.find(typeName);
			// check if it is already in the map
			if( iter != Util::gwTypeIdMap.end())
				return (*iter).second.c_str();
			else // alloc a wstring
			{
				// convert 
				Util::CSmartPtr<wchar_t> wtn = Util::chartowideptr( typeName.c_str());
				std::wstring   wstrTypeId(wtn);
				// insert the pair into the map
				std::pair<std::string,std::wstring> aPair = std::make_pair(typeName,wstrTypeId);
				std::pair<short_to_wide_string_map::iterator,bool> 
					ins = Util::gwTypeIdMap.insert( aPair);
				Assert( true == ins.second , Util::CNamedException(_T(" Bad Map Insertion")));
				short_to_wide_string_map::iterator & sw = ins.first;
				// return the inserted version...
				return ((*sw).second).c_str();
				
			}
			
#else
			return typeid( r ).name();
#endif
		}
		catch (...)
		{
			LogCatchAndThrow;
		}
	}
}

#endif
