#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QDateTime>

class Network : public QObject
{
    Q_OBJECT
public:
    explicit Network(QObject *parent = 0);
    ~Network();
    
signals:
    
public slots:
    void setOperatorID(int);
    void setMachineID(QString);
    void setIsConnected(bool);
    void setIsStatic(bool);
    void setIsAutoDNS(bool);
    void setIsWireless(bool);
    void setIsVPN(bool);
    void setSSID(QString);
    void setSecurityMethod(QString);
    void setEncryption(QString);
    void setVPNUsername(QString);
    void setVPNPassword(QString);
    void setIPAddress(QString);
    void setSubnetMask(QString);
    void setGateway(QString);
    void setPrimaryDNS(QString);
    void setSecondaryDNS(QString);
    void setTertiaryDNS(QString);
    void setOccurrence(QDateTime);
    void setWEPKey1(QString);
    void setWEPKey2(QString);
    void setWEPKey3(QString);
    void setWEPKey4(QString);
    void setDefaultKey(QString);
    void setWPAPassPhrase(QString);
    void setEAPUsername(QString);
    void setEAPPassword(QString);
    void setServerCertFilename(QString);
    void setChainFilename(QString);
    void setClientCertFilename(QString);

    int OperatorID();
    QString MachineID();
    QString SSID();
    QString IPAddress();
    QString SubnetMask();
    QString Gateway();
    QString DNS1();
    QString DNS2();
    QString DNS3();
    QString SecurityMethod();
    QString Encryption();
    QString VPNUsername();
    QString VPNPassword();
    QDateTime Occurrence();
    bool isConnected();
    bool isStatic();
    bool isAutoDNS();
    bool isWireless();
    bool isVPN();
    QString WEPKey1();
    QString WEPKey2();
    QString WEPKey3();
    QString WEPKey4();
    QString DefaultKey();
    QString WPAPassPhrase();
    QString EAPUsername();
    QString EAPPassword();
    QString serverCertFilename();
    QString chainFilename();
    QString clientCertFilename();

private:
    int _operatorID;
    bool _isConnected, _isStatic, _isAutoDNS, _isWireless, _isVPN;
    QString _ipAddress, _subnetMask, _gateway, _DNS1, _DNS2, _DNS3, _securityMethod, _encryption, _SSID, _vpnPassword, _vpnUsername, _machineID, _WEPKey1, _WEPKey2, _WEPKey3, _WEPKey4, _WPAPassPhrase, _EAPPassword, _EAPUsername, _defaultKey, _serverCertFilename, _chainFilename, _clientCertFilename;
    QDateTime _occurrence;
};

#endif // NETWORK_H
