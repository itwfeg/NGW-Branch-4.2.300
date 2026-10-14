/**
 * @file scaleStatistics.cpp
 *
 * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "scaleStatistics.h"
#include "ui_scaleStatistics.h"

#include <ScaleConfiguration.h>


scaleStatistics::scaleStatistics(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent),
    ui(new Ui::scaleStatistics)

{
    ui->setupUi(this);
    scaleScreen();

    //----------------------------------
    // Request the scale statistics data
    //----------------------------------
    session_->send( SessionDev, SnReadStatistics );

    translate();
}

scaleStatistics::~scaleStatistics()
{
    delete ui;
}

void scaleStatistics::translate()
{
    ui->retranslateUi(this);

    update();
}

void scaleStatistics::onSessionMessage( const SessionMessage& msg )
{
    // Scale Statistics
    if ((SessionDev == msg[0].toInt() ) && ( SnWriteStatistics == msg[1].toInt() ))
    {
        DomStatistics m( msg );

        // Update UI with the Scale Statistics data
        ui->lblUsedFlashData->setText(m.flashMemoryUsed() );
        ui->lblUsedRAMData->setText( m.ramMemoryUsed() );
        ui->lblFreeFlashData->setText( m.flashMemoryFree() );
        ui->lblFreeRAMData->setText( m.ramMemoryFree() );

        ui->lblProductsData->setText( m.productCount() );
        ui->lblClassesData->setText( m.classCount() );
        ui->lblExpandedTextData->setText( m.expandedTextCount() );
        ui->lblSpecialMessagesData->setText( m.specialMessageCount() );
        ui->lblNutrifactsData->setText( m.nutrifactCount() );
        ui->lblProductNotesData->setText( m.productNoteCount() );
        ui->lblLabelTypesData->setText( m.labelTypeCount() );
        ui->lblGraphicsData->setText( m.graphicsCount() );
        ui->lblOperatorIDsData->setText( m.operatorCount() );
        ui->lblCuttingTestsData->setText( m.cuttingTestCount() );
        ui->lblTransactionsData->setText( m.transactionCount() );
        ui->lblFlashkeysData->setText( m.flashkeyCount() );
    }
}
