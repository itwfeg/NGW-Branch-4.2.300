#include <math.h>

#ifdef __linux__
#include <ft2build.h>
#include FT_FREETYPE_H
#endif


#include "PAL_platformheader.h"
#include "CSmartFontHandle.h"


#include "util/e_string/utilstring.h"
#include "Debug.h"

#ifdef __linux__
#include "Configuration.h"
#endif

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace Util
{


#ifdef USE_ACCOUNTANT
	int CSmartFontHandle::_dataType_ = 0;
	std::string CSmartFontHandle::_dataTypeName_("Util::CSmartFontHandle");
#endif


#ifndef PAL_WIN32_RUN

///////////////////////////////////////////////////////////////////////////////////////////////////

int32_t initFtLib(PAL_FT_HANDLE &hndl)
{
	
	//
	//Allocate the handle and initialize the
	//font library.
	//
	hndl = new(PAL_FT);

	if (NULL == hndl)
	{
		Debug::trace("Renderer.initFont", "initFtLib: Unable to allocate memory for the handle");
		return -1;
	}

	hndl->lib = NULL;
	hndl->face = NULL;
	hndl->refCount = 1;

	if (0 != FT_Init_FreeType(&(hndl->lib)))
	{
		Debug::trace("Renderer.initFont", "initFtLib: Unable to initialize FT library");
		return -1;
	}
		
	return 0;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PAL_WIN32_RUN
	CSmartFontHandle::CSmartFontHandle ( HFONT handle ) :
		GenericHandle( handle )
#else
	CSmartFontHandle::CSmartFontHandle ( ) :
		GenericHandle( NULL )
#endif
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
#ifdef PAL_WIN32_RUN
	CSmartFontHandle::CSmartFontHandle( HDC hdc, const char* faceName, int ptHeight ) :
		GenericHandle(CreatePrinterFont(hdc, faceName, ptHeight))
#else
	CSmartFontHandle::CSmartFontHandle( const char* faceName, int ptHeight ) :
		GenericHandle(CreatePrinterFont(faceName, ptHeight))
#endif
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PAL_WIN32_RUN
	CSmartFontHandle::CSmartFontHandle( HDC hdc, const TCHAR* faceName, int ptHeight ) :
		GenericHandle(CreatePrinterFont(hdc, faceName, ptHeight))
#else
	CSmartFontHandle::CSmartFontHandle( const TCHAR* faceName, int ptHeight ) :
		GenericHandle(CreatePrinterFont(faceName, ptHeight))
#endif
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	CSmartFontHandle::CSmartFontHandle ( const CSmartFontHandle& rhs ) :
		GenericHandle(rhs)
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

#ifdef PAL_WIN32_RUN
#else
		PAL_FT_HANDLE hndl( Get() );
		if ( hndl )
			++hndl->refCount;
#endif
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

	CSmartFontHandle::~CSmartFontHandle ( )
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

#ifdef PAL_WIN32_RUN
		HFONT font = (HFONT)Release();
		
		if ( font != NULL )
			DeleteFont(font);
#else
		DeleteGenericHandle( Get() );
#endif
	}

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
#ifdef PAL_WIN32_RUN
	HFONT CreatePrinterFont(HDC hdc, const char* szFaceName, int iDeciPtHeight,
						int iDeciPtWidth, int iAttributes, bool fLogRes)
#else
	PAL_FT_HANDLE CreatePrinterFont(const char* szFaceName, int iDeciPtHeight,
						int iDeciPtWidth, int iAttributes, bool fLogRes)
#endif
	{
#ifdef PAL_WIN32_RUN
		size_t length = strlen(szFaceName);
		TCHAR tFaceName[LF_FACESIZE];

		for ( size_t i = 0; i < length; ++i )
			tFaceName[i] = szFaceName[i];
#else
		//
		//retrieve the font path from the configuration file, 
		//convert face name string and call appropriate routine 
		//for further processing.  We also save the font name
		//location in the string in order to convert it to lower
		//case later.
		//
		TCHAR tFaceName[PAL_FACESZ];
		std::string font_face = Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMFONTSDIR);
		font_face += "/";
		size_t fname_loc = font_face.length();
		font_face += szFaceName;
		font_face += ".ttf";


		Debug::trace("Renderer.CreateFont", "CreatePrinterFont: Font name= " + font_face);
		char *tmp_str = (char *) font_face.c_str();
		size_t length = font_face.length();

		for ( size_t i = 0; i < length; ++i )
		{
			if (i >= fname_loc)
				tFaceName[i] = std::tolower(tmp_str[i]);
			else
				tFaceName[i] = tmp_str[i];
		}

#endif

		tFaceName[length] = '\0';

		Debug::trace("Renderer.CreateFont", "CreatePrinterFont: creating font for face=" + std::string(szFaceName) + ", point size=" +
			util_string::inttostring(iDeciPtHeight));

#ifdef PAL_WIN32_RUN
		return CreatePrinterFont(hdc, tFaceName, iDeciPtHeight, iDeciPtWidth, iAttributes, fLogRes);
#else
		return CreatePrinterFont(tFaceName, iDeciPtHeight, iDeciPtWidth, iAttributes, fLogRes);
#endif
	}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef PAL_WIN32_RUN
	HFONT CreatePrinterFont(HDC hdc, const TCHAR* szFaceName, int iDeciPtHeight,
						int iDeciPtWidth, int iAttributes, bool fLogRes)
#else
	PAL_FT_HANDLE CreatePrinterFont(const TCHAR* szFaceName, int iDeciPtHeight,
						int UNUSED(iDeciPtWidth), int UNUSED(iAttributes), bool UNUSED(fLogRes))
#endif
	{
#ifdef PAL_WIN32_RUN
		HFONT      hFont ;
		LOGFONT    lf ;
		POINT      pt ;
		TEXTMETRIC tm ;
	     
	//     int state = SaveDC (hdc) ;
	     
	//     SetGraphicsMode (hdc, GM_ADVANCED) ;
	//     ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
	//     SetViewportOrgEx (hdc, 0, 0, NULL) ;
	//     SetWindowOrgEx   (hdc, 0, 0, NULL) ;
	     
		if (fLogRes)
		{
			pt.x = MulDiv(iDeciPtWidth,  GetDeviceCaps (hdc, LOGPIXELSX), 72);
			pt.y = MulDiv(iDeciPtHeight, GetDeviceCaps (hdc, LOGPIXELSY), 72);
	     
//			Debug::trace("Renderer.CreateFont", "CreatePrinterFont1 face=" + std::string(szFaceName) + ", point size=" +
//				util_string::inttostring(iDeciPtHeight) + ", new point size=" + util_string::inttostring(pt.y));
		}
		else
		{
			pt.x = (LONG) ( (iDeciPtWidth  * (FLOAT)203.2) / 72 + (FLOAT)0.5) ;
			pt.y = (LONG) ( (iDeciPtHeight * (FLOAT)203.2) / 72 + (FLOAT)0.5) ;
	     
//			Debug::trace("Renderer.CreateFont", "CreatePrinterFont2 face=" + std::string(szFaceName) + ", point size=" +
//				util_string::inttostring(iDeciPtHeight) + ", new point size=" + util_string::inttostring(pt.y));
		}
	   
	//     DPtoLP (hdc, &pt, 1) ;
		 lf.lfHeight         = - pt.y;
		 lf.lfWidth          = 0 ;
		 lf.lfEscapement     = 0 ;
		 lf.lfOrientation    = 0 ;
		 lf.lfWeight         = iAttributes & ATTR_BOLD      ? 700 : 0 ;
		 lf.lfItalic         = iAttributes & ATTR_ITALIC    ?   1 : 0 ;
		 lf.lfUnderline      = iAttributes & ATTR_UNDERLINE ?   1 : 0 ;
		 lf.lfStrikeOut      = iAttributes & ATTR_STRIKEOUT ?   1 : 0 ;
		 lf.lfCharSet        = DEFAULT_CHARSET ;
		 lf.lfOutPrecision   = 0 ;
		 lf.lfClipPrecision  = 0 ;
		 lf.lfQuality        = 0 ;
		 lf.lfPitchAndFamily = 0 ;
//		 lstrcpy (lf.lfFaceName, L"Arial") ;
		 lstrcpy (lf.lfFaceName, szFaceName) ;
	     
		 hFont = CreateFontIndirect (&lf) ;
	     
		 if (iDeciPtWidth != 0)
		 {
			  hFont = (HFONT) SelectObject (hdc, hFont) ;
	          
			  GetTextMetrics (hdc, &tm) ;
	          
			  DeleteObject (SelectObject (hdc, hFont)) ;
	          
			  lf.lfWidth = (int) (tm.tmAveCharWidth *
											fabs ((double)pt.x) / fabs ((double)pt.y) + 0.5) ;
	          
			  hFont = CreateFontIndirect (&lf) ;
		 }
	     
		Debug::trace("Renderer.CreateFont", "CreatePrinterFont created font with handle=" + util_string::ptrtostring(hFont));

	//     RestoreDC (hdc, state) ;
		 return hFont ;
#else

        PAL_FT_HANDLE hndl = NULL;

		//
		//We are called from the constructor always,
		//initialize the library & convert the name.
		// 
		FT_Int err = initFtLib(hndl);
		if (0 != err)
		{
			Debug::trace("Renderer.CreateFont", "CreatePrinterFont: Unable to initialize FT library");
			return NULL;
		}
		
		const uint8_t hob_font_name_sz=250;
		char f_name[hob_font_name_sz];
		const size_t str_size = wcsnlen(szFaceName, NAME_MAX-1);

		//
		// convert wide chars to char
		//
		uint32_t i;
		for(i=0;i<str_size;i++)
		{
			f_name[i] = char(szFaceName[i]);
		}
		f_name[i] = '\0';

		Debug::trace("Renderer.CreateFont", "CreatePrinterFont: " + std::string(f_name) + " is being used");
		//
		//Attempt to open the face, upon failure use the default font
		//
		err = FT_New_Face(hndl->lib,  f_name, 0, &(hndl->face));
		if (0 != err)
		{
			Debug::trace("Renderer.CreateFont", "CreatePrinterFont: Font facetype creation failed! " + util_string::inttostring(err) );

			err = FT_New_Face(hndl->lib, DEFAULT_FONT_PATH, 0, &(hndl->face));
			if (0 != err)
			{
				Debug::trace("Renderer.CreateFont", "CreatePrinterFont: Unable to acquire facetype " + util_string::inttostring(err) );

				// cleanup
				FT_Done_FreeType(hndl->lib);
				delete hndl;

				return NULL;
			}
		}

		//
		//both widht and height are set to the same value 
		//(0=same number as the other i.e heigth if width 
		//is set to zero and width if height is set to zero)
		//

		// FT-495 attempt to set pixel size before point sizes, mimic windows code above
		int pixelHeight = (int) ( (iDeciPtHeight  * (double)203.2) / 72 + (double)0.5) ;
		int pixelWidth = 0;
/*
		err = FT_Set_Pixel_Sizes(hndl->face, pixelWidth, pixelHeight);

		if ( iDeciPtWidth )
		{
			int tempPixelWidth = (int) ( (iDeciPtWidth  * (double)203.2) / 72 + (double)0.5) ;

			if ( hndl->face->available_sizes )
			{
				int avgWidth = hndl->face->available_sizes->width;

				if ( avgWidth )
					pixelWidth = (int) (avgWidth *
							fabs ((double)tempPixelWidth) / fabs ((double)pixelHeight) + 0.5) ;
			}
		}
*/
		err = FT_Set_Pixel_Sizes(hndl->face, pixelWidth, pixelHeight);
		if (0 != err)
		{
			Debug::trace("Renderer.CreateFont", "CreatePrinterFont: Could not set pixel size");

			err = FT_Set_Char_Size(hndl->face, iDeciPtHeight * 64, 0, 203, 203);
			if (0 != err)
			{
				Debug::trace("Renderer.CreateFont", "CreatePrinterFont: Unable to set size");

				// cleanup
				FT_Done_Face(hndl->face);
				FT_Done_FreeType(hndl->lib);
				delete hndl;

				return NULL;
			}
		}

		return hndl;
#endif//PAL_WIN32_RUN
	}
}
