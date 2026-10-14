//============================================================================
// Name        : SrecParser.h
// Author      : Aaron Swift
// Version     : 1.0
// Date		   : 10/31/2016
// Copyright   : Hobart Corporation 2016
// Description :
//============================================================================
#ifndef SRECPARSER_H_
#define SRECPARSER_H_
#include <vector>
class SrecParser {


public:
	typedef enum
	{
		NXP56f8322,
		NXP56f8357,
		NXP56f8367
	}ProccessorType;

	SrecParser(std::vector<unsigned char> & sRecImage_, std::vector<unsigned char> & binaryImage_, ProccessorType processor);
	~SrecParser();

	unsigned char asciiToHex(unsigned char dataByte);


	typedef enum
	{
		S0_HEADER,
		S1_DATA,
		S2_DATA,
		S3_DATA,
		S5_DATA_RECORD_COUNT = 5,
		S7_START_ADDR = 7,
		S8_START_ADDR,
		S9_START_ADDR
	}RecordTypes;

	static const int 			CHECKSUM_LEN =	1;
	static const unsigned long 	NXP56f8367_VersionAddress 	= 0x3ffe0;
	static const unsigned long 	NXP56f8367CheckSumAddress	= 0x3ffe4;
	static const unsigned long 	NXP56f8367_NodeIDAddress	= 0x3ffe8;
	static const unsigned long	NXP56f8367_BootAddress		= 0x40000;
	static const unsigned long	NXP56f8367_ImageSize		= 524288;

	static const unsigned long 	NXP56f8322_CheckSumAddress 	= 0X03ff0;
	static const unsigned long 	NXP56f8322_VersionAddress 	= 0X03ff5;
	static const unsigned long 	NXP56f8322_NodeIDAddress	= 0X03ff8;
	static const unsigned long 	NXP56f8322_BootAddress		= 0x20000;
	static const unsigned long 	NXP56f8322_ImageSize		= 32768;


private:

	std::vector<unsigned char> binaryImage_;
	bool endOfImage_;
	RecordTypes recordType_;
	unsigned short recordLength_;
	unsigned short addressFieldLength_;
	unsigned long loadAddress_;
	unsigned long versionAddress_;
	unsigned long previousLoadAddress_;
	unsigned long endAddress_;
	unsigned short dataFieldLength_;
	unsigned short previousDataLength_;

};
#endif /* SRECPARSER_H_ */
