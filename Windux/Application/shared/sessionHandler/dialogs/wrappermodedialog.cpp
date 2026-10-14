/**
 * @file wrappermodedialog.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrappermodedialog.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <toolbutton.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

#include <QHBoxLayout>
#include <QToolButton>
#include <QVBoxLayout>

const bool WrapperModeDialog::convertUppercase_ = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperModeDialog::WrapperModeDialog( modeFlags mf, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f ),
    sendMessages_( false )
{
    setObjectName( "WrapperModeDialog" );

    // initialize
    initialize( mf );
    createLayout();

    sendMessages_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperModeDialog::~WrapperModeDialog()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperModeDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 600) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 375) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *WrapperModeDialog::rendererAppearance() const
{
    return new hobart::darkOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WrapperModeDialog::rendererStyle() const
{
    return hobart::ReadOnly;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::onApplyToggled( bool value )
{
    LOG_CLASS_INFO( "WrapperModeDialog", "onApplyToggled" );

    // there are some special rules for this button:
    // 1) wrap enabled
    //    toggle on --> label on
    //    toggle off --> label off
    // 2) wrap disabled
    //    toggle on --> label on
    //    toggle off --> nothing
    if ( ( ( wrap_->isVisible() ) && ( ( wrap_->isChecked() ) || ( value ) ) ) ||

         // CLA Refresh
         ( session_->scaleConfig->isCLARefresh_ && value )
    )
    {
        if ( label_->isVisible() )
        {
            bool sendMessagesPrev( sendMessages_ );

            LOG_TRACE( "toggling label to %d", value );

            sendMessages_ = false;
            label_->setChecked( value );
            sendMessages_ = sendMessagesPrev;
        }
    }


    if ( sendMessages_ )
        updateCurrentMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::onWrapToggled( bool value )
{
    LOG_CLASS_INFO( "WrapperModeDialog", "onWrapToggled" );

    // there are some special rules for this button:
    // 1) label enabled
    //    toggle on --> apply on
    //    toggle off --> nothing
    // 2) label disabled
    //    nothing
    if (( label_->isVisible() ) && ( label_->isChecked() ) && ( value ))
        if ( apply_->isVisible() )
        {
            bool sendMessagesPrev( sendMessages_ );

            LOG_TRACE( "toggling apply to %d", value );

            sendMessages_ = false;
            apply_->setChecked( value );
            sendMessages_ = sendMessagesPrev;
        }

    if ( sendMessages_ )
        updateCurrentMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::onLabelToggled( bool value )
{
    LOG_CLASS_INFO( "WrapperModeDialog", "onLabelToggled" );

    // there are some special rules for this button:
    // 1) wrap enabled
    //    toggle on --> apply on
    //    toggle off --> apply off
    // 2) wrap disabled
    //    toggle on --> nothing
    //    toggle off --> apply off
    if ( ( ( wrap_->isVisible() ) && (( wrap_->isChecked() ) || ( !value )) ) ||

         // CLA Refresh
         ( session_->scaleConfig->isCLARefresh_ && !value )
    )
    {
        if ( apply_->isVisible() )
        {
            bool sendMessagesPrev( sendMessages_ );

            LOG_TRACE( "toggling apply to %d", value );

            sendMessages_ = false;
            apply_->setChecked( value );
            sendMessages_ = sendMessagesPrev;
        }
    }

    if ( sendMessages_ )
        updateCurrentMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::initialize( modeFlags mf )
{
    const QSize buttonSize(
        (mainWindow_->height() * 120) / SessionMainWindow::scalingDividerY_,
        (mainWindow_->height() * 120) / SessionMainWindow::scalingDividerY_ );

    const hobart::backgroundWidgetRenderer renderButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::redButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderPressedButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::greenButtonBackgroundWidgetRendererAppearance() );

    const QPixmap buttonPixmap( renderButton.drawControl( buttonSize, hobart::ReadOnly ) );
    const QPixmap buttonPixmapPressed( renderPressedButton.drawControl( buttonSize, hobart::ReadOnly ) );

    modifyFontPointSize setFontPointSize12( (mainWindow_->height() * 12) / SessionMainWindow::scalingDividerY_ );

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    setForegroundPalette( this );

    // ---- //

    QString applyButtonLabel( tr( "Apply" ) );
    QString wrapButtonLabel( tr( "Wrap" ) );
    QString labelButtonLabel( tr( "Label" ) );

    if ( convertUppercase_ )
    {
        applyButtonLabel = applyButtonLabel.toUpper();
        wrapButtonLabel = wrapButtonLabel.toUpper();
        labelButtonLabel = labelButtonLabel.toUpper();
    }

    // apply
    apply_ = new hobart::toolButton( applyButtonLabel, this, buttonPixmap, buttonPixmapPressed );
    apply_->setCheckable( true );
    apply_->setObjectName( "apply" );
    setFontPointSize12( apply_ );

    connect( apply_, SIGNAL( toggled(bool) ), SLOT( onApplyToggled(bool) ) );

    // wrap
    wrap_ = new hobart::toolButton( wrapButtonLabel, this, buttonPixmap, buttonPixmapPressed );
    wrap_->setCheckable( true );
    wrap_->setObjectName( "wrap" );
    setFontPointSize12( wrap_ );

    connect( wrap_, SIGNAL( toggled(bool) ), SLOT( onWrapToggled(bool) ) );

    // label
    label_ = new hobart::toolButton( labelButtonLabel, this, buttonPixmap, buttonPixmapPressed );
    label_->setCheckable( true );
    label_->setObjectName( "label" );
    setFontPointSize12( label_ );

    connect( label_, SIGNAL( toggled(bool) ), SLOT( onLabelToggled(bool) ) );

    // ---- //

    apply_->setHidden( ApplyHidden & mf );
    apply_->setChecked( Apply & mf );

    wrap_->setHidden( WrapHidden & mf );
    wrap_->setChecked( Wrap & mf );

    label_->setHidden( LabelHidden & mf );
    label_->setChecked( Label & mf );

    // ---- //

    // mapping of indicators to operate mode
    indicatorModes_[Apply | Wrap | Label] = OpModeAutoWeigh;        // 3
    indicatorModes_[Apply | Label] = OpModeSemiWeigh;               // 4
    indicatorModes_[Wrap] = OpModeWrapOnly;                         // 11
    indicatorModes_[Label] = OpModeAutoManualApply;                 // 14
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout() );

    buttons->addStretch();
    buttons->addWidget( apply_ );
    buttons->addStretch();
    buttons->addWidget( wrap_ );
    buttons->addStretch();
    buttons->addWidget( label_ );
    buttons->addStretch();

    QVBoxLayout *screenLayout( new QVBoxLayout( this ) );

    screenLayout->addStretch();
    screenLayout->addLayout( buttons );
    screenLayout->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::updateCurrentMode() const
{
    LOG_CLASS_INFO( "WrapperModeDialog", "updateCurrentMode" );

    int indicators = None;

    if (( apply_->isVisible() ) && ( apply_->isChecked() ))
        indicators |= Apply;
    if (( wrap_->isVisible() ) && ( wrap_->isChecked() ))
        indicators |= Wrap;
    if (( label_->isVisible() ) && ( label_->isChecked() ))
        indicators |= Label;

    // lookup mode
    int mode = OpModeManualWeigh;

    if ( indicatorModes_.contains( indicators ) )
        mode = indicatorModes_[indicators];

    // set mode
    LOG_INFO( "setting mode to %d...", mode );

    session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( mode ) );
    session_->send( SessionDev, SnReadWeighModeTransactionType );

    if( session_->scaleConfig->currentPLU.itemNumber().toInt() > 0)
        session_->send( ProductDev, ProductRead, session_->scaleConfig->currentPLU.itemNumber() + ",3" );       // didnt have to clear it, just refresh
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperModeDialog::showEvent( QShowEvent *e )
{
    //DomWeighModeTotalType *mode = &session_->scaleConfig->weighModeTotalType;

    QString modelName = session_->scaleConfig->getScaleModelName();
    bool isBooted    = session_->scaleConfig->internalConfig.scaleBootComplete;

    if( ( !isBooted ) && ( modelName.contains( "NGW", Qt::CaseInsensitive ) ) )
    {
        apply_->setChecked( true );
        label_->setChecked( true );
        wrap_->setChecked( true );

        updateCurrentMode();
    }
    else
    {
        session_->send( SessionDev, SnReadWeighModeTransactionType );
    }

    _Mybase::showEvent( e );
}

