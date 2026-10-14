/**
 * @file asuraticketprinter.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ASURATICKETPRINTER_H
#define ASURATICKETPRINTER_H

// only compile when ASURA CPRNT
#ifdef ASURA_CPRNT

#include "ticketprinter.h"

namespace StarMicronics_QAsura {
class QAPrinter;
}

class QImage;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Ticket printer for the Star AsuraCPRNT.
class asuraTicketPrinter : public ticketPrinter
{
    Q_OBJECT

    typedef asuraTicketPrinter _Myt;
    typedef ticketPrinter _Mybase;

public:

    /// Error codes.
    enum errorCodes
    {
        // internal error codes
        ERROR_INITIALIZING_PRINTER = -100,
        ERROR_CLAIMING_PRINTER = -101,
        ERROR_PRINTING_IMAGE_TO_PRINTER = -102,
        ERROR_RELEASING_PRINTER = -103,
        ERROR_RETRIEVING_PRINTER_STATUS = -104,

        // printer status errors
        PRINTER_OFFLINE = -200,
        PRINTER_OUT_OF_PAPER = -201,
        PRINTER_PAPER_JAM = -202,
        PRINTER_COVER_OPEN = -203,
        PRINTER_STATUS_ERROR = -204
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    asuraTicketPrinter( QObject *parent = 0 );

    /// Destructor.
    virtual ~asuraTicketPrinter();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve last error.
    /**
     * @return  error number
     */
    virtual int error() const {return lastError_;}

    /// Retrieve text string of last error.
    /**
     * @return  error test string
     */
    virtual QString errorString() const {return lastErrorString_;}

    /// Check if printer is ready to print.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    virtual bool isReadyPrint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Print ticket.
    /**
     * @param[in] logo  logo file
     * @param[in] customerNumber  customer number
     * @return  @c true on success, @c false otherwise
     */
    virtual bool print( const QString& logo, int customerNumber );

private:

    StarMicronics_QAsura::QAPrinter *printer_;

    int lastError_;
    QString lastErrorString_;

    /// Initialize.
    void initialize();

    /// Check printer status.
    bool checkPrinterStatus();

    /// Claim printer for exclusive use.
    bool claimPrinter();

    /// Print image to printer.
    bool printImage( const QImage& ticket );

    /// Release claim on printer.
    bool releasePrinter();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ASURA_CPRNT
#endif // ASURATICKETPRINTER_H
