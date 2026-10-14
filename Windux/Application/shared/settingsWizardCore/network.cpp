#include "network.h"

Network::Network(QObject *parent) :
    QObject(parent)
{
    _SSID = "";
    _operatorID = -1;
    _machineID = "-1";
    _isConnected = false;
    _isStatic = false;
    _isAutoDNS = true;
    _isWireless = true; // Check PM for default value here
    _isVPN = false;
    _ipAddress = "0.0.0.0";
    _subnetMask = "0.0.0.0";
    _gateway = "0.0.0.0";
    _DNS1 = "0.0.0.0";
    _DNS2 = "0.0.0.0";
    _DNS3 = "0.0.0.0";
    _securityMethod = "";
    _encryption = "";
    _occurrence = QDateTime::currentDateTime();
    _vpnPassword = "";
    _vpnUsername = "";
    _WEPKey1 = "";
    _WEPKey2 = "";
    _WEPKey3 = "";
    _WEPKey4 = "";
    _defaultKey = "1";
    _WPAPassPhrase = "";
    _EAPUsername = "";
    _EAPPassword = "";
}

Network::~Network()
{
    ;
}

void Network::setGateway(QString gateway)
{
    _gateway = gateway;
}

void Network::setOperatorID(int operatorID)
{
    _operatorID = operatorID;
}

void Network::setMachineID(QString machineID)
{
    _machineID = machineID;
}

void Network::setIsConnected(bool isConnected)
{
    _isConnected = isConnected;
}

void Network::setIsStatic(bool isStatic)
{
    _isStatic = isStatic;
}

void Network::setIsWireless(bool isWireless)
{
    _isWireless = isWireless;
}

void Network::setIsAutoDNS(bool isAutoDNS)
{
    _isAutoDNS = isAutoDNS;
}

void Network::setIsVPN(bool isVPN)
{
    _isVPN = isVPN;
}

void Network::setIPAddress(QString IPAddress)
{
    _ipAddress = IPAddress;
}

void Network::setPrimaryDNS(QString DNS1)
{
    _DNS1 = DNS1;
}

void Network::setSecondaryDNS(QString DNS2)
{
    _DNS2 = DNS2;
}

void Network::setTertiaryDNS(QString DNS3)
{
    _DNS3 = DNS3;
}

void Network::setSSID(QString SSID)
{
    _SSID = SSID;
}

void Network::setSubnetMask(QString subnetMask)
{
    _subnetMask = subnetMask;
}

void Network::setSecurityMethod(QString securityMethod)
{
    _securityMethod = securityMethod;
}

void Network::setVPNPassword(QString VPNPassword)
{
    _vpnPassword = VPNPassword;
}

void Network::setVPNUsername(QString VPNUsername)
{
    _vpnUsername = VPNUsername;
}

void Network::setEncryption(QString encryption)
{
    _encryption = encryption;
}

void Network::setOccurrence(QDateTime when)
{
    _occurrence = when;
}

void Network::setWEPKey1(QString WEPKey1)
{
    _WEPKey1 = WEPKey1;
}

void Network::setWEPKey2(QString WEPKey2)
{
    _WEPKey1 = WEPKey2;
}

void Network::setWEPKey3(QString WEPKey3)
{
    _WEPKey1 = WEPKey3;
}

void Network::setWEPKey4(QString WEPKey4)
{
    _WEPKey1 = WEPKey4;
}

void Network::setDefaultKey(QString defaultKey)
{
    _defaultKey = defaultKey;
}

void Network::setWPAPassPhrase(QString WPAPassPhrase)
{
    _WPAPassPhrase = WPAPassPhrase;
}

void Network::setEAPUsername(QString eapUserName)
{
    _EAPUsername = eapUserName;
}

void Network::setEAPPassword(QString eapPassword)
{
    _EAPPassword = eapPassword;
}

QString Network::SSID()
{
    return _SSID;
}

int Network::OperatorID()
{
    return _operatorID;
}

QString Network::MachineID()
{
    return _machineID;
}

QString Network::IPAddress()
{
    return _ipAddress;
}

QString Network::SubnetMask()
{
    return _subnetMask;
}

QString Network::Gateway()
{
    return _gateway;
}

QString Network::DNS1()
{
    return _DNS1;
}

QString Network::DNS2()
{
    return _DNS2;
}

QString Network::DNS3()
{
    return _DNS3;
}

QString Network::SecurityMethod()
{
    return _securityMethod;
}

QString Network::Encryption()
{
    return _encryption;
}

QDateTime Network::Occurrence()
{
    return _occurrence;
}

QString Network::VPNUsername()
{
    return _vpnUsername;
}

QString Network::VPNPassword()
{
    return _vpnPassword;
}

bool Network::isConnected()
{
    return _isConnected;
}

bool Network::isStatic()
{
    return _isStatic;
}

bool Network::isAutoDNS()
{
    return _isAutoDNS;
}

bool Network::isWireless()
{
    return _isWireless;
}

bool Network::isVPN()
{
    return _isVPN;
}

QString Network::WEPKey1()
{
    return _WEPKey1;
}

QString Network::WEPKey2()
{
    return _WEPKey2;
}

QString Network::WEPKey3()
{
    return _WEPKey3;
}

QString Network::WEPKey4()
{
    return _WEPKey4;
}

QString Network::DefaultKey()
{
    return _defaultKey;
}

QString Network::WPAPassPhrase()
{
    return _WPAPassPhrase;
}

QString Network::EAPUsername()
{
    return _EAPUsername;
}

QString Network::EAPPassword()
{
    return _EAPPassword;
}
