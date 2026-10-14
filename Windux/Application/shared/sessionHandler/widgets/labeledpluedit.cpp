/**
 * @file labeledpluedit.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

//	HA-176: Merged wrapperLabeledPluEdit with labeledPluEdit to incorporate the ability to show the Total/Transaction type on the run screen.
//			As a result, the files wrapperlabeledpluedit.h & wrapperlabeledpluedit.cpp were removed from the project.

#include "labeledpluedit.h"
#include "wrappermodewidget.h"

#include "../dialogs/wrappermodedialog.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>

#include <ScaleConfiguration.h>
#include <Messages.h>

static const bool FAKE_WRAPPER = false;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPluEdit::labeledPluEdit( QWidget *parent, const QPixmap& image, bool forWrapper ) :
    _Mybase( parent, image ),
    modeButtonVisible_( false ),
    modeUpdateEnabled_( false ),
    trayTypeVisible_( false ),
    totalTypeVisible_( false ),
    showApplyWrapLabel_( forWrapper ),
    prevPeripheralList_( NULL )
{
    LOG_CLASS_INFO( "labeledPluEdit", "labeledPluEdit" );

    setObjectName( "labeledPluEdit" );

    if( FAKE_WRAPPER )
    {
        forWrapper = true;
        modeButtonVisible_ = true;
        modeUpdateEnabled_ = true;
        trayTypeVisible_ = true;
        totalTypeVisible_ = true;
        showApplyWrapLabel_ = true;
    }

    // hook into session
    sessionInfo info( this );

    if (showApplyWrapLabel_)
    {
        modeButtonVisible_ = true;
        prevPeripheralList_ = new MadpPeripheralList();
    }

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

    /*
    // Get the version info
    session_->send(LabelApplierSystem, LaSysReqStatus); // 6,13

    // Get the board info
    session_->send(LabelApplier, LaReqSysInfo); // 5,29
    */

    // initialize
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledPluEdit::~labeledPluEdit()
{
    if ( prevPeripheralList_ )
    {
        delete prevPeripheralList_;
        prevPeripheralList_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setModeButtonVisible( bool value )
{
    if( FAKE_WRAPPER )
    {
        value = true;
        showApplyWrapLabel_ = true;
        modeButtonVisible_ = true;
    }

    if (showApplyWrapLabel_)
    {
        modeButton_->setVisible( modeButtonVisible_ = value );

        // refresh button
        if ( modeButtonVisible_ )
            updateModeButtonSize();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setModeUpdateEnabled( bool value )
{
    modeUpdateEnabled_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setReadOnly( bool value )
{
    if( showApplyWrapLabel_ )
    {
        // propigate to children
        modeButton_->setReadOnly( value );

        _Mybase::setReadOnly( value );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setLabelFont( const QFont& value )
{
    trayType_->setFont( value );
    totalType_->setFont( value );

    _Mybase::setLabelFont( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setTrayTypeObjectName( const QString& value )
{
    trayType_->setObjectName( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setTrayTypeForced( bool value )
{
    QPalette p( totalType_->palette() );

    if( FAKE_WRAPPER )
        value = true;

    if ( value )
    {
        trayType_->setVisible( true );
        trayType_->setText( tr( "SELECT TRAY" ) );

        p.setColor( trayType_->foregroundRole(), hobart::colorPalette::red );
    }
    else
    {
        trayType_->setVisible( trayTypeVisible_ );
        trayType_->setText( trayTypeText_ );
    }

    trayType_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setTrayTypeVisible( bool value, const QString& text )
{
    if( FAKE_WRAPPER )
        value = true;

    trayType_->setVisible( trayTypeVisible_ = value );
    trayType_->setText( trayTypeText_ = text );

    QPalette p( totalType_->palette() );
    trayType_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setTotalTypeObjectName( const QString& value )
{
    totalType_->setObjectName( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::setTotalTypeVisible( bool value )
{
    if( FAKE_WRAPPER )
        value = true;

    totalType_->setVisible( totalTypeVisible_ = value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::translate()
{
    // propagate to children
    if (showApplyWrapLabel_)
        modeButton_->translate();

    updateTotalTypeName();

    _Mybase::translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::resizeEvent( QResizeEvent* e )
{
    // modify mode button size
    updateModeButtonSize();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::updateWrapperMode()
{
    LOG_CLASS_INFO( "labeledPluEdit", "updateWrapperMode" );

    static const int missingAllDevices( wrapperModeWidget::ApplyHidden | wrapperModeWidget::WrapHidden | wrapperModeWidget::LabelHidden );

    // weigher mode
    OpModeEvent mode( (OpModeEvent) weighModeTotalType_->weighMode() );

    // I don't see why we want to go through all of this is we are coming back from
    // Supervisor Mode. I would think we would use what we have already set up.
    if ( OpModeSupervisor == mode )
        return;

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
    if ( ( ( !peripheralList_->isWrapperAttached() ) && ( MadpPeripheralList::latCLA_EPSAC2 != peripheralList_->labelApplier() ) ) || ( session_->scaleConfig->isCLARefresh_ ) )
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
    {
        if( FAKE_WRAPPER )
            modeButton_->setVisible( true );
        else
            modeButton_->setVisible( false );
    }
    else
    {
        if( FAKE_WRAPPER )
            modeButtonVisible_ = true;

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

        // we can wrap (or CLA!!! special case as per Clare (or not CLA Refresh) )
        if ( ( ( peripheralList_->isWrapperAttached() ) || ( MadpPeripheralList::latCLA_EPSAC2 == peripheralList_->labelApplier() ) )

             // CLA Refresh
             && !session_->scaleConfig->isCLARefresh_
        )
        {
            capabilities |= wrapperModeWidget::Wrap;
        }

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
void labeledPluEdit::onModeButtonPressed( int currentState )
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
void labeledPluEdit::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO("labeledPluEdit", "onSessionMessage");

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    // weigh mode
    if ( ( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ) )
    {
        if (showApplyWrapLabel_)
        {
            //session_->send(LabelApplier, LaReqSysInfo); // 5,29
            updateWrapperMode();
        }
        updateTotalTypeName();
    }
    // peripheral list
    else if ( ( System == device ) && ( SysPeripheralList == event ) && showApplyWrapLabel_ )
    {
        //session_->send(LabelApplier, LaReqSysInfo); // 5,29
        updateWrapperMode();

        // wrapper was detached and is now attached
        if( !prevPeripheralList_->isWrapperAttached() && peripheralList_->isWrapperAttached() )
        {
            LOG_INFO( "Wrapper was detached and is now attached", NULL );
        }

        // save off the peripheral list
        (*prevPeripheralList_) = (*peripheralList_);
    }
    else if( ( LabelApplier == device ) && ( LaSysInfo == event ) )
    {
        DomLabelApplierSystemInformation m(msg);

        if( m.awsModelNumber() == 3 )
        {
            session_->scaleConfig->isCLARefresh_ = true;
        }
        else
        {
            session_->scaleConfig->isCLARefresh_ = false;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::initialize()
{
    if (showApplyWrapLabel_)
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

        const QSize fieldSize(
            (mainWindow_->width() * 270) / SessionMainWindow::scalingDividerX_,
            (mainWindow_->height() * 100) / SessionMainWindow::scalingDividerY_ );

        const hobart::backgroundWidgetRenderer renderFieldNormal(
            new hobart::rectangleBackgroundWidget(),
            new hobart::darkOperatorBackgroundWidgetRendererAppearance() );

        const hobart::backgroundWidgetRenderer renderFieldGreen(
            new hobart::rectangleBackgroundWidget(),
            new hobart::greenOperatorBackgroundWidgetRendererAppearance() );

        const hobart::backgroundWidgetRenderer renderFieldYellow(
            new hobart::rectangleBackgroundWidget(),
            new hobart::yellowOperatorBackgroundWidgetRendererAppearance() );

        // map of weigher modes to backgrounds
        modeBackgrounds_[OpModeIdle] = renderFieldNormal.drawControl( fieldSize, hobart::Normal );
        modeBackgrounds_[OpModeAutoWeigh] = renderFieldGreen.drawControl( fieldSize, hobart::Normal );
        modeBackgrounds_[OpModeSemiWeigh] = renderFieldYellow.drawControl( fieldSize, hobart::Normal );

        modeBackgroundsFocused_[OpModeIdle] = renderFieldNormal.drawControl( fieldSize, hobart::Focused );
        modeBackgroundsFocused_[OpModeAutoWeigh] = renderFieldGreen.drawControl( fieldSize, hobart::Focused );
        modeBackgroundsFocused_[OpModeSemiWeigh] = renderFieldYellow.drawControl( fieldSize, hobart::Focused );
    }

    trayType_ = new QLabel( this );
    trayType_->setAlignment( Qt::AlignCenter );
    trayType_->setObjectName( "trayType" );
    trayType_->setVisible( trayTypeVisible_ );

    totalType_ = new QLabel( this );
    totalType_->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    totalType_->setObjectName( "totalType" );
    totalType_->setVisible( totalTypeVisible_ );

    QFont f( labelFont() );
    f.setItalic( true );

    trayType_->setFont( f );
    totalType_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::createLayout()
{
    if( FAKE_WRAPPER )
     showApplyWrapLabel_ = true;

    // adjust layout
    if (showApplyWrapLabel_)
        screenLayoutInner_->addWidget( modeButton_ );

    // tray type
    trayTypeLayout_ = new QVBoxLayout();
    trayTypeLayout_->setContentsMargins( QMargins() );
    trayTypeLayout_->setSpacing( 0 );

    trayTypeLayout_->addWidget( trayType_ );

    // total type
    totalTypeLayout_ = new QVBoxLayout();
    totalTypeLayout_->setContentsMargins( QMargins() );
    totalTypeLayout_->setSpacing( 0 );

    totalTypeLayout_->addWidget( totalType_ );

    // adjust layout
    topLayout_->addLayout( trayTypeLayout_, 1 );
    topLayout_->addLayout( totalTypeLayout_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::updateModeButtonSize()
{
    if (!showApplyWrapLabel_)
        return;

    // modify mode button size to always be square
    const int modeButtonHeight( height() - 16 );
    const int modeButtonWidth( (3 * modeButtonHeight) / 4 );

    modeButton_->setFixedSize( modeButtonWidth, modeButtonHeight );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledPluEdit::updateTotalTypeName()
{
    QString name( weighModeTotalType_->totalTypeName() );

    // check for second language total type name
    if ( 2 == session_->scaleConfig->currentLanguage )
        if ( weighModeTotalType_->totalTypeName2().length() )
            name = weighModeTotalType_->totalTypeName2();

    // set text value
    totalType_->setText( name );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


