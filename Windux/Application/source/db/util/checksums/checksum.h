#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "PAL_platformheader.h"



#ifndef NO_NAMESPACES
namespace util_checksums
{
#endif



unsigned short icrc(unsigned short crc, unsigned char* bufptr, size_t len);   /* (SJJC port 6/19/06) */

unsigned int crc32(unsigned int crc, const unsigned char *bufptr, size_t len);



#ifndef NO_NAMESPACES
} // namespace util_checksums
#endif



#endif // !CLOPTIONS_H

