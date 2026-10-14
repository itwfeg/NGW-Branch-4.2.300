//============================================================================
// Name        : SrecParser.cpp
// Author      : Aaron Swift
// Version     : 1.0
// Date		   : 10/31/2016
// Copyright   : Hobart Corporation 2016
// Description :
//============================================================================

#include <iostream>
#include "SrecParser.h"
#include <stdlib.h>
#include <fstream>


using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SrecParser::SrecParser(std::vector<unsigned char> & sRecImage_, std::vector<unsigned char> & binaryImage_, ProccessorType processor) {
	int sRecImageSize_ = sRecImage_.size(), index_ = 0;
	bool pad_ = false;
	unsigned long imageSize_ = 0;
	endOfImage_ = false;

	cout << "S record image size:" << sRecImageSize_ <<endl;

	//which processor is the s-record file?
	if( processor ==  NXP56f8367 ) {
		versionAddress_ 	= NXP56f8367_VersionAddress;
		endAddress_ 		= NXP56f8367_BootAddress;
		imageSize_ 			= NXP56f8367_ImageSize;
	}
	else if ( processor ==  NXP56f8322 ) {
		versionAddress_ 	= NXP56f8322_VersionAddress;
		endAddress_ 		= NXP56f8322_BootAddress;
		imageSize_			= NXP56f8322_ImageSize;
	}


	std::vector<unsigned char> v(imageSize_);	//512K OR 32k

	while(!endOfImage_) {
		//do we have a valid S record?
		if( sRecImage_[index_++] == 'S' ) {

			//get the record type
			recordType_ = (RecordTypes)asciiToHex( sRecImage_[index_++] );

			if((recordType_ == S9_START_ADDR) || (recordType_ == S8_START_ADDR) || (recordType_ == S7_START_ADDR)) {
				endOfImage_ = true;
			}
			else {

				// get the record length
				recordLength_ = asciiToHex(sRecImage_[index_++]);
				recordLength_ <<= 4;
				recordLength_ |= asciiToHex(sRecImage_[index_++]);
				if( recordLength_ != 0x1d ) {
					cout << "record is length = " << std::hex << recordLength_ << endl;
				}

				switch(recordType_) {

					case S0_HEADER:
					{
						unsigned char byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );

						loadAddress_ = byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );

						loadAddress_ |= byte_;

						addressFieldLength_ = 4;
						//include the checksum into the data field length
						dataFieldLength_ = recordLength_ - addressFieldLength_;

						//we ignore the header record so skip to the next record (line feed characters + 2)
						index_ += ( dataFieldLength_ * 2 ) + 2;

						cout << "S0_HEADER record is ingnored: " << endl;
						break;
					}
					case S1_DATA:
					case S5_DATA_RECORD_COUNT:
					case S9_START_ADDR:
					{
						loadAddress_ = 0;

						//parse load address (2 byte address)
						unsigned char byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );

						loadAddress_ = byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;

						addressFieldLength_ = 2;
						dataFieldLength_ = recordLength_ - addressFieldLength_ - CHECKSUM_LEN;
						//these record types do not apply so skip to the next record
						index_ += ( dataFieldLength_ * 2 );

						cout << "S1_DATA record ignored: " << endl;
						break;
					}
					case S2_DATA:
					case S8_START_ADDR:
					{
						loadAddress_ = 0;
						//parse load address (3 byte address)

						unsigned char byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );

						loadAddress_ = byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;

						addressFieldLength_ = 3;
						dataFieldLength_ = recordLength_ - addressFieldLength_ - CHECKSUM_LEN;

						cout << "Record load address: " << std::hex << loadAddress_ << endl;
						break;
					}
					case S3_DATA:
					case S7_START_ADDR:
					{
						loadAddress_ = 0;
						//parse load address (4 byte address)
						unsigned char byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );

						loadAddress_ = byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );
						loadAddress_ |= byte_;
						loadAddress_ <<= 8;

						byte_ = 0;
						byte_ = asciiToHex( sRecImage_[index_++] );
						byte_ <<= 4;
						byte_ |= asciiToHex( sRecImage_[index_++] );

						loadAddress_ |= byte_;

						addressFieldLength_ = 4;

						cout << "Record load address: " << std::hex << loadAddress_ << endl;

						//do not convert the bootloader section of the srecord
						if(loadAddress_  >= endAddress_) {
							cout << "Ignoring boot loader record!" << endl;

							//determine the length of the data
							dataFieldLength_ = recordLength_ - addressFieldLength_ - CHECKSUM_LEN;
							index_ += 2 * dataFieldLength_;

							//move passed checksum and line feed characters
							index_ += (2 + 2);

							continue;
						}
						break;
					}
					default:
					{
						cout << "Unknown S record type: " << std::hex << recordType_ << endl;
					}
				}

				if( ( recordType_ == S3_DATA ) || ( recordType_ == S7_START_ADDR ) ) {
					if( !endOfImage_ ) {
						//determine the length of the data
						dataFieldLength_ = recordLength_ - addressFieldLength_ - CHECKSUM_LEN;
						unsigned long address = ( loadAddress_ * 2) ;
						for( int i = 0; i < dataFieldLength_; i++ ) {
							unsigned char byte_ = 0;
							byte_ = asciiToHex( sRecImage_[index_++] );
							byte_ <<= 4;
							byte_ |= asciiToHex( sRecImage_[index_++] );
							v[ address++ ] = byte_;
						}



						//are we at the version address in the image?
						if( loadAddress_ != versionAddress_ ) {

							//save the load address so that we can pad the
							//image after we find the version address.
							previousLoadAddress_ = loadAddress_;
							previousDataLength_ =  dataFieldLength_;

							if(loadAddress_ == NXP56f8367_NodeIDAddress)
							{
								//pad the last remaining bytes from 0x3ffe8 -> 0x40000
								unsigned long startAddress_ = (loadAddress_ * 2) + 2;
								while( startAddress_ !=  ( NXP56f8367_BootAddress * 2) ) {
									v[startAddress_++] = 0xff;
								}
							}
							//pad 4 bytes between checksum and nodeid
							if(loadAddress_ == NXP56f8367CheckSumAddress ) {
								v[(NXP56f8367CheckSumAddress * 2) + 4] = 0xff;
								v[(NXP56f8367CheckSumAddress * 2) + 5] = 0xff;
								v[(NXP56f8367CheckSumAddress * 2) + 6] = 0xff;
								v[(NXP56f8367CheckSumAddress * 2) + 7] = 0xff;
							}
						}
						else {
							if( !pad_ ) {

								//pad the image with blank flash bytes until the version address is reached
								unsigned long startAddress_ = ( ( previousLoadAddress_ * 2) + previousDataLength_);
								while( startAddress_ !=  ( loadAddress_ * 2) ) {
									v[startAddress_++] = 0xff;
								}

								pad_= true;

								//add 4 bytes between the version and the checksum
								v[ address++ ] = 0xff;
								v[ address++ ] = 0xff;
								v[ address++ ] = 0xff;
								v[ address++ ] = 0xff;
							}
						}
						//move passed checksum and line feed characters
						index_ += (2 + 2);
					}
				}
			}
		}
		else {
			cout << "Unknown S record File!" << endl;
			return;
		}
	}
	binaryImage_ = v;
	cout << "End of S record File reached." << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SrecParser::~SrecParser() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned char SrecParser::asciiToHex(unsigned char dataByte)
{
	if(dataByte > 0x2F && dataByte < 0x3A)
	{
		dataByte -= 0x30;                        // ASCII characters '0', '1', ... , '9'
	}
	else
	{
		dataByte |= 0x20;                        // ASCII characters 'A', 'B', ... , 'F' to 'a', 'b', ... , 'f'
		if(dataByte > 0x60 && dataByte < 0x67)
		{
			dataByte -= 0x57;                    // ASCII characters 'a', 'b', ... , 'f'
        }
		else
		{
			cout << "asciiToHex(): Fatal Error!" << endl;
		}
	}
	return dataByte;
}

