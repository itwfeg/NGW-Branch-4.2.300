/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCNETWORKSTATUS_H
#define HFCNETWORKSTATUS_H

#include "evolution.h"
#include "NetworkStatus.h"
#include "HfcObject.h"


class HfcNetworkStatus : public NetworkStatus, public HfcObject
{
	public:

		HfcNetworkStatus(const HfcNetworkStatus&);
		HfcNetworkStatus(const char *data);

		/* We don't receive any of these messages, we only send them out when requested;
		 * therefore, we don't have the constructor that takes the buffer received from the outside world.
		 * It's just here for consistency.
		 */
		HfcNetworkStatus( const char *, int , int ) {}
		~HfcNetworkStatus();

		HobString getLastOnlineDateTime() const { return lastOnlineDateTime_; }
		void setLastOnlineDateTime(const HobString &lastOnlineDateTime) { lastOnlineDateTime_ = lastOnlineDateTime; }

		virtual const char *serialize(int format) const;

	private:

		HobString lastOnlineDateTime_;


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCNETWORKSTATUS_H
