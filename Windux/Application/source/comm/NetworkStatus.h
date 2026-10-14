/* Fastrack Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef NETWORKSTATUS_H
#define NETWORKSTATUS_H

#include "evolution.h"
#include "CEvolMsgReceiver.h"
#include "EvolMessage.h"


class NetworkStatus : public EvolMessage
{
	public:

		NetworkStatus( Hobart::CEvolMsgReceiver *pReceiver = NULL, bool online = false, int networkType = 0, int wirelessStrength = 0 );
		NetworkStatus(const char *buffer);
		~NetworkStatus();

		const char *serialize() const;

		bool isOnline() const { return online_; }
		void setOnline(bool online) { online_ = online; }

		int getNetworkType() const { return networkType_; }
		void setNetworkType(int networkType) { networkType_ = networkType; }

		int getWirelessStrength() const { return wirelessStrength_; }
		void setWirelessStrength(int wirelessStrength) { wirelessStrength_ = wirelessStrength; }

	private:

		bool online_;           // false/0 = offline, true/1 = online
		int networkType_;       // 0 = wired, 1 = wireless
		int wirelessStrength_;


#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //NETWORKSTATUS_H
