# ifndef BLOB_H
# define BLOB_H

#include <string>
#include "PAL_platformheader.h"

class Blob
{
	public:
		// --- Constructors and destructor ---
		Blob(INT32 size = 0, bool nibblized = true);
		Blob(std::string blobData, bool nibblized = true);
		~Blob();


		// --- Getter and setter methods ---
		INT32 getMaxSize() const { return maxSize_; }
		INT32 getSize() const { return size_; }
		INT32 getNibblizedSize() const { return size_ * 2; }
		UINT8 *getBlobBuffer() { return buffer_; }
		UINT8 *getNibblizedBuffer() { return nibblizedBuffer_; }
		bool isNibblized() const { return nibblized_; }
		void setNibblized(bool nibblized = true) { nibblized_ = nibblized; }
		bool isBlobLabel();
		bool isBlobGraphic();

		INT8   getInt8  (INT32 position) const { return *(INT8   *)(buffer_ + position); }
		UINT8  getUint8 (INT32 position) const { return *(UINT8  *)(buffer_ + position); }
		INT16  getInt16 (INT32 position) const { return *(INT16  *)(buffer_ + position); }
		UINT16 getUint16(INT32 position) const { return *(UINT16 *)(buffer_ + position); }
		INT32  getInt32 (INT32 position) const { return *(INT32  *)(buffer_ + position); }
		UINT32 getUint32(INT32 position) const { return *(UINT32 *)(buffer_ + position); }


		// --- Processing methods ---
		void unNibblizeBlob();
		void nibblizeBlob();

		void getBytes(long int position, UINT8 *destinationBuffer, UINT32 numberOfBytes);
		short ShortSwapBytes(short x);
		unsigned short UShortSwapBytes(unsigned short x);
		long LongSwapBytes(long x);
		unsigned long ULongSwapBytes(unsigned long x);

		void append(INT8 val);
		void append(UINT8 val);
		void append(const UINT8* bytes, int size);
		void append(INT16 val);
		void append(UINT16 val);
		void append(INT32 val);
		void append(UINT32 val);

		void ensureSize(int size);


	private:
		INT32 size_;                        // Size of the data stored in buffer
		INT32 maxSize_;                     // Size of the buffer (usually greater than size)
		UINT8 *buffer_;                     // Buffer where data is stored
		bool nibblized_;                    // True if blob is nibblized. Default: true
		UINT8 *nibblizedBuffer_;

		UINT8 toHex(UINT8 decimal);
		UINT8 blobCharToHex(UINT8 c);
		UINT8 getUpperNibble(UINT8 *byte);
		UINT8 getLowerNibble(UINT8 *byte);
		
		bool resize(int size);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//BLOB_H

