#include "PAL_platformheader.h"
#if defined( __linux__ )
    #include <netdb.h>
#endif

#include "bootParameters.h"
#include "filesys/file.h"
#include "filesys/directory.h"
#include "properties/properties.h"
#include "CfgInternal.h"
#include "Bool.h"
#include "Integer.h"

#include "LeapStatusWrite.h"


#if defined NET_DEBUG_OUT_LOG_FILE
#include "netshow.h"
#endif

#undef DEBUG

using namespace util_env;
using namespace std;


const string bootParameters::ZERO_IP_ADDRS("0.0.0.0");
const string bootParameters::LOOPBACK_ADDRS("127.0.0.1");
const string bootParameters::LOOPBACK_HOSTNAME("localhost");
const string bootParameters::LOOPBACK_DEV("lo0");

bootParameters *bootParameters::instance = (bootParameters *) NULL;

bootParameters::bootParameters()
{
	//printf ("bootParameters constructor\n");

	pMembers = 0;

	pBootProperties = new util_env::Properties(CfgInternal::getNetworkConfigFileName());
	pConfigDotDat = 0;
}

bootParameters::~bootParameters()
{
	delete pBootProperties;

	if (pConfigDotDat)
		delete pConfigDotDat;
}

bootParameters *bootParameters::getInstance()
{
	if (instance == 0)
	{
		instance = new bootParameters();
	}

	return instance;
}


bool bootParameters::isValid()
{
	string temp = getIPAddress();
    return (temp.size() > 0);
}

std::string bootParameters::getIPAddress()
{
#if defined ( __linux__ )

	PIP_ADAPTER_INFO AdapterAddresses = NULL;
	ULONG OutBufferLength = 0;	
	ULONG RetVal = 0;
	std::string result = "";

	AdapterAddresses = new IP_ADAPTER_INFO;

	for (int i=0; i < 4; i++)
	{
		RetVal = PAL_absclass::PAL_getAdaptersInfo(AdapterAddresses, &OutBufferLength);

		if (RetVal != ERROR_BUFFER_OVERFLOW)
		{
			break;
		}

		if (AdapterAddresses != NULL)
		{
			free(AdapterAddresses);
		}

		AdapterAddresses = (PIP_ADAPTER_INFO) malloc(OutBufferLength);

		if (AdapterAddresses == NULL)
		{
			break;
		}
	}

	if (RetVal == ERROR_SUCCESS)
	{
		PIP_ADAPTER_INFO AdapterList = AdapterAddresses;

		while(AdapterList)
		{
			if (strcmp(AdapterList->IpAddressList.IpAddress.String, "") != 0)
			{
				result = AdapterList->IpAddressList.IpAddress.String;				
				break;			
			}

			AdapterList = AdapterList->Next;
		}	
	}
	
	delete AdapterAddresses;
	return result;

#else
	char buff[256];
	memset(buff, 0, 256);
	gethostname(buff, 256);		
		
	hostent* localHost = gethostbyname("");

	if ( localHost )
	{
		char* ipBuffer = inet_ntoa(*(struct in_addr*)*localHost->h_addr_list);
		if ( ipBuffer )
		{
			Debug::trace("IPAddress", "The IP address of the local host is " + std::string(ipBuffer));

			return ipBuffer;
		}
		else
			Debug::trace("IPAddress", "We could not retrieve the IP address of the local host");
	}

	return "";
#endif
}


std::string bootParameters::getDNSHostName()
{
	char buffer[64];
	DWORD bufferSize = 64;
	std::string hostname;

#if defined( PAL_WIN32_RUN )
	if ( GetComputerNameExA(ComputerNamePhysicalDnsHostname, buffer, &bufferSize) != 0 )
#elif   defined( __linux__ )
    if ( gethostname(buffer, bufferSize) == 0 )
#else
    #error "Implement gethostname() for the system"
#endif
    {
		hostname.assign(buffer);
    }

	return hostname;
}


bool bootParameters::isValidIp(const string& ipOrDns)
{
string::size_type pos1 = 0, pos2 = 0;
string ipStr("");
int intIP = 0;

	if (ipOrDns.size() ==  0)
		return false;

	for (int i = 0; i < 4; i++)
	{

		ipStr = "";

		pos2 = ipOrDns.find(".", pos1);

		if ((i < 3) && (pos2 != string::npos))
		{
			ipStr = ipOrDns.substr(pos1, (pos2 - pos1));
			pos1 = pos2 + 1;
		}
		else if (i == 3)
		{
			ipStr = ipOrDns.substr(pos1);
		}

    #if defined(LINUX_FIX)
		size_t sizeSubstr = ipStr.size();
    #else
		int sizeSubstr = ipStr.size();
    #endif

		if (sizeSubstr == 0)
			return false;

    #if defined(LINUX_FIX)
		for (size_t j = 0; j < sizeSubstr; j++)
    #else
		for (int j = 0; j < sizeSubstr; j++)
    #endif
		{
			if (isdigit(ipStr[j]) == 0)
				return false;
		}

		intIP = atoi (ipStr.c_str());

		if ((intIP < 0) || (intIP > 255))
			return false;

	}


	return true;
}


#if defined( PAL_WIN32_RUN )
#include "ws2tcpip.h"
#endif
STATUS bootParameters::resolveHostNameIntoIpAddress(const string& hostName, string& ipAddress)
{
	int retVal;
	struct addrinfo aiHints;
	struct addrinfo *aiList = NULL;
	struct addrinfo *rp = NULL;

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;

	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the aiList variable will hold a linked list
	// of addrinfo structures containing response
	// information about the host
	if ((retVal = getaddrinfo(hostName.c_str(), NULL, &aiHints, &aiList)) == 0)
	{
		if ( aiList != NULL )
		{
			for ( rp = aiList; rp != NULL; rp = rp->ai_next)
			{ 			
				//get the ip address for the current list item
				struct sockaddr_in * sock = (struct sockaddr_in*)rp->ai_addr;
								
				if ( (strcmp(inet_ntoa(sock->sin_addr), "127.0.0.1")) && (strcmp(inet_ntoa(sock->sin_addr), "127.0.1.1")) )				
				{
					ipAddress.assign(inet_ntoa(sock->sin_addr));
					
					if (ipAddress.size()) //only return if we have a valid ip address					
					{					
						freeaddrinfo(aiList);
						return OK;
					}
				}
			}		
		}
		else
			Debug::trace("network", "aiList was null");
	}
	else
		Debug::trace("network", "call to getaddrinfo failed with error " + util_string::inttostring(retVal));

	//need to clean up after ourselves...
	freeaddrinfo(aiList);

	return ERROR_CODE;
}

