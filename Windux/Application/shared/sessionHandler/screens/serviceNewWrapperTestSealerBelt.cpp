

/* @file serviceNewWrapperTestSealerBelt.cpp
 * serviceNewWrapperTestSealerBelt
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Alex Zavala

   @Brief This file contains all the functions necessary for testing the wrapper's sealer belt
 */

#include "serviceNewWrapperTestSealerBelt.h"

#include "ui_serviceNewWrapperTestSealerBelt.h"

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
serviceNewWrapperTestSealerBelt::serviceNewWrapperTestSealerBelt(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceNewWrapperTestSealerBelt)
{
    ui->setupUi(this);
    scaleScreen();

    //Setup deviceTest
    deviceTest = new deviceTests();

    //Update and Initialize the Page
    initialize();
    translate();

    //Setup Signals and Slots
    connect(ui->btnCycleSealerBelt, SIGNAL( clicked()), this, SLOT(cycleSealerBelt()));
    connect(ui->cboxSealerBeltPower, SIGNAL( currentIndexChanged(int)), this, SLOT(sealerBeltPower(int)));

    //Request a wrapper status and inquire msg
    session_->send(Wrapper, WrReqStatus);
    session_->send(Wrapper, WrReqInquire);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::initialize()
{
    ui->lblBeltSettingTemperature->setText("");
    ui->lblBeltTemperature->setText("");

    //Create cbox
    ui->cboxSealerBeltPower->clear();

    ui->cboxSealerBeltPower->addItem(tr("Off"));
    ui->cboxSealerBeltPower->addItem(tr("On"));

    scaleConfigData = &session_->scaleConfig->scaleConfig;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString serviceNewWrapperTestSealerBelt::temperatureScale() const
{
    bool isCelsius( false );
    if( scaleConfigData->isScaleMetric() )
        isCelsius = true;

    return (isCelsius ? QString::fromUtf8( "°C" ) : QString::fromUtf8( "°F" ));
}

QString serviceNewWrapperTestSealerBelt::temperatureValue(int val) const
{
    QString returnVal = QString::number(val);

    if( scaleConfigData->isScaleMetric() )
        returnVal = QString::number( (val - 32) * 5/9 );

    return (returnVal);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperTestSealerBelt::~serviceNewWrapperTestSealerBelt()
{
    delete deviceTest;
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestSealerBelt::eventFilter( QObject *obj, QEvent *event )
{
    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::translate()
{
    //Translate
    ui->retranslateUi(this);

    //Refresh cbox
    ui->cboxSealerBeltPower->setItemText(0, tr("Off"));
    ui->cboxSealerBeltPower->setItemText(1, tr("On"));

    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestSealerBelt::onScreenChanging()
{
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::cycleSealerBelt()
{
    deviceTest->sendWrapperTest(session_, ui->btnCycleSealerBelt, WrSealerTest, -1, -1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::sealerBeltPower(int index)
{
    //Wrapper, WrEvolControl, SealerBelt, Off/On
    session_->send(Wrapper,WrEvolControl,"8," + QString::number(index));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::setBeltPower(int power)
{
    ui->cboxSealerBeltPower->blockSignals(true);
    ui->cboxSealerBeltPower->setCurrentIndex(power);
    ui->cboxSealerBeltPower->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::setBeltSetting(int setting)
{
    ui->lblBeltSettingTemperature->setText( temperatureValue(setting) );
    ui->lblBeltSettingTemperatureScale->setText( temperatureScale() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::setBeltTemperature(int temp)
{
    ui->lblBeltTemperature->setText( QString::number(temp) );
    ui->lblBeltTemperatureScale->setText( temperatureScale() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperTestSealerBelt::onSessionMessage( const SessionMessage& msg )
{
    if(Wrapper == msg[0].toInt() && WrEvolInquire == msg[1].toInt())
    {
        // we just asked for and received the wrapper's inquire message so update the GUI
        DomEvolWrapperConfig m(msg);

        //Update the belt setting
        setBeltSetting(m.sealerBeltSetting().toInt());

        //Turn on status tracking
        session_->send(Wrapper, WrStatusTracking, "1");//Wrapper, WrStatusTracking, On
    }
    else if(Wrapper == msg[0].toInt() && WrEvolStatus == msg[1].toInt())
    {
        DomEvolWrapperStatus m(msg);

        //Update belt power and temp
        setBeltPower(m.sealerBeltPower().toInt());
        setBeltTemperature(m.sealerBeltTemp().toInt());
    }
    else if(Wrapper == msg[0].toInt() && WrEvolTestComplete == msg[1].toInt())
    {
        DomEvolWrapperTestComplete m(msg);

        deviceTest->testComplete(m.testNum().toInt());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceNewWrapperTestSealerBelt::isErrorMessageHandler( const SessionMessage& errorMsg ) const
{
    return _Mybase::isErrorMessageHandler( errorMsg );
}
