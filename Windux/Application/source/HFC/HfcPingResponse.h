/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPINGRESPONSE_H
#define HFCPINGRESPONSE_H

#include "evolution.h"
#include "HfcObject.h"

class HobString;

class HfcPingResponse : public HfcObject
{
	public:

		HfcPingResponse(int attempts = 0, int replies = 0);

		~HfcPingResponse();

		int getAttempts() const;
		void setAttempts(int count);

		int getReplies() const;
		void setReplies(int count);

		const char *serialize(int format) const;


	private:
		int			attempts;		//	# of times the Ping was attempted
		int			replies;		//	# of successful replies

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPINGRESPONSE_H
