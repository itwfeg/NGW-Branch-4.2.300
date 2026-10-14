/**
 * @file selfservicecamerapredictions.h
 * Self service hook camera predictions.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SELFSERVICECAMERAPREDICTIONS_H
#define SELFSERVICECAMERAPREDICTIONS_H

#include <QWidget>

class selfServiceHook;

class DomLoadSoftkeyGroup;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service hook camera predictions.
class selfServiceCameraPredictions : public QWidget
{
    Q_OBJECT

    typedef selfServiceCameraPredictions _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /// Emitted when dialog button pressed.
    /**
     * @param[in] role  button role
     */
    void dialogButtonPress( int role );

    /// Emitted when product search requested.
    void productSearchRequested();

    /// Emitted when softkey clicked.
    /**
     * @param[in] w  pointer to widget emitting action
     * @param[in] b  pointer to button clicked
     * @param[in] action  action
     * @param[in] param  action parameter
     * @param[in] feature  action feature
     */
    void softkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

public:

    // ************************************************************************
    // CTOR / DTOR
    // ************************************************************************

    /// CTOR.
    /**
     * @param[in] h  hook
     * @param[in] m  softkeys
     * @param[in] parent  parent widget
     */
    selfServiceCameraPredictions( selfServiceHook *h, const DomLoadSoftkeyGroup *m, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceCameraPredictions();

private:

    selfServiceHook *hook_;

    // not implemented
    selfServiceCameraPredictions( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SELFSERVICECAMERAPREDICTIONS_H
