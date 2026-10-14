

/* @file serviceNewWrapperTestLabelApplier.cpp
 * serviceNewWrapperTestLabelApplier
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper's label applier
 */

#include "serviceNewWrapperTestLabelApplier.h"

#include "ui_serviceNewWrapperTestLabelApplier.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestLabelApplier::serviceNewWrapperTestLabelApplier(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestLabelApplier),
    slideOutTimer_( NULL  ),
    slideHomeTimer_( NULL )
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Setup Signals and Slots
    connect(ui->btnWandUp, SIGNAL( clicked()), this, SLOT(wandUp()));
    connect(ui->btnWandDown, SIGNAL( clicked()), this, SLOT(wandDown()));
    connect(ui->btnApplierUp, SIGNAL( clicked()), this, SLOT(applierUp()));
    connect(ui->btnApplierDown, SIGNAL( clicked()), this, SLOT(applierDown()));
    connect(ui->btnWandVacuumOn, SIGNAL( clicked()), this, SLOT(vacuumOn()));
    connect(ui->btnWandVacuumOff, SIGNAL( clicked()), this, SLOT(vacuumOff()));
    connect(ui->btnPrintLabel, SIGNAL( clicked()), this, SLOT(print()));
    connect(ui->btnPrintApplyLabel, SIGNAL( clicked()), this, SLOT(printAndApply()));

    connect(ui->cboxLabelApplier, SIGNAL( currentIndexChanged(int)), this, SLOT(labelApplierType(int)));
    connect(ui->cboxLabelRotation, SIGNAL( currentIndexChanged(int)), this, SLOT(updateLabelRotation()));

    connect(ui->btn2ndApplierSlideOut,  SIGNAL( clicked()), this, SLOT(slideApplierOut()));
    connect(ui->btn2ndApplierSlideHome, SIGNAL( clicked()), this, SLOT(slideApplierHome()));




    //Update and Initialize the Page
    initialize();
    translate();

    //Get info from label applier
    session_->send(LabelApplier, LaSndLabelPos, "0");

    //Send requests for wrapper sys info, inquire, and sensors msgs
    session_->send(Wrapper, WrReqInquire);              // 9,21
    session_->send(Wrapper, WrReqEvolSystemInfo);       // 9,143
    session_->send(Wrapper, WrReqSensors);              // 9,23
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::initialize()
{
    ui->btn2ndApplierSlideHome->setEnabled( false );

    //Clear out all the sensor states
    setSensorState(ui->lblRotationHomeSensor, false);
    setSensorState(ui->lblPackageSensor, false);

    //Create cboxes
    ui->cboxLabelApplier->clear();
    ui->cboxLabelRotation->clear();

    ui->cboxLabelApplier->addItem( tr("Primary") );
    ui->cboxLabelApplier->addItem( tr("Secondary") );

    ui->cboxLabelRotation->addItem("0");
    //ui->cboxLabelRotation->addItem("45");
    ui->cboxLabelRotation->addItem("90");
    //ui->cboxLabelRotation->addItem("135");
    ui->cboxLabelRotation->addItem("180");
    //ui->cboxLabelRotation->addItem("225");
    ui->cboxLabelRotation->addItem("270");
    //ui->cboxLabelRotation->addItem("315");

    //Setup slidersTimer_
    slideHomeTimer_ = new QTimer(this);
    slideOutTimer_ = new QTimer(this);
    slideHomeTimer_->setInterval(2500);
    slideOutTimer_->setInterval(2500);

    connect( slideHomeTimer_, SIGNAL( timeout() ), this, SLOT( onSlideHomeTimeout() ) );
    connect( slideOutTimer_, SIGNAL( timeout() ), this, SLOT( onSlideOutTimeout() ) );

    setLabelApplierType(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestLabelApplier::~serviceNewWrapperTestLabelApplier()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::setSensorState(QLabel* label, bool open)
{
    static const QString greenStyle( "background-color:rgb(0,255,0);color: rgb(0,0,0);" );
    static const QString redStyle( "background-color:rgb(255,0,0);color: rgb(0,0,0);" );

    if( ( ui->lbl2ndApplierSlideHome == label ) && open )
    {
        // when we return home, we turn green, and we disable the Home button and enable OUT
        slideOutTimer_->start(2500);
        slideOutTimer_->setSingleShot(true);
    }
    else if( ( ui->lbl2ndApplierSlideHome == label ) && ( open == false ) )
    {
        slideHomeTimer_->start(2500);
        slideHomeTimer_->setSingleShot(true);
    }

    label->setStyleSheet( open ? greenStyle : redStyle );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::translate()
{
    //Translate
    ui->retranslateUi(this);

    //Refresh cboxes
    ui->cboxLabelApplier->setItemText( 0, tr("Primary") );
    ui->cboxLabelApplier->setItemText( 1, tr("Secondary") );

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::setLabelApplierType(int labelApplierType)
{
    if( labelApplierType == 0 )//Primary Label Applier
    {
        isPrimaryLabelApplier = true;
        ui->cboxLabelApplier->blockSignals(true);
        ui->cboxLabelApplier->setCurrentIndex(labelApplierType);
        ui->cboxLabelApplier->blockSignals(false);

        ui->lblProductNumber_14->setVisible( false );
        ui->lbl2ndApplierSlideHome->setVisible( false );
        ui->btn2ndApplierSlideOut->setVisible( false );     // btn2ndApplierSlideOut
        ui->btn2ndApplierSlideHome->setVisible( false );    // btn2ndApplierSlideHome
    }
    else if( labelApplierType == 1 )//Secondary Label Applier
    {
        isPrimaryLabelApplier = false;
        ui->cboxLabelApplier->blockSignals(true);
        ui->cboxLabelApplier->setCurrentIndex(labelApplierType);
        ui->cboxLabelApplier->blockSignals(false);

        ui->lblProductNumber_14->setVisible( true );
        ui->lbl2ndApplierSlideHome->setVisible( true );
        ui->btn2ndApplierSlideOut->setVisible( true );
        ui->btn2ndApplierSlideHome->setVisible( true );
    }
    else
    {
        //Handle Error Case here
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::enableRotation()
{
    ui->cboxLabelRotation->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::disableRotation()
{
    ui->cboxLabelRotation->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::enableLabelApplierType()
{
    ui->cboxLabelApplier->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::disableLabelApplierType()
{
    ui->cboxLabelApplier->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::wandUp()
{
    if(isPrimaryLabelApplier)
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandUp, WrPrimaryApplierTest, WAND_UP, -1);          // 20
    }
    else
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandUp, WrSecondaryApplierTest, WAND_UP, -1);        // 27
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::wandDown()
{
    if(isPrimaryLabelApplier)
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandDown, WrPrimaryApplierTest, WAND_DOWN, -1);        // 20
    }
    else
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandDown, WrSecondaryApplierTest, WAND_DOWN, -1);      // 27
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::slideApplierOut()
{
    if(isPrimaryLabelApplier)
    {
        // always a secondary label applier test
        return;
    }
    else
    {
        ui->btn2ndApplierSlideOut->setEnabled( false );
        ui->btn2ndApplierSlideHome->setEnabled( false );
        deviceTest->sendWrapperTest(session_, ui->btn2ndApplierSlideOut, WrSecondaryApplierTest, CYCLE_2ND_APPLIER_MOTOR_OUT, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::slideApplierHome()
{
    if(isPrimaryLabelApplier)
    {
        // always a secondary label applier test
        return;
    }
    else
    {
        ui->btn2ndApplierSlideOut->setEnabled( false );
        ui->btn2ndApplierSlideHome->setEnabled( false );
        deviceTest->sendWrapperTest(session_, ui->btn2ndApplierSlideHome, WrSecondaryApplierTest, CYCLE_2ND_APPLIER_MOTOR_IN, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::applierUp()
{
    if(isPrimaryLabelApplier)
    {
        deviceTest->sendWrapperTest(session_, ui->btnApplierUp, WrPrimaryApplierTest, 2, -1);
    }
    else
    {
        deviceTest->sendWrapperTest(session_, ui->btnApplierUp, WrSecondaryApplierTest, 2, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::applierDown()
{
    if(isPrimaryLabelApplier)
    {
        deviceTest->sendWrapperTest(session_, ui->btnApplierDown, WrPrimaryApplierTest, 3, -1);
    }
    else
    {
        deviceTest->sendWrapperTest(session_, ui->btnApplierDown, WrSecondaryApplierTest, 3, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::vacuumOn()
{
    if(isPrimaryLabelApplier)
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandVacuumOn, WrPrimaryApplierTest, 4, -1);
    }
    else
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandVacuumOn, WrSecondaryApplierTest, 4, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::vacuumOff()
{
    if(isPrimaryLabelApplier)
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandVacuumOff, WrPrimaryApplierTest, 5, -1);
    }
    else
    {
        deviceTest->sendWrapperTest(session_, ui->btnWandVacuumOff, WrSecondaryApplierTest, 5, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::onSlideHomeTimeout()
{
    ui->btn2ndApplierSlideHome->setEnabled( true );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::onSlideOutTimeout()
{
     ui->btn2ndApplierSlideOut->setEnabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::labelApplierType(int index)
{
    if(index == 0)//Primary Label Applier
    {
        isPrimaryLabelApplier = true;
        enableRotation();

        ui->lblProductNumber_14->setVisible( false );
        ui->lbl2ndApplierSlideHome->setVisible( false );
        ui->btn2ndApplierSlideOut->setVisible( false );
        ui->btn2ndApplierSlideHome->setVisible( false );
    }
    else if(index == 1)//Secondary Label Applier
    {
        isPrimaryLabelApplier = false;
        
	    //SOF-5163 Allow Secondary too
        enableRotation();

        ui->lblProductNumber_14->setVisible( true );
        ui->lbl2ndApplierSlideHome->setVisible( true );
        ui->btn2ndApplierSlideOut->setVisible( true );
        ui->btn2ndApplierSlideHome->setVisible( true );
    }

    // If the label applier type changes, then clear the sensor fields so any new changes will be reflected
    ui->lblApplierVacuum->setText( "" );
    ui->lblWandUpEvent->setText( "" );
    ui->lblWandDownEvent->setText( "" );
    ui->lblApplierHomeEvent->setText( "" );
    ui->lbl2ndApplierSlideHome->setText( "" );

    session_->send(Wrapper, WrReqSensors);              // 9,23 - request sensor update NOW instead of waiting 30s for it
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::updateLabelRotation()
{
    QString my_msg = "";//SOF-5163 Set Pri and Sec with current Rotation setting
    
    my_msg = ui->cboxLabelRotation->currentText();//Pri rot
    my_msg+=",,"+ui->cboxLabelRotation->currentText();//Pri loc, Sec rot
    
    session_->send(LabelApplier, LaSndLabelPos, my_msg);		//	HA-90: We need to send the rotation.
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::print()
{
    QString msg = "";
    int printNum = 0;
    int prCheckerBoard = 2;
    int misc = 0;
    if(!isPrimaryLabelApplier)
    {
        printNum = 1;
    }
    //Add printerNum, checkBoardLabel, and misc to the message
    msg+=QString::number(printNum);
    msg+=","+QString::number(prCheckerBoard);
    msg+=","+QString::number(misc);

    session_->send(Printer, PrTestLabel, msg);//Printer PrTestLabel, printerNum, PrCheckerboard,  ?
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::printAndApply()
{
    print();
    if(isPrimaryLabelApplier)
    {
        //Wrapper, WrEvolTest, WrPrimaryApplierTest, LACycle
        deviceTest->sendWrapperTest(session_,ui->btnPrintApplyLabel, WrPrimaryApplierTest, 6, -1);
    }
    else
    {
        //Wrapper, WrEvolTest, WrSecondaryApplierTest, LACycle
        deviceTest->sendWrapperTest(session_,ui->btnPrintApplyLabel, WrSecondaryApplierTest, 6, -1);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestLabelApplier::onSessionMessage( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    if (( Wrapper == device ) && ( WrEvolSensors == event ))                    // 9,115
    {
        // we just asked for and received the wrapper's sensor message so update the GUI
        DomEvolWrapperSensors m( msg );
        // SOF-3838  second label applier has fixed manual rotation 
        setSensorState( ui->lblPackageSensor, m.transferPkg() );
        if( isPrimaryLabelApplier )
        {
            // Set Sensor States
            setSensorState( ui->lblRotationHomeSensor, m.labelRotationHome() );
            setSensorState( ui->lblWandUpSensor, m.wandUp() );
            setSensorState( ui->lblWandDownSensor, m.wandDown() );
            setSensorState( ui->lblApplierHomeSensor, m.applierHand() );         
            setSensorState( ui->lbl2ndApplierSlideHome, m.secLabelApplierSlideHome() );     // decided to always show this if secondary active or not
        }
        else 
        {
            setSensorState( ui->lblRotationHomeSensor, m.secLabelRotationHome() );
            setSensorState( ui->lblWandUpSensor, m.secWandUp() );
            setSensorState( ui->lblWandDownSensor, m.secWandDown() );
            setSensorState( ui->lblApplierHomeSensor, m.secApplierHand() );         
            setSensorState( ui->lbl2ndApplierSlideHome, m.secLabelApplierSlideHome() );
        }
    }
    else if (( Wrapper == device ) && ( WrEvolInquire == event ))               // 9,56
    {
        //Save parsing of comma delimited message in case it ever changes
        DomEvolWrapperConfig m( msg );

        if ( m.hardwareAddOn() )//Second Printer/Applier present
            enableLabelApplierType();
        else
            disableLabelApplierType();
    }
    else if (( Wrapper == device ) && ( WrEvolSystemInfo == event ))            // 9,144
    {
        DomEvolWrapperSystemInformation m( msg );

        //Disable the Lower Brakes and SelectorPosition buttons if we have an EWS Wrapper
        if ( m.isManualLabelApplier() )
            disableRotation();
        else
            enableRotation();
    }
    else if (( Wrapper == device ) && ( WrEvolTestComplete == event ))          // 9,165
    {
        DomEvolWrapperTestComplete m( msg );

        deviceTest->testComplete(m.testNum().toInt());
    }

    else if (( Wrapper == device ) && ( WrEvolEventCapture == event ))          // 9,180
    {
        DomEvolWrapperEventCapture m( msg );
// 18,27 once each
        if ( ( DomEvolWrapperEventCapture::ApplierVacuumReadingEvent == m.captureType() ) ||
             ( DomEvolWrapperEventCapture::SecondApplierVacuumReadingEvent == m.captureType() ) )
            ui->lblApplierVacuum->setText( QString::number( m.value() ) );

// 12,24 - 2x 1=(454,454), 2x 2=(454,455)
        else if ( ( DomEvolWrapperEventCapture::WandUpEvent == m.captureType() ) ||
                ( DomEvolWrapperEventCapture::SecondWandUpEvent == m.captureType() ) )
            ui->lblWandUpEvent->setText( QString::number( m.value() ) );

// 13,25 - 2x 1=(704,754)       2x 1=(704,705)
        else if ( ( DomEvolWrapperEventCapture::WandDownEvent == m.captureType() ) ||
                ( DomEvolWrapperEventCapture::SecondWandDownEvent == m.captureType() ) )
            ui->lblWandDownEvent->setText( QString::number( m.value() ) );

//19,27   4x 1=54,180,54,180    4x 2=77,227,77,228
        else if ( ( DomEvolWrapperEventCapture::ApplierHandHomeEvent == m.captureType() ) ||
                  ( DomEvolWrapperEventCapture::SecondApplierHandHomeEvent == m.captureType() ) )
            ui->lblApplierHomeEvent->setText( QString::number( m.value() ) );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestLabelApplier::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
