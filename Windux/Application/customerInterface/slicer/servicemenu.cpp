#include "servicemenu.h"
#include "ui_servicemenu.h"
#include "setuputility.h"
#include <iostream>
#include "weighconfig.h"
#include "weighcalib.h"
#include "popupmessage.h"

servicemenu::servicemenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::servicemenu)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    connect(ui->ConfigKey, SIGNAL(clicked(bool)), SLOT(onConfigButtonClicked()));
    connect(ui->CalibKey, SIGNAL(clicked(bool)), SLOT(onCalibButtonClicked()));
    connect(ui->setupUtilityBtn, SIGNAL(clicked(bool)), SLOT(onSetupUtility()));
    connect(ui->backKey, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(ui->homeKey, SIGNAL(clicked(bool)), SLOT(accept()));

    //To Weigher
    connect(this, SIGNAL(sig_ReqConfig()), (mainMenu*) this->parent(), SLOT(slt_ReqConfig()));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (mainMenu*) this->parent(), SLOT(slt_WriteConfig(WeigherConfiguration&)));
    connect(this, SIGNAL(sig_RezeroWeigher()), (mainMenu*) this->parent(), SLOT(slt_RezeroWeigher()));
    connect(this, SIGNAL(sig_SetWeigherMode(WeighingMode)), (mainMenu*) this->parent(), SLOT(slt_SetWeigherMode(WeighingMode)));
    connect(this, SIGNAL(sig_WeigherReset()), (mainMenu*) this->parent(), SLOT(slt_WeigherReset()));

    //From Weigher
    connect((mainMenu*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SLOT(slt_WeigherConfigChanged(WeigherConfiguration&)));
    connect((mainMenu*) this->parent() , SIGNAL(sig_WeigherKeyPressed()), this, SLOT(slt_WeigherKeyPressed()));
    connect((mainMenu*) this->parent(), SIGNAL(sig_WeigherStatus(char*,int)), this , SLOT(slt_WeigherStatus(char*,int)));
}

servicemenu::~servicemenu()
{
    delete ui;
}

void servicemenu::onConfigButtonClicked()
{
    weighconfig d(this);
    d.exec();
}

void servicemenu::onCalibButtonClicked()
{
    weighcalib d(this);
    d.exec();
}

void servicemenu::slt_WeigherKeyPressed()
{
    Q_EMIT sig_WeigherKeyPressed();
}

void servicemenu::slt_WeigherConfigChanged( WeigherConfiguration &config)
{
    std::cout << "Service Menu: Config Changed" << std::endl;
    Q_EMIT sig_WeigherConfigChanged(config);
}

void servicemenu::slt_WriteConfig(WeigherConfiguration &config)
{
    Q_EMIT sig_WriteConfig(config);
}

void servicemenu::slt_ReqConfig()
{
    Q_EMIT sig_ReqConfig();
}

void servicemenu::slt_RezeroWeigher()
{
    Q_EMIT sig_RezeroWeigher();
}

void servicemenu::slt_WeigherStatus(char * buffer, int buffLen)
{
    Q_EMIT sig_WeigherStatus(buffer, buffLen);
}

void servicemenu::slt_SetWeigherMode(WeighingMode mode)
{
    Q_EMIT sig_SetWeigherMode(mode);
}

void servicemenu::slt_WeigherReset()
{
    Q_EMIT sig_WeigherReset();
}

void servicemenu::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

void servicemenu::onSetupUtility()
{
    QTimer::singleShot(0,this,SLOT(setupUtility()));
}

void servicemenu::setupUtility()
{
    SetupUtility d(this);
    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));

    //Propagate signal from generalconfig
    connect(&d, SIGNAL(sig_UpdateLanguage(QString)), this, SIGNAL(sig_UpdateLanguage(QString)));
    d.exec();
}

