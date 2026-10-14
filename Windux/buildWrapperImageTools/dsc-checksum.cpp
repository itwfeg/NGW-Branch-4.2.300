//============================================================================
// Name        : dsc-checksum.cpp
// Author      : Aaron Swift
// Version     : 1.0
// Date		   : 9/1/2016
// Copyright   : Hobart Corporation 2016
// Description :
//============================================================================
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <iterator>
#include <vector>
#include <string>
#include "SrecParser.h"

using namespace std;

//#define DEFAULT_CHECKSUM

int main(int argc, char* argv[]) {
	std::vector<unsigned char> imageBuffer;
	unsigned long checksum = 0;
	std::string path, version, id, major, minor;

	if( argc >= 4 )
	{
		path = argv[1];
		version = argv[2];
		id = argv[3];
	}
	else
	{
		cout << "Invalid number of arguments!" << endl;
		cout << "arguments: " << "path and file name " << "version " << "node id" << endl;
		cout << "Valid dsc-checksum arguments example: " << "/node1/source.bin 1.5 1" << endl;
		return -1;
	}

	std::size_t loc =  version.find('.', 0);
	if( ( version.size() < 3 ) || ( loc == std::string::npos ) )
	{
		cout << "Invalid Version number string!" << endl;
		cout << "Please use following format: " << "0.0" << endl;
		return -1;
	}

	major = version.substr (0,loc);
	minor = version.substr (loc + 1, version.size());

	cout << "dsc-checksum" << endl;
	cout << "Hobart Corporation " << endl;

	cout << endl;
	cout << endl;
	cout << endl;

	cout << "path: " << path << endl;
	cout << "version: " << version << endl;
	cout << "image id: " << id << endl;

	std::ifstream upgradeFile( path.data(), std::ifstream::in | std::ifstream::out | std::ios::binary  );

	/*stop removing my data! */
	upgradeFile.unsetf(std::ios::skipws);

	imageBuffer.insert(imageBuffer.begin(),
				std::istream_iterator<unsigned char>(upgradeFile),

				std::istream_iterator<unsigned char>());
	upgradeFile.close();


	//create a new binary image container
	std::vector<unsigned char> binImage;

	//convert the s-record file to binary
	if(( "1" == id ) || ( "2" == id ) || ( "3" == id ) || ( "4" == id ) || ( "5" == id )) {
		SrecParser sRecParser( imageBuffer, binImage, SrecParser::NXP56f8367);
	} else if ( "6" == id ) {
		SrecParser sRecParser( imageBuffer, binImage, SrecParser::NXP56f8322);
	} else {
		cout << "Invalid node id!" << endl;
		return -1;
	}

	int imageSize( binImage.size() );
	int versionOffset;
	int checksumOffset;
	int nodeIdOffset;
	int checksumSize;

	if (id != "6") {
		versionOffset = 0x3ffe0 * 2;
		checksumOffset = 0x3ffe4 * 2;
		nodeIdOffset = 0x3ffe8 * 2;
		checksumSize = versionOffset;
	} else {
		versionOffset = 0x3ff5 * 2;
		checksumOffset = 0x3ff0 * 2;
		nodeIdOffset = 0x3ff8 * 2;
		checksumSize = checksumOffset;
	}

	//calculate the image checksum
	if(!imageSize){
		cout << "failed to open source file!  " << path << endl;
		return 1;
	}

	cout << "image size: " << imageSize << endl;

#ifdef DEFAULT_CHECKSUM
	checksum = 0x12345678;
#else
	int index = 0;

	do {
		unsigned char x = 0, y = 0;
		unsigned short result = 0;
		x = binImage[index++];
		y = binImage[index++];
		result = y;
		result <<= 8;
		result |= x;
		checksum += result;
	} while (index < checksumSize);

	checksum &= 0x00FFFFFFFF;
#endif

	unsigned char msb1 = ( checksum >> 24 );
	unsigned char msb2 = ( checksum >> 16 );
	unsigned char lsb1 = ( checksum >> 8 );
	unsigned char lsb2 = ( checksum  & 0x00000000FF);

	unsigned char verh;
	unsigned char verl;

	//copy the version into the image
	int offset = versionOffset;

	verh = (unsigned char)( ( atoi(major.c_str()) & 0xff00 ) >> 8 ) ;
	verl = (unsigned char)( ( atoi(major.c_str())  & 0x00ff )  );

	binImage[offset++] = verl;
	binImage[offset++] = verh;

	verh = (unsigned char)( ( atoi(minor.c_str()) & 0xff00 ) >> 8 ) ;
	verl = (unsigned char)( ( atoi(minor.c_str())  & 0x00ff )  );

	binImage[offset++] = verl;
	binImage[offset++] = verh;

	//copy the checksum into the image
	offset = checksumOffset;
	binImage[offset++] = msb2;
	binImage[offset++] = msb1;
	binImage[offset++] = lsb2;
	binImage[offset++] = lsb1;

	//copy the node id into the image
	offset = nodeIdOffset;
	binImage[offset++] = id[0] - '0';
	binImage[offset++] = 0x00;

	path = "node" + id + ".bin";
	std::ofstream myFile( path.data(), std::ifstream::out | std::ios::binary  );
	myFile.write(reinterpret_cast<const char*>( &binImage[0] ), imageSize);
	myFile.close();

	cout << "checksum: " << std::hex << checksum << endl;

	return 0;
}
