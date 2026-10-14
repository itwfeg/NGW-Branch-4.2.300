/**
 * @file serviceWeigherCalibrate.cpp
 *
 * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "serviceWeigherCalibrate.h"

#include "../sessionmainwindow.h"

#include "../widgets/weigherCalibrate.h"
#include "../dialogs/messageboxdialog.h"

#include <ScaleConfiguration.h>
#include <backgroundwidgettypes.h>

#include <QHBoxLayout>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceWeigherCalibrate::serviceWeigherCalibrate( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    // FT-695
    weigherCalibrateView_ = NULL;

    session_ = mainWindow->session();
    internalConfig_ = &session_->scaleConfig->internalConfig;

    // initialize
    initialize();

    // NGW-141
    scaleScreen();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceWeigherCalibrate::~serviceWeigherCalibrate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherCalibrate::translate()
{
    // FT-695
    if( weigherCalibrateView_ )
        weigherCalibrateView_->translate();

    update();
}


bool serviceWeigherCalibrate::onScreenChanging()
{

    // do not allow screen change while calibrating
    if ( internalConfig_->isScaleCalibrating )
    {
        MessageBoxDialog dialog( mainWindow_ );

        dialog.setText( tr( "Please finish calibrating or press Cancel!" ) );
        dialog.setStandardButtons( QDialogButtonBox::Ok );

        dialog.exec();

        return false;
    }

    return _Mybase::onScreenChanging();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceWeigherCalibrate::initialize()
{
    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->setContentsMargins( QMargins() );

    // create Weigher Calibrate instance
    weigherCalibrateView_ = new hobart::weigherCalibrate( this, mainWindow_, 1 );
    screenLayout->addWidget( weigherCalibrateView_ );
}

