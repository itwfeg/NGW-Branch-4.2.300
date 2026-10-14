/**
 * @file serviceWeigherPowerupTestResults.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceWeigherPowerupTestResults.h"
#include "ui_serviceWeigherPowerupTestResults.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"

#include "../sessionmainwindow.h"


serviceWeigherPowerupTestResults::serviceWeigherPowerupTestResults(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceWeigherPowerupTestResults)
{
    ui->setupUi(this);
    scaleScreen();

    translate();

    ui->lblField1Data->setText("0.0.0");
    ui->lblField2Data->setText( "0.0" );
}

serviceWeigherPowerupTestResults::~serviceWeigherPowerupTestResults()
{
    delete ui;
}

void serviceWeigherPowerupTestResults::translate()
{
    //------------------------------------------------
    // Request the Weigher info in translate() method,
    // labels and text are determined by the messages
    //------------------------------------------------
    session_->send( WeigherSystem, SysReqStatus );

    ui->retranslateUi(this);

    update();
}

void serviceWeigherPowerupTestResults::onSessionMessage( const SessionMessage& msg )
{
    MadpPeripheralList *periphSettings = &session_->scaleConfig->peripheralList;
    QString weigherKind = QString::number( periphSettings->firstWeigher() );


    if ((WeigherSystem == msg[0].toInt() ) && ( SysUsbWgStatus == msg[1].toInt() ))
    {
        // SOF-671:  Check if scale has Valuemax
        DomWeigherConfigRead *weigherFields = &session_->scaleConfig->weigherConfigRead;
        bool scaleHasValuemax = weigherFields->isScaleValuemax();

        //------------------------------------
        // USB Weigher - WeigherKind = 2, 3, 4
        //------------------------------------

        DomUsbWeigherPowerupTests m( msg );

        ui->lblField1->setText(tr( "Software Version" ) );
        ui->lblField1Data->setText( m.softwareVersion() );

        ui->lblField2->setText(tr( "Hardware Version" ) );
        ui->lblField2Data->setText( m.hardwareVersion() );

        ui->lblField3->setText( ( scaleHasValuemax ) ? tr( "System Serial Flash (Value Max Enabled)" ) : tr( "System Eeprom Test" ) );
        ui->lblField3Data->setText( (m.systemEepromError() == "0" ? tr( "Passed") : tr( "Failed" ) ) );

        ui->lblField4->setText( ( scaleHasValuemax ) ? tr( "Weigher Serial Flash (Value Max Enabled)" ) : tr( "Weigher Eeprom Test" ) );
        ui->lblField4Data->setText( (m.weigherEepromError() == "0" ? tr( "Passed") : tr( "Failed" ) ) );
    }


    if ((WeigherSystem == msg[0].toInt() ) && ( WgSysStatus == msg[1].toInt() ))
    {
        //---------------------------------
        // Ultima Weigher - WeigherKind = 1
        //---------------------------------

        DomUltimaWeigherPowerupTests m( msg );

        ui->lblField1->setText(tr( "Calculated checksum" ) );
        ui->lblField1Data->setText( m.calculatedChecksum() );

        ui->lblField2->setText(tr( "Stored checksum" ) );
        ui->lblField2Data->setText( m.storedChecksum() );

        ui->lblField3->setText(tr( "EEP checksum test" ) );
        ui->lblField3Data->setText( (m.EEPChecksum() == "0" ? tr( "Passed") : tr( "Failed" ) ) );

        ui->lblField4->setText(tr( "Word test" ) );
        ui->lblField4Data->setText( (m.wordTest() == "0" ? tr( "Passed") : tr( "Failed" ) ) );

        ui->lblField5->setText(tr( "Address test" ) );
        ui->lblField5Data->setText( (m.addressTest() == "0" ? tr( "Passed") : tr( "Failed" ) ) );
    }

    if(weigherKind != "1" && weigherKind != "2" && weigherKind != "3" && weigherKind != "4" && weigherKind != "5")
    {
        ui->lblField1->setText(tr( "No Weigher Attached" ) );
    }
}
