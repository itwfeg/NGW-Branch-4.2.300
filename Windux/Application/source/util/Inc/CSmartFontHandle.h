#ifndef CSMARTFONTHANDLE_H
#define CSMARTFONTHANDLE_H



#include "PAL_platformheader.h"

#if !defined( PAL_WIN32_RUN )
#include <PAL_graphics.h>
#endif

#include "GenericHandle.h"

#include <string>



#define DEFAULT_FONT_PATH		"/usr/local/hobart/system/fonts/sp.ttf"

namespace Util
{
	static const int ATTR_ITALIC           = 0x0001;
	static const int ATTR_UNDERLINE        = 0x0002;
	static const int ATTR_STRIKEOUT        = 0x0004;
	static const int ATTR_BOLD             = 0x0010;

#ifdef UNICODE
#if defined(PAL_WIN32_RUN)
	HFONT CreatePrinterFont(HDC hdc, const char* szFaceName, int iDeciPtHeight,
		int iDeciPtWidth = 0, int iAttributes = 0, bool fLogRes = false);
#else
	PAL_FT_HANDLE CreatePrinterFont(const char* szFaceName, int iDeciPtHeight,
		int iDeciPtWidth = 0, int iAttributes = 0, bool fLogRes = false);
#endif
#endif

#if defined(PAL_WIN32_RUN)
	PAL_HFONT CreatePrinterFont(HDC hdc, const TCHAR* szFaceName, int iDeciPtHeight,
		int iDeciPtWidth = 0, int iAttributes = 0, bool fLogRes = false);
#else
	PAL_FT_HANDLE CreatePrinterFont(const TCHAR* szFaceName, int iDeciPtHeight,
		int iDeciPtWidth = 0, int iAttributes = 0, bool fLogRes = false);

	/// @name FreeType
	/// FreeType library routines.
	//@{
	/// Allocates handle and initialize the font library.
	///@return 0 on success and -1 on failure.
	int32_t initFtLib(PAL_FT_HANDLE &hndl);

	//@}
#endif//PAL_WIN32_RUN

	class CSmartFontHandle : public GenericHandle
	{
		public:
			/* --- Types and subtypes --- */


			/* --- Constructors and destructor --- */
#if defined(PAL_WIN32_RUN)
			explicit CSmartFontHandle ( HFONT handle = NULL );
			CSmartFontHandle( HDC hdc, const TCHAR* faceName, int ptHeight );
#ifdef UNICODE
			CSmartFontHandle( HDC hdc, const char* faceName, int ptHeight );
#endif
#else
			explicit CSmartFontHandle ( );
			CSmartFontHandle( const TCHAR* faceName, int ptHeight );
#ifdef UNICODE
			CSmartFontHandle( const char* faceName, int ptHeight );
#endif
#endif

			CSmartFontHandle ( const CSmartFontHandle& rhs );
			~CSmartFontHandle ( );


			/* --- Getters and setters --- */
#if defined(PAL_WIN32_RUN)
			HFONT Get ( ) const { return (HFONT)GenericHandle::Get(); }
#else
			PAL_FT_HANDLE Get ( ) const { return (PAL_FT_HANDLE)GenericHandle::Get(); }
#endif

			/* --- Instance function members --- */
			CSmartFontHandle& operator= ( HANDLE handle ) {
#if defined(PAL_WIN32_RUN)
#else
				DeleteGenericHandle( Get() );
#endif
				GenericHandle::operator=(handle);
#if defined(PAL_WIN32_RUN)
#else
				PAL_FT_HANDLE hndl( Get() );
				if ( hndl )
					++hndl->refCount;
#endif
				return *this;
			}

			CSmartFontHandle& operator= ( const CSmartFontHandle& rhs ) {
#if defined(PAL_WIN32_RUN)
#else
				DeleteGenericHandle( Get() );
#endif
				GenericHandle::operator=(rhs);
#if defined(PAL_WIN32_RUN)
#else
				PAL_FT_HANDLE hndl( Get() );
				if ( hndl )
					++hndl->refCount;
#endif
				return *this;
			}


		protected:
			/* --- Instance function members --- */
			void DeleteGenericHandle( HANDLE h ) {
				if ( h != NULL ) {
#if defined(PAL_WIN32_RUN)
					DeleteFont(h);
#else
					PAL_FT_HANDLE hndl = (PAL_FT_HANDLE) h;
					if ( 0 >= --hndl->refCount ) {
						if (NULL != hndl->face) {
							FT_Done_Face(hndl->face);
							hndl->face = NULL;
						}
						if (NULL != hndl->lib) {
							FT_Done_FreeType(hndl->lib);
							hndl->lib = NULL;
						}

						delete hndl;
						h = NULL;
					}
#endif
				}
			}

		private:
			/* --- Class data members --- */
#ifdef USE_ACCOUNTANT
			static int _dataType_;
			static std::string _dataTypeName_;
#endif
	};



}

#endif
