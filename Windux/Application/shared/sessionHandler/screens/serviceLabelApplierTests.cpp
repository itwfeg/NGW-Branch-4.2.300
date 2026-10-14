

/* @file serviceLabelApplierTests.cpp
 * serviceLabelApplierTests
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the standalone label applier
 */

#include "serviceLabelApplierTests.h"

#include "ui_serviceLabelApplierTests.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QtCore>

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////
serviceLabelApplierTests::serviceLabelApplierTests(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceLabelApplierTests)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Setup Signals and Slots
    connect(ui->btnMotorCycle, SIGNAL( clicked()), this, SLOT(cycleMotor()));
    connect(ui->btnBeltsUp, SIGNAL( clicked()), this, SLOT(beltsUp()));
    connect(ui->btnBeltsDown, SIGNAL( clicked()), this, SLOT(beltsDown()));
    connect(ui->btnWandUp, SIGNAL( clicked()), this, SLOT(wandUp()));
    connect(ui->btnWandDown, SIGNAL( clicked()), this, SLOT(wandDown()));
    connect(ui->btnApplierUp, SIGNAL( clicked()), this, SLOT(applierUp()));
    connect(ui->btnApplierDown, SIGNAL( clicked()), this, SLOT(applierDown()));
    connect(ui->btnGateUp, SIGNAL( clicked()), this, SLOT(gateUp()));
    connect(ui->btnGateDown, SIGNAL( clicked()), this, SLOT(gateDown()));
    connect(ui->btnCompressorOn, SIGNAL( clicked()), this, SLOT(compressorOn()));
    connect(ui->btnCompressorOff, SIGNAL( clicked()), this, SLOT(compressorOff()));
    connect(ui->btnWandVacuumOn, SIGNAL( clicked()), this, SLOT(vacuumOn()));
    connect(ui->btnWandVacuumOff, SIGNAL( clicked()), this, SLOT(vacuumOff()));
    connect(ui->btnCLARunInOn, SIGNAL( clicked()), this, SLOT(runInOn()));
    connect(ui->btnCLARunInOff, SIGNAL( clicked()), this, SLOT(runInOff()));
    connect(ui->cboxLabelApplier, SIGNAL( currentIndexChanged(int)), this, SLOT(labelApplierType(int)));

    //Update and Initialize the Page
    initialize();
    translate();

    //Set applier to service mode and request config
    session_->send(LabelApplier, LaMode, "5");
    session_->send(LabelApplier, LaReqInquire);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::initialize()
{
    isLabelApplier = true;
    inMfgRunIn = false;

    //Create cbox
    ui->cboxLabelApplier->clear();

    ui->cboxLabelApplier->addItem( tr("Primary Label Applier") );
    ui->cboxLabelApplier->addItem( tr("Weigher Conveyor") );
    ui->cboxLabelApplier->addItem( tr("Secondary Label Applier") );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceLabelApplierTests::~serviceLabelApplierTests()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierTests::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::translate()
{
    //Translate
    ui->retranslateUi(this);

    //Refresh cbox
    ui->cboxLabelApplier->setItemText(0, tr("Primary Label Applier"));
    ui->cboxLabelApplier->setItemText(1, tr("Weigher Conveyor"));
    ui->cboxLabelApplier->setItemText(2, tr("Secondary Label Applier"));


    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierTests::onScreenChanging()
{
    //If a test is running, cancel it
    deviceTest->cancelCurrentTest();

    //Turn mfg. off run in if it was running
    if(inMfgRunIn)
    {
        runInOff();
        inMfgRunIn = false;
    }

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::cycleMotor()
{
    if(isLabelApplier)
    {
        deviceTest->sendLabelApplierTest(session_, ui->btnMotorCycle, LaTestCyclePrinterMotor, getCurrentStation());
    }
    else
    {
        deviceTest->sendLabelApplierTest(session_, ui->btnMotorCycle, LaTestCycleWeigherMotor, WeigherConveyor);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::beltsUp()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnBeltsUp, LaTestBeltsUp, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::beltsDown()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnBeltsDown, LaTestBeltsDown, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::wandUp()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnWandUp, LaTestWandUp, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::wandDown()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnWandDown, LaTestWandDown, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::applierUp()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnApplierUp, LaTestApplierUp, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::applierDown()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnApplierDown, LaTestApplierDown, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::gateUp()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnGateUp, LaTestGateUp, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::gateDown()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnGateDown, LaTestGateDown, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::compressorOn()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnCompressorOn, LaTestCompressorOn, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::compressorOff()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnCompressorOff, LaTestCompressorOff, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::vacuumOn()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnWandVacuumOn, LaTestWandVacuumOn, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::vacuumOff()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnWandVacuumOff, LaTestWandVacuumOff, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::runInOn()
{
    inMfgRunIn = true;
    deviceTest->sendLabelApplierTest(session_, ui->btnCLARunInOn, LaTestRunInOn, getCurrentStation());
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::runInOff()
{
    deviceTest->sendLabelApplierTest(session_, ui->btnCLARunInOff, LaTestRunInOff, getCurrentStation());
    inMfgRunIn = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::loadLabelApplierTests()
{
    isLabelApplier = true;
    showLabelApplierTestLabels();
    showLabelApplierTestButtons();
    enableLabelApplierTestButtons();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::loadWeigherConveyorTests()
{
    isLabelApplier = false;
    hideLabelApplierTestLabels();
    hideLabelApplierTestButtons();
    disableLabelApplierTestButtons();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::labelApplierType(int index)
{
    if( index == 0 || index == 2 )//Label Applier
    {
        loadLabelApplierTests();
    }
    else if( index == 1 )//Weigher Conveyor
    {
        loadWeigherConveyorTests();
    }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::onSessionMessage( const SessionMessage& msg )
{
    if(LabelApplier == msg[0].toInt() && LaTestComplete == msg[1].toInt())
    {
        DomLabelApplierTestComplete m(msg);

        deviceTest->testComplete(m.testNum().toInt());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceLabelApplierTests::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::showLabelApplierTestButtons()
{
    ui->btnBeltsUp->setVisible(true);
    ui->btnBeltsDown->setVisible(true);
    ui->btnWandUp->setVisible(true);
    ui->btnWandDown->setVisible(true);
    ui->btnApplierUp->setVisible(true);
    ui->btnApplierDown->setVisible(true);
    ui->btnGateUp->setVisible(true);
    ui->btnGateDown->setVisible(true);
    ui->btnCompressorOn->setVisible(true);
    ui->btnCompressorOff->setVisible(true);
    ui->btnWandVacuumOn->setVisible(true);
    ui->btnWandVacuumOff->setVisible(true);
    ui->btnCLARunInOn->setVisible(true);
    ui->btnCLARunInOff->setVisible(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::hideLabelApplierTestButtons()
{
    ui->btnBeltsUp->setVisible(false);
    ui->btnBeltsDown->setVisible(false);
    ui->btnWandUp->setVisible(false);
    ui->btnWandDown->setVisible(false);
    ui->btnApplierUp->setVisible(false);
    ui->btnApplierDown->setVisible(false);
    ui->btnGateUp->setVisible(false);
    ui->btnGateDown->setVisible(false);
    ui->btnCompressorOn->setVisible(false);
    ui->btnCompressorOff->setVisible(false);
    ui->btnWandVacuumOn->setVisible(false);
    ui->btnWandVacuumOff->setVisible(false);
    ui->btnCLARunInOn->setVisible(false);
    ui->btnCLARunInOff->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::showLabelApplierTestLabels()
{
    ui->lblBelts->setVisible(true);
    ui->lblWand->setVisible(true);
    ui->lblApplier->setVisible(true);
    ui->lblGate->setVisible(true);
    ui->lblCompressor->setVisible(true);
    ui->lblWandVacuum->setVisible(true);
    ui->lblCLARunIn->setVisible(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::hideLabelApplierTestLabels()
{
    ui->lblBelts->setVisible(false);
    ui->lblWand->setVisible(false);
    ui->lblApplier->setVisible(false);
    ui->lblGate->setVisible(false);
    ui->lblCompressor->setVisible(false);
    ui->lblWandVacuum->setVisible(false);
    ui->lblCLARunIn->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::disableLabelApplierTestButtons()
{
    ui->btnBeltsUp->setEnabled(false);
    ui->btnBeltsDown->setEnabled(false);
    ui->btnWandUp->setEnabled(false);
    ui->btnWandDown->setEnabled(false);
    ui->btnApplierUp->setEnabled(false);
    ui->btnApplierDown->setEnabled(false);
    ui->btnGateUp->setEnabled(false);
    ui->btnGateDown->setEnabled(false);
    ui->btnCompressorOn->setEnabled(false);
    ui->btnCompressorOff->setEnabled(false);
    ui->btnWandVacuumOn->setEnabled(false);
    ui->btnWandVacuumOff->setEnabled(false);
    ui->btnCLARunInOn->setEnabled(false);
    ui->btnCLARunInOff->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceLabelApplierTests::enableLabelApplierTestButtons()
{
    ui->btnBeltsUp->setEnabled(true);
    ui->btnBeltsDown->setEnabled(true);
    ui->btnWandUp->setEnabled(true);
    ui->btnWandDown->setEnabled(true);
    ui->btnApplierUp->setEnabled(true);
    ui->btnApplierDown->setEnabled(true);
    ui->btnGateUp->setEnabled(true);
    ui->btnGateDown->setEnabled(true);
    ui->btnCompressorOn->setEnabled(true);
    ui->btnCompressorOff->setEnabled(true);
    ui->btnWandVacuumOn->setEnabled(true);
    ui->btnWandVacuumOff->setEnabled(true);
    ui->btnCLARunInOn->setEnabled(true);
    ui->btnCLARunInOff->setEnabled(true);
}

int serviceLabelApplierTests::getCurrentStation()
{
    int station = PrimaryLabelApplier;
    if(ui->cboxLabelApplier->currentIndex() == 2)
    {
        station = SecondaryLabelApplier;
    }

    return station;
}















