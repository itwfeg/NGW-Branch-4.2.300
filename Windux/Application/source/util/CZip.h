#if !defined CZip_h
#define CZip_h

//#include "zip.h"

namespace Hobart
{
	class CZip
	{
    // ***** WARNING: This is the same definition as in OutputLogger.h
#if defined(LINUX_FIX)

#if defined(PAL_WIN32_RUN)
    #define PATH_SEPARATOR              "\\"

#elif   defined(__linux__)
    #define PATH_SEPARATOR              "/"

#else
    #error  "define related VARIABLE"
#endif

#endif  // LINUX_FIX

	public:
		static int zipDir(const char * path, const char * dest_file);
		static int unZip( const char * zipfilename, const char * dirname);
		static int unZipSingleFile( const char * zipfilename, const char *targetFile, const char * dirname);
		
	};

}// end of namespace

#endif
