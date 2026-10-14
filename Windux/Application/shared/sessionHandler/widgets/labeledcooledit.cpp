/**
 * @file labeledcooledit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "labeledcooledit.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include "../dialogs/cooldialog.h"

#include <QTimer>

/// Hobart namespace.
namespace hobart {

const int labeledCoolEdit::noCoolTextNum_ = -1;

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledCoolEdit::labeledCoolEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    cool_( 0 ),
    defaultCool_( 0 ),
    coolEnabled_( true ),
    trackingNumberEnabled_( true ),
    coolRequired_( false ),
    trackingNumberRequired_( false ),
    dialogOpen_( false ),
    timer_( NULL ),
    mainWindow_( NULL )
{
    setObjectName( "hobartLabeledCoolEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledCoolEdit::~labeledCoolEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledCoolEdit::coolText() const
{
    // when cool is not required it will not be in the list so do not bother checking
    if ( !isCoolNotRequired() )
        for ( coolTextMap::const_iterator i = coolText_.begin(); i != coolText_.end(); ++i )
            if ( i.key() == cool_ )
                return i.value();

    return "";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString labeledCoolEdit::trackingNumber() const
{
    if ( isCoolNotRequired() )
        return "";

    return trackingNumber_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::clear()
{
    cool_ = defaultCool_ = 0; // FT-733
    icon_->setVisible( false );

    _Mybase::clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::clearSecond()
{
    trackingNumber_.clear();

    _Mybase::clearSecond();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::setCool( int value )
{
    bool emitCoolChanged = true;

    if ( cool_ == value )
        emitCoolChanged = false;

    cool_ = value;

    // set text (elided)
    QFontMetrics fontMetrics( child_->fontMetrics() );
    QMargins margins( child_->contentsMargins() );

    int width = child_->width() - margins.left() - margins.right();
    QString elidedCoolText( fontMetrics.elidedText( coolText(), Qt::ElideRight, width ) );

    setText( elidedCoolText ); // FT-549 use elided text
    updateIconVisible();

    // update tracking number also
    childSecond_->setText( trackingNumber() );

    // emit signal when changed
    if ( emitCoolChanged )
        Q_EMIT coolChanged();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::setCoolEnabled( bool value )
{
    child_->setVisible( coolEnabled_ = value );
    updateIconVisible();

    // setup new focus proxy
    updateFocusProxy();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::setTrackingNumber( const QString& value )
{
    if ( trackingNumber() == value )
        return;

    trackingNumber_ = value;

    childSecond_->setText( trackingNumber() );

    Q_EMIT trackingNumberChanged();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::setTrackingNumberEnabled( bool value )
{
    childSecond_->setVisible( trackingNumberEnabled_ = value );

    // setup new focus proxy
    updateFocusProxy();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::promptForInput( bool coolRequired, bool trackingNumberRequired )
{
    // check access level
    if ( !checkOperatorAccessLevel() )
    {
        Q_EMIT invalidAccessLevel();
        return;
    }

    coolRequired_ = coolRequired;
    trackingNumberRequired_ = trackingNumberRequired;

    // pop the dialog
    timer_->setSingleShot( true );
    timer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledCoolEdit::eventFilter( QObject *obj, QEvent *event )
{
    // for focus in we handle the event ourselves
    if (( child_ == obj ) || ( childSecond_ == obj ))
        if ( QEvent::FocusIn == event->type() )
        {
            // prompt
            promptForInput();

            // eat the event
            return true;
        }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::mousePressEvent( QMouseEvent *e )
{
    // when the widget is clicked move focus to the input box
    if ( Qt::LeftButton == e->button() )
        if (( !child_->isVisible() ) && ( childSecond_->isVisible() ))
            childSecond_->setFocus();

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::onSelectionChanged()
{
    // do not allow text selection in this widget
    if ( child_->hasSelectedText() )
        child_->deselect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::onSelectionChangedSecond()
{
    // do not allow text selection in this widget
    if ( childSecond_->hasSelectedText() )
        childSecond_->deselect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::onTimeout()
{
    LOG_CLASS_INFO( "labeledCoolEdit", "onTimeout" );

    CoolDialog dialog( mainWindow_ );
    int rc;

    // set cool text
    dialog.addCoolTextItem( noCoolTextNum_, tr( "Do Not Print COOL Text" ) ); // FT-955

    for ( coolTextMap::const_iterator i = coolText_.begin(); i != coolText_.end(); ++i )
        dialog.addCoolTextItem( i.key(), i.value(), (i.key() == defaultCool_) );

    // set cool
    dialog.setCoolEnabled( true ); // always show cool
    dialog.setCoolRequired( coolRequired_ );
    dialog.setCool( cool_ );

    // set tracking number
    dialog.setTrackingNumberEnabled( trackingNumberRequired_ || isTrackingNumberEnabled() ); // show when required or enabled
    dialog.setTrackingNumberRequired( trackingNumberRequired_ );
    dialog.setTrackingNumber( trackingNumber_ );

    // set dialog title
    dialog.setTitle( labelText() );

    // open the dialog
    dialogOpen_ = true;
    updateBackgroundPalette();

    coolRequired_ = false;
    trackingNumberRequired_ = false;

    if ( QDialog::Accepted == (rc = dialog.exec()) )
    {
        // get cool
        if ( dialog.isCoolEnabled() )
            setCool( dialog.cool() );

        // get tracking number (only change if cool required)
        if ( dialog.isTrackingNumberEnabled() )
        {
            if ( noCoolTextNum_ == cool() )
                LOG_INFO( "not saving cool tracking because 'no cool' selected", NULL );
            else
            {
                setTrackingNumber( dialog.trackingNumber() );
            }
        }
    }

    // done
    dialogOpen_ = false;
    updateBackgroundPalette();

    Q_EMIT dialogActionComplete( rc );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::initialize()
{
    static const QSize iconSize( 15, 13 );

    QPixmap iconPixmap( ":/images/dialogs/warningIcon.png" );

    modifyPaletteBackground setBackgroundIcon( iconPixmap.scaled( iconSize ) );

    // icon
    icon_ = new QWidget( this );
    icon_->setAutoFillBackground( true );
    icon_->setFixedSize( iconSize );
    icon_->setObjectName( "icon" );
    setBackgroundIcon( icon_ );

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addSpacing( 8 );
    vlayout->addWidget( icon_ );
    vlayout->addStretch();

    bottomLayout_->addLayout( vlayout );

    icon_->hide();

    // create timer
    timer_ = new QTimer( this );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    // retrieve main window
    sessionInfo mySession( this );
    mainWindow_ = mySession();

    // connect signals
    connect( child_, SIGNAL( selectionChanged() ), SLOT( onSelectionChanged() ) );
    connect( childSecond_, SIGNAL( selectionChanged() ), SLOT( onSelectionChangedSecond() ) );

    // install event filters
    child_->installEventFilter( this );
    childSecond_->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::updateIconVisible()
{
    // cool is not visible so icon is not either
    if ( !isCoolEnabled() )
        icon_->setVisible( false );

    // set default cool warning icon visibility
    else
        icon_->setVisible( defaultCool_ ? (defaultCool_ != cool_) : false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledCoolEdit::updateFocusProxy()
{
    // setup new focus proxy
    if ( child_->isVisible() )
        setFocusProxy( child_ );
    else if ( childSecond_->isVisible() )
        setFocusProxy( childSecond_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

