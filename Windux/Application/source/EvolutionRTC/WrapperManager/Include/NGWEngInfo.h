/**
 * @file NGWEngInfo.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message for engineering debug on internal wrapper CAN messages.
 * Created on: Mar 9, 2018
 *
 **/


#ifndef NGWENGINFO_H
#define NGWENGINFO_H

#include "EvolMessage.h"

class NGWEngInfo : public EvolMessage
{
	public:
	typedef enum
	{
		_NGW_ENG_INFO_NA,		// Reserved for Camera board Begin
		_NGW_ENG_INFO0,
		_NGW_ENG_INFO1,
		_NGW_ENG_INFO2,
		_NGW_ENG_INFO3,
		_NGW_ENG_INFO4,
		_NGW_ENG_INFO5,
		_NGW_ENG_INFO6,
		_NGW_ENG_INFO7,
		_NGW_ENG_INFO8,
		_NGW_ENG_INFO9,
		_NGW_ENG_INFO10,
		_NGW_ENG_INFO11,
		_NGW_ENG_INFO12,
		_NGW_ENG_INFO13,
		_NGW_ENG_INFO14,
		_NGW_ENG_INFO15,
		_NGW_ENG_INFO16,
		_NGW_ENG_INFO17,
		_NGW_ENG_INFO18,
		_NGW_ENG_INFO19,
		_NGW_ENG_INFO20,
		_NGW_ENG_INFO21,
		_NGW_ENG_INFO22,
		_NGW_ENG_INFO23,
		_NGW_ENG_INFO24,
		_NGW_ENG_INFO25,		// Reserved for Camera board End
		_NGW_ENG_INFO26,		// Reserved for Node 1 Begin
		_NGW_ENG_INFO27,
		_NGW_ENG_INFO28,
		_NGW_ENG_INFO29,
		_NGW_ENG_INFO30,
		_NGW_ENG_INFO31,
		_NGW_ENG_INFO32,
		_NGW_ENG_INFO33,
		_NGW_ENG_INFO34,
		_NGW_ENG_INFO35,
		_NGW_ENG_INFO36,
		_NGW_ENG_INFO37,
		_NGW_ENG_INFO38,
		_NGW_ENG_INFO39,
		_NGW_ENG_INFO40,
		_NGW_ENG_INFO41,
		_NGW_ENG_INFO42,
		_NGW_ENG_INFO43,
		_NGW_ENG_INFO44,
		_NGW_ENG_INFO45,
		_NGW_ENG_INFO46,
		_NGW_ENG_INFO47,
		_NGW_ENG_INFO48,
		_NGW_ENG_INFO49,
		_NGW_ENG_INFO50,
		_NGW_ENG_INFO51,
		_NGW_ENG_INFO52,
		_NGW_ENG_INFO53,
		_NGW_ENG_INFO54,
		_NGW_ENG_INFO55,
		_NGW_ENG_INFO56,
		_NGW_ENG_INFO57,
		_NGW_ENG_INFO58,
		_NGW_ENG_INFO59,
		_NGW_ENG_INFO60,		// Reserved for Node 1 End
		_NGW_ENG_INFO70  = 71,	// Reserved for Node 2 Begin
		_NGW_ENG_INFO110 = 101, // Reserved for Node 2 End
		_NGW_ENG_INFO111,		// Reserved for Node 3 Begin
		_NGW_ENG_INFO150 = 151, // Reserved for Node 3 End
		_NGW_ENG_INFO151
	}EngInfoType;

	NGWEngInfo(Hobart::CEvolMsgReceiver *pHobThread = NULL, EngInfoType infoType = _NGW_ENG_INFO_NA );
	NGWEngInfo(const char *buffer);
	~NGWEngInfo();

	void setEngInfo( unsigned char *pData, int length );
	std::string getEngInfo();

	const char *serialize()const;

	private:
	EngInfoType type;
	static const unsigned int  EngInfoSize = 16;
	unsigned char info[EngInfoSize];

};
#endif
