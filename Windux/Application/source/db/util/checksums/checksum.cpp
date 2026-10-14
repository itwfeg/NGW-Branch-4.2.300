#include "checksum.h"



#ifndef NO_NAMESPACES
namespace util_checksums
{
#endif

#if defined(LINUX_FIX)
    // Remove them (check if Win could pass compiling)
#else
#define LOBYTE(x) ((unsigned char)((x) & 0xFF))
#define HIBYTE(x) ((unsigned char)((x) >> 8))
#endif


/* The following two methods implement a CRC-16 checksum algorithm. */
unsigned short icrc1(unsigned short crc)
{
	for ( int i = 0; i < 8; ++i )
	{
		if ( crc & 0x8000 )
        {
        #if defined(LINUX_FIX)
			crc = (unsigned short)((crc << 1) ^ 4129);
        #else
			crc = (unsigned short)((crc <<= 1) ^ 4129);
        #endif
        }
		else
        {
			crc <<= 1;
        }
	}

	return crc;
}


unsigned short icrc(unsigned short crc, unsigned char* bufptr, size_t len)  /* (SJJC port 6/19/06) */
{
	static unsigned short crcTable[256], init = 0;
	if ( !init )
	{
		for ( unsigned short j = 0; j < 256; ++j )
			crcTable[j] = icrc1((unsigned short)(j<<8));

		init = 1;
	}

	for ( unsigned long j = 0; j < len; ++j )
		crc = (unsigned short)(crcTable[bufptr[j] ^ HIBYTE(crc)] ^ (LOBYTE(crc) << 8));

	return crc;
}



/* The following two methods implement a CRC-32 checksum algorithm.
This code is copyright © 1993 Richard Black. All rights are reserved.
You may use this code only if it includes a statement to that effect. */

const int QUOTIENT = 0x04c11db7;
unsigned int crctab[256];

void crc32_init(void)
{
    unsigned int crc;

    for ( int i = 0; i < 256; ++i )
    {
        crc = i << 24;
        for ( int j = 0; j < 8; ++j )
        {
            if ( crc & 0x80000000 )
                crc = (crc << 1) ^ QUOTIENT;
            else
                crc = crc << 1;
        }
#if !defined(BIG_ENDIAN)
        crctab[i] = (crc&0xff000000)>>24 | (crc&0x00ff0000)>>16 | (crc&0x0000ff00)>>8 | (crc&0x000000ff)<<24; // htonl(crc)
#else
		crctab[i] = crc;
#endif
    }
}


unsigned int crc32(unsigned int crc, const unsigned char *data, size_t len)
{
	static bool init = true;
    unsigned int result = ~crc;
    unsigned int *p = (unsigned int *)data;
    unsigned int *e = (unsigned int *)(data + len - (len & 0x00000003));
    
	if ( init ) { crc32_init(); init = false; }

    while( p < e )
    {
        result ^= *p++;

#if !defined(BIG_ENDIAN)
        result = crctab[result & 0xff] ^ result >> 8;
        result = crctab[result & 0xff] ^ result >> 8;
        result = crctab[result & 0xff] ^ result >> 8;
        result = crctab[result & 0xff] ^ result >> 8;
#else
        result = crctab[result >>  24] ^ result << 8;
        result = crctab[result >>  24] ^ result << 8;
        result = crctab[result >>  24] ^ result << 8;
        result = crctab[result >>  24] ^ result << 8;
#endif
    }
    
	if ( len & 0x00000003 )
	{
		size_t bytes = len & 0x00000003;
		unsigned char *c = (unsigned char *)p;
#if !defined(BIG_ENDIAN)
		for ( size_t i = 0; i < bytes; ++i )
			result ^= *c++ << (i * 8);
#else
		for ( size_t i = bytes; i > 0; ++i )
			result ^= *c++ << (i * 8);
#endif

		for ( size_t i = 0; i < bytes; ++i )
#if !defined(BIG_ENDIAN)
			result = crctab[result & 0xff] ^ result >> 8;
#else
	        result = crctab[result >>  24] ^ result << 8;
#endif
	}

    return ~result;
}



#ifndef NO_NAMESPACES
} // namespace util_env
#endif

