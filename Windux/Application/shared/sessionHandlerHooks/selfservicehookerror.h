/**
 * @file selfservicehook.h
 * Self service hook error.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup selfServeCsu
 * @{
 */

#ifndef SELFSERVICEHOOKERROR_H
#define SELFSERVICEHOOKERROR_H

#include "selfservicehook.h"

#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service hook error.
class selfServiceHookError : public QWidget
{
    Q_OBJECT

    typedef selfServiceHookError _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * @brief  Emitted when dialog button pressed.
     * @param[in] role  button role
     */
    void dialogButtonPress( int role );

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /**
     * @brief CTOR.
     * @param[in] title  dialog title
     * @param[in] message  dialog message
     * @param[in] buttons  dialog buttons
     * @param[in] parent  parent widget
     */
    selfServiceHookError( const QString& title, const QString& message, const selfServiceHook::buttonMap& buttons, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceHookError();

private:

    // not implemented
    selfServiceHookError( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SELFSERVICEHOOKERROR_H
