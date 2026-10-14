#ifndef BOOTPARAMETERS_H
#define BOOTPARAMETERS_H

//#include "resolvLib.h"
//#include <end.h>
#include "PAL_platformheader.h"
#include "evolution.h"


typedef struct bp_member_vars
{
	BOOL m_DHCPCLibInitialized;
	BOOL m_bLeaseObtained;
	BOOL m_bLoopBackCreated;
	BOOL m_attached;
	BOOL m_wirelessCard;
	BOOL m_bConnected;
	BOOL m_wirelessCardSetup;
	BOOL m_bDnsOneShot;
	char m_interface[10];
	int m_unit;
	char m_interfaceID[11];
#define MAXIPADDRLEN 200 
	char m_oldNet1[MAXIPADDRLEN];
	char m_oldNet2[MAXIPADDRLEN];

	int m_oldMask;

} BP_MEMBER_VARS;


#include <string>

namespace util_env
{
class Properties;
}


class bootParameters
{
	public:

		~bootParameters();

		static bootParameters *getInstance();

		#define WEP_KEY_LENGTH 26
		#define ENCRYPTED_STRING_ID_CHAR 0x7E

		typedef enum NetworkBootType
		{
			imgHBALLNetworkBootType,
			imgReleaseNetworkBootType,
			EndOfBootTypes //This is not a real component, just a marker of the end of the list. it must always be there
		}   NetworkBootType_t;


		const static std::string ZERO_IP_ADDRS;
		const static std::string LOOPBACK_ADDRS;
		const static std::string LOOPBACK_HOSTNAME;
		const static std::string LOOPBACK_DEV;

		static bool isValidIp(const std::string& ipOrDns);
		static STATUS resolveHostNameIntoIpAddress(const std::string& hostName, std::string& ipAddress);

		bool isValid();
		static std::string getIPAddress();
		static std::string getDNSHostName();

	private:
		bootParameters();

		static bootParameters *instance;
	
		BP_MEMBER_VARS * pMembers;

		util_env::Properties * pBootProperties;
		util_env::Properties * pConfigDotDat;
};

#endif //BOOTPARAMETERS_H
