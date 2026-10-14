/**
 * @file serviceWeigherInfo.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceWeigherInfo.h"
#include "ui_serviceWeigherInfo.h"

#include <ScaleConfiguration.h>
#include "TokenParser.h"
#include "screenwidget.h"
#include <UiUtilities.h>

#include "../sessionmainwindow.h"

serviceWeigherInfo::serviceWeigherInfo(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::serviceWeigherInfo),
    valuemaxIsEnabled_( false )
{
    ui->setupUi(this);
    scaleScreen();

    hideDegreesSection();

    //-------------------------
    // Request the Weigher info
    //-------------------------
    session_->send( Weigher, WgReqInquire );

    translate();
}

serviceWeigherInfo::~serviceWeigherInfo()
{
    TurnOffInfoCountsMessage();

    delete ui;
}

void serviceWeigherInfo::translate()
{
    ui->retranslateUi(this);

    update();
}

void serviceWeigherInfo::onSessionMessage( const SessionMessage& msg )
{
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    QString weigherAttached = settings->weigherAttached();

    //----------------------------------
    // Weigher Configuration Information
    //----------------------------------
    if ((Weigher == msg[0].toInt() ) && ( WgInquire == msg[1].toInt() ))
    {
        DomWeigherConfigRead m( msg );

        if(weigherAttached == "1")
        {
            ui->lblCalibratedNumberOfTimesData->setText( m.timesCalibratedRaw() );
            ui->lblCalibratedLastTimeData->setText( m.lastDateCalibratedRaw() );

            ui->lblConfiguredNumberOfTimesData->setText( m.timesConfiguredRaw() );
            ui->lblConfiguredLastTimeData->setText( m.lastDateConfiguredRaw());
        }
        else
        {
            // Clear all of the data fields

            ui->lblZeroedCountsData->clear();
            ui->lblZeroedWeightData->clear();
            ui->lblNonZeroedCountsData->clear();
            ui->lblCalibratedNumberOfTimesData->clear();
            ui->lblCalibratedLastTimeData->clear();
            ui->lblConfiguredNumberOfTimesData->clear();
            ui->lblConfiguredLastTimeData->clear();
        }

        valuemaxIsEnabled_       = m.isScaleValuemax();

        //------------------------------------------------------------
        // Send message to start the repeating Weigher Counts messages
        //------------------------------------------------------------
        session_->send( Weigher, WgMode, "1" );

        //--------------------------------
        // Request the Weigher Status info
        //--------------------------------
        session_->send( Weigher, WgReqStatus );
    }

    //------------------------------------------
    // Weigher Configuration Counts Information
    //
    // Note:    This message keeps repeating
    //          until it is turned off by the
    //          "1,7,3" message.
    //------------------------------------------
    if ((Weigher == msg[0].toInt() ) && ( WgStatusCounts == msg[1].toInt() ))
    {
        DomWeigherInfoCountsRead m( msg );

        if(weigherAttached == "1")
        {
            ui->lblZeroedCountsData->setText( m.zeroes() );
            ui->lblNonZeroedCountsData->setText( m.nonZeroes() );

            QString zeroedWeight = m.weight();

            if(zeroedWeight == "" || zeroedWeight == " ")
            {
                ui->lblZeroedWeightData->clear();
            }
            else
            {
                // First need to set the correct Unit separator
                session_->uiUtilities->youGottaSetTheSeparator( zeroedWeight );

                ui->lblZeroedWeightData->setText( zeroedWeight + (settings->isScaleMetric() ? " kg" : " lb") );
            }

            if( valuemaxIsEnabled_ )
            {
                QString tiltX;
                QString tiltY;

                tiltX.sprintf( "%.2f", m.tiltX().toDouble() / 100.0 );
                tiltY.sprintf( "%.2f", m.tiltY().toDouble() / 100.0 );

                ui->lblTiltXData->setText( tiltX );
                ui->lblTiltYData->setText( tiltY );

                // For testing ONLY
                //if( tiltX.toDouble() > 1.001 || tiltY.toDouble() > 1.001 )
                //{
                //    session_->send( Error, ErrWeigherDevice, "1021, 5" );
                //}
                //else if( tiltX.toDouble() < -1.001 || tiltY.toDouble() < -1.001 )
                //{
                //    session_->send( Error, ErrWeigherDevice, "1021, 5" );
                //}
            }
            else
            {
                ui->lblTiltXData->setText( "--" );
                ui->lblTiltYData->setText( "--" );
            }
        }
    }
}

void serviceWeigherInfo::TurnOffInfoCountsMessage() const
{
    //-----------------------------------------------
    // Turn off the repeating Weigher Counts messages
    //-----------------------------------------------
    session_->send( Weigher, WgMode, "3" );
}

// SOF-3153
void serviceWeigherInfo::hideDegreesSection() const
{
    MadpScaleConfig *settings( &session_->scaleConfig->scaleConfig );
    const QString scaleModel( settings->scaleName() );

    if( ( scaleModel.size() >= 3 && !( scaleModel.toUpper().startsWith( "HTM" ) ) ) &&
        ( scaleModel.size() >= 2 && !( scaleModel.toUpper().startsWith( "FS" ) ) ) &&   // Free Standing Self-Serve
        ( scaleModel.size() >= 2 && !( scaleModel.toUpper().startsWith( "KS" ) ) ) )    // Kiosk
    {
        // Hide "Degrees" section if not an "HTm" scale model
        ui->lblDegrees->hide();
        ui->lblTiltX->hide();
        ui->lblTiltY->hide();
        ui->lblTiltXData->hide();
        ui->lblTiltYData->hide();
    }
}
