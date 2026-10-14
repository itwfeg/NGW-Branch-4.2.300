#include "Blob.h"
#include "stdlib.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Blob::dataType = 0;
std::string Blob::dataTypeName("Blob"); 
#endif


Blob::Blob(INT32 size, bool nibblized) :
		size_(0), maxSize_(0), buffer_(0), nibblized_(nibblized), nibblizedBuffer_(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( size > 0 )
		resize(size);
}


Blob::Blob(std::string blobData, bool nibblized) :
		size_(0), maxSize_(0), buffer_(0), nibblized_(true), nibblizedBuffer_(0)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ( resize((int)blobData.length()) )
	{
		memcpy(buffer_, blobData.data(), maxSize_);
		size_ = maxSize_;
	}
	nibblized_ = nibblized;
}


Blob::~Blob()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if ( buffer_ )
		free(buffer_);
	if ( nibblizedBuffer_ )
		free(nibblizedBuffer_);
}


void Blob::unNibblizeBlob()
{
	//do not allow user to unnibblize the blob multiple times
#if defined(LINUX_FIX)
	if ( nibblized_ && size_ && buffer_)
#else
	if ( nibblized_ && size_ )
#endif
	{
		UINT8* source = buffer_;
		UINT8* destination = buffer_;

		/* Create the unNibblized data. */
		size_ = size_ / 2;
		for ( INT32 i = 0 ; i < size_; ++i )
        {
        #if defined(LINUX_FIX)
			*(destination)  = (UINT8)(blobCharToHex(*(source++))) << 4;
			*(destination) = *(destination) + (UINT8)(blobCharToHex(*(source++)));
            destination++;
        #else
			*(destination ++) = (UINT8)(blobCharToHex(*(source ++)) * 16 + blobCharToHex(*(source ++)));
        #endif
        }
	}
	
	nibblized_ = false;
}


void Blob::nibblizeBlob()
{
	if ( nibblizedBuffer_ )
		free(nibblizedBuffer_);

	nibblizedBuffer_ = (UINT8 *)malloc(size_ * 2);

	/* Spin through the data, creating nibblized version of each byte. */
	if ( nibblizedBuffer_ )
	{
		UINT8* ptr = buffer_;
		
		for ( INT32 i = 0; i < size_; ++i, ++ptr )
		{
			*(nibblizedBuffer_++) = blobCharToHex(getUpperNibble(ptr));
			*(nibblizedBuffer_++) = blobCharToHex(getLowerNibble(ptr));
		}
	}
}


void Blob::append(INT8 val)
{
	if ( resize(size_ + 1) )
	{
		*(INT8 *)(buffer_ + size_) = val;
		++size_;
	}
}


void Blob::append(UINT8 val)
{
	if ( resize(size_ + 1) )
	{
		*(UINT8 *)(buffer_ + size_) = val;
		++size_;
	}
}


void Blob::append(const UINT8* bytes, int size)
{
	if ( resize(size_ + size) )
	{
		memcpy(buffer_ + size_, bytes, size);
		size_ += size;
	}
}


void Blob::append(INT16 val)
{
	if ( resize(size_ + sizeof(INT16)) )
	{
		*(INT16 *)(buffer_ + size_) = val;
		size_ += sizeof(INT16);
	}
}


void Blob::append(UINT16 val)
{
	if ( resize(size_ + sizeof(UINT16)) )
	{
		*(UINT16 *)(buffer_ + size_) = val;
		size_ += sizeof(UINT16);
	}
}


void Blob::append(INT32 val)
{
	if ( resize(size_ + sizeof(INT32)) )
	{
		*(INT32 *)(buffer_ + size_) = val;
		size_ += sizeof(INT32);
	}
}


void Blob::append(UINT32 val)
{
	if ( resize(size_ + sizeof(UINT32)) )
	{
		*(UINT32 *)(buffer_ + size_) = val;
		size_ += sizeof(UINT32);
	}
}


UINT8 Blob::getUpperNibble(UINT8 *byte)
{
	int value = 0, add = 1;

	for ( int mask = 16 ; mask <= 128 ; mask <<= 1, add <<= 1)
	{
		if ( mask & *byte )
			value += add;
	}

	return (UINT8)value;
}


UINT8 Blob::getLowerNibble(UINT8 *byte)
{
	int value = 0;

	for ( int mask = 1 ; mask <= 8 ; mask <<= 1)
	{
		if ( mask & *byte )
			value = value + mask;
	}

	return (UINT8)value;
}


bool Blob::isBlobLabel()
{
	char temp[5] = {0};

	//check the first 4 bytes of the blobBuffer to see if a Label
	for(int i = 0; i < 4; i++)
	{
		temp[i] = (char) *buffer_++;
	}
	temp[4] = '\0';

	//set the pointer back to the start of the blobBuffer
	for(int i = 0; i < 4; i++)
	{
		buffer_--;
	}

	if(!strcmp(temp, "LABL"))
	{
		return true;
	}
	return false;

}


bool Blob::isBlobGraphic()
{
	char temp[5] = {0};

	//check the first 4 bytes of the blobBuffer to see if a Graphic
	for(int i = 0; i < 4; i++)
	{
		temp[i] = (char) *buffer_++;
	}
	temp[4] = '\0';

	//set the pointer back to the start of the blobBuffer
	for(int i = 0; i < 4; i++)
	{
		buffer_--;
	}

	if(!strcmp(temp, "PICT"))
	{
		return true;
	}
	return false;
}


void Blob::getBytes(long int position, UINT8 *destinationBuffer, UINT32 numberOfBytes)
{
	const UINT8 * source;
//	UINT8 * destination;

	source = buffer_ + position;

	while (numberOfBytes--)
	{
		*(destinationBuffer ++) = *(source ++);
	}
}


UINT8 Blob::toHex(UINT8 decimal)
{
	switch (decimal)
	{
		case 0: return (UINT8)'0';
		case 1: return (UINT8)'1';
		case 2: return (UINT8)'2';
		case 3: return (UINT8)'3';
		case 4: return (UINT8)'4';
		case 5: return (UINT8)'5';
		case 6: return (UINT8)'6';
		case 7: return (UINT8)'7';
		case 8: return (UINT8)'8';
		case 9: return (UINT8)'9';
		case 10: return (UINT8)'A';
		case 11: return (UINT8)'B';
		case 12: return (UINT8)'C';
		case 13: return (UINT8)'D';
		case 14: return (UINT8)'E';
		case 15: return (UINT8)'F';
		default: return (UINT8)'X';
	}
}


UINT8 Blob::blobCharToHex(UINT8 c)
{
	if ( '0' <= c && c <= '9' )
		return (UINT8)(c - '0');
	if ( 'A' <= c && c <= 'F' )
		return (UINT8)(c - '7');
	if ( 'a' <= c && c <= 'f' )
		return (UINT8)(c - 'W');

	return 0;
}


short Blob::ShortSwapBytes(short x)
{
	unsigned short t = x;
	return ((unsigned short)((t&0x00FF)<<8) + (unsigned short)(t>>8));
}


unsigned short Blob::UShortSwapBytes(unsigned short x)
{
	return ((unsigned short)((x&0x00FF)<<8) + (unsigned short)(x>>8));
}


long Blob::LongSwapBytes(long x)
{
	unsigned long t = x;

	return (((unsigned long) UShortSwapBytes((unsigned short)(x&0x0000FFFF))<<16) +
				(unsigned long)(UShortSwapBytes((unsigned short)(t>>16))));
}


unsigned long Blob::ULongSwapBytes(unsigned long x)
{
	return (((unsigned long) UShortSwapBytes((unsigned short)(x&0x0000FFFF))<<16) +
				(unsigned long)(UShortSwapBytes((unsigned short)(x>>16))));
}


void Blob::ensureSize(int size)
{
	resize(size);
}


bool Blob::resize(int newMaxSize)
{
	if ( newMaxSize > maxSize_ )
	{
		UINT8* newBuffer = (UINT8 *)realloc(buffer_, newMaxSize);
		if ( newBuffer )
		{
			buffer_ = newBuffer;
			maxSize_ = newMaxSize;
		}
	}
	return true;
}

