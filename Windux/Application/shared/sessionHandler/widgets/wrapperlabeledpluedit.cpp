/**
 * @file wrapperlabeledpluedit.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "wrapperlabeledpluedit.h"
#include "wrappermodewidget.h"

#include "../dialogs/wrappermodedialog.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>

#include <ScaleConfiguration.h>
#include <Messages.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
wrapperLabeledPluEdit::wrapperLabeledPluEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    modeButtonVisible_( true ),
    modeUpdateEnabled_( false ),
    prevPeripheralList_( new MadpPeripheralList() )
{
    LOG_CLASS_INFO( "wrapperLabeledPluEdit", "wrapperLabeledPluEdit" );

    setObjectName( "wrapperLabeledPluEdit" );

    // initialize
    initialize();
    createLayout();
    translate();

    // hook into session
    sessionInfo info( this );

    if ( !(mainWindow_ = info()) )
        LOG_WARN( "could not retrieve main window!", NULL );
    else if ( !(session_ = mainWindow_->session()) )
        LOG_WARN( "bad session pointer", NULL );
    else
    {
        peripheralList_ = &session_->scaleConfig->peripheralList;
        weighModeTotalType_ = &session_->scaleConfig->weighModeTotalType;

        // hook into session
        connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessage(SessionMessage) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
wrapperLabeledPluEdit::~wrapperLabeledPluEdit()
{
    if ( prevPeripheralList_ )
    {
        delete prevPeripheralList_;
        prevPeripheralList_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::setModeButtonVisible( bool value )
{
    modeButton_->setVisible( modeButtonVisible_ = value );

    // refresh button
    if ( modeButtonVisible_ )
        updateModeButtonSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::setModeUpdateEnabled( bool value )
{
    modeUpdateEnabled_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::setReadOnly( bool value )
{
    // propigate to children
    modeButton_->setReadOnly( value );

    _Mybase::setReadOnly( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::translate()
{
    // propigate to children
    modeButton_->translate();

    _Mybase::translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::resizeEvent( QResizeEvent* e )
{
    _Mybase::resizeEvent( e );

    // modify mode button size
    updateModeButtonSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::updateWrapperMode()
{
    LOG_CLASS_INFO( "wrapperLabeledPluEdit", "updateWrapperMode" );

    static const int missingAllDevices( wrapperModeWidget::ApplyHidden | wrapperModeWidget::WrapHidden | wrapperModeWidget::LabelHidden );

    // weigher mode
    OpModeEvent mode( (OpModeEvent) weighModeTotalType_->weighMode() );

    //
    // set indicators
    //

    int indicators( wrapperModeWidget::None );
    bool invalidMode( false );

    if ( modeIndicators_.contains( mode ) )
        indicators = modeIndicators_[mode];

    // no label applier
    if ( !peripheralList_->isLabelApplierAttached() )
    {
        indicators |= wrapperModeWidget::ApplyHidden;
        invalidMode |= (wrapperModeWidget::Apply == (wrapperModeWidget::Apply & indicators));
    }

    // no wrapper (and no CLA!!! special case as per Clare)
    if (( !peripheralList_->isWrapperAttached() ) && ( MadpPeripheralList::latCLA_EPSAC2 != peripheralList_->labelApplier() ))
    {
        indicators |= wrapperModeWidget::WrapHidden;
        invalidMode |= (wrapperModeWidget::Wrap == (wrapperModeWidget::Wrap & indicators));
    }

    // no printers
    if ( !peripheralList_->isPrinterAttached() )
    {
        indicators |= wrapperModeWidget::LabelHidden;
        invalidMode |= (wrapperModeWidget::Label == (wrapperModeWidget::Label & indicators));
    }

    modeButton_->setIndicators( (wrapperModeWidget::indicatorFlags) indicators );

    // when no devices attached hide button
    if ( missingAllDevices == (missingAllDevices & indicators) )
        modeButton_->setVisible( false );
    else
    {
        modeButton_->setVisible( modeButtonVisible_ );
        updateModeButtonSize();
    }

    //
    // mode validation
    //

    bool lostPeripheral(
        ( prevPeripheralList_->isLabelApplierAttached() && !peripheralList_->isLabelApplierAttached() ) ||
        ( prevPeripheralList_->isPrinterAttached() && !peripheralList_->isPrinterAttached() ) ||
        ( prevPeripheralList_->isWrapperAttached() && !peripheralList_->isWrapperAttached() ));

    bool gainedPeripheral(
        ( peripheralList_->isLabelApplierAttached() && !prevPeripheralList_->isLabelApplierAttached() ) ||
        ( peripheralList_->isPrinterAttached() && !prevPeripheralList_->isPrinterAttached() ) ||
        ( peripheralList_->isWrapperAttached() && !prevPeripheralList_->isWrapperAttached() ));

    // we need to validate the mode with what peripherals we have, there are two cases:
    // 1) lost peripheral and we are now in an invalid mode
    // 2) new peripheral was attached
    if ((( lostPeripheral ) && ( invalidMode )) || ( gainedPeripheral ))
    {
        if ( invalidMode )
            LOG_WARN( "INVALID MODE", NULL );

        int capabilities( wrapperModeWidget::None );
        int newMode( OpModeManualWeigh );

        // we can apply
        if (( peripheralList_->isLabelApplierAttached() ) && ( peripheralList_->isPrinterAttached() ))
            capabilities |= wrapperModeWidget::Apply;

        // we can wrap (or CLA!!! special case as per Clare)
        if (( peripheralList_->isWrapperAttached() ) || ( MadpPeripheralList::latCLA_EPSAC2 == peripheralList_->labelApplier() ))
            capabilities |= wrapperModeWidget::Wrap;

        // we can label
        if ( peripheralList_->isPrinterAttached() )
            capabilities |= wrapperModeWidget::Label;

        // okay so now we know what we can do... what mode is it?
        for ( modeIndicatorMap::iterator i( modeIndicators_.begin() ); i != modeIndicators_.end(); ++i )
            if ( capabilities == i.value() )
            {
                newMode = i.key();
                break;
            }

        // set the mode!
        if (( modeUpdateEnabled_ ) && ( newMode != mode ))
        {
            // set mode
            LOG_INFO( "setting mode to %d (old %d)...", newMode, mode );

            session_->send( SessionDev, SnWriteWeighModeTransactionType, QString::number( newMode ) );
            session_->send( SessionDev, SnReadWeighModeTransactionType );
        }
    }

    //
    // set background
    //

    QPixmap background( modeBackgrounds_[OpModeIdle] );
    QPixmap backgroundFocused( modeBackgroundsFocused_[OpModeIdle] );

    if ( modeBackgrounds_.contains( mode ) )
        background = modeBackgrounds_[mode];

    if ( modeBackgroundsFocused_.contains( mode ) )
        backgroundFocused = modeBackgroundsFocused_[mode];

    setBackground( background );
    setBackgroundFocused( backgroundFocused );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::onModeButtonPressed( int currentState )
{
    typedef QPair<int, int> stateMapping;

    // create list of mappings from indicator state to dialog flags
    QList<stateMapping> mappings;
    mappings.append( stateMapping( wrapperModeWidget::Apply, WrapperModeDialog::Apply ) );
    mappings.append( stateMapping( wrapperModeWidget::Wrap, WrapperModeDialog::Wrap ) );
    mappings.append( stateMapping( wrapperModeWidget::Label, WrapperModeDialog::Label ) );
    mappings.append( stateMapping( wrapperModeWidget::ApplyHidden, WrapperModeDialog::ApplyHidden ) );
    mappings.append( stateMapping( wrapperModeWidget::WrapHidden, WrapperModeDialog::WrapHidden ) );
    mappings.append( stateMapping( wrapperModeWidget::LabelHidden, WrapperModeDialog::LabelHidden ) );

    // setup dialog flags
    int dialogFlags( WrapperModeDialog::None );

    foreach ( const stateMapping& m, mappings )
        if ( m.first & currentState )
            dialogFlags |= m.second;

    // show dialog
    WrapperModeDialog *dialog( new WrapperModeDialog( (WrapperModeDialog::modeFlags) dialogFlags, mainWindow_ ) );

    dialog->open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "wrapperLabeledPluEdit", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // weigh mode
    if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
        updateWrapperMode();

    // peripheral list
    else if (( System == device ) && ( SysPeripheralList == event ))
    {
        updateWrapperMode();

        // save off the peripheral list
        (*prevPeripheralList_) = (*peripheralList_);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::initialize()
{
    modeButton_ = new wrapperModeWidget( this );
    modeButton_->setObjectName( "wrapperModeWidget" );
    modeButton_->setReadOnly( isReadOnly() );
    modeButton_->setVisible( true );

    connect( modeButton_, SIGNAL( pressed(int) ), SLOT( onModeButtonPressed(int) ) );

    // ---- //

    // map of weigher modes to indicator states
    modeIndicators_[OpModeAutoWeigh] = wrapperModeWidget::Apply | wrapperModeWidget::Wrap | wrapperModeWidget::Label;
    modeIndicators_[OpModeSemiWeigh] = wrapperModeWidget::Apply | wrapperModeWidget::Label;
    modeIndicators_[OpModeWrapOnly] = wrapperModeWidget::Wrap;
    modeIndicators_[OpModeAutoManualApply] = wrapperModeWidget::Label;
    modeIndicators_[OpModeManualWeigh] = wrapperModeWidget::None;

    // ---- //

    hobart::backgroundWidgetRenderer<
         hobart::rectangleBackgroundWidget<270, 100>,
         hobart::darkOperatorBackgroundWidgetRendererAppearance> renderField270x100_Normal;

    hobart::backgroundWidgetRenderer<
         hobart::rectangleBackgroundWidget<270, 100>,
         hobart::greenOperatorBackgroundWidgetRendererAppearance> renderField270x100_Green;

    hobart::backgroundWidgetRenderer<
         hobart::rectangleBackgroundWidget<270, 100>,
         hobart::yellowOperatorBackgroundWidgetRendererAppearance> renderField270x100_Yellow;

    // map of weigher modes to backgrounds
    modeBackgrounds_[OpModeIdle] = renderField270x100_Normal.drawControl( hobart::Normal );
    modeBackgrounds_[OpModeAutoWeigh] = renderField270x100_Green.drawControl( hobart::Normal );
    modeBackgrounds_[OpModeSemiWeigh] = renderField270x100_Yellow.drawControl( hobart::Normal );

    modeBackgroundsFocused_[OpModeIdle] = renderField270x100_Normal.drawControl( hobart::Focused );
    modeBackgroundsFocused_[OpModeAutoWeigh] = renderField270x100_Green.drawControl( hobart::Focused );
    modeBackgroundsFocused_[OpModeSemiWeigh] = renderField270x100_Yellow.drawControl( hobart::Focused );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::createLayout()
{
    // adjust layout
    screenLayoutInner_->addWidget( modeButton_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wrapperLabeledPluEdit::updateModeButtonSize()
{
    // modify mode button size to always be square
    modeButton_->setFixedWidth( (3 * modeButton_->height()) / 4 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


