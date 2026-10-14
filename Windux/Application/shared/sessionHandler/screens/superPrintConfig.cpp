/* @file superPrintConfig.cpp
 * SuperPrintConfig 
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Joe E. Crew
 */

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"

#include "../modes/supervisormode.h"
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "superPrintConfig.h"
#include "ui_superPrintConfig.h"

#define READY_PRINT 0x01
#define CANCEL_PRINT 0x02
#define STOP_PRINT  0x03


/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

superPrintConfig::superPrintConfig(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::superPrintConfig)
{
    ui->setupUi(this);
    scaleScreen();

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),SLOT(timerSlot()));
    connect(ui->butCancel,SIGNAL(clicked()), this, SLOT(butCancel_clicked()));
    state = READY_PRINT;
    ui->lblReportStatus->setText(tr("Preparing configuration report for printing..."));
    ui->butCancel->setEnabled(true);
    update();
    timer->start(4000);
    translate();
}

superPrintConfig::~superPrintConfig()
{
    delete ui;
}

void superPrintConfig::butCancel_clicked()
{

    session_->send(ReportManagerDev, CancelReport);
    state = CANCEL_PRINT;
    timer->stop();
    timerSlot();
}

void superPrintConfig::timerSlot()
{
    switch(state)
    {
        case READY_PRINT:
          //
          state = STOP_PRINT;
          ui->progressBar->show();
          session_->send(ReportManagerDev, StartReport, "1,0");
          ui->lblReportStatus->setText(tr("Printing report."));
          timer->start(120000);
          break;
        case CANCEL_PRINT:
          ui->lblReportStatus->setText(tr("Report printing cancelled."));
          ui->progressBar->hide();
          ui->butCancel->hide();
          break;
        case STOP_PRINT:
          timer->stop();
          ui->butCancel->hide();
          ui->progressBar->hide();
          ui->lblReportStatus->setText(tr("Finished printing report."));
          break;
    }
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool superPrintConfig::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    /*
    title = tr( "Product Defaults Page One" );

    description = tr(
    "This option allows some text.\n"
    "\n"
    "more text.\n"
    "\n"
    "last line.\n"
    "\n" );
	*/
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superPrintConfig::translate()
{
    ui->butCancel->setText(tr("Cancel"));
    ui->lblReportStatus->setText(tr("Preparing configuration report for printing..."));
    ui->lblStaticReportHeader->setText(tr("Configuration Report"));
    update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void superPrintConfig::onSessionMessage( const SessionMessage& msg )
{
    if (ReportManagerDev == msg[0].toInt() && TerminateReport == msg[1].toInt())
    {
        state = STOP_PRINT;
        timerSlot();
    }
}
