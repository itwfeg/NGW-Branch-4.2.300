#include "usbkey.h"

#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QSettings>

#define INTERVALCHECK 1000
#define TMPDIRLOC "/tmp/"


UsbKey *UsbKey::instance_( NULL );


bool fileExists(QString path){
    QFileInfo file(path);
    if(file.exists() && file.isFile())
        return true;
    else
        return false;
}

UsbKey::UsbKey(QObject *parent) :
    QObject(parent),
    m_supervisorInserted(false),
    m_serviceInserted(false),
    m_selfServiceInserted(false),
    m_systemTestInserted(false),
    m_manufacturingInserted(false)
{
    QSettings settings;
    m_requiresServiceKey = settings.value("RequiresServiceKey", true).toBool();
    m_requiresSupervisorKey = settings.value("RequiresSupervisorKey", false).toBool();

    //run a check at initialization
    slt_CheckKeyFiles();
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(slt_CheckKeyFiles()) );
    timer->start(INTERVALCHECK);

}

UsbKey *UsbKey::instance(){
    if(!instance_){
        instance_ = new UsbKey();
    }
    return instance_;
}

bool UsbKey::getSupervisorKeyInserted()
{
    return bool(m_supervisorInserted);
}

bool UsbKey::getServiceKeyInserted()
{
    return bool(m_serviceInserted);
}

bool UsbKey::getSelfServiceKeyInserted()
{
    return bool(m_selfServiceInserted);
}

bool UsbKey::getSystemTestKeyInserted()
{
    return bool(m_systemTestInserted);
}

bool UsbKey::getManufacturingKeyInserted()
{
    return bool(m_manufacturingInserted);
}

bool UsbKey::getRequiresSupervisorKey()
{
    return bool(m_requiresSupervisorKey);
}

bool UsbKey::getRequiresServiceKey()
{
    return bool(m_requiresServiceKey);
}

void UsbKey::slt_CheckKeyFiles()
{

    QString supervisorPath = QString(TMPDIRLOC) + "SUPERVISOR";
    QString servicePath = QString(TMPDIRLOC) + "SERVICE";
    QString selfServicePath = QString(TMPDIRLOC) + "SELFSERVICE";
    QString systemTestPath = QString(TMPDIRLOC) + "SYSTEMTEST";
    QString manufacturingPath = QString(TMPDIRLOC) + "MANUFACTURING";

    bool supervisorKeyIn = (fileExists(supervisorPath));
    bool serviceKeyIn = (fileExists(servicePath));
    bool selfServiceKeyIn = (fileExists(selfServicePath));
    bool systemTestKeyIn = (fileExists(systemTestPath));
    bool manufacturingKeyIn = (fileExists(manufacturingPath));

    //only signal if there is a change in the key
    if(supervisorKeyIn != m_supervisorInserted){
        m_supervisorInserted = supervisorKeyIn;
        //qDebug() << "Change to SUPERVISOR key (inserted): " << m_supervisorInserted;
        //Q_EMIT sig_Supervisor(supervisorKeyIn);
    }
    if(serviceKeyIn != m_serviceInserted){
        m_serviceInserted = serviceKeyIn;
        //qDebug() << "Change to SERVICE key (inserted): " << m_serviceInserted;
        //Q_EMIT sig_Service(serviceKeyIn);
    }
    if(selfServiceKeyIn != m_selfServiceInserted){
        m_selfServiceInserted = selfServiceKeyIn;
        //qDebug() << "Change to SELFSERVICE key (inserted): " << m_selfServiceInserted;
        //Q_EMIT sig_SelfService(selfServiceKeyIn);
    }
    if(systemTestKeyIn != m_systemTestInserted){
        m_systemTestInserted = systemTestKeyIn;
        //qDebug() << "Change to SYSTEMTEST key (inserted): " << m_systemTestInserted;
        //Q_EMIT sig_SystemTest(systemTestKeyIn);
    }
    if(manufacturingKeyIn != m_manufacturingInserted){
        m_manufacturingInserted = manufacturingKeyIn;
        //qDebug() << "Change to MANUFACTURING key (inserted): " << m_manufacturingInserted;
        //Q_EMIT sig_Manufacturing(manufacturingKeyIn);
    }


}
