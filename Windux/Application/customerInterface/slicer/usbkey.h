#ifndef USBKEY_H
#define USBKEY_H

#include <QObject>

class UsbKey : public QObject
{
    Q_OBJECT
public:
    static UsbKey* instance();
    bool getSupervisorKeyInserted();
    bool getServiceKeyInserted();
    bool getSelfServiceKeyInserted();
    bool getSystemTestKeyInserted();
    bool getManufacturingKeyInserted();

    bool getRequiresSupervisorKey();
    bool getRequiresServiceKey();
Q_SIGNALS:
    //void sig_Supervisor(bool keyInserted);
    //void sig_Service(bool keyInserted);
    //void sig_SelfService(bool keyInserted);
    //void sig_SystemTest(bool keyInserted);
    //void sig_Manufacturing(bool keyInserted);
private slots:
    void slt_CheckKeyFiles();
private:
    UsbKey(QObject *parent = 0);
    bool m_supervisorInserted;
    bool m_serviceInserted;
    bool m_selfServiceInserted;
    bool m_systemTestInserted;
    bool m_manufacturingInserted;


    bool m_requiresSupervisorKey;
    bool m_requiresServiceKey;

    static UsbKey *instance_;
};

#endif // USBKEY_H
