#ifndef NETWORKEVENT_H
#define NETWORKEVENT_H

	// This file contains all the events that occur in the Evolution Wrapper Project
	#include "Event.h"

	// Network Generated Events - NetworkDev
	typedef enum NetworkEvent
	{
		NetworkReqPing = 20,
		NetworkPingResponse,
		NetworkStopPing,

		CfgTcpIpRead = 30,
		CfgTcpIpWrite,
		CfgTcpIpSave,
		CfgTcpConfigInProgress,

		CfgDnsRead = 40,
		CfgDnsWrite,
		CfgDnsSave,
		CfgDnsSetupSuccessful,

		CfgWirelessCardRead = 50,
		CfgWirelessCardWrite,
		CfgWirelessCardSave,
		CfgWirelessCardActivateWEP,
		CfgWirelessCardSetActiveKey,
		CfgWirelessCardSetEncryptionKey,

		NetworkStartSignalStrength = 60,
		NetworkStopSignalStrength,
		NetworkSignalStrength,
		NetworkStartConnectionStatus,
		NetworkStopConnectionStatus,
		NetworkConnectionStatus,

		NetworkTftpUpgradeRead = 70,
		NetworkTftpUpgradeWrite,
		NetworkTftpUpgradeSave,

		cfgLEAPRead = 80,
		cfgLEAPWrite,
		cfgLEAPSave,
		cfgLEAPStatusRead,
		cfgLEAPStatusWrite,
		cfgLEAPStatusPollOn,
		cfgLEAPStatusPollOff,

		vpnCfgLoad = 90,
		vpnRevert,
		
		NetworkErrReadAll = 100,
		NetworkErrDeleteSelected,
		NetworkErrDeleteAll,
	} NetworkEvent_t;

	typedef enum NetworkDevError
	{
		NetworkDevUnknownError = NetworkDev * DEVICE_ERROR_OFFSET,
		NetworkSetupFailed,
		DnsSetupFailed,
		EncryptionKeyInvalid,
		ReinitializingNetwork,
		ReinitializingDNS,
		ReinitializingWirelessCard,
		ReinitDNSAutoMatic
	} NetworkDevError_t;

#endif // NETWORKEVENT_H
