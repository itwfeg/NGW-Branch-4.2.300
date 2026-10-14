/**
 * @file ticketprinter.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef TICKETPRINTER_H
#define TICKETPRINTER_H

#include <QObject>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract ticket printer class.
class ticketPrinter : public QObject
{
    Q_OBJECT

    typedef ticketPrinter _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~ticketPrinter() {}

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
    virtual QString errorString() const = 0;

    /// Check if printer is ready to print.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    virtual bool isReadyPrint() const = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Print ticket.
    /**
     * @param[in] logo  logo file
     * @param[in] customerNumber  customer number
     * @return  @c true on success, @c false otherwise
     */
    virtual bool print( const QString& logo, int customerNumber ) = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    ticketPrinter( QObject *parent = 0 ) : _Mybase( parent ), lastError_( 0 ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set last error.
    /**
     * @param[in] value  error number
     */
    virtual void setError( int value ) {lastError_ = value;}

private:

    int lastError_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TICKETPRINTER_H
