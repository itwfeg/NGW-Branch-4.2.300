/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef CFGNETWORK_H
#define CFGNETWORK_H

#include <list>
#include "evolution.h"
#include "CfgBase.h"
#include "HfcObject.h"
#include "HobIpAddress.h"

class HobString;
namespace util_env
{
class Properties;
class PropertiesTree;
}
class DBAccess;


typedef enum NetworkSetupOptions
{
	NetworkSetupManual,
	NetworkSetupBootp,
	NetworkSetupDhcp
}   NetworkSetupOptions_t;


class CfgNetwork : public CfgBase, public HfcObject
{
	public:

		CfgNetwork();
		CfgNetwork(const CfgNetwork &cfgNetwork);
		CfgNetwork(const char *data, int len, int formatType);
		~CfgNetwork();

		// Read the network configuration data from the database.
		void load();
		void load(DBAccess* /*dbAccess*/) {}

		void updateField(int fieldCode, const HobString &token);
		void update(const CfgBase *newValues, bool scaleOnlyFields = false);

		void save(DBAccess *dbAccess, const CfgBase *newValues = (CfgBase *)NULL, bool scaleOnlyFields = false);

		void show();

		void operator=(const CfgNetwork &cfgNetwork);


		virtual const char *serialize(int format) const;

		void getFieldAsString(int fieldCode, HobString &result, int format) const;
		const HobString& getField(int fieldCode, int format) const;

		static void initFieldLists();


		int getScaleId() const;
		void setScaleId(int id);

		NetworkSetupOptions getNetworkSetup() const;
		void setNetworkSetup(const NetworkSetupOptions setup);

		bool isManuallyConfigured() const;
		void setManuallyConfigured();

		bool isBootpEnabled() const;
		void setBootpEnabled();

		bool isDhcpEnabled() const;
		void setDhcpEnabled();

		const HobIpAddress &getIpAddress() const;
		const HobIpAddress &getSubnetMask() const;

		int getWellKnownPort() const;
		void setWellKnownPort(int port);

		bool isDnsEnabled() const;
		void setDnsEnabled(bool enabled);

		bool isDnsAutomatic() const;
		void setDnsAutomatic(bool automatic);

		const HobString &getDnsHostName() const;
		void setDnsHostName(const HobString &hostName);

		const HobString &getDnsDomainName() const;
		void setDnsDomainName(const HobString &domainName);

		const HobIpAddress &getDnsIpAddress0() const;
		const HobIpAddress &getDnsIpAddress1() const;
		const HobIpAddress &getDnsIpAddress2() const;

		int getExternalCommSetting() const;
		void setExternalCommSetting(int value);

		const HobIpAddress &getServerIpAddress() const;

		const HobString &getServerHostName() const;
		void setServerHostName(const HobString &name);

		const HobIpAddress &getServerAppIpAddress() const;

		const HobString &getServerAppHostName() const;
		void setServerAppHostName(const HobString &name);


		bool isTftpAutomatic() const;
		void setTftpAutomatic(bool automatic);

		const HobIpAddress &getTftpServerIpAddress() const;

		const HobString &getTftpBootFilePath() const;
		void setTftpBootFilePath(const HobString &path);

		void setIPAddress(int fieldCode, const HobString &address);
		void setIPAddress(int fieldCode, const HobIpAddress &newAddress);


	private:

		util_env::Properties		*properties;

		int scaleId;

		/**
		 * The name of the device for the ethernet port.  (i.e. ln0)
		 */
		HobString ethernetInterfaceName;

		NetworkSetupOptions networkSetup;	// The mechanism to setup the network: manual, bootp, dhcp

		HobIpAddress ipAddress;				//	XXX.XXX.XXX.XXX - Used for manual entry, bootp, and dhcp
											//	ipAddress - The IP address of this scale.
		HobIpAddress subnetMask;			//	XXX.XXX.XXX.XXX - If enableBootp is false

		int wellKnownPort;				//	Well known port number (0 - 65535)
										//	The port at which the Session Manager will listen for connections to this scale.

		//	Start DNS support
		bool dnsEnabled;
		bool dnsAuto;
		HobString dnsHostName;		//	63 characters MAX
		HobString dnsDomainName;
		HobIpAddress dnsIpAddress0;		//	XXX.XXX.XXX.XXX
		HobIpAddress dnsIpAddress1;		//	XXX.XXX.XXX.XXX
		HobIpAddress dnsIpAddress2;		//	XXX.XXX.XXX.XXX
		//	End DNS support

		//	Start external comm support
		int externalCommSetting;			//	Server application, Intranet, Server app. & Intranet, None.
		HobIpAddress serverIpAddress;		//	XXX.XXX.XXX.XXX - If intranet enabled and DNS disabled.
		HobString serverHostName;			//	63 characters MAX - If intranet enabled and DNS disabled.
		HobIpAddress serverAppIpAddress;	//	XXX.XXX.XXX.XXX - If server application enabled and DNS disabled.
		HobString serverAppHostName;		//	63 characters MAX - If server application enabled and DNS disabled.
//	Note 1:	DO WE WANT serverIpAddress AND serverAppIpAddress TO BE DIFFERENT ELEMENTS
//			OR CAN ONE FIELD HANDLE THE IPAddress, DEPENDENT ON THE externalCommSetting?
//	Note 2:	DO WE WANT serverHostName AND serverAppHostName TO BE DIFFERENT ELEMENTS
//			OR CAN ONE FIELD HANDLE THE HostName, DEPENDENT ON THE externalCommSetting?
		//	End external comm support

		bool			tftpAutomatic;
		HobIpAddress	tftpServerIpAddress;		//	XXX.XXX.XXX.XXX
		HobString		tftpBootFilePath;

		mutable HobString tempValue_;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //CFGNETWORK_H
