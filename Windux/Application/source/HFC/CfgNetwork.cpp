/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgNetwork.h"
#include "CfgInternal.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "properties/properties.h"
#include "properties/propertiestree.h"
#include "bootParameters.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgNetwork::dataType = 0;
std::string CfgNetwork::dataTypeName("CfgNetwork");
#endif


using namespace std;


CfgNetwork::CfgNetwork()
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();
}

#if defined(LINUX_FIX)
    CfgNetwork::CfgNetwork(const CfgNetwork &cfgNetwork)
: HfcObject()
#else
    CfgNetwork::CfgNetwork(const CfgNetwork &cfgNetwork)
#endif
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();

    scaleId               = cfgNetwork.getScaleId();
    networkSetup          = cfgNetwork.getNetworkSetup();
    ipAddress             = cfgNetwork.getIpAddress();
    subnetMask            = cfgNetwork.getSubnetMask();
    wellKnownPort         = cfgNetwork.getWellKnownPort();

    dnsEnabled          = cfgNetwork.isDnsEnabled();
    dnsAuto                     = cfgNetwork.isDnsAutomatic();
    dnsHostName         = cfgNetwork.getDnsHostName();
    dnsDomainName       = cfgNetwork.getDnsDomainName();
    dnsIpAddress0       = cfgNetwork.getDnsIpAddress0();
    dnsIpAddress1       = cfgNetwork.getDnsIpAddress1();
    dnsIpAddress2       = cfgNetwork.getDnsIpAddress2();

    externalCommSetting = cfgNetwork.getExternalCommSetting();
    serverIpAddress     = cfgNetwork.getServerIpAddress();
    serverHostName      = cfgNetwork.getServerHostName();
    serverAppIpAddress  = cfgNetwork.getServerAppIpAddress();
    serverAppHostName   = cfgNetwork.getServerAppHostName();

    tftpAutomatic               = cfgNetwork.isTftpAutomatic();
    tftpServerIpAddress = cfgNetwork.getTftpServerIpAddress();
    tftpBootFilePath    = cfgNetwork.getTftpBootFilePath();

}

CfgNetwork::CfgNetwork(const char *data, int len, int UNUSED(formatType) )
{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif

    init();

    HobString temp(data, len);
    std::list<CfgFieldOffsets>::iterator fieldIter;

}

CfgNetwork::~CfgNetwork()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif

    if (properties)
        delete properties;
}

//private
void CfgNetwork::init()
{
    properties = (util_env::Properties *)NULL;

    scaleId               = 99;
    ethernetInterfaceName = "st0";
    networkSetup          = NetworkSetupManual;

    HobString tempStr("127.0.0.1");
    ipAddress.set("127.0.0.1");
    tempStr = "255.255.255.0";
    subnetMask.set("255.255.255.0");
    wellKnownPort         = 6000;

    dnsEnabled    = false;
    dnsAuto       = false;
    dnsHostName   = "";
    dnsDomainName = "";

    externalCommSetting = 0;
    serverHostName      = "";
    serverAppHostName   = "";

    tftpAutomatic               = false;
    tftpBootFilePath    = "";

    initFieldLists();
}


// static
void CfgNetwork::initFieldLists()
{
}


void CfgNetwork::load()
{
    Debug::trace("Configuration.CfgNetwork", HobString("Loading file: ") + CfgInternal::getNetworkConfigFileNameStr());

    if (properties)
        delete properties;
    properties = new util_env::Properties(CfgInternal::getNetworkConfigFileNameStr());
    if (!properties)
        return;


    // Currently not in bootParams.cfg
    const string *paramValue = NULL;
    paramValue = properties->getProperty("scaleId");
    if (paramValue)
        setScaleId(Integer::valueOf(*paramValue));
		  
    paramValue = NULL;
    paramValue = properties->getProperty("interface0.wellKnownPort");
    if (paramValue)
        setWellKnownPort(Integer::valueOf(*paramValue));
		  
    // Currently not in bootParams.cfg
    paramValue = NULL;
    paramValue = properties->getProperty("externalCommSetting");
    if (paramValue)
        setExternalCommSetting(Integer::valueOf(*paramValue));

    // Currently not in bootParams.cfg
    paramValue = NULL;
    paramValue = properties->getProperty("serverIpAddress");
    if (paramValue)
        setIPAddress(fieldOffsetCfgNetServerIpAddress, HobString(*paramValue));

    // Currently not in bootParams.cfg
    paramValue = NULL;
    paramValue = properties->getProperty("serverHostName");
    if (paramValue)
        setServerHostName(HobString(*paramValue));

    // Currently not in bootParams.cfg
    paramValue = NULL;
    paramValue = properties->getProperty("serverAppIpAddress");
    if (paramValue)
        setIPAddress(fieldOffsetCfgNetServerAppIpAddress, HobString(*paramValue));

    // Currently not in bootParams.cfg
    paramValue = NULL;
    paramValue = properties->getProperty("serverAppHostName");
    if (paramValue)
        setServerAppHostName(HobString(*paramValue));
}


void CfgNetwork::updateField(int fieldCode, const HobString &token)
{
    if (fieldCode <= 0)
        return;

    if (Debug::isTracing("Configuration.CfgNetwork"))
    {
        Debug::trace("Configuration.CfgNetwork", HobString("updateField() - field:") + HobString(Integer::toString(fieldCode)) + HobString("  data:") + token);
    }

    bool foundField = true;
    switch (fieldCode)
    {
        case fieldOffsetCfgNetExternalCommSetting:
            setExternalCommSetting(Integer::valueOf(token));
            break;
        case fieldOffsetCfgNetServerIpAddress:
            setIPAddress(fieldCode, token);
            break;
        case fieldOffsetCfgNetServerHostName:
            setServerHostName(token);
            break;
        case fieldOffsetCfgNetServerAppIpAddress:
            setIPAddress(fieldCode, token);
            break;
        case fieldOffsetCfgNetServerAppHostName:
            setServerAppHostName(token);
            break;
        default:
                Debug::trace("Configuration.CfgNetwork", HobString("updateField() - couldn't find the field"));
                foundField = false;
                break;
    }

    if (foundField)
    {

        setReceived(fieldCode, true);
        Debug::trace("Configuration.CfgNetwork", HobString("updateField() - isReceived(") + HobString(Integer::toString(fieldCode)) + HobString(") = ") + HobString(Bool::toString(isReceived(fieldCode))));
    }
}


void CfgNetwork::update(const CfgBase *newValues, bool scaleOnlyFields)
{
    if (!newValues)
        return;

    Debug::trace("Configuration.CfgNetwork", "update()");
    const CfgNetwork *myNewValues = dynamic_cast<const CfgNetwork *> (newValues);

    short loopCnt = endCfgSSFieldOffsets;
    while (loopCnt < endCfgNetworkFieldOffsets)
    {
        Debug::trace("Configuration.CfgNetwork", "update():  loopCnt = " + HobString(Integer::toString(loopCnt)) + HobString("  isReceived = ") + HobString(Bool::toString(myNewValues->isReceived(loopCnt))));
        if (myNewValues->isReceived(loopCnt) && (!scaleOnlyFields || isFieldScaleOnly(loopCnt)))
        {

            switch (loopCnt)
            {
                case fieldOffsetCfgNetExternalCommSetting:
                    setExternalCommSetting(myNewValues->getExternalCommSetting());
                    break;
                case fieldOffsetCfgNetServerIpAddress:
                    setIPAddress(loopCnt, myNewValues->getServerIpAddress());
                    break;
                case fieldOffsetCfgNetServerHostName:
                    setServerHostName(myNewValues->getServerHostName());
                    break;
                case fieldOffsetCfgNetServerAppIpAddress:
                    setIPAddress(loopCnt, myNewValues->getServerAppIpAddress());
                    break;
                case fieldOffsetCfgNetServerAppHostName:
                    setServerAppHostName(myNewValues->getServerAppHostName());
                    break;
                default:
                        break;
            }
            if (Debug::isTracing("Configuration.CfgNetwork"))
                Debug::trace("Configuration.CfgNetwork", HobString("update() - field:") + HobString(Integer::toString(loopCnt)) + HobString(" received.  saveNeeded = ") + HobString(Bool::toString(saveNeeded)));
        }
        loopCnt++;
    }
}


void CfgNetwork::save(DBAccess * /* dbAccess */, const CfgBase *newValues, bool scaleOnlyFields)
{
    Debug::trace("Configuration.CfgNetwork", "save()");
    if (newValues)
        update(newValues, scaleOnlyFields);

    if (saveNeeded)
    {
        HobString       tempStr;
        Debug::trace("Configuration.CfgNetwork", "saving data ...");

        //show();

        // Should we allow the user to save this?
        //properties->setProperty("interface0.name", ethernetInterfaceName);
        //properties->setProperty("scaleId", Integer::toString(scaleId));
        properties->setProperty("externalCommSetting", Integer::toString(externalCommSetting));

        serverIpAddress.getAsString(tempStr);           //      Get the string version of the IpAddress
        properties->setProperty("serverIpAddress", tempStr);

        properties->setProperty("serverHostName", serverHostName);

        serverAppIpAddress.getAsString(tempStr);                //      Get the string version of the IpAddress
        properties->setProperty("serverAppIpAddress", tempStr);

        properties->setProperty("serverAppHostName", serverAppHostName);
		  
        properties->save();

        saveNeeded = false;
    }
    else
        Debug::trace("Configuration.CfgNetwork", "save method called, but no fields modified");
}


void CfgNetwork::operator=(const CfgNetwork &cfgNetwork)
{
    scaleId               = cfgNetwork.getScaleId();
    networkSetup          = cfgNetwork.getNetworkSetup();
    ipAddress             = cfgNetwork.getIpAddress();
    subnetMask            = cfgNetwork.getSubnetMask();
    wellKnownPort         = cfgNetwork.getWellKnownPort();

    dnsEnabled = cfgNetwork.isDnsEnabled();
    dnsAuto = cfgNetwork.isDnsAutomatic();
    dnsDomainName = cfgNetwork.getDnsDomainName();
    dnsHostName = cfgNetwork.getDnsHostName();
    dnsIpAddress0 = cfgNetwork.getDnsIpAddress0();
    dnsIpAddress1 = cfgNetwork.getDnsIpAddress1();
    dnsIpAddress2 = cfgNetwork.getDnsIpAddress2();

    externalCommSetting = cfgNetwork.getExternalCommSetting();
    serverIpAddress     = cfgNetwork.getServerIpAddress();
    serverHostName      = cfgNetwork.getServerHostName();
    serverAppIpAddress  = cfgNetwork.getServerAppIpAddress();
    serverAppHostName   = cfgNetwork.getServerAppHostName();

    tftpAutomatic       = cfgNetwork.isTftpAutomatic();
    tftpServerIpAddress = cfgNetwork.getTftpServerIpAddress();
    tftpBootFilePath    = cfgNetwork.getTftpBootFilePath();
}


const char *CfgNetwork::serialize(int format) const
{
    serializedData = "";

    if (getFormatType() != UnknownFormat)
        format = getFormatType();

    std::list<CfgFieldOffsets>::iterator fieldIter;
#if defined(LINUX_FIX)
    // appendSeparator: unused variable
    // offset: unused variable
#else
    bool appendSeparator = false;
    int offset;
#endif
    HobString fieldData = "";

    switch (format)
    {
        case NetworkReportFormat:
    {
        hfc::MultiLanguageText internationalize;
        ConfigurationTags configNames;

        serializedData.assign(internationalize("ConfigReport.Network")).append(1, '\n');
        break;
    }
        case SynchronizationTaggedFormat:
    {
        break;
    }

        default:
    // todo - throw exception("invalid format");
    break;
    }

    return serializedData.c_str();
}


void CfgNetwork::getFieldAsString(int fieldCode, HobString &result, int format) const
{
    result = "";

    switch (fieldCode)
    {
        case fieldOffsetCfgWellKnownPort:
            result = Integer::toString(wellKnownPort);
            break;
        case fieldOffsetCfgNetExternalCommSetting:
            result = Integer::toString(externalCommSetting);
            break;
        case fieldOffsetCfgNetServerIpAddress:
            serverIpAddress.getAsString(result);
            break;
        case fieldOffsetCfgNetServerHostName:
            result = serverHostName;
            break;
        case fieldOffsetCfgNetServerAppIpAddress:
            serverAppIpAddress.getAsString(result);
            break;
        case fieldOffsetCfgNetServerAppHostName:
            result = serverAppHostName;
            break;
        default:
                break;
    }
    Debug::trace("Configuration.CfgNetwork", HobString("  getFieldAsString():  fieldCode = ") + HobString(Integer::toString(fieldCode)) + HobString("  result = ") + result + HobString("  format = ") + HobString(Integer::toString(format)));
}


const HobString& CfgNetwork::getField(int fieldCode, int format) const
{
    getFieldAsString(fieldCode, tempValue_, format);

    return tempValue_;
}


void CfgNetwork::show()
{
    HobString tempStr;

    if (Debug::isTracing("Configuration"))
    {
        Debug::trace("Configuration", "*** CfgNetwork parameters ***");
        Debug::trace("Configuration", HobString("scaleId:") + HobString(Integer::toString(scaleId)));
        Debug::trace("Configuration", HobString("ethernetInterfaceName:") + ethernetInterfaceName);

        Debug::trace("Configuration", HobString("networkSetup:") + HobString(Integer::toString(networkSetup)));

        ipAddress.getAsString(tempStr);
        Debug::trace("Configuration", HobString("ipAddress:") + tempStr);
        subnetMask.getAsString(tempStr);
        Debug::trace("Configuration", HobString("subnetMask:") + tempStr);
        Debug::trace("Configuration", HobString("wellKnownPort:") + HobString(Integer::toString(wellKnownPort)));

        Debug::trace("Configuration", HobString("dnsEnabled:") + HobString(Bool::toString(dnsEnabled)));
        Debug::trace("Configuration", HobString("dnsAutomatic:") + HobString(Bool::toString(dnsAuto)));
        Debug::trace("Configuration", HobString("dnsHostName:") + dnsHostName);
        Debug::trace("Configuration", HobString("dnsDomainName:") + dnsDomainName);
        dnsIpAddress0.getAsString(tempStr);
        Debug::trace("Configuration", HobString("dnsIpAddress0:") + tempStr);
        dnsIpAddress1.getAsString(tempStr);
        Debug::trace("Configuration", HobString("dnsIpAddress1:") + tempStr);
        dnsIpAddress2.getAsString(tempStr);
        Debug::trace("Configuration", HobString("dnsIpAddress2:") + tempStr);

        Debug::trace("Configuration", HobString("externalCommSetting:") + HobString(Integer::toString(externalCommSetting)));
        serverIpAddress.getAsString(tempStr);
        Debug::trace("Configuration", HobString("serverIpAddress:") + tempStr);
        Debug::trace("Configuration", HobString("serverHostName:") + serverHostName);
        serverAppIpAddress.getAsString(tempStr);
        Debug::trace("Configuration", HobString("serverAppIpAddress:") + tempStr);
        Debug::trace("Configuration", HobString("serverAppHostName:") + serverAppHostName);

        Debug::trace("Configuration", HobString("tftpAutomatic:") + HobString(Bool::toString(isTftpAutomatic())));
        tftpServerIpAddress.getAsString(tempStr);
        Debug::trace("Configuration", HobString("tftpServerIpAddress:") + tempStr);
        Debug::trace("Configuration", HobString("tftpBootFilePath:") + tftpBootFilePath);

    }
}

int CfgNetwork::getScaleId() const
{
    return scaleId;
}
void CfgNetwork::setScaleId(int id)
{
    if (scaleId != id)
    {
        scaleId = id;
        saveNeeded = true;
    }
}

NetworkSetupOptions CfgNetwork::getNetworkSetup() const
{
    return networkSetup;
}
void CfgNetwork::setNetworkSetup(const NetworkSetupOptions setup)
{
    if (networkSetup != setup)
    {
        networkSetup = setup;
        saveNeeded = true;
    }
}

bool CfgNetwork::isManuallyConfigured() const
{
    return (networkSetup == NetworkSetupManual) ? true : false;
}
void CfgNetwork::setManuallyConfigured()
{
    if (networkSetup != NetworkSetupManual)
    {
        networkSetup = NetworkSetupManual;
        saveNeeded = true;
    }
}

bool CfgNetwork::isBootpEnabled() const
{
    return (networkSetup == NetworkSetupBootp) ? true : false;
}
void CfgNetwork::setBootpEnabled()
{
    if (networkSetup != NetworkSetupBootp)
    {
        networkSetup = NetworkSetupBootp;
        saveNeeded = true;
    }
}

bool CfgNetwork::isDhcpEnabled() const
{
    return (networkSetup == NetworkSetupDhcp) ? true : false;
}
void CfgNetwork::setDhcpEnabled()
{
    if (networkSetup != NetworkSetupDhcp)
    {
        networkSetup = NetworkSetupDhcp;
        saveNeeded = true;
    }
}

const HobIpAddress &CfgNetwork::getIpAddress() const
{
    return ipAddress;
}

const HobIpAddress &CfgNetwork::getSubnetMask() const
{
    return subnetMask;
}

int CfgNetwork::getWellKnownPort() const
{
    return wellKnownPort;
}
void CfgNetwork::setWellKnownPort(int port)
{
    if (wellKnownPort != port)
    {
        wellKnownPort = port;
        saveNeeded = true;
    }
}

bool CfgNetwork::isDnsEnabled() const
{
    return dnsEnabled;
}
void CfgNetwork::setDnsEnabled(bool enabled)
{
    if (dnsEnabled != enabled)
    {
        dnsEnabled = enabled;
        saveNeeded = true;
    }
}

bool CfgNetwork::isDnsAutomatic() const
{
    return dnsAuto;
}
void CfgNetwork::setDnsAutomatic(bool automatic)
{
    if (dnsAuto != automatic)
    {
        dnsAuto = automatic;
        saveNeeded = true;
    }
}

const HobString &CfgNetwork::getDnsHostName() const
{
    return dnsHostName;
}
void CfgNetwork::setDnsHostName(const HobString &hostName)
{
    if (dnsHostName != hostName)
    {
        dnsHostName = hostName;
        saveNeeded = true;
    }
}

const HobString &CfgNetwork::getDnsDomainName() const
{
    return dnsDomainName;
}
void CfgNetwork::setDnsDomainName(const HobString &domainName)
{
    if (dnsDomainName != domainName)
    {
        dnsDomainName = domainName;
        saveNeeded = true;
    }
}

const HobIpAddress &CfgNetwork::getDnsIpAddress0() const
{
    return dnsIpAddress0;
}

const HobIpAddress &CfgNetwork::getDnsIpAddress1() const
{
    return dnsIpAddress1;
}

const HobIpAddress &CfgNetwork::getDnsIpAddress2() const
{
    return dnsIpAddress2;
}

int CfgNetwork::getExternalCommSetting() const
{
    return externalCommSetting;
}
void CfgNetwork::setExternalCommSetting(int value)
{
    externalCommSetting = value;
}

const HobIpAddress &CfgNetwork::getServerIpAddress() const
{
    return serverIpAddress;
}

const HobString &CfgNetwork::getServerHostName() const
{
    return serverHostName;
}
void CfgNetwork::setServerHostName(const HobString &name)
{
    serverHostName = name;
}

const HobIpAddress &CfgNetwork::getServerAppIpAddress() const
{
    return serverAppIpAddress;
}

const HobString &CfgNetwork::getServerAppHostName() const
{
    return serverAppHostName;
}
void CfgNetwork::setServerAppHostName(const HobString &name)
{
    serverAppHostName = name;
}


bool CfgNetwork::isTftpAutomatic() const
{
    return tftpAutomatic;
}
void CfgNetwork::setTftpAutomatic(bool automatic)
{
    if (tftpAutomatic != automatic)
    {
        tftpAutomatic = automatic;
        saveNeeded = true;
    }
}

const HobIpAddress &CfgNetwork::getTftpServerIpAddress() const
{
    return tftpServerIpAddress;
}

const HobString &CfgNetwork::getTftpBootFilePath() const
{
    return tftpBootFilePath;
}
void CfgNetwork::setTftpBootFilePath(const HobString &path)
{
    if (tftpBootFilePath != path)
    {
        tftpBootFilePath = path;
        saveNeeded = true;
    }
}

void CfgNetwork::setIPAddress(int fieldCode, const HobString &address)
{
    if (Debug::isTracing("Configuration.CfgNetwork"))
        Debug::trace("Configuration.CfgNetwork", "setIPAddress1:  fieldCode = " + HobString(Integer::toString(fieldCode)) + "   address string = " + address);
    HobString tempAddr = address;
    HobIpAddress newAddress(tempAddr);
    setIPAddress(fieldCode, newAddress);
}

void CfgNetwork::setIPAddress(int fieldCode, const HobIpAddress &newAddress)
{
    if (fieldCode <= 0)
        return;

    if (Debug::isTracing("Configuration.CfgNetwork"))
        Debug::trace("Configuration.CfgNetwork", "setIPAddress2:  fieldCode = " + HobString(Integer::toString(fieldCode)));

    switch (fieldCode)
    {
        case fieldOffsetCfgNetServerIpAddress:
        if (serverIpAddress != newAddress)
        {
            serverIpAddress = newAddress;
            saveNeeded = true;
            Debug::trace("Configuration.CfgNetwork", "Setting:  serverIpAddress");
        }
        break;
        case fieldOffsetCfgNetServerAppIpAddress:
        if (serverAppIpAddress != newAddress)
        {
            serverAppIpAddress = newAddress;
            saveNeeded = true;
            Debug::trace("Configuration.CfgNetwork", "Setting:  serverAppIpAddress");
        }
        break;
        default:
        Debug::trace("Configuration.CfgNetwork", "Don't have a case for this one!");
        break;
    }
}
