#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainmenu.h"
#include "message.h"
#include "speedkey.h"
#include "speedkeyconfig.h"
#include "alertmessagepopup.h"
#include "float.h"
#include "popupmessage.h"
#include "weighcalib.h"

#include "hobart/usb/weigherconfiguration.h"

#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QSettings>
#include <QDebug>
#include <QMessageBox>
#include <QtMath>
#include <QFont>
#include <QErrorMessage>
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

#include <fstream>
#include <unistd.h>
#include "stdint.h"

#define MAX_NUMBER_CHARACTERS_DISPLAYED_WEIGHT 5
static const float OUNCE_RATIO = 16.0;
static const float KG_RATIO = 0.45359237;

MainWindow::MainWindow( bool simulate, QWidget *parent, Qt::WindowFlags flags) :
    _Mybase( parent, flags ),
    ui_( new Ui::MainWindow ),
    mode_( displayPounds ),
    scaleState_( ScaleInterface::zero ),
    weight_( 0.0 ),
    ounces_( 0.0 ),
    setpointWeight_( 0.0 ),
    setpointOunces_( 0.0 ),
    currentTextScrollIndex_( 0 ),
    flashTimes_( 0 ),
    setpointInfState_(Scale::noInfinityMode),
    isRunning_(false)

{
    prevWeight_ = 0.0;
    lastPressed = -1;
    inputType_ = none;

    Q_UNUSED( simulate );

    QPalette p( palette() );
    p.setBrush( backgroundRole(), Qt::black );
    p.setBrush( foregroundRole(), Qt::white );
    setPalette( p );

    ui_->setupUi( this );

    // read settings
    QSettings settings;
    QString logo( settings.value("Logo", QString()).toString() );

    if ( logo.length() )
    {
        QString logoFull( ":/images/res/" + logo );
        if ( QFile::exists( logoFull ) )
            ui_->m_plogo->setPixmap( QPixmap( logoFull ) );
    }

    flashingTimer_.setInterval( 500 );
    connect( &flashingTimer_, SIGNAL(timeout()), SLOT(slt_HandleFlash()) );

    textScrollTimer_.setInterval( settings.value("TextScrollMs", 2000).toInt() );
    connect( &textScrollTimer_, SIGNAL(timeout()), SLOT(slt_ScrollText()) );

    mBox = new msgbox(this);
    connect(mBox, SIGNAL(sig_DisplayMessage(QString, QString, QString, int)), this, SLOT(slt_DisplayMessage(QString, QString, QString, int)));
    connect(this->ui_->m_pUpIcon, SIGNAL(clicked(bool)),this, SLOT(onUpIconPressed()));
    connect(this->ui_->m_pDownIcon, SIGNAL(clicked(bool)),this, SLOT(onDownIconPressed()));
    //Don't Resize when hidden
    QSizePolicy sp = ui_->m_pUpIcon->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui_->m_pUpIcon->setSizePolicy(sp);

    sp = ui_->m_pDownIcon->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui_->m_pDownIcon->setSizePolicy(sp);

    sp = ui_->m_pSetpointUnitLabel->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui_->m_pSetpointUnitLabel->setSizePolicy(sp);

    initialize();

    //Continually check for a change in input
    QTimer* inputTimer = new QTimer(this);
    QTimer::singleShot(0, this, SLOT(slt_CheckInputType()));
    connect(inputTimer, SIGNAL(timeout()), this, SLOT(slt_CheckInputType()));
    inputTimer->start(2000);
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::slt_SetpointChanged( ScaleInterface::weightInPounds_t weight )
{
    setpointOunces_ = (OUNCE_RATIO * (setpointWeight_ = weight));// + 0.5;
    //qDebug() <<"SW: " << setpointWeight_ <<"So: " << setpointOunces_ ;
    updateWeight();
}

void MainWindow::slt_WeightUpdated( ScaleInterface::State state, ScaleInterface::weightInPounds_t weight )
{
    scaleState_ = state;
    if (( ScaleInterface::zero == state ) || ( ScaleInterface::stable == state )){
        ounces_ = (OUNCE_RATIO * (weight_ = weight));// + 0.5;
    }
    updateWeight();
}

void MainWindow::slt_Zeroing()
{
    ui_->m_pCurrentLabel->setText( "" );
}

void MainWindow::slt_Running()
{
    //qDebug() << "running...";
    textScrollTimer_.stop();
    ui_->m_pInstructText->setText( tr("Running...") );
    ui_->m_pUpIcon->setIcon(QIcon(":/images/res/upArrow.png"));
    ui_->m_pUpIcon->setEnabled(false);
    ui_->m_pUpIcon->hide();
    ui_->m_pDownIcon->setIcon(QIcon(":/images/res/downArrow.png"));
    ui_->m_pDownIcon->setEnabled(false);
    ui_->m_pDownIcon->hide();

    ui_->m_pWeightIcon->setEnabled(false);
    ui_->m_pWeightIcon->hide();

    ui_->btn30sec->hide();

    ui_->btnMenu->setEnabled(false);

    isRunning_ = true;
}

void MainWindow::slt_SlicingComplete()
{
    QSettings settings;

    const int total( settings.value("OrdersCompleted").toInt() );
    settings.setValue("OrdersCompleted",total+1);

    textScrollTimer_.stop();
    ui_->m_pInstructText->setText( tr( "Take Product" ) );
}

void MainWindow::slt_Paused()
{
    textScrollTimer_.stop();
    ui_->m_pInstructText->setText( tr( "Press to Clear" ) );
}

void MainWindow::slt_Stopped()
{
    qDebug() << "slt_Stopped";
    textScrollTimer_.stop();
    ui_->m_pInstructText->setText( tr( "Press to Zero" ) );

    ui_->m_pUpIcon->setEnabled(true);
    ui_->m_pUpIcon->show();
    ui_->m_pDownIcon->setEnabled(true);
    ui_->m_pDownIcon->show();
    isRunning_ = false;
    prevWeight_ = 0.0;

    ui_->m_pWeightIcon->setEnabled(true);
    ui_->m_pWeightIcon->show();

    ui_->btnMenu->setEnabled(true);
    ui_->btn30sec->show();
}

void MainWindow::slt_Error(ScaleInterface::State state)
{
    qDebug() << "ERROR";
    ui_->m_pUpIcon->show();
    ui_->m_pDownIcon->show();
    isRunning_ = false;

    flashType_ = redFlashing;
    flashingTimer_.start();

    QPalette p( palette() );
    p.setBrush( backgroundRole(), Qt::white );
    p.setBrush( foregroundRole(), Qt::black );

    PopupMessage * popup = new PopupMessage(this);
    popup->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    popup->setAttribute(Qt::WA_DeleteOnClose, true);
    popup->setHeader(tr("30 Second Timeout"));

    if(state == ScaleInterface::unstable){
        popup->setMessage(tr("Slicer Unstable\nPlease check the stability of the slicer"));
    }else{
        popup->setMessage(tr("Weight did not change for more than 30s"));
    }
    QFont f(popup->font());
    f.setPointSize(18);
    popup->setPalette(p);
    popup->setFont(f);
    popup->repaint();
    popup->setModal(true);
    popup->show();
}

void MainWindow::slt_TimeoutPopup()
{
    QPalette p( palette() );
    p.setBrush( backgroundRole(), Qt::white );
    p.setBrush( foregroundRole(), Qt::black );

    PopupMessage * popup = new PopupMessage(this);
    popup->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    popup->setAttribute(Qt::WA_DeleteOnClose, true);
    popup->setHeader(tr("30 Second Timeout"));

    popup->setMessage(tr("Weight did not change for more than 30s"));
    QFont f(popup->font());
    f.setPointSize(18);
    popup->setPalette(p);
    popup->setFont(f);
    popup->repaint();
    popup->setModal(true);
    popup->show();
}

void MainWindow::slt_WeightReached()
{
    flashType_ = greenFlashing;
    flashingTimer_.start();
    //qDebug() << "Flash Timer Started";
    //qDebug()<< "LastPressed:" << (lastPressed != -1);
    if( lastPressed != -1 ){
        speedKey* k = getSpeedKeys().at(lastPressed);
        //qDebug() << "weight" << weight_ << "prevWeight" << prevWeight_;
        //qDebug() << "BEFORE:" << k->totalWeight();
        k->setTotalWeight(k->totalWeight() + (weight_ - prevWeight_));
        speedKeyConfig::writeSpeedKeys(ui_->speedkeys->findChildren<speedKey*>());
        //qDebug() << "AFTER:" << k->totalWeight();
        prevWeight_ = weight_;
    }
    //qDebug() << "MainWindow::slt_WeightReached(): Done";
}

void MainWindow::slt_IncrementModeChanged( SlicerStateMachine::incrementMode_e incrementMode )
{
    this->incrementMode = incrementMode;
    //qDebug()<< "Increment mode changed to: " << this->incrementMode;
    QString icon = "";
    if(incrementMode == SlicerStateMachine::IncrementBySetpoint)
        icon = ":/images/multipleSlicesIcon";
    else if(incrementMode == SlicerStateMachine::IncrementBySingleSlice)
        icon = ":/images/singleSliceIcon";
    else if(incrementMode == SlicerStateMachine::IncrementBySetpointManual)
        icon = ":/images/multipleSlicesIconManual";
    QPixmap pixmap( icon );
    ui_->m_pIncrementModeIcon->setPixmap( pixmap );
    Q_EMIT sig_IncrementModeChanged(this->incrementMode);
}

void MainWindow::resizeEvent( QResizeEvent *e )
{
    ui_->m_pCurrentLabel->setFixedWidth( width() / 2 );
    ui_->m_pSetpointLabel->setFixedWidth( width() / 2 );

    _Mybase::resizeEvent( e );
}

void MainWindow::slt_WeigherKeyPressed(){
    Q_EMIT sig_WeigherKeyPressed();
}

bool MainWindow::eventFilter( QObject *obj, QEvent *e )
{
    if ( ui_->m_pWeightIcon == obj )
    {
        if ( QEvent::MouseButtonPress == e->type() )
        {
            lastPressed  = -1;
            ui_->m_pWeightIcon->setStyleSheet("background-color: #a0a0a4;position: relative; top:50px; left:50px");
            ui_->m_pWeightIcon->setPixmap(invertPixmap(ui_->m_pWeightIcon->pixmap()));
        }
        else if ( QEvent::MouseButtonRelease == e->type() )
        {
            ui_->m_pWeightIcon->setStyleSheet("");
            ui_->m_pWeightIcon->setPixmap(QPixmap(":/images/weightIcon"));

            if ( displayPounds == mode_ )
                mode_ = displayOunces;
            else if ( displayOunces == mode_ )
                mode_ = displayPounds;
            else if ( displayKg == mode_ )
                mode_ = displayGrams;
            else if ( displayGrams == mode_ )
                mode_ = displayKg;
            updateMode();
            updateWeight();
        }
    }

    else if ( ui_->m_pUpIcon == obj )
    {
        if ( QEvent::MouseButtonPress == e->type() )
        {
            lastPressed  = -1;
            ui_->m_pUpIcon->setIcon(QIcon(":/images/res/upArrowInverted.png"));
        }
        else if ( QEvent::MouseButtonRelease == e->type() ){
            ui_->m_pUpIcon->setIcon(QIcon(":/images/res/upArrow.png"));

        }
    }

    else if ( ui_->m_pDownIcon == obj )
    {
        if ( QEvent::MouseButtonPress == e->type() )
        {
            lastPressed  = -1;
            ui_->m_pDownIcon->setIcon(QIcon(":/images/res/downArrowInverted.png"));
        }
        else if ( QEvent::MouseButtonRelease == e->type() ){
            ui_->m_pDownIcon->setIcon(QIcon(":/images/res/downArrow.png"));

        }

    }

    else if ( ui_->m_pInstructText == obj )
    {
        if ( QEvent::MouseButtonPress == e->type() )
        {
            ui_->m_pInstructText->setStyleSheet("background-color: #a0a0a4; color: black");
        }
        else if ( QEvent::MouseButtonRelease == e->type() ){
            const QString text( ui_->m_pInstructText->text() );
            ui_->m_pInstructText->setStyleSheet("color: white");
            if ( text == tr( "Press to Zero" ))
            {
                qDebug() << "Working!";
                Q_EMIT sig_RezeroWeigher();
            }
            else if (text == tr( "Press to Clear" ))
            {
                qDebug() << "Cancel";
                qDebug() << "Good!";
                Q_EMIT sig_Cancel();
            }
            else if (text.contains("Appuyez")){
                qDebug() << "Y'all I don't think it worked.";
                Q_EMIT sig_Cancel();
            }
        }
    }

    else if ( ui_->m_pIncrementModeIcon == obj)
    {
        if ( QEvent::MouseButtonPress == e->type() )
        {
            ui_->m_pIncrementModeIcon->setStyleSheet("background-color: #A8A8A8;position: relative; top:50px; left:50px");
        }
        else if (QEvent::MouseButtonRelease == e->type() ){
            ui_->m_pIncrementModeIcon->setStyleSheet("");
            if(!isRunning_){
                Q_EMIT sig_CycleIncrementMode();
            }
        }
    }
    else if ( ui_->btn30sec == obj )
    {
        if ( QEvent::MouseButtonRelease == e->type() )
        {
            QSettings settings;
            //get current value and invert it for next time
            bool usr30secTimeoutEnabled = !(settings.value("usr30secTimeoutEnabled", false).toBool());
            settings.setValue("usr30secTimeoutEnabled", usr30secTimeoutEnabled);

            if (usr30secTimeoutEnabled)
            {
                //ui_->m_pIncrementModeIcon->setStyleSheet("");
                ui_->btn30sec->setIcon(QIcon(":/res/30SecTimeoutEnabled.png"));
            }
            else
            {
                ui_->btn30sec->setIcon(QIcon(":/res/30SecTimeoutDisabled.png"));
            }
        }
    }
    else if ( ui_->speedkeys == obj )
    {
        if ( QEvent::Resize == e->type() ){
            //qDebug() << "RESIZE________________________";
            setupSpeedKeys();
        }
    }

    return _Mybase::eventFilter( obj, e );
}

void MainWindow::onUpIconPressed(){
    Q_EMIT sig_IncrementSetpoint();
    emit sig_updateWeightTolerance( 0 );
}

void MainWindow::onDownIconPressed(){
    Q_EMIT sig_DecrementSetpoint();
    emit sig_updateWeightTolerance( 0 );
}

void MainWindow::slt_RetrieveInfinityMode(Scale::InfinityModeState infMode)
{
    setpointInfState_ = infMode;
    handleInfinityMode();
}

void MainWindow::handleInfinityMode()
{
    qDebug() << "handleInfMode: " << setpointInfState_;
    if(setpointInfState_ == Scale::noInfinityMode){
        this->ui_->m_pUpIcon->setEnabled(true);
        this->ui_->m_pUpIcon->show();
        this->ui_->m_pDownIcon->setEnabled(true);
        this->ui_->m_pDownIcon->show();

    }
    else if(setpointInfState_ == Scale::infinityFromBottomMode){
        ui_->m_pDownIcon->setIcon(QIcon(":/images/res/downArrow.png"));
        this->ui_->m_pDownIcon->setEnabled(false);
        this->ui_->m_pDownIcon->hide();
    }
    else if(setpointInfState_ == Scale::infinityFromTopMode){
        ui_->m_pUpIcon->setIcon(QIcon(":/images/res/upArrow.png"));
        this->ui_->m_pUpIcon->setEnabled(false);
        this->ui_->m_pUpIcon->hide();
    }
}

void MainWindow::updateWeight()
{
    //qDebug() << "Update Weight" << setpointWeight_;
    QString unit;
    if ( displayPounds == mode_ )
    {
        ui_->m_pSetpointLabel->setText( (QString("%1").arg( setpointWeight_, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_LBS) ))).left(m_number_chars_weight_displayed)) ;
        unit = "lb";
    }
    else if ( displayOunces == mode_ )
    {
        ui_->m_pSetpointLabel->setText( (QString("%1").arg( setpointOunces_, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_OZ) ) )).left(m_number_chars_weight_displayed) );
        unit = "oz";
    }
    else if (displayKg == mode_ ){
        ui_->m_pSetpointLabel->setText( (QString("%1").arg( setpointWeight_, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_KG) ) )).left(m_number_chars_weight_displayed) );
        unit = "kg";
    }else if (displayGrams == mode_){
        ui_->m_pSetpointLabel->setText( (QString("%1").arg( setpointWeight_ * 1000.0 , 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_G) ) )).left(m_number_chars_weight_displayed) );
        unit = "g";
    }

    ui_->m_pCurrentUnitLabel->setText( unit );
    ui_->m_pSetpointUnitLabel->setText( unit );
    ui_->m_pSetpointLabel->setStyleSheet("font: 90px; color: white");
    ui_->m_pCurrentLabel->setStyleSheet("font: 90px; color: white");

    if( setpointWeight_ == FLT_MAX){
        ui_->m_pSetpointLabel->setText("  ∞  ");
        ui_->m_pSetpointUnitLabel->setText("   ");
        ui_->m_pSetpointLabel->setStyleSheet("font: 147px; color: white; ");
    }


    switch ( scaleState_ )
    {
        case ScaleInterface::zero:
        case ScaleInterface::stable:
        //qDebug() << "weight" << weight_;
        if ( displayPounds == mode_ )
            ui_->m_pCurrentLabel->setText( (QString("%1").arg( weight_, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_LBS)) )).left(m_number_chars_weight_displayed) );
        else if ( displayOunces == mode_ )
            ui_->m_pCurrentLabel->setText( (QString("%1").arg( ounces_, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_OZ) ) )).left(m_number_chars_weight_displayed) );
        else if( displayKg == mode_ )
            ui_->m_pCurrentLabel->setText( (QString("%1").arg( weight_, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_KG) ) )).left(m_number_chars_weight_displayed) );
        else if( displayGrams == mode_ )
            ui_->m_pCurrentLabel->setText( (QString("%1").arg( weight_ * 1000.0, 0, 'f', qMin(m_digitsPrecision, int(MAX_PRECISION_G) ) )).left(m_number_chars_weight_displayed) );
        break;

        case ScaleInterface::unstable:
        ui_->m_pCurrentLabel->setText( "" );
        break;

        case ScaleInterface::underWeight:
        ui_->m_pCurrentLabel->setText( tr("under") );
        break;

        case ScaleInterface::overWeight:
        ui_->m_pCurrentLabel->setText( tr("over") );
        break;

        case ScaleInterface::error:
        default:
        ui_->m_pCurrentLabel->setText( tr("ERR") );
        break;
    }
}

void MainWindow::slt_ScrollText()
{
    if ( scrollTexts_.isEmpty() )
    return;

    const int i( ++currentTextScrollIndex_ % scrollTexts_.count() );
    ui_->m_pInstructText->setText( scrollTexts_[i] );
}

void MainWindow::slt_HandleFlash()
{


    QString styleSheet( "QWidget #centralWidget{font: 80px; background-color: rgb(0, 0, 0); color: rgb(255, 255, 255);}" );

    // set background
    if ( 0 == (flashTimes_ % 2) )
    {
        if ( greenFlashing == flashType_ )
        styleSheet = "QWidget #centralWidget{font: 80px; background-color: rgb(0, 255, 0); color: rgb(255, 255, 255);}";
        else if ( redFlashing == flashType_ )
        styleSheet = "QWidget #centralWidget{font: 80px; background-color: rgb(255, 0, 0); color: rgb(255, 255, 255);}";
    }

    ui_->centralWidget->setStyleSheet( styleSheet );

    // check disable flashing
    if ( 6 == ++flashTimes_ )
    {
        flashTimes_ = 0;
        flashingTimer_.stop();
    }
}

void MainWindow::slt_DisplayMessage(QString header, QString msg, QString lastCompleted, int uniqueId)
{
    AlertMessagePopup d(this);
    d.setHeader(header);
    d.setMessage(msg);
    d.setLastCompleted(lastCompleted);
    d.setUniqueId(uniqueId);
    d.setModal(true);
    connect(&d, SIGNAL(sig_messageAccepted(int)), mBox, SLOT(slt_messageAccepted(int)));
    d.exec();
}

void MainWindow::slt_homepage()
{
    //check will show false
    //qDebug()<<"Home Page";

    ui_->btnHome->hide();
    ui_->btnMenu->show();
}

void MainWindow::slt_menupage()
{
    //check will show false
    //qDebug()<<"Menu page";
    lastPressed  = -1;
    qDebug() << lastPressed;
    mainMenu d(this);
    connect(&d, SIGNAL(sig_MessageUpdated()), mBox, SLOT(slt_MessageUpdated()));

    //Listen for signals from menu
    connect(&d, SIGNAL(sig_UpdateLanguage(QString)), this, SLOT(slt_UpdateLanguage(QString)));

    int execReturn = d.exec();
    if ( QDialog::Accepted == execReturn )
    {
        setupSpeedKeys();
        emit sig_CheckUpdateSlicerType();
        //check and update the number of decimals shown.
        checkUpdateWeigherModel();
    }
    if ( QDialog::Rejected == execReturn){
        //check and update the number of decimals shown.
        checkUpdateWeigherModel();
    }
}

void MainWindow::setLabel(double iLabel)
{
    //qDebug()<<__FUNCTION__<<iLabel;
    if ( displayPounds == mode_ )
    {
        setpointWeight_ = iLabel;
        ui_->m_pSetpointLabel->setText( QString("%1").arg( iLabel, 0, 'f', 2 ) );
    }
    else if ( displayOunces == mode_ )
    {
        setpointOunces_ = iLabel;
        ui_->m_pSetpointLabel->setText( QString("%1").arg( iLabel, 0, 'f', 2 ) );
    }
}

void MainWindow::slt_RezeroWeigher()
{
    Q_EMIT sig_RezeroWeigher();
}

void MainWindow::slt_WeigherStatus(char *buffer, int buffLen)
{
    Q_EMIT sig_WeigherStatus(buffer, buffLen);
}

void MainWindow::slt_SetWeigherMode(WeighingMode mode)
{
    Q_EMIT sig_SetWeigherMode(mode);
}

void MainWindow::slt_WeigherReset()
{
    Q_EMIT sig_WeigherReset();
}

void MainWindow::slt_ReqConfig(){
    Q_EMIT sig_ReqConfig();
}

void MainWindow::slt_WeigherConfigChanged(WeigherConfiguration & config)
{
    if((config.getFlags() & WeighMode::WeighMode_Metric)){
        qDebug() << "MainWindow::slt_WeigherConfigChanged() Metric Mode";
        mode_ = displayKg;
        speedKeyConfig::setMode(WeighMode::WeighMode_Metric);
        updateWeight();
        updateMode();
    }else if (!(config.getFlags() & WeighMode::WeighMode_Metric)){
        qDebug() << "MainWindow::slt_WeigherConfigChanged() Avoir Mode";
        mode_ = displayPounds;
        speedKeyConfig::setMode(WeighMode::WeighMode_Avoir);
        updateWeight();
        updateMode();
    }else{
        //qDebug() << "apparently  we don't have a mode: " << config.getScaleWeighMode();
    }
    Q_EMIT sig_WeigherConfigChanged(config); //Emit signal to [NOT FINAL] mainmenu
    setupSpeedKeys();
    this->repaint();
}

//Change system language from General Config
void MainWindow::slt_UpdateLanguage(QString language){
    QTranslator *translator(NULL);
    translator = new QTranslator;

    if(language == "English"){
        translator->load(":/languages/lang_en_ca.qm");
        qApp->installTranslator(translator);
    }else if(language == "French"){
        translator->load(":/languages/lang_fr_ca.qm");
        qApp->installTranslator(translator);
    }

    qDebug() << "Language set to: " << language;
    ui_->retranslateUi(this);

    //Sending a cancel signal makes sure language doesn't get changed mid-slicing,
    //and also refreshes the whole slicer for translation purposes
    Q_EMIT sig_Cancel();
}

void MainWindow::slt_WriteConfig(WeigherConfiguration & config){
    Q_EMIT sig_WriteConfig(config); //Propagating signal from mainmenu
}

void MainWindow::slt_HandleSpeedKey()
{
    if(isRunning_)
        return;
    speedKey *key( qobject_cast<speedKey*>( sender() ) );

    if ( !key )
    return;
    for(int i = 0; i < getSpeedKeys().length(); i++){
        if(getSpeedKeys().at(i) == key){
            lastPressed = i;
            break;
        }
    }
    //qDebug() << key;
    //qDebug() << lastPressed;

    switch ( key->type() )
    {
        case speedKeyConfig::None:
        break;

        case speedKeyConfig::PresetWeight:
        {
            const int weightType( key->val1().toInt() );
            const double weightTarget( key->val2().toDouble() );
            const double weightTolerance( key->val3().toDouble() );
            //qDebug() << "WeightTolerance: " << weightTolerance;
            //qDebug() << "WeightType: " << weightType;
            //qDebug() << "WeightTarget" << weightTarget;
            // lbs
            if ( 0 == weightType )
            {
                mode_ = displayPounds;
                emit sig_updateWeightTolerance( weightTolerance );
                emit sig_updateSetPoint( weightTarget );
            }

            // oz
            else if ( 1 == weightType )
            {
                mode_ = displayOunces;
                emit sig_updateWeightTolerance( weightTolerance / OUNCE_RATIO);
                emit sig_updateSetPoint( (weightTarget / OUNCE_RATIO) );//- 0.005 );
            }
            // kg
            else if ( 2 == weightType )
            {
                mode_ = displayKg;
                emit sig_updateWeightTolerance( weightTolerance);
                emit sig_updateSetPoint( weightTarget );
            }
            // g
            else if ( 3 == weightType )
            {
                mode_ = displayGrams;
                emit sig_updateWeightTolerance( weightTolerance / 1000.0 );
                emit sig_updateSetPoint( weightTarget / 1000.0 );//- 0.005 );
                //qDebug() << "setpoint " << (weightTarget ) / 1000.0;
            }
            updateMode();
            break;
        }
        case speedKeyConfig::InfinityMode:
            //qDebug()<<"Infinity Speed Key Pressed" << FLT_MAX;
            emit sig_updateSetPoint (FLT_MAX);
            break;

        default:
        break;
    }
}

void MainWindow::initialize()
{
    QSettings settings;

    //Set language on startup
    slt_UpdateLanguage(settings.value("slicerLanguage").toString());

    ui_->m_pWeightIcon->installEventFilter( this );
    ui_->m_pUpIcon->installEventFilter( this );
    ui_->m_pDownIcon->installEventFilter( this );

    ui_->m_pInstructText->installEventFilter( this );
    ui_->m_pIncrementModeIcon->installEventFilter( this );

    ui_->speedkeys->installEventFilter( this );

    ui_->btn30sec->installEventFilter( this );

    ui_->btnHome->hide();

    connect(ui_->btnHome,SIGNAL(clicked(bool)),this,SLOT(slt_homepage()));
    connect(ui_->btnMenu,SIGNAL(clicked(bool)),this,SLOT(slt_menupage()));

    QPixmap menuLogo( (":/images/menu-logo"));
    QPixmap convertedMenuLogo(menuLogo.size());
    convertedMenuLogo.fill(Qt::white);
    convertedMenuLogo.setMask(menuLogo.createMaskFromColor(Qt::transparent));
    //convertedMenuLogo.scaled(ui_->btnMenu->size().width(), ui_->btnMenu->size().height());
    ui_->btnMenu->setIcon(convertedMenuLogo);

    bool usr30secTimeoutEnabled = (settings.value("usr30secTimeoutEnabled", true).toBool());
    if (usr30secTimeoutEnabled)
    {
        //ui_->m_pIncrementModeIcon->setStyleSheet("");
        ui_->btn30sec->setIcon(QIcon(":/res/30SecTimeoutEnabled.png"));
    }
    else
    {
        ui_->btn30sec->setIcon(QIcon(":/res/30SecTimeoutDisabled.png"));
    }

    checkUpdateWeigherModel();
    ui_->btn30sec->show();

}

void MainWindow::setupSpeedKeys()
{
    const QList<speedKey*> doomed( ui_->speedkeys->findChildren<speedKey*>() );

    foreach ( speedKey *k, doomed )
    k->deleteLater();

    const QSize s( speedKeyConfig::speedKeyMenuSize() );
    const speedKeyConfig::speedKeyList keys( speedKeyConfig::readSpeedKeys() );

    foreach ( const QStringList &conf, keys )
    {
        speedKey *key( new speedKey( ui_->speedkeys ) );
        key->create( s, conf );
        key->show();

        connect(key, SIGNAL(clicked()), SLOT(slt_HandleSpeedKey()));
    }
}

void MainWindow::checkUpdateWeigherModel()
{
    QSettings settings;
    int division = settings.value("divSize").toInt();
    settings.sync();
    //1 = 0.01 lb
    //2 = 0.002 kg
    //5 = 0.005 lb/kg
    if(division == 1){
        m_digitsPrecision = 2;
    }
    else if(division == 2 || division == 5){
        m_digitsPrecision = 3;
    }
    updateWeight(); //to reset the mainwindow
}

const QList<speedKey *> MainWindow::getSpeedKeys(){
    const QList<speedKey*> keys( ui_->speedkeys->findChildren<speedKey*>() );
    return keys;
}

const QList<message *> MainWindow::getMessages(){
    //qDebug() << "mBox:" << mBox->getMessageList();
    return mBox->getMessageList();
}

QPixmap MainWindow::invertPixmap(const QPixmap * src){
    QImage temp(src->toImage());
    temp.invertPixels();
    QPixmap inverted;
    inverted.convertFromImage(temp);
    return inverted;
}

void MainWindow::updateMode(){
    QSettings settings;
    Q_EMIT sig_ModeChanged((int)mode_);
    if(mode_ == displayOunces || mode_ == displayPounds){
        settings.setValue("MaximumWeight",10.0);
        Q_EMIT sig_IncrementStepChanged(0.125);
    }else if( mode_ == displayKg ){
        settings.setValue("MaximumWeight", 6);
        Q_EMIT sig_IncrementStepChanged(0.05);
    }else if ( mode_ == displayGrams){
        settings.setValue("MaximumWeight", 6);
        Q_EMIT sig_IncrementStepChanged(0.01);
    }
}

//Check for any change in input (touchscreen or mouse)
void MainWindow::slt_CheckInputType(){
    QFileInfo touchscreen_file("dev/input/touchscreenusb");
    QFileInfo service_mouse_file1("dev/input/mouse0");
    QFileInfo service_mouse_file2("dev/input/mouse1");
    int previousInputType = inputType_;

    if(service_mouse_file1.exists() && service_mouse_file2.exists()){
        inputType_ = mouse;
    } else if(touchscreen_file.exists()){
        inputType_ = touch;
    } else {
        inputType_ = none;
    }

    //If input has changed, do something
    //If mouse, show cursor
    //If touch, hide cursor
    //If none, send alert message
    if(inputType_ != previousInputType){
        switch( inputType_ )
        {
        case mouse:
            qDebug() << "Found mouse";
            QApplication::setOverrideCursor( QCursor( Qt::ArrowCursor ) );
            break;

        case touch:
            qDebug() << "Found touch";
            QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
            break;

        case none:
        {
            /*QPalette p( palette() );
            p.setBrush( backgroundRole(), Qt::white );
            p.setBrush( foregroundRole(), Qt::black );

            PopupMessage * popup = new PopupMessage(this);
            popup->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
            popup->setAttribute(Qt::WA_DeleteOnClose, true);
            popup->setHeader(tr("Touchscreen not detected"));

            popup->setMessage(tr("Touchscreen not detected. Please contact your local Hobart-authorized service office."));
                                // "\n\nL'ecran tactile non detecte. Contactez\nle bureau de service Hobart autorise local."));
            QFont f(popup->font());
            f.setPointSize(18);
            popup->setPalette(p);
            popup->setFont(f);
            popup->repaint();
            popup->setModal(true);
            popup->show();*/
            break;
        }

        default:
            break;
        }
    }
}
