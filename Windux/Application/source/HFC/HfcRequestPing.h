/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCREQUESTPING_H
#define HFCREQUESTPING_H

#include "evolution.h"
#include "HfcObject.h"

class HobString;

typedef enum HfcRequestPingFieldOffsets
{
	fieldOffsetPingAddress = 1,
	fieldOffsetPingCount,
	fieldOffsetRepeatCount,

	endHfcRequestPingFieldOffsets	//	MUST be last!
} HfcRequestPingFieldOffsets_t;

class HfcRequestPing : public HfcObject
{
	public:

		HfcRequestPing();
		HfcRequestPing(const char *data, int len, int formatType);

		~HfcRequestPing();

		HobString &getAddress();
		void setAddress(HobString &addr);

		int getPingCount();
		void setPingCount(int count);

		int getRepeatCount();
		void setRepeatCount(int count);


	private:
		HobString	address;
		int			pingCount;		//	# of times to Ping the address
		int			repeatCount;			//	# of times to repeat the pingCount

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCREQUESTPING_H
