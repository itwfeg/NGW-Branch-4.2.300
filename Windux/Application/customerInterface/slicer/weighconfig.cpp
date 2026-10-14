#include "weighconfig.h"
#include "ui_weighconfig.h"
#include "popupmessage.h"
#include <iostream>
#include <ctime>
#include <math.h>
#include <QTimer>
#include <QSettings>
#include <QProcess>
#include <QDateTime>
#include <QDebug>
#include "hobart/usb/weigherconfiguration.h"
#include "servicemenu.h"
#include "standardpopup.h"
#include "generalconfig.h"

weighconfig::weighconfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::weighconfig),
    mode_( WeighMode::WeighMode_Avoir)
{
    this->ConfigNeedsUpdated = true;
    this->writingConfig = false;
    this->weigherKeyPressed = false;
    ui->setupUi(this);
    this->ui->cmbDivisionsize->setEnabled(false);
    this->ui->cmbFilterSpeed->setEnabled(false);
    this->ui->cmbMaxzweight->setEnabled(false);
    this->ui->cmbweighmodel->setEnabled(false);
    this->ui->lineEdit->setEnabled(false);
    this->ui->largeMotion->setEnabled(false);
    this->setEnabled(true);
    this->ui->backBtn->setEnabled(true);
    this->ui->initWeigherBoardBtn->setEnabled(true);

    //Init button does the same thing as back button.
    connect(this->ui->initWeigherBoardBtn, SIGNAL(clicked(bool)), this, SLOT(on_initBtn_clicked()));
    connect(this->ui->backBtn, SIGNAL(clicked(bool)), this, SLOT(on_backBtn_clicked()));

    if(qobject_cast<GeneralConfig*>(parent) == NULL){ //If we are coming from ServiceMenu
        connect(this, SIGNAL(sig_ReqConfig()), (servicemenu*) this->parent(), SLOT(slt_ReqConfig()));
        connect(this, SIGNAL(sig_WeigherReset()), (servicemenu*) this->parent(), SLOT(slt_WeigherReset()));
        connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (servicemenu*) this->parent(), SLOT(slt_WriteConfig(WeigherConfiguration &)));
        connect((servicemenu*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SLOT(slt_WeigherConfigChanged(WeigherConfiguration&)),Qt::UniqueConnection);
        connect((servicemenu*) this->parent() , SIGNAL(sig_WeigherKeyPressed()), this, SLOT(onEnableConfig()));
    } else { //If we are coming from GeneralConfig
        connect(this, SIGNAL(sig_ReqConfig()), (GeneralConfig*) this->parent(), SIGNAL(sig_ReqConfig()));
        connect(this, SIGNAL(sig_WeigherReset()), (GeneralConfig*) this->parent(), SIGNAL(sig_WeigherReset()));
        connect(this, SIGNAL(sig_WriteConfig(WeigherConfiguration&)), (GeneralConfig*) this->parent(), SIGNAL(sig_WriteConfig(WeigherConfiguration &)));
        connect((GeneralConfig*) this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), this, SLOT(slt_WeigherConfigChanged(WeigherConfiguration&)),Qt::UniqueConnection);

        this->ConfigNeedsUpdated = false;
        this->weigherKeyPressed = true;
        init();
    }

    connect(ui->cmbweighmodel,SIGNAL(currentIndexChanged(int)),this,SLOT(onWeigherModelChanged(int)));
    connect(ui->cmbMaxzweight,SIGNAL(currentIndexChanged(int)),this,SLOT(onMaxzWeightChanged()));

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    //init();

    //DEBUG>>>>
    //onEnableConfig();
    //DEBUG>>>>

    if(!(qobject_cast<GeneralConfig*>(parent) == NULL)){
        saveFromGeneralConfig();
        //qDebug() << "After SAVE";
        Q_EMIT sig_ReqConfig(); //Emit signal to mainwindow
        done(1);
        //qDebug() << "After DONE";
    }

    //TODO: Show alert message only after screen loads?
    //TODO: Translate this
    //PopupMessage * msg = new PopupMessage(this);
    //msg->setHeader(tr("WARNING"));
    //msg->setMessage(tr("Pressing weigher switch may reset Speedkeys"));
    //msg->exec();
}

weighconfig::~weighconfig()
{
    delete ui;
}

void weighconfig::init()
{
    ui->cmbweighmodel->clear();
    ui->cmbweighmodel->addItem( tr( "None" ), 0 );
    //ui->cmbweighmodel->addItem( "AWS" , MainWindow::EzUSB );         // EzUSB weigher - not used on 5i and 6x (VxWorks and Windux)
    ui->cmbweighmodel->addItem( "AWS REV 2" , WeigherModels::AWS2 );    // single range ATMEL weigher - 2 digit precision
    ui->cmbweighmodel->addItem( "AWS REV 2 DR" , WeigherModels::AWS );  // dual range ATMEL weigher - 3 digit precision
    //ui->cmbweighmodel->addItem( "EPSA" , MainWindow::EPSA );         // EzUSB weigher - not used on 5i or 6x (VxWorks and Windux)
    ui->cmbweighmodel->addItem( "EPSA REV 2" , WeigherModels::EPSA2 );

    ui->cmbFilterSpeed->clear();

    ui->cmbFilterSpeed->addItem( tr( "Normal" ), 0 );
    ui->cmbFilterSpeed->addItem( tr( "Slow" ), 1 );
    ui->cmbFilterSpeed->addItem( tr( "Fast" ), 2 );

    ui->largeMotion->addItem("25",25);
    ui->largeMotion->addItem("35",35);
    ui->largeMotion->addItem("40",40);
    ui->largeMotion->addItem("50",50);

    PopulateDivisionSize();
    PopulateMaximumWeight();

    //Select Correct data in dropdowns
    int index = ui->cmbweighmodel->findData(weigherModel);
    if(index == -1){
        // If not found then select the first item in the list - i.e. "None"
        ui->cmbweighmodel->setCurrentIndex( 0 );
    } else {
        ui->cmbweighmodel->setCurrentIndex( index );
    }
    ui->cmbFilterSpeed->setCurrentIndex( ui->cmbFilterSpeed->findData( weigherFilterSpeed.toInt() ) );
    ui->cmbMaxzweight->setCurrentIndex( ui->cmbMaxzweight->findText( weigherMaximumWt, Qt::MatchContains ) );
    QTimer::singleShot(0, this, SLOT(slt_initConfigReq()));
    QTimer::singleShot(1000, this, SLOT(slt_matchSettings()));
}

void weighconfig::slt_initConfigReq()
{
    //This sends out a signal to request the configuration settings from the weigher
    Q_EMIT sig_ReqConfig(); //Emit signal to mainwindow
    QEventLoop loop(this);
    connect((servicemenu* )this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), &loop, SLOT(quit()));
    QTimer::singleShot(60000, &loop, SLOT(quit()));
    loop.exec();
    if(ConfigNeedsUpdated){
        this->setEnabled(true);
        standardPopup d(this);
        d.setMessage(tr("Timeout: No response from weigher"));
        d.exec();
        this->close();
    }
}

void weighconfig::setweight(QString weight)
{
    if(weight == "lbs")
    {
        mode_ = WeighMode::WeighMode_Avoir;
    }
    else if(weight == "kg")
    {
        mode_ = WeighMode::WeighMode_Metric;
    }
}

void weighconfig::on_backBtn_clicked()
{
    this->ui->cmbDivisionsize->setEnabled(false);
    this->ui->cmbFilterSpeed->setEnabled(false);
    this->ui->cmbMaxzweight->setEnabled(false);
    this->ui->cmbweighmodel->setEnabled(false);
    this->ui->lineEdit->setEnabled(false);
    this->weigherKeyPressed = false;
    accept();
}

void weighconfig::on_initBtn_clicked()
{
    if(weigherKeyPressed){
        //qDebug() << "weighconfig::on_initBtn_clicked()";
        saveWeigherConfig();
        Q_EMIT sig_ReqConfig(); //Emit signal to mainwindow
    }
    this->ui->cmbDivisionsize->setEnabled(false);
    this->ui->cmbFilterSpeed->setEnabled(false);
    this->ui->cmbMaxzweight->setEnabled(false);
    this->ui->cmbweighmodel->setEnabled(false);
    this->ui->lineEdit->setEnabled(false);
    this->weigherKeyPressed = false;
    accept();
}

//called when the weigher key is pressed
void weighconfig::onEnableConfig()
{
    std::cout << "enabling" << std::endl;
    this->ui->cmbDivisionsize->setEnabled(true);
    this->ui->cmbFilterSpeed->setEnabled(true);
    this->ui->cmbMaxzweight->setEnabled(true);
    this->ui->cmbweighmodel->setEnabled(true);
    this->ui->lineEdit->setEnabled(true);
    this->ui->largeMotion->setEnabled(true);
    this->weigherKeyPressed = true;
    init();
}

void weighconfig::PopulateDivisionSize()
{
    this->ui->cmbDivisionsize->blockSignals(true);


    this->ui->cmbDivisionsize->clear();

    if( weigherModel == WeigherModels::EPSA2 ||
            weigherModel == WeigherModels::AWS2  ||
            weigherModel == WeigherModels::SASCALE1 )
    {
        if(mode_ == WeighMode::WeighMode_Metric)
        {
            this->ui->cmbDivisionsize->addItem( "0.005 kg", 5);
        }
        else
        {
            this->ui->cmbDivisionsize->addItem( "0.01 lb", 1);
        }
    }
    else if( weigherModel == WeigherModels::HLX1 ||
             weigherModel == WeigherModels::SASCALE2 )
    {
        if(mode_ == WeighMode::WeighMode_Metric)
        {
            this->ui->cmbDivisionsize->addItem( "0.002 kg", 2);
            this->ui->cmbDivisionsize->addItem( "0.005 kg", 5);
        }
        else
        {
            this->ui->cmbDivisionsize->addItem( "0.01 lb", 1);           // adds 0.01 division size and precision weighing
            this->ui->cmbDivisionsize->addItem( "0.005 lb", 5);
        }
    }
    else if( weigherModel == WeigherModels::AWS)
    {
        // HT-209 AWS REV2 DR should only allow 0.002 kg / 0.005 lb
        if(mode_ == WeighMode::WeighMode_Metric)
        {
            this->ui->cmbDivisionsize->addItem( "0.002 kg", 2);
        }
        else
        {
            this->ui->cmbDivisionsize->addItem( "0.005 lb", 5);
        }
    }
}

void weighconfig::PopulateMaximumWeight()
{
    this->ui->cmbMaxzweight->clear();

    if( weigherModel == WeigherModels::HLX1 ||
            weigherModel == WeigherModels::AWS  ||
            weigherModel == WeigherModels::AWS2 ||
            weigherModel == WeigherModels::SASCALE2 )
    {
        this->ui->cmbMaxzweight->addItem( "30 lb", cap30lb );
        this->ui->cmbMaxzweight->addItem( "15 kg", cap15kg );
    }
    else if( weigherModel == WeigherModels::EPSA2 )
    {
        this->ui->cmbMaxzweight->addItem( "30 lb", cap30lb );
        this->ui->cmbMaxzweight->addItem( "50 lb", cap50lb );
        this->ui->cmbMaxzweight->addItem( "15 kg", cap15kg );
        this->ui->cmbMaxzweight->addItem( "25 kg", cap25kg );
    }
    else if( weigherModel == WeigherModels::SASCALE1 )
    {
        this->ui->cmbMaxzweight->addItem( "50 lb", cap50lb );
        this->ui->cmbMaxzweight->addItem( "25 kg", cap25kg );
    }
}

//Set max Weight according to what is in settings
void weighconfig::slt_matchSettings()
{
    QSettings settings;
    if((settings.value("slicerUnits").toString() == "English") && ((currentConfig.getFlags() & 0x01) == 0x01)){
        setweight("lbs");
        if( weigherModel == WeigherModels::HLX1 ||
                weigherModel == WeigherModels::AWS  ||
                weigherModel == WeigherModels::AWS2 ||
                weigherModel == WeigherModels::SASCALE2 )
        {
            this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData( cap30lb ));
        }
        else if( weigherModel == WeigherModels::EPSA2 )
        {
            this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData( cap50lb ));
        }
        else if( weigherModel == WeigherModels::SASCALE1 )
        {
            this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData( cap50lb ));
        }
    } else if((settings.value("slicerUnits").toString() == "Metric") && ((currentConfig.getFlags() & 0x01) == 0x00)){
        setweight("kg");
        if( weigherModel == WeigherModels::HLX1 ||
                weigherModel == WeigherModels::AWS  ||
                weigherModel == WeigherModels::AWS2 ||
                weigherModel == WeigherModels::SASCALE2 )
        {
            this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData( cap15kg ));
        }
        else if( weigherModel == WeigherModels::EPSA2 )
        {
            this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData( cap25kg ));
        }
        else if( weigherModel == WeigherModels::SASCALE1 )
        {
            this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData( cap25kg ));
        }
    }
    saveWeigherConfig();
    Q_EMIT sig_ReqConfig(); //Emit signal to mainwindow
}

void weighconfig::onWeigherModelChanged(int index)
{
    weigherModel = ui->cmbweighmodel->itemData( index ).toInt();
    PopulateMaximumWeight();
    PopulateDivisionSize();
}

void weighconfig::onMaxzWeightChanged()
{
    if(this->ui->cmbMaxzweight->currentData() == cap30lb  ||
            this->ui->cmbMaxzweight->currentData() == cap50lb ||
            this->ui->cmbMaxzweight->currentData() == cap300lb){
        mode_ = WeighMode::WeighMode_Avoir;
        this->ui->lineEdit->setText( QString::number((this->ui->lineEdit->text().toDouble() * 1000.0) * ONEGRAMASLBS, 'f' , 3));
    }

    else if(this->ui->cmbMaxzweight->currentData() == cap15kg  ||
             this->ui->cmbMaxzweight->currentData() == cap25kg ||
             this->ui->cmbMaxzweight->currentData() == cap150kg){
        this->ui->lineEdit->setText( QString::number((this->ui->lineEdit->text().toDouble()  / ONEGRAMASLBS) / 1000.0, 'f' , 3));
        mode_ = WeighMode::WeighMode_Metric;
    }
    //don't pop up unless the user changed it
    //qDebug() << "config " << currentConfig.getScaleWeighMode() << " box data " << mode_;
    if((!ConfigNeedsUpdated) && (currentConfig.getScaleWeighMode() != mode_) && (qobject_cast<GeneralConfig*>(this->parent()) == NULL)){
        PopupMessage * msg = new PopupMessage(this);
        msg->setHeader(tr("WARNING"));
        msg->setMessage(tr("Changing units will erase all Speedkeys"));
        msg->exec();
    }
    PopulateDivisionSize();
}

void weighconfig::updateSettingDivSize()
{
    QSettings settings;
    settings.setValue("divSize", currentConfig.getScaleFactor());
    settings.sync();
    //run the linux sync to ensure file saves on power shut off before written into persistent storage
    static const QString linuxSync( "/bin/sync");
    QProcess p(this);
    p.start(linuxSync);
    p.waitForFinished(30000);
}

void weighconfig::slt_WeigherConfigChanged(WeigherConfiguration &config)
{
    std::cout << "WeighConfig: configChanged" << std::endl;
    if (!writingConfig){
        WeigherModels model = (WeigherModels)config.getWeigherModel();
        this->ui->cmbweighmodel->setCurrentIndex(this->ui->cmbweighmodel->findData(model));

        // The flags field is used to indicate the weigh mode and the NetWeightDualRangeLimitCheck flag.
        // weighMode_Metric = 0x01
        // netWeightDualRangeLimitCheck = 0x02
        MaximumWeightCapacity maxCap;
        if((config.getFlags() & 0x01) > 0){
            mode_ = WeighMode::WeighMode_Metric;
            maxCap = cap15kg;
        }else{
            mode_ = WeighMode::WeighMode_Avoir;
            maxCap = cap30lb;
        }
        currentConfig = WeigherConfiguration(config);
        currentConfig.setScaleWeighMode(mode_);

        this->ui->cmbDivisionsize->setCurrentIndex(this->ui->cmbDivisionsize->findData(config.getScaleFactor()));
        this->ui->cmbMaxzweight->setCurrentIndex(this->ui->cmbMaxzweight->findData(maxCap));
        this->ui->cmbFilterSpeed->setCurrentIndex(config.getFilterSpeed());
        QString minWt = "-1";
        if(mode_ == WeighMode::WeighMode_Avoir){
            minWt = QString::number(((double)config.getMinWeightToPrint()) * ONEGRAMASLBS, 'f' ,3);
        } else{
            minWt = QString::number(((double)config.getMinWeightToPrint()) / 1000.0, 'f' , 3);
        }
        this->ui->lineEdit->setText(minWt);

        this->setEnabled(true);
        this->ConfigNeedsUpdated = false;
        int index = this->ui->largeMotion->findData(QString::number(config.getLargeMotionLimit()));
        //qDebug() << "Large Motion Limit: " << config.getLargeMotionLimit();
        //qDebug() << "Large Motion index: " << index;
        if(index != -1){
            this->ui->largeMotion->setCurrentIndex(index);
        } else {
            this->ui->largeMotion->setCurrentIndex(0);
            saveWeigherConfig();
        }
    }
}

void weighconfig::saveWeigherConfig()
{
    /*Q_EMIT sig_ReqConfig(); //Emit signal to mainwindow
    QEventLoop loop(this);
    connect((servicemenu* )this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), &loop, SLOT(quit()));
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();
    */
    //I already have a config at this point
    if(!ConfigNeedsUpdated){
        this->writingConfig = true;
        long date = QDateTime::currentMSecsSinceEpoch() / 1000;
        currentConfig.setLastConfigurationDate(date);
        currentConfig.setNumberOfConfigurations(currentConfig.getNumberOfConfigurations() + 1);
        currentConfig.setWeigherModel( (WeigherModel) this->ui->cmbweighmodel->currentData().toInt() );
        /*currentConfig.setConfigType(WgConfigType::WgPermanent);
        currentConfig.setScaleWeighMode((WeighMode) this->ui->cmbMaxzweight->currentIndex());
        currentConfig.setFilterSpeed((WgFilterSpeed) this->ui->cmbFilterSpeed->currentIndex());
        currentConfig.setScaleFactor(this->ui->cmbDivisionsize->currentData().toInt());
        currentConfig.setLargeMotionLimit(this->ui->largeMotion->currentData().toInt());*/

        //If we were metric
        if(mode_ == WeighMode_Avoir && ((currentConfig.getFlags() & 0x01) == 0x01) ){
            //qDebug() << "Changing to Avoir";
            //qDebug() << "Old Gain " << currentConfig.getGainFactor();
            //qDebug() << "Old COM " << currentConfig.getCenterOfMaintenanceZone();
            currentConfig.setGainFactor(weighcalib::convertGainFactorToAvoir(currentConfig.getGainFactor()));
            currentConfig.setCenterOfMaintenanceZone(weighcalib::convertGainFactorToAvoir(currentConfig.getCenterOfMaintenanceZone()));
            //qDebug() << "New Gain " << currentConfig.getGainFactor();
            //qDebug() << "New COM " << currentConfig.getCenterOfMaintenanceZone();
        //If we were avoir
        }else if(mode_ == WeighMode_Metric && ((currentConfig.getFlags() & 0x01) == 0x00)){
            //qDebug() << "Changing to Metric";
            //qDebug() << "Old Gain " << currentConfig.getGainFactor();
            //qDebug() << "Old COM " << currentConfig.getCenterOfMaintenanceZone();
            currentConfig.setGainFactor(weighcalib::convertGainFactorToMetric(currentConfig.getGainFactor()));
            currentConfig.setCenterOfMaintenanceZone(weighcalib::convertGainFactorToMetric(currentConfig.getCenterOfMaintenanceZone()));
            //qDebug() << "New Gain " << currentConfig.getGainFactor();
            //qDebug() << "New COM " << currentConfig.getCenterOfMaintenanceZone();
        }

        if(mode_){
            currentConfig.setFlags(currentConfig.getFlags() | 0x01); // set metric mode flag
        }else{
            currentConfig.setFlags(currentConfig.getFlags() & 0xFE); // clear metric mode flag
        }
        //Not sure why this was here originally
        //currentConfig.setFlags(currentConfig.getFlags() | mode_);

        QSettings settings;
        if(mode_ == WeighMode::WeighMode_Avoir){
            double minWtLbs = this->ui->lineEdit->text().toDouble();
            currentConfig.setMinWeightToPrint( (uint32_t) round(minWtLbs / ONEGRAMASLBS));
            settings.setValue("slicerUnits", "English");
        } else{
            double minWtkg = this->ui->lineEdit->text().toDouble();
            currentConfig.setMinWeightToPrint((uint32_t) round(minWtkg * 1000)); //kg to gram
            settings.setValue("slicerUnits", "Metric");
        }
        Q_EMIT sig_WriteConfig(currentConfig); //Emit signal to mainwindow
        updateSettingDivSize();
        this->writingConfig = false;
        Q_EMIT sig_WeigherReset(); //Emit signal to mainwindow
    }else{
        standardPopup d(this);
        d.setMessage(tr("Timeout: No response from weigher. Configuration not written"));
        d.exec();
    }
}

//Mimic the saveWeigherConfig function when coming from GeneralConfig
void weighconfig::saveFromGeneralConfig() {
    QEventLoop loop(this);
    connect((GeneralConfig* )this->parent(), SIGNAL(sig_WeigherConfigChanged(WeigherConfiguration&)), &loop, SLOT(quit()));
    QTimer::singleShot(0, &loop, SLOT(quit()));
    loop.exec();

    QSettings settings;
    if(settings.value("slicerUnits") == "English"){
        mode_ = WeighMode_Avoir;
    }else if(settings.value("slicerUnits") == "Metric"){
        mode_ = WeighMode_Metric;
    }

    long date = QDateTime::currentMSecsSinceEpoch() / 1000;
    currentConfig.setLastConfigurationDate(date);
    currentConfig.setNumberOfConfigurations(currentConfig.getNumberOfConfigurations() + 1);
    currentConfig.setWeigherModel((WeigherModel) currentConfig.getWeigherModel());
    /*currentConfig.setConfigType(WgConfigType::WgPermanent);
    currentConfig.setFilterSpeed(currentConfig.getFilterSpeed());
    currentConfig.setLargeMotionLimit(currentConfig.getLargeMotionLimit());
    currentConfig.setMinWeightToPrint((uint32_t) 0.000);*/

    //If we were metric
    if(mode_ == WeighMode_Avoir && ((currentConfig.getFlags() & 0x01) == 0x01) ){
        currentConfig.setGainFactor(weighcalib::convertGainFactorToAvoir(currentConfig.getGainFactor()));
        currentConfig.setCenterOfMaintenanceZone(weighcalib::convertGainFactorToAvoir(currentConfig.getCenterOfMaintenanceZone()));
        qDebug() << "Override Config: Avoir Mode";
    //If we were avoir
    }else if(mode_ == WeighMode_Metric && ((currentConfig.getFlags() & 0x01) == 0x00)){
        currentConfig.setGainFactor(weighcalib::convertGainFactorToMetric(currentConfig.getGainFactor()));
        currentConfig.setCenterOfMaintenanceZone(weighcalib::convertGainFactorToMetric(currentConfig.getCenterOfMaintenanceZone()));
        qDebug() << "Override Config: Metric Mode";
    }

    if(mode_ == WeighMode_Avoir){
        currentConfig.setScaleFactor(5);
        currentConfig.setFlags(currentConfig.getFlags() & 0xFE); // clear metric mode flag
        currentConfig.setScaleWeighMode((WeighMode::WeighMode_Avoir));
    }else if(mode_ == WeighMode_Metric){
        currentConfig.setScaleFactor(2);
        currentConfig.setFlags(currentConfig.getFlags() | 0x01); // set metric mode flag
        currentConfig.setScaleWeighMode((WeighMode::WeighMode_Metric));
    }


    Q_EMIT sig_WriteConfig(currentConfig); //Emit signal to mainwindow
    updateSettingDivSize();
    this->writingConfig = false;
    Q_EMIT sig_WeigherReset(); //Emit signal to mainwindow
}

void weighconfig::showEvent(QShowEvent *e)
{
    const QRect p( parentWidget()->geometry() );
    const QRect c( rect() );

    move( p.center() - c.center() );

    QDialog::showEvent( e );
}
