/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "VersionInfo.h"
#include "Configuration.h"
#include "BuildNumber.h"
#include "VersionManager.h"
#include "HobComplexVersion.h"
#include "bootParameters.h"
#include "HobVersion.h"
#include "HobTokenizerCSV.h"
#include "PythonScripter.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#if defined( __linux__ )
#include "Hex.h"                        // For DNS IP address (simplpy Hex output)
#endif

#if defined(PAL_WIN32_RUN)
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#elif   defined( __linux__ )
#include <ifaddrs.h>                 // for getifaddres()
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int VersionInfo::dataType = 0;
std::string VersionInfo::dataTypeName("VersionInfo");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
VersionInfo::VersionInfo()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
VersionInfo::~VersionInfo()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////

// Needed to get a list of the current DNS IP addresses.
// This should really go into the PAL but the code below that
// uses it is already split between Linux and Windows. (Originally,
// the windows code used the registry (which is wrong) to get at
// the IP's and the Linux uses something else.
//
// on entry, buf is a buffer allocated by the caller, bufLen is the length of that buffer in bytes. 
// on exit, buf will contain a list of IP address, if buf is not big enough, it will be a partial list
// return value is the number of characters actually copied into buf. (zero indicates an error.)
bool VersionInfo::getDNSList( char* buf, unsigned int bufLen )
{
	bool bReturn( false );

#if defined(PAL_WIN32_RUN)
	FIXED_INFO*		pFixedInfo;		// Pointer to return buffer. new'ed and delete'd here
	IP_ADDR_STRING* pIPAddr;		// Linked list of IP addresses. This will contain the DNS IP addresses
	unsigned long	ulNeeded;	
	unsigned long	ulTotal;
	int				ii;

	ulNeeded = 0; //sizeof(FIXED_INFO);
	ulTotal = 0;

	// Determine how much memory we actuall need.
	if (GetNetworkParams( NULL, &ulNeeded ) == ERROR_BUFFER_OVERFLOW )
	{
		//  ulNeeded will have the size. Allocate it
		pFixedInfo = (FIXED_INFO*)new char[ulNeeded];
		if( pFixedInfo != NULL ) 
		{
			// Get stuff about the network
			if( GetNetworkParams( pFixedInfo, &ulNeeded ) == NO_ERROR )
			{
				// Start copying the DNS IP's. This is the first.
				ii = 0;
				while( pFixedInfo->DnsServerList.IpAddress.String[ii] && ulTotal++ < bufLen )
				{
					*buf++ = pFixedInfo->DnsServerList.IpAddress.String[ii++];
				}
				// Next IP in the list
				pIPAddr = pFixedInfo->DnsServerList.Next;
				
				// Until there are no more, keep getting them
				while( pIPAddr ) 
				{
					ulTotal++;
					*buf++ = ' '; //Need to delimit with spaces for the parser

					// Get the next IP
					ii = 0;
					while( pIPAddr->IpAddress.String[ii] && ulTotal++ < bufLen )
					{
						*buf++ = pIPAddr->IpAddress.String[ii++];
					}
					pIPAddr = pIPAddr->Next;
				}
				if( ulTotal < bufLen ) // means we should have got them all the IP's and there's one more for the null
				{
					// terminate the string
					*buf = '\0';
					// everything worked. If we don't get here, we've failed
					bReturn = true;
				}
			}
		}
	}
	delete [] pFixedInfo;

#elif defined( __linux__ )

	buf[0] = '\0';

	res_state res( (res_state) malloc( sizeof(*res) ) );

	if (res != NULL)
	{
		if (res_ninit( res ) == 0)
		{
			// res->nsaddr_list[0].sin_addr.s_addr contains 32-bits IP address (uint32_t)
//	PTR 1445: Does not format as an IP address				serializedData.append( HobString( Hex::toString( res->nsaddr_list[0].sin_addr.s_addr ) ) );
			//	PTR 1445:	Created a new method in HobIpAddress to take this struct and process it
			//				so we can format the IP Address string properly.
			for (int i = 0; i < res->nscount; i++)
			{
				if (i > 0)
					strcat( buf, " " );

				HobIpAddress theAddress(res->nsaddr_list[i].sin_addr);
				HobString tempStr;
				theAddress.getAsString(tempStr);
				strcat( buf, tempStr.c_str() );
			}

			bReturn = true;
		}

		free(res);
	}

#else

	#error "Get DNS IP address and append it to serializedData."

#endif

	return bReturn;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string VersionInfo::getInstalledFeatures()
{
#if defined( PAL_WIN32_RUN )
	static const std::string sep( "\\" );
#elif defined( __linux__ )
	static const std::string sep( "/" );
#endif

	std::string result;

	// setup location of installed features
	std::string folder( Configuration::getInstance()->toStringCfgI( CfgInternal::SYSTEMUSERROOTDIR ) );
	folder.append( sep );
	folder.append( "features" );

#if defined( PAL_WIN32_RUN )

	// TODO

#elif defined( __linux__ )

	DIR *d( opendir( folder.c_str() ) );

	if ( d )
	{
		struct dirent *dir;

		// iterate all folders installed into features
		while ( (dir = readdir( d )) )
			if ( DT_DIR == dir->d_type )
			{
				std::string featDir( dir->d_name );

				// ignore '.' and '..' directories
				if (( "." != featDir ) && ( ".." != featDir ))
				{
					std::string featName;

					// attempt to open install script
					std::string installScript( folder );
					installScript.append( sep );
					installScript.append( featDir );
					installScript.append( sep );
					installScript.append( "install.py" );

					// open!
					std::ifstream is( installScript.c_str(), std::ifstream::in );

					if ( is.is_open() )
					{
						size_t maxIter( 32 );

						while (( !is.eof() ) && ( is.good() ) && ( maxIter-- ))
						{
							std::string line;
							std::getline( is, line );

							// locate feature name
							if ( std::string::npos != line.find( "featureName" ) )
							{
								size_t start( line.find_first_of( '\"' ) );
								size_t stop( line.find_last_of( '\"' ) );

								// parse out variable
								if (( std::string::npos != start ) &&
									( std::string::npos != stop ) &&
									( start < stop ))
								{
									featName = line.substr( start + 1, stop - start - 1 );
									break;
								}
							}
						}

						is.close();
					}

					// append result to list
					if ( 0 < result.length() )
						result.append( "," );

					result.append( featName.length() ? featName : featDir );
				}
			}

		closedir( d );
	}

#endif

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void VersionInfo::getIpAndGatewayAddress( std::string &ip, std::string &gateway )
{
	if ( !Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
		return;

	/////////////////////////////////////////
	// Get IP Address from python call
	ip.clear();

	try
	{
		const string command( PythonScripter::buildPythonCall("getIpAddress.py") );
		string getIPAddressOutput;

		PAL_absclass::PAL_executeWithResults(command, getIPAddressOutput);

		if (getIPAddressOutput.size())
		{
			HobTokenizerCSV tok(&getIPAddressOutput, '\n');
			HobString token;

			if (tok.next(&token) != -1)  // First line will be the IP Address
				ip = token;
		}
	}
	catch(...)
	{
		Debug::trace("VersionInfo.serialize", "Got an exception on python ip address probably. Continue!");
		LogCatch;
	}

#ifdef __linux__

	// backup method in case python script fails
	if ( ip.empty() )
	{
		std::map<std::string, std::string> addrmap;

	    struct ifaddrs *ifaddr;

	    if ( -1 == getifaddrs( &ifaddr ) )
			Debug::trace( "NetworkConnectivityManager.Run", "Error retrieving if adapters!" );
	    else
	    {
		    for ( struct ifaddrs *ifa( ifaddr ); ifa != NULL; ifa = ifa->ifa_next )
		    	if (( ifa->ifa_addr ) && ( AF_INET == ifa->ifa_addr->sa_family ))
				{
				    char host[NI_MAXHOST];
				    int s;

				    if ( 0 != (s = getnameinfo( ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST )) )
						Debug::trace( "NetworkConnectivityManager.Run", "getnameinfo() failed: " + std::string( gai_strerror( s ) ) );
				    else
				    {
						Debug::trace( "NetworkConnectivityManager.Run", "name info: '" + std::string( ifa->ifa_name ) + "' '" + std::string( host ) + "'" );

						if ( strlen( host ) )
							addrmap[ifa->ifa_name] = host;
				    }
				}
	    }

	    if ( addrmap.end() != addrmap.find( "wlan0" ) )
	    	ip = addrmap["wlan0"];
	    else if ( addrmap.end() != addrmap.find( "eth0" ) )
	    	ip = addrmap["eth0"];
	}
#endif
	/////////////////////////////////////////
	// Get Gateway from python call
	gateway.clear();

	try
	{
		const string command( PythonScripter::buildPythonCall("getGateway.py") );
		string getGatewayOutput;

		PAL_absclass::PAL_executeWithResults(command, getGatewayOutput);

		if (getGatewayOutput.size())
		{
			HobTokenizerCSV tok(&getGatewayOutput, '\n');
			HobString token;

			if (tok.next(&token) != -1)  // Frist line will be the gateway.
				gateway = token;
		}
	}
	catch(...)
	{
		Debug::trace("NetworkConnectivityManager.Run", "Got an exception on python gateway probably. Continue!");
		LogCatch;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string VersionInfo::getApplicationVersion()
{
	std::string result;

	HobComplexVersion *myVersion( VersionManager::getInstance()->getSystemControllerSoftwareVersion() );
	if (myVersion)
		result = myVersion->getRevMajMinEngStr();

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string VersionInfo::getSMBackendVersion()
{
	std::string result;

	/////////////////////////////////////////
	// Get smBackend version from python call

	if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		try
		{
			util_filesystem::Filename smBackendScriptFilename;
			smBackendScriptFilename.setPath(Configuration::getInstance()->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR));
			smBackendScriptFilename.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONSCRIPTSDIR));
			smBackendScriptFilename.setFileName("smBackend.py");

			HobString smBackendPath( smBackendScriptFilename.getFullPath() );
			smBackendPath.quote();

			const string command( PythonScripter::buildPythonCall("getSmBackendVersion.py", -1, smBackendPath) );
			string getSmBackendVersionOutput;

			PAL_absclass::PAL_executeWithResults(command, getSmBackendVersionOutput);
			result = getSmBackendVersionOutput;
		}
		catch(...)
		{
			Debug::trace("NetworkConnectivityManager.Run", "Got an exception on python gateway probably. Continue!");
			LogCatch;
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string VersionInfo::getHardwareRev()
{
	std::string result;

	////////////////////////////////////
	// Get hardware rev from python call

	if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
	{
		try
		{
			File hwrev( "/etc/hobart/hwrev.dat" );
			string r;

			// do not invoke python when hwrev already exists
			if ( !hwrev.exists() )
			{
				const string command( PythonScripter::buildPythonCall("hwRev.py") );
				string results;

				PAL_absclass::PAL_executeWithResults(command, results);
			}

			// read in rev
			if ( hwrev.exists() )
			{
				ifstream is( hwrev.getFullPath().c_str(), ios::in | ios::binary );

				if ( is.is_open() )
				{
					stringstream strStream;
					strStream << is.rdbuf(); //read the file
					result = strStream.str();

					is.close();
				}
			}
		}
		catch (...)
		{
			Debug::trace("NetworkConnectivityManager.Run", "Got an exception on python hwrev. Continue!");
			LogCatch;
		}
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *VersionInfo::serialize( int format ) const
{
    serializedData = "";

    switch (format)
    {
	case BrowserFormat:
		{
			// Build Number
			serializedData.append(BuildNumber::buildNumber);

			// Version
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(getApplicationVersion());

			// Revision
			serializedData.append(BrowserFormatSeparator);
			serializedData.append("0");

			std::string osVer = VersionManager::getInstance()->getOSVersionAsString();
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(osVer);

			// IP Address
			string ipAddressString;
			string gatewayAddressString;

			getIpAndGatewayAddress( ipAddressString, gatewayAddressString );

			serializedData.append(BrowserFormatSeparator);
			serializedData.append(ipAddressString);
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(gatewayAddressString);

			// Host Name
			serializedData.append(BrowserFormatSeparator);
			//ipaddr = Configuration::getInstance()->getServerIpAddress();
			//ipaddr.getAsString(addr);
			// The getDNSHostname really just returns the host name it is misnamed
			serializedData.append(bootParameters::getDNSHostName());

			// DNS Address
			serializedData.append(BrowserFormatSeparator);

			char buffer[256]; // room for 16 IP's
			if ( getDNSList( buffer, sizeof(buffer) ) )
				serializedData.append(buffer);

			// Last Communication
			serializedData.append(BrowserFormatSeparator);
			HobString comm = Configuration::getInstance()->getCommunicationHistory();
			comm.quoteForCSV();
			serializedData.append(comm);

			// Last Item Communication
			serializedData.append(BrowserFormatSeparator);
			comm = Configuration::getInstance()->getCommunicationItemReceived();
			comm.quoteForCSV();
			serializedData.append(comm);

			// smBackend version
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(getSMBackendVersion());

			// hardware rev
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(getHardwareRev());

			// Installed features.
			HobString feat( getInstalledFeatures() );
			feat.quoteForCSV();

			serializedData.append( BrowserFormatSeparator );
			serializedData.append( feat );

			break;
		}

	default:
		serializedData = HobString("VersionInfo::serialize - unknown format:") + HobString(Integer::toString(format));
		// todo - throw exception("invalid format");
		break;
    }

    return serializedData.data();
}
