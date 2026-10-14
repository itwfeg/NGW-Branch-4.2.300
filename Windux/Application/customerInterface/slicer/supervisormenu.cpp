#include "supervisormenu.h"
#include "ui_supervisormenu.h"

#include "msgboxconfig.h"
#include "speedkeyconfig.h"
#include "setuputility.h"
#include "mainmenu.h"
#include "slicerstatisticsviewer.h"

#include <QDebug>
#include <QFile>
#include <QGridLayout>
#include <QSpacerItem>
#include <QTimer>

supervisorMenu::supervisorMenu(QWidget *parent) :
    _Mybase(parent),
    ui_(new Ui::supervisorMenu)
{
    ui_->setupUi(this);

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    initialize();
    createLayout();

    connect(ui_->alertMsg_lbl,SIGNAL(clicked(bool)),SLOT(onAlertMsgSetup()));
    connect(ui_->speedKeySett_lbl,SIGNAL(clicked(bool)),SLOT(onSpeedKeySetup()));
    connect(ui_->setupUtilityBtn,SIGNAL(clicked(bool)),SLOT(onSetupUtility()));
    connect(ui_->statsButton,SIGNAL(clicked(bool)),SLOT(onStats()));
    connect(ui_->backKey,SIGNAL(clicked(bool)),SLOT(reject()));
    connect(ui_->homeKey,SIGNAL(clicked(bool)),SLOT(accept()));

    //Signals used to handle units changed by GeneralConfig
    //From GeneralConfig to Weigher:
    connect(this, SIGNAL(sig_ReqConfig()), (mainMenu*) this->parent(), SLOT(slt_ReqConfig()));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (mainMenu*) this->parent(), SLOT(slt_WriteConfig(WeigherConfiguration &)));
    connect(this, SIGNAL(sig_WeigherReset()), (mainMenu*) this->parent(), SLOT(slt_WeigherReset()));
    //From Weigher to GeneralConfig:
    connect((mainMenu*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)));
}

supervisorMenu::~supervisorMenu()
{
    delete ui_;
}

void supervisorMenu::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

void supervisorMenu::onAlertMsgSetup()
{
    //qDebug()<<"Alert setup page";
    QTimer::singleShot(0,this,SLOT(alertMsgSetup()));
}

void supervisorMenu::alertMsgSetup()
{
    msgBoxConfig d(this);
    connect(&d, SIGNAL(sig_MessageUpdated()), this, SIGNAL(sig_MessageUpdated()));

    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));
    d.exec();
}

void supervisorMenu::onSpeedKeySetup()
{
    //qDebug()<<"Supervisor Page";
    QTimer::singleShot(0,this,SLOT(speedKeySetup()));
}

void supervisorMenu::speedKeySetup()
{
    speedKeyConfig d(this);


    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));
    d.exec();
}

void supervisorMenu::onSetupUtility()
{
    //qDebug()<<"Setup Utility Page";
    QTimer::singleShot(0,this,SLOT(setupUtility()));
}

void supervisorMenu::setupUtility()
{
    SetupUtility d(this);
    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));

    //Propagate signal from generalconfig
    connect(&d, SIGNAL(sig_UpdateLanguage(QString)), this, SIGNAL(sig_UpdateLanguage(QString)));
    d.exec();
}

void supervisorMenu::onStats()
{
    //qDebug()<<"Status Page";
    QTimer::singleShot(0,this,SLOT(initStats()));
}

void supervisorMenu::initStats()
{
    SlicerStatisticsViewer d(this->parent()->parent());

    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));
    d.exec();
}

void supervisorMenu::initialize()
{
}

void supervisorMenu::createLayout()
{
}
