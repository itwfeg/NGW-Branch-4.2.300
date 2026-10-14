/**
 * @file selfservicecamerapredictionsG2.h
 * Self service hook camera predictions.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SELFSERVICECAMERAPREDICTIONSG2_H
#define SELFSERVICECAMERAPREDICTIONSG2_H

#include <QWidget>

namespace Ui {
class selfServiceCameraPredictionsG2;
}

class selfServiceHook;

class DomLoadSoftkeyGroup;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Self service hook camera predictions.
class selfServiceCameraPredictionsG2 : public QWidget
{
    Q_OBJECT

    typedef selfServiceCameraPredictionsG2 _Myt;
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
    selfServiceCameraPredictionsG2( selfServiceHook *h, const DomLoadSoftkeyGroup *m, QWidget *parent = NULL );

    /// DTOR
    virtual ~selfServiceCameraPredictionsG2();

    /// Update the connectivity background
    virtual void updateNetworkConnectivity( bool online );

private:

    Ui::selfServiceCameraPredictionsG2 *ui;

    selfServiceHook *hook_;

    QWidget *screen_;

    // not implemented
    selfServiceCameraPredictionsG2( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SELFSERVICECAMERAPREDICTIONSG2_H
