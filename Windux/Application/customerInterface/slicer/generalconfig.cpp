#include "generalconfig.h"
#include "ui_generalconfig.h"
#include "slicerstatemachine.h"
#include "setuputility.h"
#include "hobart/usb/weigherconfiguration.h"
#include "weighcalib.h"
#include "weighconfig.h"

#include <QSettings>
#include <QProcess>
#include <QDebug>

GeneralConfig::GeneralConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GeneralConfig)
{
    ui->setupUi(this);

    //Connect signals
    connect(this, SIGNAL(sig_ReqConfig()), (SetupUtility*) this->parent(), SIGNAL(sig_ReqConfig()));
    connect(this, SIGNAL(sig_WeigherReset()), (SetupUtility*) this->parent(), SIGNAL(sig_WeigherReset()));
    connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (SetupUtility*) this->parent(), SIGNAL(sig_WriteConfig(WeigherConfiguration &)));
    connect((SetupUtility*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)),Qt::UniqueConnection);

    // init
    QSettings settings;
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    initialize();
    loadSettings();

}

GeneralConfig::~GeneralConfig()
{
    delete ui;
}

void GeneralConfig::initialize()
{
    ui->modelComboBox->addItem( tr( "HS6" ), "HS6" );
    ui->modelComboBox->addItem( tr( "HS7" ), "HS7" );
    //Only show HS9 option if we are coming from Service Menu
    if(qobject_cast<servicemenu*>(this->parent()->parent()) != NULL){
        ui->modelComboBox->addItem( tr( "HS9" ), "HS9" );
    }

    ui->unitsComboBox->addItem( tr( "English" ), "English" );
    ui->unitsComboBox->addItem( tr( "Metric" ), "Metric" );

    ui->languageComboBox->addItem( tr( "English" ), "English" );
    ui->languageComboBox->addItem( tr( "French" ), "French" );

    connect(ui->modelComboBox, SIGNAL(currentIndexChanged(QString)), SLOT(slt_ModelChanged(QString)));
    connect(ui->supervisorPasswordCheckBox, SIGNAL(toggled(bool)), ui->supervisorPasswordLineEdit, SLOT(setEnabled(bool)));
    connect(ui->closeButton, SIGNAL(clicked(bool)), SLOT(closeDialog()));
}

void GeneralConfig::slt_ModelChanged(QString model)
{
    QSettings settings;

    //If no change, do not update
    if(model == settings.value("slicerModel").toString()){
        return;
    } else {
        settings.setValue("slicerModel", model);
    }

    //Update settings according to model configuration
    if(model == "HS6"){
        settings.setValue("isManualSlicer", true);
    } else if(model == "HS7"){
        settings.setValue("isManualSlicer", false);
    } else if(model == "HS9"){
        settings.setValue("isManualSlicer", false);
        //This Model also defaults the password to 8934
        ui->supervisorPasswordCheckBox->setChecked(true);
        settings.setValue("supervisorPasswordEnabled", true);
        ui->supervisorPasswordLineEdit->setText("8934");
        settings.setValue("supervisorPassword", ui->supervisorPasswordLineEdit->text());
    }

    //Run the linux sync to ensure file saves on power shut off before written into persistent storage
    settings.sync();
    static const QString linuxSync( "/bin/sync");
    QProcess p(this);
    p.start(linuxSync);
    p.waitForFinished(30000);
}

/*void GeneralConfig::slt_UnitsChanged(QString unit)
{
    QSettings settings;
    if(units == settings.value("slicerUnits").toString()){
        return;
    } else {
        PopupMessage * msg = new PopupMessage(this);
        msg->setHeader(tr("WARNING"));
        msg->setMessage(tr("Changing units will erase all Speedkeys"));
        msg->exec();
    }
}*/

void GeneralConfig::loadSettings()
{
    QSettings settings;
    ui->supervisorPasswordCheckBox->setChecked(settings.value("supervisorPasswordEnabled", true).toBool());
    ui->supervisorPasswordLineEdit->setEnabled(ui->supervisorPasswordCheckBox->isChecked());
    ui->supervisorPasswordLineEdit->setText(settings.value("supervisorPassword").toString());
    ui->modelComboBox->setCurrentText(settings.value("slicerModel").toString());
    ui->unitsComboBox->setCurrentIndex(ui->unitsComboBox->findData(settings.value("slicerUnits").toString()));
    ui->languageComboBox->setCurrentIndex(ui->languageComboBox->findData(settings.value("slicerLanguage").toString()));
}

void GeneralConfig::save()
{
    QSettings settings;
    settings.setValue("slicerModel", ui->modelComboBox->currentData());
    settings.setValue("slicerUnits", ui->unitsComboBox->currentData());
    settings.setValue("slicerLanguage", ui->languageComboBox->currentData());

    //Handle language changed
    Q_EMIT sig_UpdateLanguage(settings.value("slicerLanguage").toString()); //Emit signal to mainwindow

    // Add any future service password requirement here
    if(ui->supervisorPasswordCheckBox->isChecked()){
        settings.setValue("supervisorPassword", ui->supervisorPasswordLineEdit->text());
        settings.setValue("supervisorPasswordEnabled", true);
    } else {
        settings.setValue("supervisorPassword", "");
        settings.setValue("supervisorPasswordEnabled", false);
    }

    settings.setValue("slicerUnitsText", ui->unitsComboBox->currentText());
    settings.setValue("slicerLanguageText", ui->languageComboBox->currentText());

    //run the linux sync to ensure file saves on power shut off before written into persistent storage
    settings.sync();
    static const QString linuxSync( "/bin/sync");
    QProcess p(this);
    p.start(linuxSync);
    p.waitForFinished(30000);
}

void GeneralConfig::closeDialog()
{
    save();

    //Change weighing config according to units selected
    weighconfig d(this);
    connect(&d, SIGNAL(finished()), this, SLOT(accept()));
    d.open();

    accept();
    this->close();
}

void GeneralConfig::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}

