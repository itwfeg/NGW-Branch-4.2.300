#ifndef CRendererUtil_h
#define CRendererUtil_h



#include "PAL_platformheader.h"
#include <string>




static const COLORREF COLOR_BLACK = RGB( 255, 255, 255 );
static const COLORREF COLOR_WHITE = RGB( 0, 0, 0 );



class BitMap;


namespace Hobart
{
	
	class CRendererUtil
	{
	public:

    #if defined(PAL_WIN32_RUN)
		static bool GetHobartBitmap( HBITMAP win_bitmap, BitMap * hobart_bitmap );
    #else
		static bool GetHobartBitmap( PAL_HBITMAP win_bitmap, BitMap * hobart_bitmap );
    #endif
    #if defined(PAL_WIN32_RUN)
		static HBITMAP GetWindowsBitmap( BitMap * hobart_bitmap );
    #else
		static PAL_HBITMAP GetWindowsBitmap( BitMap * hobart_bitmap );
    #endif
    #if defined(PAL_WIN32_RUN)
		static void Draw( BitMap * hobart_bitmap, HBITMAP win_bitmap, int left = 0, int top = 0, DWORD mode = SRCPAINT );
    #else
		static void Draw( BitMap * hobart_bitmap, BitMap *bitmap, int left = 0, int top = 0, DWORD mode = SRCPAINT );
    #endif

    #if defined(PAL_WIN32_RUN)
		static void SaveBitmapToFile( BitMap * hobart_bitmap, const std::string & file_name );
    #else
		static void SaveBitmapToFile( BitMap * hobart_bitmap, const std::string & file_name );
    #endif

    #if defined(PAL_WIN32_RUN)
		static void SaveBitmapToFile( HBITMAP win_bitmap, const std::wstring & file_name );
//    #else
//		static void SaveBitmapToFile( PAL_HBITMAP win_bitmap, const std::string & file_name );
    #endif
	};

}//end of namesapce Hobart



#endif  //CRendererUtil_h
