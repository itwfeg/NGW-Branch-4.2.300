/**
 * @file serviceSecurityLabels.h
 * @brief ...describe...
 *
 * @copyright Copyright (c) 2015, ITW/Hobart, Food Equipment Group, U.S.A.
 * @Author: Clare Dickey
 * @issues:
 * @{
*/

#include <logmanager.h>

#include <QtGui>
#include <QtCore>

#include "../sessionmainwindow.h"
#include "../modes/supervisormode.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include "TokenParser.h"

#include "serviceSecurityLabels.h"
#include "ui_serviceSecurityLabels.h"

static const int MAX_PRINTERS = 3;

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceSecurityLabels::serviceSecurityLabels(SessionMainWindow *mainWindow, ModeWidget *parent) :
    _Mybase( mainWindow, parent ),
    ui(new Ui::serviceSecurityLabels),
    activePrinter_( 0 )
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "serviceSecurityLabels" );

    ui->setupUi(this);
    scaleScreen();

    initialize();
    translate();

    // request printer configuration
	session_->send( Printer, PrReqInquire, QString::number( activePrinter_ ) );

    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceSecurityLabels::~serviceSecurityLabels()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "~serviceSecurityLabels" );
    delete ui;
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::initialize()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "initialize" );

    // request printer settings
    if ( 0 < MAX_PRINTERS )
        session_->send( Printer, PrReqInquire, QString::number( activePrinter_ ) );

    ui->le_lineOffset->setValidator(lineOffsetValidator_ = new QIntValidator(0, 999, this));
    ui->le_lineOffset->setDecimalPlaces(0);
    ui->le_lineOffset->setInputWrapping(true);

    // create all of the signals and slots
    // text fields
    connect(ui->butTest,        SIGNAL( clicked() ),         this, SLOT( onTestButtonClicked() ) );
    connect(ui->le_lineOffset,  SIGNAL( editingFinished() ), this, SLOT( lineOffsetEditingFinished() ) );

    update();
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::translate()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "translate" );
	ui->retranslateUi( this );

    update();
    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceSecurityLabels::helpText( QString& title, QString& description ) const
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "helpText" );
    Q_UNUSED( title );
    Q_UNUSED( description );

    LOG_TRACE( "Done", NULL );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool serviceSecurityLabels::onScreenChanging()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "onScreenChanging" );

    // save every time b\c we dont know what will happen when
    saveSecurityLabelOffset();

    LOG_TRACE( "Done", NULL );
    // base method
    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                    PRIVATE METHODS                                               //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::saveSecurityLabelOffset()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "saveSecurityLabelOffset" );

    LOG_TRACE( "saving...", NULL );

    QString param;

    param = activePrinter_;
    param += "," + printerConfig_.printContrastRaw();
    param += "," + printerConfig_.vertPrintPositionRaw();
    param += "," + printerConfig_.horizPrintPositionRaw();
    param += ",\"" + printerConfig_.labelWidth() + "\"";
    param += "," + printerConfig_.expelPositionRaw();
    param += "," + ui->le_lineOffset->text();

    session_->send( Printer, PrConfig, param);

    LOG_TRACE( "Done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::displaySLALineOffset()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "displaySLALineOffset" );

    // sanity check
    if (( 0 <= activePrinter_ ) && ( activePrinter_ < MAX_PRINTERS ))
	{
        LOG_TRACE( "slaEjectLineRaw = [%s]", qPrintable(printerConfig_.slaEjectLineRaw()) );
	    ui->le_lineOffset->blockSignals(true);
	    ui->le_lineOffset->setText( printerConfig_.slaEjectLineRaw() );
	    ui->le_lineOffset->blockSignals(false);
	}
	else
	{
	    LOG_TRACE( "Problem!!! activePrinter_ = %d", activePrinter_ );
        return;
	}

    LOG_TRACE( "Done", NULL );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "onSessionMessage" );

    // printer inquire
    if (( Printer == msg[0].toInt() ) && ( PrInquire == msg[1].toInt() ))
    {
        DomPrinterConfigRead m( msg );
        int type( m.printerType() );

        if (( 0 <= type ) && ( type < MAX_PRINTERS ))
        {
            LOG_DEBUG( "updating config...", NULL );

            printerConfig_ = m;

            displaySLALineOffset();
        }
		else
		{
		    LOG_TRACE( "Problem!!! printer = %d,  activePrinter_ = %d.", type, activePrinter_ );
	        return;
		}
    }
    LOG_TRACE( "Done", NULL );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                  //
//                                   SLOT METHODS                                                   //
//                                                                                                  //
//////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::lineOffsetEditingFinished()		//(QString unitPrice)
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "lineOffsetEditingFinished" );

    LOG_TRACE( "Line offset = %d.", qPrintable(ui->le_lineOffset->text()) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceSecurityLabels::onTestButtonClicked()
{
    LOG_CLASS_INFO( "serviceSecurityLabels", "onTestButtonClicked" );

    saveSecurityLabelOffset();

    QString param;
    param = activePrinter_;
    param += ",4,0,1";	//	PrVerticalLines, PrOneTestLabel, applySecurityLabel
    session_->send( Printer, PrTestLabel, param);

    LOG_TRACE( "Done", NULL );
}
