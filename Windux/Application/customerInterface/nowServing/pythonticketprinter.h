/**
 * @file pythonticketprinter.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PYTHONTICKETPRINTER_H
#define PYTHONTICKETPRINTER_H

#include "ticketprinter.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Ticket printer that just invokes python script.
class pythonTicketPrinter : public ticketPrinter
{
    Q_OBJECT

    typedef pythonTicketPrinter _Myt;
    typedef ticketPrinter _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    pythonTicketPrinter( QObject *parent = 0 );

    /// Destructor.
    virtual ~pythonTicketPrinter();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve text string of last error.
    /**
     * @return  error test string
     */
    virtual QString errorString() const;

    /// Check if printer is ready to print.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    virtual bool isReadyPrint() const;

    // ========================================================================
    // Static Properties
    // ========================================================================

    /// Check if printer installed.
    /**
     * @return  @c true if installed, @c false otherwise
     */
    static bool isInstalled();

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

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PYTHONTICKETPRINTER_H
