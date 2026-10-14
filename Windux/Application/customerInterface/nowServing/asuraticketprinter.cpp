/**
 * @file asuraticketprinter.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

// only compile when ASURA CPRNT
#ifdef ASURA_CPRNT

#include "asuraticketprinter.h"
#include "fontutil.h"
#include "pixmaputil.h"

#include "starmicronics/printerstatus.h"
#include "starmicronics/qaexception.h"
#include "starmicronics/qaprinter.h"

#include <QDateTime>
#include <QImage>
#include <QPainter>
#include <QTransform>

using namespace StarMicronics_QAsura;

///////////////////////////////////////////////////////////////////////////////////////////////////
asuraTicketPrinter::asuraTicketPrinter( QObject *parent ) :
    _Mybase( parent ),
    lastError_( 0 )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
asuraTicketPrinter::~asuraTicketPrinter()
{
    if ( printer_ )
        delete printer_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool asuraTicketPrinter::isReadyPrint() const
{
    return ( const_cast<_Myt*>( this )->checkPrinterStatus() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool asuraTicketPrinter::print( const QString& logoFilename, int customerNumber )
{
    bool result( false );

    // claim printer
    if ( claimPrinter() )
    {
        // from looking at the examples here the printer appears to be 192 dpi.
        static const int dpi( 192 );
        static const int receiptWidth( 3 * dpi );

        // receipt format (3"):
        // logo (1")
        // number label (0.5")
        // number (1")
        // date/time (0.25")

        QImage canvas( receiptWidth, 3 * dpi, QImage::Format_RGB32 );
        canvas.fill( Qt::white );

        QPainter painter( &canvas );
        painter.setRenderHint( QPainter::Antialiasing, true );

        QFont f( painter.font() );
        int offset( 0 );

        // render logo
        QSize logoSize( receiptWidth, dpi );
        QPixmap logo( pixmapUtil::scalePixmap( QPixmap( logoFilename ), logoSize, Qt::KeepAspectRatio ) );

        painter.drawPixmap( 0, offset, logo );
        offset += logoSize.height();

        // render number label
        QSize numberLabelSize( receiptWidth, dpi / 2 );
        QString numberLabel( tr( "Customer Number" ) );

        f.setPointSize( fontUtil::calcFontPointSize( f, numberLabelSize, numberLabel ) );
        painter.setFont( f );

        painter.drawText( 0, offset, numberLabelSize.width(), numberLabelSize.height(), Qt::AlignCenter, numberLabel );
        offset += numberLabelSize.height();

        // render number
        QSize numberSize( receiptWidth, dpi );
        QString number( QString::number( customerNumber ) );

        f.setPointSize( fontUtil::calcFontPointSize( f, numberSize, number ) );
        painter.setFont( f );

        painter.drawText( 0, offset, numberSize.width(), numberSize.height(), Qt::AlignCenter, number );
        offset += numberSize.height();

        // render date/time
        QDateTime now( QDateTime::currentDateTime() );

        QSize timestampSize( receiptWidth, dpi / 4  );
        QString timestamp( now.toString( "dddd MMM d h:mm:ss ap" ) );

        f.setPointSize( fontUtil::calcFontPointSize( f, timestampSize, timestamp ) );
        painter.setFont( f );

        painter.drawText( 0, offset, timestampSize.width(), timestampSize.height(), Qt::AlignCenter, timestamp );
        offset += timestampSize.height();

        // rotate image so number is face up to customer instead of upside down
        QTransform rotated;
        rotated.rotate( 180 );

        // print!
        result = printImage( canvas.transformed( rotated ) );

        // release printer
        result &= releasePrinter();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void asuraTicketPrinter::initialize()
{
    try
    {
        printer_ = new QAPrinter();
    }
    catch ( QAException& ex )
    {
        lastError_ = ex.getErrorCode();
        lastErrorString_ = ex.toString().trimmed();

        if ( lastErrorString_.isEmpty() )
            lastErrorString_ = tr( "Error initializing printer." );

        printer_ = NULL;
    }
    catch ( ... )
    {
        lastError_ = ERROR_INITIALIZING_PRINTER;
        lastErrorString_ = tr( "Error initializing printer." );
        printer_ = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool asuraTicketPrinter::checkPrinterStatus()
{
    bool result( false );

    if ( printer_ )
    {
        try
        {
            QAPrinterStatus stat( printer_->Status() );

            // check for errors we can recover from
            if ( stat.offline )
            {
                lastError_ = PRINTER_OFFLINE;
                lastErrorString_ = tr( "Printer is offline." );
            }
            else if ( stat.receiptPaperEmpty )
            {
                lastError_ = PRINTER_OUT_OF_PAPER;
                lastErrorString_ = tr( "Printer is out of paper." );
            }
            else if ( stat.receiptPaperJam )
            {
                lastError_ = PRINTER_PAPER_JAM;
                lastErrorString_ = tr( "Printer receipt paper jam." );
            }
            else if ( stat.coverOpen )
            {
                lastError_ = PRINTER_COVER_OPEN;
                lastErrorString_ = tr( "Printer cover is open." );
            }
            else
            {
                // check for errors we cannot recover from
                int statusCodes( 0 );

                statusCodes <<= 1;
                statusCodes |= (stat.blackMarkError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.cutterError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.headThermistorError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.headUpError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.mechError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.pageModeCmdError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.unrecoverableError ? 1 : 0);
                statusCodes <<= 1;
                statusCodes |= (stat.voltageError ? 1 : 0);

                if ( !statusCodes )
                    result = true;
                else
                {
                    lastError_ = PRINTER_STATUS_ERROR;
                    lastErrorString_ = tr( "Printer has error status: " ) + QString::number( statusCodes );
                }
            }
        }
        catch ( QAException& ex )
        {
            lastError_ = ex.getErrorCode();
            lastErrorString_ = ex.toString().trimmed();

            if ( lastErrorString_.isEmpty() )
                lastErrorString_ = tr( "Error retrieving printer status." );
        }
        catch ( ... )
        {
            lastError_ = ERROR_RETRIEVING_PRINTER_STATUS;
            lastErrorString_ = tr( "Error retrieving printer status." );
        }
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
bool asuraTicketPrinter::claimPrinter()
{
    bool result( false );

    if ( printer_ )
    {
        try
        {
            printer_->Claim();
            result = true;
        }
        catch ( QAException& ex )
        {
            lastError_ = ex.getErrorCode();
            lastErrorString_ = ex.toString().trimmed();

            if ( lastErrorString_.isEmpty() )
                lastErrorString_ = tr( "Error claiming printer." );
        }
        catch ( ... )
        {
            lastError_ = ERROR_CLAIMING_PRINTER;
            lastErrorString_ = tr( "Error claiming printer." );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool asuraTicketPrinter::printImage( const QImage& ticket )
{
    bool result( false );

    if ( printer_ )
    {
        try
        {
            QImage job( ticket );

            printer_->PrintQImage( job );
            printer_->Cut( true );

            result = true;
        }
        catch ( QAException& ex )
        {
            lastError_ = ex.getErrorCode();
            lastErrorString_ = ex.toString().trimmed();

            if ( lastErrorString_.isEmpty() )
                lastErrorString_ = tr( "Error sending image to printer." );
        }
        catch ( ... )
        {
            lastError_ = ERROR_PRINTING_IMAGE_TO_PRINTER;
            lastErrorString_ = tr( "Error sending image to printer." );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool asuraTicketPrinter::releasePrinter()
{
    bool result( false );

    if ( printer_ )
    {
        try
        {
            printer_->Release();
            result = true;
        }
        catch ( QAException& ex )
        {
            lastError_ = ex.getErrorCode();
            lastErrorString_ = ex.toString().trimmed();

            if ( lastErrorString_.isEmpty() )
                lastErrorString_ = tr( "Error releasing printer." );
        }
        catch ( ... )
        {
            lastError_ = ERROR_RELEASING_PRINTER;
            lastErrorString_ = tr( "Error releasing printer." );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ASURA_CPRNT
