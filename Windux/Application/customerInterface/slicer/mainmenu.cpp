#include "mainmenu.h"
#include "ui_mainmenu.h"

#include "supervisormenu.h"
#include "servicemenu.h"
#include "mainwindow.h"
#include "passwordentry.h"
#include "usbkey.h"
#include "popupmessage.h"

#include <QDebug>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
mainMenu::mainMenu(QWidget *parent) :
    _Mybase(parent),
    ui_(new Ui::mainMenu)
{
    ui_->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    initialize();
    createLayout();


    m_usbKey = UsbKey::instance();


    //To Weigher
    connect(this, SIGNAL(sig_ReqConfig()), (MainWindow*) this->parent(),                        SLOT(slt_ReqConfig()));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (MainWindow*) this->parent(), SLOT(slt_WriteConfig(WeigherConfiguration &)));
    connect(this, SIGNAL(sig_RezeroWeigher()), (MainWindow*) this->parent(),                    SLOT(slt_RezeroWeigher()));
    connect(this, SIGNAL(sig_SetWeigherMode(WeighingMode)), (MainWindow*) this->parent(),       SLOT(slt_SetWeigherMode(WeighingMode)));
    connect(this, SIGNAL(sig_WeigherReset()), (MainWindow*) this->parent(),                     SLOT(slt_WeigherReset()));

    //From Weigher
    connect((MainWindow*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)));
    connect((MainWindow*) this->parent(), SIGNAL(sig_WeigherKeyPressed()), this,                         SIGNAL(sig_WeigherKeyPressed()));
    connect((MainWindow*) this->parent(), SIGNAL(sig_WeigherStatus(char*, int)) ,                        SIGNAL(sig_WeigherStatus(char*,int)));
    connect(ui_->serviceBtn,    SIGNAL(clicked(bool)), SLOT(onServiceScreen()));
    connect(ui_->supervisorBtn, SIGNAL(clicked(bool)), SLOT(onSupervisorScreen()));
    connect(ui_->backBtn,       SIGNAL(clicked(bool)), SLOT(reject()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainMenu::~mainMenu()
{
    delete ui_;
    delete timerSupervisorKeyLabel;
    delete timerServiceKeyLabel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

//void mainMenu::slt_WeigherKeyPressed()
//{
//    Q_EMIT sig_WeigherKeyPressed();
//}

//void mainMenu::slt_WeigherConfigChanged( WeigherConfiguration &config)
//{
//    Q_EMIT sig_WeigherConfigChanged(config);
//}

void mainMenu::slt_WriteConfig(WeigherConfiguration &config)
{
    Q_EMIT sig_WriteConfig(config); //Propagate signal from servicemenu or supervisormenu
}

void mainMenu::slt_ReqConfig()
{
    Q_EMIT sig_ReqConfig(); //Propagate signal from servicemenu or supervisormenu
}

void mainMenu::slt_RezeroWeigher()
{
    Q_EMIT sig_RezeroWeigher(); //Propagate signal from servicemenu (replace with just a signal?)
}

void mainMenu::slt_WeigherStatus(char *buffer, int buffLen)
{
    Q_EMIT sig_WeigherStatus(buffer, buffLen);
}

void mainMenu::slt_SetWeigherMode(WeighingMode mode)
{
    Q_EMIT sig_SetWeigherMode(mode);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::onServiceScreen()
{
    if(m_usbKey->getServiceKeyInserted() || !m_usbKey->getRequiresServiceKey() )
    {
        //qDebug()<<"Service page";
        QTimer::singleShot(0,this,SLOT(serviceScreen()));
        timerServiceKeyLabel->stop();
    }
    else
    {
        //qDebug()<<"Missing service key";
        QTimer::singleShot(0,this,SLOT(updateServiceKeyMessage()));
        timerServiceKeyLabel->start(500);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::serviceScreen()
{
    servicemenu d(this);
    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));

    //Propagate signal from generalconfig
    connect(&d, SIGNAL(sig_UpdateLanguage(QString)), this, SIGNAL(sig_UpdateLanguage(QString)));
    d.exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::onSupervisorScreen()
{
    // Check for the existence of a supervisor key or no key required
    if (m_usbKey->getSupervisorKeyInserted())
    {
        //qDebug()<<"supervisor page";
        QTimer::singleShot(0,this,SLOT(supervisorScreen()));
        timerSupervisorKeyLabel->stop();
    }

    // if no supervisor key is required
    else if (!m_usbKey->getRequiresSupervisorKey())
    {
        // Check to see if a supervisor password has been configured
        if (passwordEntry::passwordConfigured(passwordEntry::supervisor))
        {
            passwordEntry dialog(this, passwordEntry::supervisor);
            if (dialog.exec() == QDialog::Accepted)
            {
                QTimer::singleShot(0,this,SLOT(supervisorScreen()));
                timerSupervisorKeyLabel->stop();
            }
        }
        // else no supervisor password has been configured so not required
        // if no supervisor USB key is inserted
        else
        {
            // Go to service screen since no supervisor USB key or password is required
            //qDebug()<<"supervisor page";
            QTimer::singleShot(0,this,SLOT(supervisorScreen()));
            timerSupervisorKeyLabel->stop();
        }
    }
    // service usb key is required
    else
    {
        //qDebug()<<"Missing supervisor key";
        QTimer::singleShot(0,this,SLOT(updateSupervisorKeyMessage()));
        timerSupervisorKeyLabel->start(500);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::supervisorScreen()
{
    supervisorMenu d(this);
    connect(&d, SIGNAL(sig_MessageUpdated()), this, SIGNAL(sig_MessageUpdated()));
    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));

    //Propagate signal from generalconfig
    connect(&d, SIGNAL(sig_UpdateLanguage(QString)), this, SIGNAL(sig_UpdateLanguage(QString)));
    d.exec();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::initialize()
{
    this->ui_->supervisorKeyLabel->hide();
    this->ui_->serviceKeyLabel->hide();
    this->timerServiceKeyLabel = new QTimer(this);
    this->timerSupervisorKeyLabel = new QTimer(this);
    connect(timerServiceKeyLabel, SIGNAL(timeout()), this, SLOT(updateServiceKeyMessage()));
    connect(timerSupervisorKeyLabel, SIGNAL(timeout()), this, SLOT(updateSupervisorKeyMessage()));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainMenu::createLayout()
{
}

void mainMenu::slt_WeigherReset(){
    Q_EMIT sig_WeigherReset();
}

void mainMenu::updateSupervisorKeyMessage()
{
    if(!m_usbKey->getRequiresSupervisorKey() || m_usbKey->getSupervisorKeyInserted())
    {
        this->ui_->supervisorKeyLabel->hide();
    }
    else
    {
        this->ui_->supervisorKeyLabel->show();
    }
}

void mainMenu::updateServiceKeyMessage()
{
    if(!m_usbKey->getRequiresServiceKey() || m_usbKey->getServiceKeyInserted())
    {
        this->ui_->serviceKeyLabel->hide();
    }
    else
    {
        this->ui_->serviceKeyLabel->show();
    }
}
