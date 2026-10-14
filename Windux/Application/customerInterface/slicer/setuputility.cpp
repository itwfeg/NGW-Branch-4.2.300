#include "setuputility.h"
#include "ui_setuputility.h"

#include "generalconfig.h"
#include "timeedit.h"
#include "servicemenu.h"
#include "supervisormenu.h"

#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QTimer>
#include <QFileDialog>


SetupUtility::SetupUtility(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetupUtility)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    //Hide General Configuration page if coming from Supervisor Menu
    //QSizePolicy temp = this->ui->ConfigKey->sizePolicy();
    //temp.setRetainSizeWhenHidden(true);
    //this->ui->ConfigKey->setSizePolicy(temp);
    //if(qobject_cast<servicemenu*>(parent) == NULL){
    //    this->ui->ConfigKey->hide();
    //}

    connect(this->ui->calibrateTouchBtn, SIGNAL(clicked(bool)), SLOT(onCalibTouch()));
    connect(this->ui->installToolButton, SIGNAL(clicked(bool)), SLOT(onInstallTouch()));
    connect(this->ui->timeEditBtn,SIGNAL(clicked(bool)), SLOT(onTimeEdit()));
    connect(this->ui->ConfigKey, SIGNAL(clicked(bool)), SLOT(onConfigure()));
    connect(this->ui->backKey, SIGNAL(clicked(bool)), SLOT(reject()));
    connect(this->ui->homeKey, SIGNAL(clicked(bool)), SLOT(accept()));

    //Signals to handle units changed by GeneralConfig
    if(qobject_cast<servicemenu*>(parent) == NULL){ //If parent is SupervisorMenu
        connect(this, SIGNAL(sig_ReqConfig()), (supervisorMenu*) this->parent(), SIGNAL(sig_ReqConfig()));
        connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (supervisorMenu*) this->parent(), SIGNAL(sig_WriteConfig(WeigherConfiguration &)));
        connect(this, SIGNAL(sig_WeigherReset()), (supervisorMenu*) this->parent(), SIGNAL(sig_WeigherReset()));//From Weigher to GeneralConfig
        connect((supervisorMenu*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)));
    } else { //If parent is ServiceMenu
        connect(this, SIGNAL(sig_ReqConfig()), (servicemenu*) this->parent(), SIGNAL(sig_ReqConfig()));
        connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (servicemenu*) this->parent(), SIGNAL(sig_WriteConfig(WeigherConfiguration &)));
        connect(this, SIGNAL(sig_WeigherReset()), (servicemenu*) this->parent(), SIGNAL(sig_WeigherReset()));//From Weigher to GeneralConfig
        connect((servicemenu*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)));
    }
}

SetupUtility::~SetupUtility()
{
    delete ui;
}

void SetupUtility::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

void SetupUtility::onConfigure()
{
    //qDebug() << "Setup Configure Page";
    QTimer::singleShot(0,this,SLOT(generalConfigure()));
}

void SetupUtility::generalConfigure()
{
    GeneralConfig d(this);
    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));

    //Propagate signal from generalconfig
    connect(&d, SIGNAL(sig_UpdateLanguage(QString)), this, SIGNAL(sig_UpdateLanguage(QString)));
    d.exec();
}

void SetupUtility::onCalibTouch()
{
    static const QString script("/usr/local/hobart/python/calibrateTouchscreen.py");

    //qDebug()<<"Calibrate Touch";

    if (!QFile::exists(script))
        return;

    QStringList args;
    args.append(script);

    QProcess p(this);
    p.start("python",args);

    p.waitForFinished(300000);

    accept();
}

void SetupUtility::onInstallTouch()
{
    QFileDialog qfd;
    QString installFilename;

    static const QString script("/usr/local/hobart/python/installUpgrade.py");

    //qDebug()<<"Install Touch";

    /* Display File Dialog to select install file */

    if (!QFile::exists(script)) {
        //qDebug()<<"Install upgrade python script does not exist.";
        return;
    }

    QString startDirStr = "/media/thumb";
    if ( !QDir( startDirStr ).exists() )
        startDirStr = "/media";

    qfd.setAcceptMode(QFileDialog::AcceptOpen);
    qfd.setDirectory(startDirStr);
    qfd.setFileMode( QFileDialog::ExistingFile);
    qfd.setOption(QFileDialog::ReadOnly);
    qfd.setLabelText( QFileDialog::Accept, "Install");
    qfd.setNameFilter( tr( "Core File (hti-linux-core-*.package.tgz);;U-boot File (hti-linux-uboot-*.package.tgz)" ) );
    qfd.setWindowTitle( tr( "Choose Install File" ) );
    qfd.setWindowFlags(Qt::FramelessWindowHint);
    qfd.setWindowState(qfd.windowState() | Qt::WindowFullScreen);
    qfd.setViewMode(QFileDialog::List);

    if ( QDialog::Accepted == qfd.exec() )
    {
        QStringList selected( qfd.selectedFiles() );

        if ( 1 == selected.count() )
            installFilename = selected[0];

        if ( !installFilename.isEmpty() )
        {

           QStringList args;
           args.append(script);
           args.append(installFilename);
           //qDebug()<<installFilename;

           QProcess p(this);
           p.start("python",args);

           p.waitForFinished(-1);

           accept();
        }
    }
}

void SetupUtility::onTimeEdit()
{
    //qDebug()<<"Time Edit Page";
    QTimer::singleShot(0,this,SLOT(timeEdit()));
}

void SetupUtility::timeEdit()
{
    TimeEdit d(this);
    connect(&d, SIGNAL(accepted()), this, SLOT(accept()));
    d.exec();
}
