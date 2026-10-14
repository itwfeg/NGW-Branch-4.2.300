#include "SystemFontsList.h"
#include <ugl/ugl.h>
#include <ugl/ugltypes.h>
#include "string.h"
#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

LOCAL int dataType = 0;
LOCAL string dataTypeName = "SystemFontsList";
#endif

SystemFontsList::SystemFontsList()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = registerDataType(dataTypeName);

	addInstance(dataType);
#endif

	empty = (char)NULL;
	listEmpty = false;	
	char *fontName = new char[UGL_FONT_FAMILY_NAME_MAX_LENGTH];
	UGL_FONT_DRIVER_ID fontDrvId;
	UGL_SEARCH_ID searchId;	


	fontList = new std::list<char *>;

	//find the font engine	UGL_PRT_FONT_ENGINE_TYPE	UGL_FONT_ENGINE_TYPE
	uglDriverFind (UGL_FONT_ENGINE_TYPE, 0, (UGL_UINT32 *)&fontDrvId);

	//find the first font on the list
	searchId = uglFontFindFirst(fontDrvId, &fontDescription);

	if(searchId != UGL_NULL)
	{
		//push onto the list only if we have a valid list from the driver	
		strcpy(fontName, fontDescription.faceName);  
 		Debug::trace("SystemFontsList",  HobString("Inserting:") + fontName);

		fontList->push_back(fontName);
	
		UGL_STATUS status = UGL_STATUS_OK;

		//get all remaining fonts
	   	while(status == UGL_STATUS_OK)
	   	{ 
			if((status = uglFontFindNext(fontDrvId, &fontDescription, searchId)) != UGL_STATUS_ERROR)
			{
				if(status != UGL_STATUS_FINISHED)
				{			
					//push font name onto the list
					fontName = new char[UGL_FONT_FAMILY_NAME_MAX_LENGTH];

					strcpy(fontName, fontDescription.faceName); 
					Debug::trace("SystemFontsList",  HobString("Inserting:") + fontName);
					fontList->push_back(fontName);
				}
			}
			else
			{
	  			Debug::trace("SystemFontsList", "Font Find Error");
			}

		}	
		iter = fontList->begin();
	}
	else
	{
		//the font driver does not have a list of fonts
		listEmpty = true;
		delete fontList; 
	}
}

SystemFontsList::~SystemFontsList()
{
#ifdef USE_ACCOUNTANT
	removeInstance(dataType);
#endif

	if(!listEmpty)
	{
		//delete the allocated char arrays then delete the list
		iter = fontList->begin();
		while(iter != fontList->end())
		{
	 		Debug::trace("SystemFontsList",  HobString("deleting:") + *iter);
			delete[] *iter++;
		}
 		Debug::trace("SystemFontsList",  "deleting font list");
		delete fontList;

	}
}

char *SystemFontsList::getFontName()
{
char *pFontName = NULL;

	if(!listEmpty)
	{
   		if(iter != fontList->end())
		{
			pFontName = *iter++;
		}
		else
		{
			iter = fontList->begin();
		}		
	}
	return pFontName; 
}
