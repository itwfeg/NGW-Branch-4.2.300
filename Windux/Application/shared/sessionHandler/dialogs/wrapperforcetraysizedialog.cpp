/**
 * @file wrapperforcetraysizedialog.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 * @inspired by Joe E. Crew (cutting test screen)
 * @inspired by Randy Blankley
 */

#include <logmanager.h>
#include <softkeyswidget.h>

#include "wrapperforcetraysizedialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <QApplication>
#include <QVBoxLayout>

const bool WrapperForcedTraySizeDialog::convertUppercase_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperForcedTraySizeDialog::WrapperForcedTraySizeDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    response_( false ),
    numTrays_( 0 )
{
    setObjectName( "WrapperForcedTraySizeDialog" );

    // initialize
    initialize();
    createLayout();

    resetFlashkeys( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperForcedTraySizeDialog::~WrapperForcedTraySizeDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperForcedTraySizeDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 600) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 400) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WrapperForcedTraySizeDialog::traySizesExist()
{
    const QDateTime timeout( QDateTime::currentDateTime().addSecs( 5 ) );

    // wait for response and number of trays
    while ( !response_ )
    {
        if ( timeout <= QDateTime::currentDateTime() )
            return 0;

        QApplication::processEvents();
    }

    return numTrays_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *WrapperForcedTraySizeDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WrapperForcedTraySizeDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperForcedTraySizeDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // softkeys
    if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        DomLoadSoftkeyGroup m( msg );

        if ( trays_ )
            trays_->create( &m );

        if ( hobart::softkeysWidget::ForcedCameraOverride == m.featureNum() )
        {
            response_ = true;
            numTrays_ = m.totalNumOfSoftkeys();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperForcedTraySizeDialog::resetFlashkeys( bool force )
{
    trays_->sendResetMessage( force );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperForcedTraySizeDialog::onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    Q_UNUSED( b )
    Q_UNUSED( param )
    Q_UNUSED( feature )

    if ( trays_ != w )
        return;
    else if ( Action_OverrideWrapperCameraDimensions != action )
        return;

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperForcedTraySizeDialog::initialize()
{
    static const QMargins screenMargins( 16, 16, 16, 16 );

    setContentsMargins( screenMargins );

    trays_ = new hobart::softkeysWidget( session_, this );
    trays_->setAutoFillBackground( true );
    trays_->setCreateFilter( hobart::softkeysWidget::ForcedCameraOverride );
    trays_->setObjectName( "trays" );

    OperateMode *operateMode( mainWindow_->retrieveMode<OperateMode>() );

    if ( operateMode )
        connect( trays_, &hobart::softkeysWidget::clicked, operateMode, &OperateMode::handleFlashkey );

    connect( trays_, &hobart::softkeysWidget::clicked, this, &_Myt::onSoftkeyClicked );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperForcedTraySizeDialog::createLayout()
{
    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );
    screenLayout->setContentsMargins( QMargins() );
    screenLayout->addWidget( trays_ );
}
